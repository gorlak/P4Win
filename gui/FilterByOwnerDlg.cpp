// FilterByOwnerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "P4win.h"
#include "FilterByOwnerDlg.h"
#include "MainFrm.h"


// CFilterByOwnerDlg dialog

IMPLEMENT_DYNAMIC(CFilterByOwnerDlg, CDialog)
CFilterByOwnerDlg::CFilterByOwnerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterByOwnerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilterByOwnerDlg)
	m_Owner = GET_P4REGPTR()->GetLabelFilterByOwner();
	m_IncBlank = GET_P4REGPTR()->GetLabelFilterIncBlank();
	//}}AFX_DATA_INIT
	m_bShowIncBlanks = FALSE;
}

CFilterByOwnerDlg::~CFilterByOwnerDlg()
{
}

void CFilterByOwnerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterByOwnerDlg)
	DDX_Radio(pDX, IDC_RADIO1, m_NotUser);
	DDX_Text(pDX, IDC_OWNER, m_Owner);
	DDV_MaxChars(pDX, m_Owner, 1024);
	DDX_Check(pDX, IDC_INCBLANK, m_IncBlank);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilterByOwnerDlg, CDialog)
	//{{AFX_MSG_MAP(CFilterByOwnerDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowseUsers)
	ON_BN_CLICKED(IDC_RADIO1, OnOwner)
	ON_BN_CLICKED(IDC_RADIO2, OnOwner)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_BROWSECALLBACK2, OnBrowseUsersCallBack)
END_MESSAGE_MAP()


// CFilterByOwnerDlg message handlers
BOOL CFilterByOwnerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (!m_bShowIncBlanks)
	{
		GetDlgItem(IDC_INCBLANK)->EnableWindow(FALSE);
		GetDlgItem(IDC_INCBLANK)->ShowWindow(SW_HIDE);
	}

	if (m_Owner.IsEmpty())
		m_Owner = GET_P4REGPTR()->GetP4User();
	OnOwner();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFilterByOwnerDlg::OnOK() 
{
	UpdateData();
    CDialog::OnOK();
}

void CFilterByOwnerDlg::OnBrowseUsers()
{
	::SendMessage(MainFrame()->UserWnd(), WM_FETCHOBJECTLIST, 
					(WPARAM)(this->m_hWnd), WM_BROWSECALLBACK2);
}

LRESULT CFilterByOwnerDlg::OnBrowseUsersCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_Owner = *str;
	UpdateData(FALSE);
	return 0;
}

void CFilterByOwnerDlg::OnOwner()
{
	UpdateData( );
	BOOL b = m_NotUser;
	GetDlgItem(IDC_OWNER)->EnableWindow(b);
	GetDlgItem(IDC_BROWSE)->EnableWindow(b);
	if (b)
		GotoDlgCtrl(GetDlgItem(IDC_OWNER));
}
