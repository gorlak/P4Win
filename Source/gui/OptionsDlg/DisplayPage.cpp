//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// DisplayPage.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "DisplayPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDisplayPage property page

IMPLEMENT_DYNCREATE(CDisplayPage, CPropertyPage)

CDisplayPage::CDisplayPage() : CPropertyPage(CDisplayPage::IDD)
{
	//{{AFX_DATA_INIT(CDisplayPage)
	m_OptionStartUpOption = 2;
	//}}AFX_DATA_INIT
	m_Inited = FALSE;
}

CDisplayPage::~CDisplayPage()
{
}

void CDisplayPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDisplayPage)
	DDX_Control(pDX, IDC_AUTOEXPANDOPTIONS, m_AutoExpandOptions);
	DDX_Radio(pDX, IDC_RADIO1, m_OptionStartUpOption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDisplayPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDisplayPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisplayPage message handlers

void CDisplayPage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	if(UpdateData(TRUE))
	{
		int errors=0;

		if( m_OptionStartUpOption != GET_P4REGPTR()->GetOptionStartUpOption() )
			if(!GET_P4REGPTR()->SetOptionStartUpOption(m_OptionStartUpOption))
				errors++;

		BOOL autoExpandOptions = TRUE;
		if( m_AutoExpandOptions.GetCheck() == 0 )
			autoExpandOptions = FALSE;
		if( autoExpandOptions != GET_P4REGPTR()->AutoExpandOptions( ) )
			if(!GET_P4REGPTR()->SetAutoExpandOptions( autoExpandOptions ) )
				errors++;

		if(errors)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
	}
}

BOOL CDisplayPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_AutoExpandOptions.SetCheck(GET_P4REGPTR()->AutoExpandOptions()); 
	m_OptionStartUpOption = GET_P4REGPTR()->GetOptionStartUpOption();
	if (m_OptionStartUpOption < 0 || m_OptionStartUpOption > 2)
		m_OptionStartUpOption = 2;

	UpdateData(FALSE);

	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
