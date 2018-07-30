//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4Label.cpp

#include "stdafx.h"
#include "p4win.h"
#include "p4label.h"
#include "tokenstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CP4Label, CObject)


CP4Label::CP4Label()
{
	m_Initialized=FALSE;
	m_Owner   = _T("");
 	m_Options = _T("");
}

CP4Label::~CP4Label()
{
	
}

// Eat a row of text from 'P4 labels' that looks like:
//
// Label s1223a 02/07/1996 'Source for release #1223a. '
// Label s1223b 02/12/1996 'Souce for release #1223b. '
// Label s810 01/02/1996 'Source for datatools #810.  Includes 1-810, 813-816'
// Label s827a 01/03/1996 'Source for datatools #827a from branch r827.  Incl'
// Label shebanow-copy 09/05/1996 'Client code given to Mark Shebanow for Mac'
// Label special-rns-1033 01/09/1996 'Special release for RNS @1033. '

BOOL CP4Label::Create(LPCTSTR labelRow)
{
	ASSERT(_tcsncmp(labelRow, _T("Label "), 6) ==0);
	CTokenString str;
	str.Create(labelRow+6);

	m_LabelName=str.GetToken();  // Any labelname is valid
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


// An alternate create, to make a CP4Label from the spec dialog, for use in a single row
// update to the jobview
void CP4Label::Create(LPCTSTR name, LPCTSTR owner, LPCTSTR options, LPCTSTR date, LPCTSTR desc)
{
	m_LabelName= name;
	m_Owner = owner;
	m_Options = options;
	m_Date= date;
	m_Description= desc;
		
	m_Initialized=TRUE;
}

// Yet another create, to do the task from TAGged output
BOOL CP4Label::Create(StrDict *varlist)
{
	StrPtr *str;
	str= varlist->GetVar( "label" );
	m_LabelName = CharToCString(!str ? "" : str->Value());

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
		//TODO: what happens if a date has non-ascii characters in it?
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
