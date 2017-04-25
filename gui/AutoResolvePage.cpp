// AutoResolvePage.cpp : implementation file
//

#include "stdafx.h"
#include "P4win.h"
#include "AutoResolvePage.h"


// CAutoResolvePage dialog

IMPLEMENT_DYNAMIC(CAutoResolvePage, CPropertyPage)
CAutoResolvePage::CAutoResolvePage()
	: CPropertyPage(CAutoResolvePage::IDD)
{
	//{{AFX_DATA_INIT(CAutoResolvePage)
	m_ResolveAutoDefault = GET_P4REGPTR()->GetResolveDefault();
	m_Resolve2wayDefault = GET_P4REGPTR()->GetResolve2wayDefault();
	m_Resolve3wayDefault = GET_P4REGPTR()->GetResolve3wayDefault();
	//}}AFX_DATA_INIT
	m_Inited = m_Closing = FALSE;
	//}}AFX_DATA_INIT
}

CAutoResolvePage::~CAutoResolvePage()
{
}

void CAutoResolvePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoResolvePage)
	DDX_Radio(pDX, IDC_DEFAULTTHEIRS, m_ResolveAutoDefault);
	DDX_Radio(pDX, IDC_DEFAULTYOURS2, m_Resolve2wayDefault);
	DDX_Radio(pDX, IDC_DEFAULTYOURS3, m_Resolve3wayDefault);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoResolvePage, CPropertyPage)
END_MESSAGE_MAP()


// CAutoResolvePage message handlers

BOOL CAutoResolvePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// Update input fields
	UpdateData(FALSE);
	
	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAutoResolvePage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	int errors=0;

	UpdateData(TRUE);
	if (m_ResolveAutoDefault != GET_P4REGPTR()->GetResolveDefault())
		if (!GET_P4REGPTR()->SetResolveDefault(m_ResolveAutoDefault))
			errors++;
	if (m_Resolve2wayDefault != GET_P4REGPTR()->GetResolve2wayDefault())
		if (!GET_P4REGPTR()->SetResolve2wayDefault(m_Resolve2wayDefault))
			errors++;
	if (m_Resolve3wayDefault != GET_P4REGPTR()->GetResolve3wayDefault())
		if (!GET_P4REGPTR()->SetResolve3wayDefault(m_Resolve3wayDefault))
			errors++;
	if(errors)
		AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
}
