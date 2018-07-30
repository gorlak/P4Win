// ClientWizBegin.cpp : implementation file
//

#include "stdafx.h"
#include "P4win.h"
#include "MainFrm.h"
#include "ClientWizBegin.h"


// CClientWizBegin dialog

IMPLEMENT_DYNAMIC(CClientWizBegin, CPropertyPage)
CClientWizBegin::CClientWizBegin()
	: CPropertyPage(CClientWizBegin::IDD)
{
	m_Radio = 0;
}

CClientWizBegin::~CClientWizBegin()
{
}

void CClientWizBegin::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClientWizBegin)
	DDX_Text(pDX, IDC_EDIT1, m_Message);
	DDX_Radio(pDX, IDC_RADIO1, m_Radio);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClientWizBegin, CPropertyPage)
	//{{AFX_MSG_MAP(CClientWizBegin)
	ON_BN_CLICKED(IDC_VIEW_GETSTART, OnViewGettingStarted)
	ON_BN_CLICKED(IDC_GETADOBE, OnGetAdobe)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CClientWizBegin message handlers

BOOL CClientWizBegin::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// Load the Get Abode bitmap
	GetDlgItem(IDC_GETADOBE)->SendMessage(BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)HBITMAP(m_Bitmap));

	if (!m_AllowBrowse)
	{
		GetDlgItem(IDC_RADIO1)->EnableWindow(FALSE);
		m_Radio = 1;
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CClientWizBegin::OnSetActive() 
{
	EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
	CPropertySheet *sheet= (CPropertySheet *) GetParent();
	sheet->SetWizardButtons( PSWIZB_NEXT );
	return CPropertyPage::OnSetActive();
}

LRESULT CClientWizBegin::OnWizardNext() 
{
	UpdateData();
	if (!m_Radio && m_AllowBrowse)
	{
		EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, FALSE);
		((CPropertySheet *)GetParent())->EndDialog(IDIGNORE);
		return -1;
	}
	return CPropertyPage::OnWizardNext();
}

void CClientWizBegin::OnViewGettingStarted()
{
	MainFrame()->OnGettingStartedWithP4win();
	UpdateData();
	GotoDlgCtrl(GetDlgItem(m_Radio ? IDC_RADIO2 : IDC_RADIO1));
}

void CClientWizBegin::OnGetAdobe()
{
	UpdateData();
	GotoDlgCtrl(GetDlgItem(m_Radio ? IDC_RADIO2 : IDC_RADIO1));
	ShellExecute( m_hWnd, _T("open"), LoadStringResource(IDS_WWW_ADOBE), NULL, NULL, SW_SHOWNORMAL);
	TheApp()->StatusAdd(LoadStringResource(IDS_WAIT4ADOBE), SV_MSG);
}
