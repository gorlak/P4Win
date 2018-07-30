// ClientWizSheet.cpp : implementation file
//

#include "stdafx.h"
#include "P4win.h"
#include "ClientWizSheet.h"
#include "hlp\p4win.hh"


// CClientWizSheet

IMPLEMENT_DYNAMIC(CClientWizSheet, CPropertySheet)
CClientWizSheet::CClientWizSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CClientWizSheet::CClientWizSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage( &m_ClientWizBegin );
	AddPage( &m_ClientWizCreate );
	SetWizardMode();
	m_AllowBrowse = TRUE;
}

CClientWizSheet::~CClientWizSheet()
{
}


BEGIN_MESSAGE_MAP(CClientWizSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CClientWizSheet)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CClientWizSheet message handlers
BOOL CClientWizSheet::OnInitDialog() 
{
	// These MUST go before the call to CPropertySheet::OnInitDialog()
	m_Root = m_Port + _T('_') + m_Name;
	m_Root.Remove(_T(':'));
	m_Root = _T("C:\\p4_") + m_Root;
	m_Root.Remove(_T(' '));
	m_ClientWizBegin.SetMsg(m_Message);
	m_ClientWizBegin.SetAllowBrowse(m_AllowBrowse);
	m_ClientWizBegin.m_Bitmap.LoadBitmap(MAKEINTRESOURCE(IDB_GETADOBE));
	m_ClientWizCreate.SetName(m_Name);
	m_ClientWizCreate.SetRoot(m_Root);

	BOOL bResult = CPropertySheet::OnInitDialog();

	return bResult;
}


///////////////////////////////////////
// Handler for help button

void CClientWizSheet::OnHelp() 
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
	case 0: helpID= INTRO_CLIENT_WORKSPACE; break;
	case 1: helpID= INTRO_CLIENT_WORKSPACE; break;
	default:
		ASSERT(0);
	}

	if(helpID)
		AfxGetApp()->WinHelp(helpID);
	else
		CPropertySheet::OnHelp();
}

BOOL CClientWizSheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// Help for this property sheet is only specific down
	// to the property page level
	OnHelp();
	return TRUE;
}
