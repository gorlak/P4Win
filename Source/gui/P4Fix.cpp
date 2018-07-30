//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4Fix.cpp

#include "stdafx.h"
#include "P4Fix.h"
#include "tokenstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CP4Fix, CObject)


CP4Fix::CP4Fix()
{
	m_Initialized=FALSE;
}

CP4Fix::~CP4Fix()
{
	
}

	
// Eat a row of text from 'P4 fixes' that looks like:
// job000043 fixed by change 3010 on 1997/04/25 by seiwald@lemon
// job000104 fixed by change 3006 on 1997/05/02 by seiwald@lemon
// job000256 fixed by change 3033 on 1997/04/28 by seiwald@lemon
// job000305 fixed by change 3007 on 1997/04/24 by seiwald@lemon
// job000307 fixed by change 3006 on 1997/04/24 by seiwald@lemon
// job000315 fixed by change 3044 on 1997/04/30 by seiwald@lemon

BOOL CP4Fix::Create(LPCTSTR fixesRow)
{
	CTokenString str;
	str.Create(fixesRow);

	m_JobName=str.GetToken();  // Any jobname is valid

	VERIFY(_tcscmp(str.GetToken(), _T("fixed"))==0);   // Skip over word, 'fixed'
	VERIFY(_tcscmp(str.GetToken(), _T("by"))==0);   // Skip over word, 'by'
	VERIFY(_tcscmp(str.GetToken(), _T("change"))==0);   // Skip over word, 'change'

	CString chg= str.GetToken();
	m_ChangeNum= _ttol(chg);
	ASSERT(m_ChangeNum);

	VERIFY(_tcscmp(str.GetToken(), _T("on"))==0);   // Skip over word, 'on'

	m_FixDate=str.GetToken();
	if(m_FixDate.Find(_T("/")) == -1)
		{ ASSERT(0); return FALSE; }

	VERIFY(_tcscmp(str.GetToken(), _T("by"))==0);   // Skip over word, 'by'
	m_User=str.GetToken();     // Any username is valid
	
	m_Initialized=TRUE;
	return TRUE;
}

// Eat a row of text from 'P4 fix' that looks like:
// testjob fixed by change 3255.
BOOL CP4Fix::CreateMyFix(LPCTSTR fixesRow)
{
	CTokenString str;
	str.Create(fixesRow);

	m_JobName=str.GetToken();  // Any jobname is valid

	VERIFY(_tcscmp(str.GetToken(), _T("fixed"))==0);   // Skip over word, 'fixed'
	VERIFY(_tcscmp(str.GetToken(), _T("by"))==0);   // Skip over word, 'by'
	VERIFY(_tcscmp(str.GetToken(), _T("change"))==0);   // Skip over word, 'change'

	CString chg= str.GetToken();
	m_ChangeNum= _ttol(chg);
	ASSERT(m_ChangeNum);

	m_Initialized=TRUE;
	return TRUE;
}

// Eat a row of text from 'P4 fix -d -c change jobname' that looks like:
// Deleted fix testjob by change 3255.
BOOL CP4Fix::CreateFromUnfix(LPCTSTR unfixRow)
{
	CTokenString str;
	str.Create(unfixRow);

	VERIFY(_tcscmp(str.GetToken(), _T("Deleted"))==0);   // Skip over word, 'Deleted'
	VERIFY(_tcscmp(str.GetToken(), _T("fix"))==0);   // Skip over word, 'fix'

	m_JobName=str.GetToken();  // Any jobname is valid
	
	VERIFY(_tcscmp(str.GetToken(), _T("by"))==0);   // Skip over word, 'by'
	VERIFY(_tcscmp(str.GetToken(), _T("change"))==0);   // Skip over word, 'change'

	CString chg= str.GetToken();
	m_ChangeNum= _ttol(chg);
	ASSERT(m_ChangeNum);
	
	m_Initialized=TRUE;
	return TRUE;
}


