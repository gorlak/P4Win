//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4Lists.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "P4Lists.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CHistory    A collection of CRevisions, to contain all results from "P4 filelog -l"

IMPLEMENT_DYNCREATE(CRevision, CObject)

CRevision::CRevision()
{
	m_RevisionNum = m_ChangeNum = m_FNbr = 0;
}


CRevision::~CRevision()
{
	
}


BOOL CRevision::Create(LPCTSTR fname, int fnbr, LPCTSTR text, LPCTSTR branchInfo)
{
	CString buf(text);

	// Parsing: example serverOutput follows
	//
	//... #13 change 36 edit on 01/10/1997 by NIRIAS@ELWOOD
	//
    //    Added view function, but have not yet added capability to track and delete
    //    temp files after viewing	

	// Find the revision and change numbers
	int pound=buf.Find(_T("#"));
	int change=buf.Find(_T("change"));
	if(pound < 0 || change < 0 || change < pound)
		return FALSE; 
	
	change+=6;   // skip over "change "
	// Find the action word ('edit', 'add',...)
	int action;
	for(action=change; action < buf.GetLength(); action++)
	{
		if(!_istdigit(buf[action]) && buf[action] != _T(' ') && buf[action] != _T('\t'))
			break;  
	}
	if(action == buf.GetLength())
		return FALSE;

	// Find the date
	int date;
	for(date = action; date < buf.GetLength(); date++)
	{
		if(_istdigit(buf[date]))
			break;  
	}
	if(date == buf.GetLength())
		return FALSE;

	// store what was found, then cut off all chars up to date
	m_FName = fname;
	m_FNbr  = fnbr;
	m_RevisionNum = _ttoi(buf.Mid(pound+1,10));
	m_ChangeNum  = _ttoi(buf.Mid(change,10));
	m_ChangeType = buf.Mid(action, date-1-action);
	m_ChangeType = m_ChangeType.Left(m_ChangeType.FindOneOf(_T(" \t")));

	// Find the user
	int user = buf.Find(_T(" by "));
	if (user == -1)
		user = buf.GetLength();

	// The Date is between 'date' and 'user'
	m_Date = buf.Mid(date, user - date);

	// skip over " by " and trim off date
	user += sizeof(_T(" by "))/sizeof(TCHAR)-1;
	if (user > buf.GetLength())
		user = buf.GetLength();
	m_User = buf = buf.Mid(user);
	
	int whtsp;
	if ((whtsp = m_User.FindOneOf(_T(" \t\r\n"))) >= 0)
		m_User = m_User.Left(whtsp);

	// Find the description
	int	desc=m_User.GetLength();
	for( ; desc < buf.GetLength(); desc++)
	{
		if(buf[desc] != _T(' ') && buf[desc] != _T('\t') && buf[desc] != _T('\n') && buf[desc] != _T('\r'))
			break;  
	}

	// User might have entered a blank description
	if(desc == buf.GetLength())
		m_ChangeDescription=LoadStringResource(IDS_NO_DESCRIPTION);
	else
		m_ChangeDescription=RemoveTabs(buf.Mid(desc));

	if( lstrlen(branchInfo) > 0 )
	{
		m_ChangeType += (m_ChangeType == _T("integrate") || m_ChangeType == _T("branch")) ? _T(": ") : _T("; ");
		m_ChangeType += branchInfo;
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CHistory    A collection of CRevisions, to contain all results from "P4 filelog -l"

CHistory::CHistory()
{
	m_pList= new CObList; 
}

CHistory::~CHistory()
{
	Clear();
	delete m_pList;
}

void CHistory::Clear()
{
	while(!m_pList->IsEmpty())
	{
		if(m_pList->GetHead()->IsKindOf(RUNTIME_CLASS(CRevision)))
			delete m_pList->GetHead();
		m_pList->RemoveHead();
	}
	m_FileName=_T("");
	m_pos=NULL;
}

BOOL CHistory::AddRevision(LPCTSTR fname, int fnbr, LPCTSTR desc, LPCTSTR branchInfo)
{
	ASSERT( _tcsncmp(fname, _T("//"), 2)==0 );
	
	if( m_FileName.IsEmpty() )
		m_FileName=fname;
	
	CRevision *rev=new CRevision;
	if(rev->Create(fname, fnbr, desc, branchInfo))
	{
		m_pList->AddHead(rev);
		return TRUE;
	}
	else
	{
		delete rev;
		return FALSE;
	}
}



CRevision *CHistory::GetLatestRevision()
{
	ASSERT(!m_pList->IsEmpty());

	m_pos=m_pList->GetHeadPosition();
	// This looks wrong, but m_pos++ happens AFTER the get
	return (CRevision *) m_pList->GetNext(m_pos);
}


CRevision *CHistory::GetNextRevision()
{
	ASSERT(!m_pList->IsEmpty());
	ASSERT(m_pos != NULL);

	return (CRevision *) m_pList->GetNext(m_pos);
}

	
CRevision *CHistory::GetHeadRevision()
{
	ASSERT(!m_pList->IsEmpty());

	m_pos=m_pList->GetTailPosition();
	// This looks wrong, but m_pos++ happens AFTER the get
	return (CRevision *) m_pList->GetPrev(m_pos);
}


CRevision *CHistory::GetPrevRevision()
{
	if (m_pList->IsEmpty() || m_pos == NULL)
		return NULL;

	return (CRevision *) m_pList->GetPrev(m_pos);
}

	
INT_PTR CHistory::GetRevisionCount()
{
	return m_pList->GetCount();
}


CString CHistory::GetFileName()
{
	return m_FileName;
}

