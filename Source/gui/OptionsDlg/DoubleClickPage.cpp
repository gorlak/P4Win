// DoubleClickPage.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "DoubleClickPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDoubleClickPage dialog


IMPLEMENT_DYNCREATE(CDoubleClickPage, CPropertyPage)

CDoubleClickPage::CDoubleClickPage() : CPropertyPage(CDoubleClickPage::IDD)
{
	//{{AFX_DATA_INIT(CDoubleClickPage)
	m_DblClick = -1;
	//}}AFX_DATA_INIT
	m_Inited = FALSE;
}

CDoubleClickPage::~CDoubleClickPage()
{
}

void CDoubleClickPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDoubleClickPage)
	DDX_Radio(pDX, IDC_DBLCLK_DEFAULT, m_DblClick);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDoubleClickPage, CDialog)
	//{{AFX_MSG_MAP(CDoubleClickPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDoubleClickPage message handlers

BOOL CDoubleClickPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Init all data members from the registry
	m_DblClick = GET_P4REGPTR()->GetDoubleClickOption();

	UpdateData(FALSE);
	
	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDoubleClickPage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	int errors=0;

	if(UpdateData(TRUE))
	{
		if( m_DblClick != GET_P4REGPTR()->GetDoubleClickOption() )
			if(!GET_P4REGPTR()->SetDoubleClickOption(m_DblClick))
				errors++;
		
		if(errors)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
	}		
}

