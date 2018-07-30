// FilterDepotDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "FilterDepotDlg.h"
#include "MainFrm.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FilterDepotDlg dialog


FilterDepotDlg::FilterDepotDlg(CWnd* pParent /*=NULL*/)
	: CDialog(FilterDepotDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(FilterDepotDlg)
	m_FilterType = -1;
	m_FileList = _T("");
	//}}AFX_DATA_INIT
}


void FilterDepotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FilterDepotDlg)
	DDX_Radio(pDX, IDC_ALL_OPENED, m_FilterType);
	DDX_Text(pDX, IDC_FILELIST, m_FileList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FilterDepotDlg, CDialog)
	//{{AFX_MSG_MAP(FilterDepotDlg)
	ON_BN_CLICKED(IDC_DEPOT_LIST, OnDepotList)
	ON_BN_CLICKED(IDC_ALL_OPENED, OnAllOpened)
	ON_BN_CLICKED(IDC_MY_OPENED, OnMyOpened)
	ON_BN_CLICKED(ID_CLEARFILTER, OnClearfilter)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FilterDepotDlg message handlers

BOOL FilterDepotDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_FilterType == DFT_LIST)
		OnDepotList();
	
	UpdateData(FALSE);
	
	return (m_FilterType == DFT_LIST) ? FALSE : TRUE;  // return TRUE unless you set the focus to a control
}

void FilterDepotDlg::OnDepotList() 
{
	GetDlgItem(IDC_FILELIST)->EnableWindow( TRUE );
	GotoDlgCtrl(GetDlgItem(IDC_FILELIST));
}

void FilterDepotDlg::OnAllOpened() 
{
	GetDlgItem(IDC_FILELIST)->EnableWindow( FALSE );
}

void FilterDepotDlg::OnMyOpened() 
{
	GetDlgItem(IDC_FILELIST)->EnableWindow( FALSE );
}

void FilterDepotDlg::OnClearfilter() 
{
	::SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, WM_COMMAND, ID_VIEW_CLEARFILTER, 0);
	OnCancel();
}

void FilterDepotDlg::OnHelp() 
{
	AfxGetApp()->WinHelp(ALIAS_84_FILTER_DEPOT_FILES);
}

BOOL FilterDepotDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
