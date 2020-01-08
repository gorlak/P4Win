//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Changes.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_changes.h"
#include "p4change.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Changes, CP4Command)

int compareChanges( const void *arg1, const void *arg2 );

CCmd_Changes::CCmd_Changes(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4CHANGES;
	m_TaskName= _T("Changes");
	m_pBatch=NULL;
}

CCmd_Changes::~CCmd_Changes()
{
	if(m_pBatch != NULL)
		delete m_pBatch;
}

// 'loquatious' values can be any of the following:
//	0 -> no, do not add -l or -L
//	1 -> yes, use -l to get full descriptions
//	2 -> yes, use -L to get max 250 byte descriptions (valid only on 2005.1 and later servers)
BOOL CCmd_Changes::Run(ECmdChangesFilter filter, int loquatious, CStringList *viewSpec/*=NULL*/, 
					   long numToFetch/*=0*/, BOOL inclInteg/*=FALSE*/, 
					   CString *user/*=NULL*/, CString *client/*=NULL*/)
{
	ClearArgs();
	m_BaseArgs= AddArg(_T("changes"));

	// We use the tagged output in order to detected shelved changelists.
	// Luckily shelving isn't supported until server level 28.
	m_UsedTagged = GET_SERVERLEVEL() >= 8 ? TRUE : FALSE;

	// May only want numToFetch most recent changes
	if(numToFetch > 0)
	{          
		AddArg(_T("-m"));
		m_BaseArgs= AddArg(numToFetch);
	}

	if (inclInteg)
		m_BaseArgs= AddArg(_T("-i"));

	if (user && (GET_SERVERLEVEL() >= 12))
	{
		m_User = *user;
		m_BaseArgs= AddArg(_T("-u"));
		m_BaseArgs= AddArg(m_User);
	}
	
	if (client && (GET_SERVERLEVEL() >= 12))
	{
		m_Client = *client;
		m_BaseArgs= AddArg(_T("-c"));
		m_BaseArgs= AddArg(m_Client);
	}
	
	if(loquatious)
		m_BaseArgs= AddArg(loquatious == 2 ? _T("-L") : _T("-l"));

	if(filter != ALL_CHANGES)
	{
		AddArg(_T("-s"));
		if(filter == PENDING_CHANGES)
			AddArg(_T("pending"));
		else if(filter == SUBMITTED_CHANGES)
		{
			AddArg(_T("submitted"));
			if (GET_SERVERLEVEL() >= 16)
				m_BaseArgs= AddArg(_T("-t"));
		}
		else if(filter == SHELVED_CHANGES)
			AddArg(_T("shelved"));
		else
			ASSERT(0);
	}

	m_pBatch= new CObList;

	// We may or may not have a view spec to limit the fetching of 
	// changes, so sometimes we have an input list and sometimes not
	ASSERT(viewSpec==NULL || viewSpec->IsKindOf(RUNTIME_CLASS(CStringList)));
	if(viewSpec != NULL && viewSpec->GetCount())
	{
		m_pStrListIn= viewSpec;
		m_posStrListIn= m_pStrListIn->GetHeadPosition();
		NextListArgs();
	}

	return CP4Command::Run();
}

void CCmd_Changes::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	// Parse into a CP4Change and send that back
	CP4Change *change= new CP4Change;
	if( change->Create(data) )
       	m_Changes.Add(change);
    else
    {
        #ifdef _DEBUG
            CString errMsg;
            errMsg.Format(_T("Change parse failed:\n%s"), data);
            TheApp()->StatusAdd(errMsg, SV_DEBUG);
        #endif
        delete change;
    }
}

void CCmd_Changes::OnOutputStat( StrDict *varList )
{
	CP4Change *change= new CP4Change;
	if( change->Create(varList) )
       	m_Changes.Add(change);
    else
    {
        #ifdef _DEBUG
            CString errMsg;
            errMsg.Format(_T("Change parse failed (tagged output)"));
            TheApp()->StatusAdd(errMsg, SV_DEBUG);
        #endif
        delete change;
    }
}

void CCmd_Changes::PostProcess()
{
	// First, sort the changes in decending order
	unsigned int size= (unsigned int) m_Changes.GetSize();
    CP4Change *array= (CP4Change *) m_Changes.GetData();
    qsort( (void *) array, size, sizeof( CP4Change * ), compareChanges );

	// Then, dole out unique results 50 at a time, so the UI wont freeze up
	int lastChange=0;
	for( int i=0; i < m_Changes.GetSize(); i++ )
	{
		CP4Change *change= (CP4Change *) m_Changes.GetAt(i);
		if( change->GetChangeNumber() != lastChange || change->GetChangeNumber() == 0 )
		{
			lastChange= change->GetChangeNumber();
			m_pBatch->AddTail( change );
		}
		else
			delete change;

		if(m_pBatch->GetCount() > 49)
	    {
		    // Send a full batch to gui
		    ::PostMessage(m_ReplyWnd, m_ReplyMsg, (WPARAM) m_pBatch, 1);
		    m_pBatch= new CObList;
	    }
	}

	if(m_pBatch->GetCount() > 0)
	{
		// Send a partial batch to gui
		::PostMessage(m_ReplyWnd, m_ReplyMsg, (WPARAM) m_pBatch, 1);
		m_pBatch= NULL;
	}
}


// return <0 if arg1 < arg2, 0 if arg1=arg2, >0 if arg1 > arg2
int compareChanges( const void *arg1, const void *arg2 )
{
    CP4Change *c1= *((CP4Change **) arg1);
    CP4Change *c2= *((CP4Change **) arg2);
    
    // Next compare change number
    return c2->GetChangeNumber() - c1->GetChangeNumber();
}
