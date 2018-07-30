#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "ClientFilterDlg.h"
#include "cmd_password.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientFilterDlg dialog


CClientFilterDlg::CClientFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientFilterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClientFilterDlg)
	m_NotUser = 0;
	m_NotMyHost = 0;
	m_Owner = _T("");
	m_Host  = _T("");
	m_DescContains = _T("");
	//}}AFX_DATA_INIT

	m_Flags = 0;
}


void CClientFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClientFilterDlg)
	DDX_Control(pDX, IDC_CHECK1, m_IsOwner);
	DDX_Control(pDX, IDC_CHECK2, m_IsHost);
	DDX_Control(pDX, IDC_CHECK3, m_IsDesc);
	DDX_Radio(pDX, IDC_RADIO1, m_NotUser);
	DDX_Radio(pDX, IDC_RADIO3, m_NotMyHost);
	DDX_Text(pDX, IDC_OWNER, m_Owner);
	DDX_Text(pDX, IDC_HOST, m_Host);
	DDX_Text(pDX, IDC_DESCCONTAINS, m_DescContains);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClientFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CClientFilterDlg)
	ON_BN_CLICKED(IDC_CHECK1, OnOwner)
	ON_BN_CLICKED(IDC_RADIO1, OnOwner)
	ON_BN_CLICKED(IDC_RADIO2, OnOwner)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_CHECK2, OnHost)
	ON_BN_CLICKED(IDC_RADIO3, OnHost)
	ON_BN_CLICKED(IDC_RADIO4, OnHost)
	ON_BN_CLICKED(IDC_CHECK3, OnDesc)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_BROWSECALLBACK2, OnBrowseUsersCallBack)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientFilterDlg message handlers

BOOL CClientFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int i = GET_P4REGPTR()->GetClientFilteredFlags();
	if (i & 0x11)
	{
		m_IsOwner.SetCheck(TRUE);
		if (i & 0x01)
			m_NotUser = 1;
	}
	if (i & 0x22)
	{
		m_IsHost.SetCheck(TRUE);
		if (i & 0x02)
			m_NotMyHost = 1;
	}
	if (i & 0x04)
		m_IsDesc.SetCheck(TRUE);
	m_Owner = GET_P4REGPTR()->GetClientFilterOwner();
	m_Host = GET_P4REGPTR()->GetClientFilterHost();
	m_DescContains = GET_P4REGPTR()->GetClientFilterDesc();
	
	UpdateData(FALSE);

	OnDesc();
	OnHost();
	OnOwner();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CClientFilterDlg::OnOK() 
{
	UpdateData( );
	int m_ErrorCount=0;

	int i = 0;
	if (m_IsOwner.GetCheck())
		i = m_NotUser ? 0x01 : 0x10;
	if (m_IsHost.GetCheck())
		i += m_NotMyHost ? 0x02 : 0x20;
	if (m_IsDesc.GetCheck())
		i += 0x04;
	if (i != GET_P4REGPTR()->GetClientFilteredFlags())
		if (!GET_P4REGPTR()->SetClientFilteredFlags( i ) )
				m_ErrorCount++;

	if (m_Owner != GET_P4REGPTR()->GetClientFilterOwner())
		if (!GET_P4REGPTR()->SetClientFilterOwner( m_Owner ) )
				m_ErrorCount++;

	if (m_Host != GET_P4REGPTR()->GetClientFilterHost())
		if (!GET_P4REGPTR()->SetClientFilterHost( m_Host ) )
				m_ErrorCount++;

	if (m_DescContains != GET_P4REGPTR()->GetClientFilterDesc())
		if (!GET_P4REGPTR()->SetClientFilterDesc( m_DescContains ) )
				m_ErrorCount++;

	if(m_ErrorCount)
		AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);

	CDialog::OnOK();
}

void CClientFilterDlg::OnOwner()
{
	BOOL b  = m_IsOwner.GetCheck();
	GetDlgItem(IDC_RADIO1)->EnableWindow(b);
	GetDlgItem(IDC_RADIO2)->EnableWindow(b);
	UpdateData( );
	b &= m_NotUser;
	GetDlgItem(IDC_OWNER)->EnableWindow(b);
	GetDlgItem(IDC_BROWSE)->EnableWindow(b);
	if (b)
		GotoDlgCtrl(GetDlgItem(IDC_OWNER));
}

void CClientFilterDlg::OnHost()
{
	BOOL b = m_IsHost.GetCheck();
	GetDlgItem(IDC_RADIO3)->EnableWindow(b);
	GetDlgItem(IDC_RADIO4)->EnableWindow(b);
	UpdateData( );
	b &= m_NotMyHost;
	GetDlgItem(IDC_HOST)->EnableWindow(b);
	if (b)
		GotoDlgCtrl(GetDlgItem(IDC_HOST));
}

void CClientFilterDlg::OnDesc()
{
	BOOL b;
	GetDlgItem(IDC_DESCCONTAINS)->EnableWindow(b = m_IsDesc.GetCheck());
	GotoDlgCtrl(GetDlgItem(IDC_DESCCONTAINS));
}

void CClientFilterDlg::OnBrowse()
{
	::SendMessage(MainFrame()->UserWnd(), WM_FETCHOBJECTLIST, 
					(WPARAM)(this->m_hWnd), WM_BROWSECALLBACK2);
	GotoDlgCtrl(GetDlgItem(IDC_OWNER));
}

LRESULT CClientFilterDlg::OnBrowseUsersCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_Owner = *str;
	UpdateData(FALSE);
	GotoDlgCtrl(GetDlgItem(IDC_OWNER));
	return 0;
}
