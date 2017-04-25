//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4Branch.cpp

#include "stdafx.h"
#include "p4win.h"
#include "p4branch.h"
#include "tokenstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CP4Branch, CObject)


CP4Branch::CP4Branch()
{
	m_Initialized=FALSE;
 	m_Owner   = _T("");
  	m_Options = _T("");
}

CP4Branch::~CP4Branch()
{
	
}

// Eat a row of text from 'P4 branchs' that looks like:
//
// Branch b1223 02/25/1996 'Created by seiwald. '
// Branch b1525 04/29/1996 'Branch view for //depot/r1525/... '
// Branch b2020 12/14/1996 'Branch view for //depot/r2020/... '
// Branch b827 01/03/1996 'Source + doc branch for release 827. Label s810 is'
// Branch fm 02/28/1997 'Created by seiwald. '
// Branch mac-dev 10/12/1996 'Port of mainline code for Mac development, as s'

BOOL CP4Branch::Create(LPCTSTR branchRow)
{
	ASSERT(_tcsncmp(branchRow, _T("Branch "), 7) ==0);
	CTokenString str;
	str.Create(branchRow+7);

	m_BranchName=str.GetToken();  // Any branchname is valid
	m_Date=str.GetToken();
	if(m_Date.Find(_T("/")) == -1)
		{ ASSERT(0); return FALSE; }

	m_Description=str.GetToken();
	if(m_Description[0]==_T('\''))
		m_Description=m_Description.Mid(1);   // skip the leading quote
	
	m_Description+= str.GetRemainder();
	
	// Trim off the right "'"
	int right=m_Description.ReverseFind(_T('\''));
	if(right != -1)
	m_Description= m_Description.Left(right-1);

	m_Initialized=TRUE;
	return TRUE;
}


// An alternate create, to make a CP4Branch from the spec dialog, for use in a single row
// update to the jobview
void CP4Branch::Create(LPCTSTR name, LPCTSTR owner, LPCTSTR options, LPCTSTR date, LPCTSTR desc)
{
	m_BranchName= name;
	m_Owner = owner;
	m_Options = options;
	m_Date= date;
	m_Description= desc;
		
	m_Initialized=TRUE;
}

// Yet another create, to do the task from TAGged output
BOOL CP4Branch::Create(StrDict *varlist)
{
 	StrPtr *str;
 	str= varlist->GetVar( "branch" );
 	m_BranchName = CharToCString(!str ? "" : str->Value());
 
 	str= varlist->GetVar( "Owner" );
 	m_Owner = CharToCString(!str ? "" : str->Value());
 
 	str= varlist->GetVar( "Options" );
 	m_Options = CharToCString(!str ? "" : str->Value());
 
 	m_Date.Empty();
 	str= varlist->GetVar( "Update" );
 	if (str)
 	{
 		char buf[64];
 		DateTime datetime;
 		Error e;
 		datetime.Set(str->Value(), &e);
 		if( !e.Test() )
 		{
 			datetime.Fmt(buf);
 			m_Date = buf;
 		}
 	}
 
 	str= varlist->GetVar( "Description" );
 	if (!str)
 		m_Description.Empty();
 	else
 	{
 		m_Description = CharToCString(str->Value());
 		m_Description.TrimRight();
 	}
 
 	m_Initialized=TRUE;
 	return TRUE;
}
