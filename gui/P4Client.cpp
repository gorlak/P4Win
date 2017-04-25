//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4Client.cpp

#include "stdafx.h"
#include "p4win.h"
#include "p4client.h"
#include "tokenstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CP4Client, CObject)


CP4Client::CP4Client()
{
	m_Initialized=FALSE;
	m_Owner = _T("");
 	m_Host  = _T("");
}

CP4Client::~CP4Client()
{
	
}

// Eat a row of text from 'P4 clients' that looks like:
// Client Administrator 02/03/1996 root d:\users\default 'Created by Administ'
// Client ELWOOD 02/14/1997 root d:\ 'Created by NIRIAS. '
// Client Ishiisan 03/12/1996 root /home/seiwald 'Created by seiwald. '
// Client alley 05/29/1996 root e:\users\default 'Created by seiwald. '
// Client axposf 02/06/1997 root /usr/team/seiwald/work/p4 'Created by seiwal'

BOOL CP4Client::Create(LPCTSTR clientRow)
{
	ASSERT(_tcsncmp(clientRow, _T("Client "), 7) ==0);
	CTokenString str;
	str.Create(clientRow+7);

	m_ClientName=str.GetToken();  // Any clientname is valid
	m_Date=str.GetToken();
	if(m_Date.Find(_T("/")) == -1)
		{ ASSERT(0); return FALSE; }

	m_Root=str.GetToken();
	if(m_Root.Compare(_T("root")) != 0)
		{ ASSERT(0); return FALSE; }
	m_Root=str.GetToken();

	m_Description=_T("");
	while(m_Description.GetLength()==0)
	{
		m_Description=str.GetToken();
		if(m_Description.GetLength() && m_Description[0]!=_T('\''))
		{
			m_Root+=(_T(" ")+m_Description);
			m_Description=_T("");
		}
	}

	if(m_Description.GetLength() && m_Description[0]==_T('\''))
		m_Description=m_Description.Mid(1);   // skip the leading quote

	m_Description+= str.GetRemainder();
	
	// Trim off the right "'"
	int right=m_Description.ReverseFind(_T('\''));
	if(right != -1)
	m_Description= m_Description.Left(right-1);

	m_Initialized=TRUE;
	return TRUE;
}


// An alternate create, to make a CP4Client from the spec dialog, for use in a single row
// update to the jobview
void CP4Client::Create(LPCTSTR name, LPCTSTR owner, LPCTSTR host, LPCTSTR date, LPCTSTR root, LPCTSTR desc)
{
	m_ClientName= name;
	m_Owner= owner;
	m_Host= host;
	m_Date= date;
	m_Root= root;
	m_Description= desc;
		
	m_Initialized=TRUE;
}

// Yet another create, to do the task from TAGged output
BOOL CP4Client::Create(StrDict *varlist)
{
	StrPtr *str;
	str= varlist->GetVar( "client" );
	m_ClientName = CharToCString(!str ? "" : str->Value());

	str= varlist->GetVar( "Owner" );
	m_Owner = CharToCString(!str ? "" : str->Value());

	str= varlist->GetVar( "Host" );
	m_Host = CharToCString(!str ? "" : str->Value());

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

	str= varlist->GetVar( "Root" );
	m_Root = CharToCString(!str ? "" : str->Value());

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
