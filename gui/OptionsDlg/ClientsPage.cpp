//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// ClientsPage.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "ClientsPage.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientsPage property page

IMPLEMENT_DYNCREATE(CClientsPage, CPropertyPage)

CClientsPage::CClientsPage() : CPropertyPage(CClientsPage::IDD)
{
	//{{AFX_DATA_INIT(CClientsPage)
	m_UseDepotInClientView = TRUE;
	m_LocalCliTemplateSw = FALSE;
	m_UseClientSpecSubmitOpts = FALSE;
	//}}AFX_DATA_INIT
	m_Inited = FALSE;
}

CClientsPage::~CClientsPage()
{
}

void CClientsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClientsPage)
	DDX_Control(pDX, IDC_CLEARANDRELOAD, m_ClearAndReload);
	DDX_Check(pDX, IDC_USEDEPOTINCLIENTVIEW, m_UseDepotInClientView);
	DDX_Check(pDX, IDC_LOCALCLITEMPLATE, m_LocalCliTemplateSw);
	DDX_Check(pDX, IDC_USECLISUBMITOPTS, m_UseClientSpecSubmitOpts);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClientsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CClientsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientsPage message handlers


BOOL CClientsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CString str;

	// Init all data members from the registry
	m_ClearAndReload.SetCheck(GET_P4REGPTR()->GetClearAndReload()); 
	m_UseDepotInClientView = GET_P4REGPTR()->UseDepotInClientView();
	m_LocalCliTemplateSw = GET_P4REGPTR()->LocalCliTemplateSw();
	m_UseClientSpecSubmitOpts = GET_P4REGPTR()->UseClientSpecSubmitOpts();

	// Update input fields
	UpdateData(FALSE);
	
	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CClientsPage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	m_ErrorCount=0;

	if( UpdateData( TRUE ) )
	{
		BOOL clearAndReload=TRUE;
		if(m_ClearAndReload.GetCheck() == 0)
			clearAndReload=FALSE;
		if( clearAndReload != GET_P4REGPTR()->GetClearAndReload() )
			if(!GET_P4REGPTR()->SetClearAndReload( clearAndReload) )
				m_ErrorCount++;

		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetUseDepotInClientView( m_UseDepotInClientView) )
				m_ErrorCount++;
		}

		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetLocalCliTemplateSw( m_LocalCliTemplateSw) )
				m_ErrorCount++;
		}

		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetUseClientSpecSubmitOpts( m_UseClientSpecSubmitOpts) )
				m_ErrorCount++;
		}

		if(m_ErrorCount)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
	}		
}
