// Diff2ObjsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "Diff2ObjsDlg.h"
#include "MainFrm.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDiff2ObjsDlg dialog


CDiff2ObjsDlg::CDiff2ObjsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDiff2ObjsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiff2ObjsDlg)
	m_Edit1 = _T("");
	m_Edit2 = _T("");
	//}}AFX_DATA_INIT

	m_Type = -1;
}


void CDiff2ObjsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiff2ObjsDlg)
	DDX_Text(pDX, IDC_EDIT1, m_Edit1);
	DDX_Text(pDX, IDC_EDIT2, m_Edit2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDiff2ObjsDlg, CDialog)
	//{{AFX_MSG_MAP(CDiff2ObjsDlg)
//	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_F_BROWSE, On1Browse)
	ON_BN_CLICKED(IDC_T_BROWSE, On2Browse)
//	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_BROWSECALLBACK1, On1BrowseCallBack)
	ON_MESSAGE(WM_BROWSECALLBACK2, On2BrowseCallBack)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiff2ObjsDlg message handlers

BOOL CDiff2ObjsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString caption;
	switch(m_Type)
	{
	case COMBO_CHGNBR:
		GetDlgItem(IDC_F_BROWSE)->EnableWindow( FALSE );
		GetDlgItem(IDC_T_BROWSE)->EnableWindow( FALSE );
		caption = LoadStringResource(IDS_DIFF2CHGLISTS);
		break;
	case COMBO_CLIENT:
		caption = LoadStringResource(IDS_DIFF2CLIENTS);
		break;
	case COMBO_LABEL:
		caption = LoadStringResource(IDS_DIFF2LABELS);
		break;
	default:
		ASSERT(0);
		GetWindowText(caption);
		break;
	}

	SetWindowText(caption);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDiff2ObjsDlg::On1Browse()
{
	HWND hWnd;
	switch(m_Type)
	{
	case COMBO_CLIENT:
		hWnd = MainFrame()->ClientWnd();
		break;
	case COMBO_LABEL:
		hWnd = MainFrame()->LabelWnd();
		break;
	default:
		ASSERT(0);
		return;
	}
	::SendMessage(hWnd, WM_FETCHOBJECTLIST, (WPARAM)(this->m_hWnd), WM_BROWSECALLBACK1);
}

LRESULT CDiff2ObjsDlg::On1BrowseCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_Edit1 = *str;
	UpdateData(FALSE);
	return 0;
}

void CDiff2ObjsDlg::On2Browse()
{
	HWND hWnd;
	switch(m_Type)
	{
	case COMBO_CLIENT:
		hWnd = MainFrame()->ClientWnd();
		break;
	case COMBO_LABEL:
		hWnd = MainFrame()->LabelWnd();
		break;
	default:
		ASSERT(0);
		return;
	}
	::SendMessage(hWnd, WM_FETCHOBJECTLIST, (WPARAM)(this->m_hWnd), WM_BROWSECALLBACK2);
}

LRESULT CDiff2ObjsDlg::On2BrowseCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_Edit2 = *str;
	UpdateData(FALSE);
	return 0;
}

//void CDiff2ObjsDlg::OnHelp() 
//{
//	AfxGetApp()->WinHelp(ALIAS_99_COMPARING_ANY_2_FILES);
//}

//BOOL CDiff2ObjsDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
//{
//	OnHelp();
//	return TRUE;
//}

void CDiff2ObjsDlg::OnOK() 
{
	UpdateData();
	if (m_Edit1.IsEmpty() || m_Edit2.IsEmpty())
	{
		MessageBeep(0);
		GotoDlgCtrl(m_Edit1.IsEmpty() ? GetDlgItem(IDC_EDIT1) : GetDlgItem(IDC_EDIT2));
		return;
	}
	UpdateData(FALSE);
	CDialog::OnOK();
}
