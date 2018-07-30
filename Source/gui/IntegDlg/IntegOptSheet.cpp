/*
 * Copyright 1997, 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */



// IntegOptSheet.cpp : implementation file
//

#include "stdafx.h"
#include "..\p4win.h"
#include "IntegOptSheet.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIntegOptSheet

IMPLEMENT_DYNAMIC(CIntegOptSheet, CPropertySheet)

CIntegOptSheet::CIntegOptSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CIntegOptSheet::CIntegOptSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage( &m_OptsPage );
	AddPage( &m_RevRngPage );
}

CIntegOptSheet::~CIntegOptSheet()
{
}


BEGIN_MESSAGE_MAP(CIntegOptSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CIntegOptSheet)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIntegOptSheet message handlers


BOOL CIntegOptSheet::OnInitDialog() 
{
	CPropertySheet::OnInitDialog();
	
	// Hide the apply now button
	GetDlgItem(ID_APPLY_NOW)->ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


///////////////////////////////////////
// Handler for help button

void CIntegOptSheet::OnHelp() 
{
	DWORD helpID=0;

	
	// TODO: might want to use the p4win.hm file for topic IDs in the help
	// file, and then call SetHelpID for each property page.  Then see what 
	// happens when OnHelp() isnt handled here..

	// Currently, the app's WinHelp function always sets HELP_FINDER mode
	// So long as items with no help do not have help IDs, it will be 
	// possible to have the app help function still run HELP_FINDER when
	// the help ID is not provided.

	switch(GetActiveIndex())
	{
	case 0: helpID= TASK_SPECIFYING_FILE_INTEGRATION_OPTIONS; break;
	case 1: helpID= ALIAS_54_INTEG_OPTIONS; break;
	default:
		ASSERT(0);
	}

	if(helpID)
		AfxGetApp()->WinHelp(helpID);
	else
		CPropertySheet::OnHelp();
}

BOOL CIntegOptSheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// Help for this property sheet is only specific down
	// to the property page level
	OnHelp();
	return TRUE;
}
