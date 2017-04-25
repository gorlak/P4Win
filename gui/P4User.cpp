//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4User.cpp

#include "stdafx.h"
#include "p4user.h"
#include "tokenstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CP4User, CObject)


CP4User::CP4User()
{
	m_Initialized=FALSE;
}

CP4User::~CP4User()
{
	
}

// Eat a row of text from 'P4 users' that looks like:
// NIRIAS <NIRIAS@ELWOOD.com> (Nick Irias) accessed 03/15/1997
// " <@> () accessed 04/28/1997"  <--- a bad row that we shouldnt choke on

BOOL CP4User::Create(LPCTSTR userRow)
{
	CTokenString str;
	str.Create(userRow);

	m_UserName=str.GetToken();  // Any username is valid, except <@>, which indicates a blank username
	if(m_UserName.GetLength() == 0 || (m_UserName[0]==_T('<') && m_UserName.Right(1)==_T('>')))
		return FALSE;

	m_Email=str.GetToken(FALSE, TRUE);  // Always just one word delimited by < >
	if(m_Email[0]!=_T('<'))				// Always starts w/ '<'
		return FALSE;

	m_Email= m_Email.Mid(1, m_Email.GetLength()-2);  // lose the < > brackets

	m_FullName=str.GetToken();
	while(m_FullName.GetLength() < lstrlen(userRow))
	{
		CString nextword = str.GetToken();
		if ((nextword == _T("accessed")) && (m_FullName.Right(1) == _T(')')))
			break;	// the word "accessed" means we have passed the end of the fullname
		m_FullName += _T(" ");
		m_FullName += nextword;
	}
	if(m_FullName[0] != _T('(') || m_FullName.Right(1) != _T(')') || m_FullName.GetLength() < 3)
		return FALSE;

	m_FullName=m_FullName.Mid(1, m_FullName.GetLength()-2);  // lose the ( ) brackets
	
	m_LastAccessDate=str.GetToken();
	if(m_LastAccessDate.Find(_T("/")) == -1)
		{ ASSERT(0); return FALSE; }

	m_Initialized=TRUE;
	return TRUE;
}


// An alternate create, to make a CP4User from the spec dialog, for use in a single row
// update to the userview
void CP4User::Create(LPCTSTR name, LPCTSTR email, LPCTSTR fullname, LPCTSTR accessdate)
{

	m_UserName= name;
	m_Email= email;
	m_FullName= fullname;
	m_LastAccessDate= accessdate;
	
	m_Initialized=TRUE;
}