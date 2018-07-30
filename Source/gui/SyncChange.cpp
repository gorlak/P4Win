// SyncChange.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "SyncChange.h"
#include "cmd_get.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UPDATE_STATUS(x) ((CMainFrame *)AfxGetMainWnd())->UpdateStatus(x)

/////////////////////////////////////////////////////////////////////////////
// CSyncChange dialog


CSyncChange::CSyncChange(CWnd* pParent /*=NULL*/)
	: CDialog(CSyncChange::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSyncChange)
	m_SyncDef  = GET_P4REGPTR()->GetChglistSyncDef();
	m_SyncType = m_SyncDef;
	m_OldChgWnd = pParent->m_hWnd;
	//}}AFX_DATA_INIT
}


void CSyncChange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSyncChange)
	DDX_Radio(pDX, IDC_SYNC_ALL, m_SyncType);
	DDX_Radio(pDX, IDC_DEFAULT1, m_SyncDef);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSyncChange, CDialog)
	//{{AFX_MSG_MAP(CSyncChange)
	ON_BN_CLICKED(ID_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDOK, OnSync)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyncChange message handlers

BOOL CSyncChange::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateData(FALSE);

	ShowWindow(SW_SHOW);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSyncChange::OnPreview() 
{
	UpdateData(TRUE);
	::PostMessage(m_OldChgWnd, WM_DOCUSTOMGET, (WPARAM)IDGETPREVIEW, (LPARAM)m_SyncType);
}

void CSyncChange::OnSync() 
{
	UpdateData(TRUE);
	GET_P4REGPTR()->SetChglistSyncDef(m_SyncDef);
	::PostMessage(m_OldChgWnd, WM_DOCUSTOMGET, (WPARAM)IDGET, (LPARAM)m_SyncType);
}

void CSyncChange::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_COPYING_FILES_TO_THE_CLIENT_WORKSPACE);
}

BOOL CSyncChange::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void CSyncChange::OnCancel() 
{
	::PostMessage(m_OldChgWnd, WM_DOCUSTOMGET, (WPARAM)IDCANCEL, (LPARAM)0);
}
