//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Labels.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_labels.h"
#include "p4label.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Labels, CP4Command)

int compareLabels( const void *arg1, const void *arg2 );

CCmd_Labels::CCmd_Labels(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4LABELS;
	m_TaskName= _T("Labels");
	m_pList=NULL;
}

CCmd_Labels::~CCmd_Labels()
{
	if(m_pList != NULL)
		delete m_pList;
}

BOOL CCmd_Labels::Run(CStringList *viewSpec)
{
	ClearArgs();
	m_BaseArgs= AddArg(_T("labels"));

	m_UsedTagged = GET_SERVERLEVEL() >= 8 ? TRUE : FALSE;

	m_pList= new CObList;

	if (GET_SERVERLEVEL() >= 11)
	{
		// We may or may not have a view spec to limit the fetching of 
		// labels, so sometimes we have an input list and sometimes not
		ASSERT(viewSpec==NULL || viewSpec->IsKindOf(RUNTIME_CLASS(CStringList)));
		if(viewSpec != NULL && viewSpec->GetCount())
		{
			m_pStrListIn= viewSpec;
			m_posStrListIn= m_pStrListIn->GetHeadPosition();
			NextListArgs();
		}
	}
	return CP4Command::Run();
}

void CCmd_Labels::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	// Parse into a CP4Label and send that back
	CP4Label *label= new CP4Label;
	if( label->Create(data) )
	    m_Labels.Add(label);
    else
    {
        #ifdef _DEBUG
            CString errMsg;
            errMsg.Format(_T("Label parse failed:\n%s"), data);
            TheApp()->StatusAdd(errMsg, SV_DEBUG);
        #endif
        delete label;
    }
}

void CCmd_Labels::OnOutputStat( StrDict *varList )
{
	// Check for possible abort request
	if(APP_ABORTING())
	{
		ReleaseServerLock();
		ExitThread(0);
	}
	else
	{
        CP4Label *label= new CP4Label;
		if(label->Create(varList))
			m_Labels.Add(label);
		else
			delete label;
	}
}

void CCmd_Labels::PostProcess()
{
	// First, sort the labels in ascending order
	unsigned int size= (unsigned int) m_Labels.GetSize();
    CP4Label *array= (CP4Label *) m_Labels.GetData();
    qsort( (void *) array, size, sizeof( CP4Label * ), compareLabels );

	// Then, dole out unique results
	CString lastLabel="";
	for( int i=0; i < m_Labels.GetSize(); i++ )
	{
		CP4Label *label= (CP4Label *) m_Labels.GetAt(i);
		if( label->GetLabelName() != lastLabel )
		{
			lastLabel= label->GetLabelName();
			m_pList->AddTail( label );
		}
		else
			delete label;
	}
}

// return <0 if arg1 < arg2, 0 if arg1=arg2, >0 if arg1 > arg2
int compareLabels( const void *arg1, const void *arg2 )
{
    CP4Label *c1= *((CP4Label **) arg1);
    CP4Label *c2= *((CP4Label **) arg2);
    
    // Next compare label names
    CString str1 = c1->GetLabelName();
	CString str2 = c2->GetLabelName();
	return  str1.Compare(str2);
}

