// ClientWizCreate.cpp : implementation file
//

#include "stdafx.h"
#include "P4win.h"
#include "ClientWizCreate.h"
#include "MainFrm.h"


// CClientWizCreate dialog

IMPLEMENT_DYNAMIC(CClientWizCreate, CPropertyPage)
CClientWizCreate::CClientWizCreate()
	: CPropertyPage(CClientWizCreate::IDD)
{
	m_bTmplate = 0;
	m_Tmplate = GET_P4REGPTR()->GetLastTemplate();
	if (m_Tmplate == LOCALDEFTEMPLATE)
		m_Tmplate.Empty();
}

CClientWizCreate::~CClientWizCreate()
{
	m_bEdit = m_bTmplate = 0;
}

void CClientWizCreate::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClientWizCreate)
	DDX_Text(pDX, IDC_EDITNAME, m_Name);
	DDX_Text(pDX, IDC_EDITROOT, m_Root);
	DDX_Radio(pDX, IDC_RADIO1, m_bTmplate);
	DDX_Text(pDX, IDC_TEMPLATE, m_Tmplate);
	DDX_Check(pDX, IDC_CHECK1, m_bEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClientWizCreate, CPropertyPage)
	//{{AFX_MSG_MAP(CClientWizCreate)
	ON_BN_CLICKED(IDC_RADIO1, OnTmplateClick)
	ON_BN_CLICKED(IDC_RADIO2, OnTmplateClick)
	ON_BN_CLICKED(IDC_RADIO3, OnTmplateClick)
	//}}AFX_MSG_MAP
	ON_COMMAND(IDC_BROWSE, OnBrowse)
	ON_COMMAND(IDC_BROWSE2, OnBrowseTemplates)
	ON_MESSAGE(WM_BROWSECALLBACK1, OnBrowseClientsCallBack)
END_MESSAGE_MAP()


// CClientWizCreate message handlers
BOOL CClientWizCreate::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CString localdeftmplate = GET_P4REGPTR()->GetLocalCliTemplate();
	if (!GET_P4REGPTR()->LocalCliTemplateSw() 
	 || localdeftmplate.IsEmpty() || localdeftmplate.Find(_T("\n\nClient:\t")) != 0)
		GetDlgItem(IDC_RADIO3)->EnableWindow(FALSE);

	if (TheApp()->m_RunClientWizOnly)
		GetDlgItem(IDC_EDITNAME)->EnableWindow(FALSE);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CClientWizCreate::OnSetActive() 
{
	CPropertySheet *sheet= (CPropertySheet *) GetParent();
	sheet->SetWizardButtons( PSWIZB_BACK | PSWIZB_FINISH );
	return CPropertyPage::OnSetActive();
}

BOOL CClientWizCreate::OnWizardFinish() 
{
	UpdateData();
	if (m_bTmplate)
	{
		if (m_bTmplate == 2)
		{
			m_Tmplate = LOCALDEFTEMPLATE;
			UpdateData(FALSE);
		}
		GET_P4REGPTR()->SetLastTemplate(m_Tmplate);
	}
	return CPropertyPage::OnWizardFinish();
}

void CClientWizCreate::OnTmplateClick()
{
	UpdateData();
	GetDlgItem(IDC_EDITROOT)->EnableWindow(m_bTmplate != 2);
	GetDlgItem(IDC_BROWSE)->EnableWindow(m_bTmplate != 2);
	GetDlgItem(IDC_TEMPLATE)->EnableWindow(m_bTmplate == 1);
	GetDlgItem(IDC_BROWSE2)->EnableWindow(m_bTmplate == 1);
	if (m_bTmplate == 1)
		GotoDlgCtrl(GetDlgItem(IDC_TEMPLATE));
}

void CClientWizCreate::OnBrowse()
{
	UpdateData();
	CString txt;
	txt.FormatMessage(IDS_CHOOSE_FOLDER_FOR, LoadStringResource(IDS_ROOT));
	CString path;
	DWORD attr = GetFileAttributes(m_Root);
	path = ((attr != -1) && (attr & FILE_ATTRIBUTE_DIRECTORY)) ? m_Root : _T("C:\\");
	path = TheApp()->BrowseForFolder(m_hWnd, path, txt, BIF_NEWDIALOGSTYLE);
	if (!path.IsEmpty())
	{
		m_Root = path;
		UpdateData(FALSE);
	}
	GotoDlgCtrl(GetDlgItem(IDC_EDITROOT));
}

void CClientWizCreate::OnBrowseTemplates()
{
	UpdateData();
	::SendMessage(MainFrame()->ClientWnd(), WM_FETCHOBJECTLIST, 
					(WPARAM)(this->m_hWnd), WM_BROWSECALLBACK1);
	GotoDlgCtrl(GetDlgItem(IDC_TEMPLATE));
}

LRESULT CClientWizCreate::OnBrowseClientsCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_Tmplate = *str;
	UpdateData(FALSE);
	GotoDlgCtrl(GetDlgItem(IDC_TEMPLATE));
	return 0;
}
