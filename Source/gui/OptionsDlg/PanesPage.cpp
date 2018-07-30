//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// PanesPage.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "PanesPage.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPanesPage property page

IMPLEMENT_DYNCREATE(CPanesPage, CPropertyPage)

CPanesPage::CPanesPage() : CPropertyPage(CPanesPage::IDD)
{
	//{{AFX_DATA_INIT(CPanesPage)
	//}}AFX_DATA_INIT
	m_Inited = FALSE;
}

CPanesPage::~CPanesPage()
{
}

void CPanesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPanesPage)
	DDX_Control(pDX, IDC_USEALLICONCOLORS, m_UseAllIconColors);
//	DDX_Control(pDX, IDC_SWAPBTNPOS, m_SwapButtonPosition);
	DDX_Control(pDX, IDC_ENABLE_SUBCHG_INTEG_FILTER, m_EnableSubChgIntegFilter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPanesPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPanesPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPanesPage message handlers

BOOL CPanesPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_UseAllIconColors.SetCheck(!GET_P4REGPTR()->Use256colorIcons());
//	m_SwapButtonPosition.SetCheck(GET_P4REGPTR( )->SwapButtonPosition());
	m_EnableSubChgIntegFilter.SetCheck(GET_P4REGPTR()->GetEnableSubChgIntegFilter());


	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPanesPage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	int errors=0;

	// All we have on this page are controls, so dont call
	// the UpdateData function
    BOOL UseAllIconColors = TRUE;
	if( m_UseAllIconColors.GetCheck() == 0 )
		UseAllIconColors = FALSE;
	if( UseAllIconColors != !GET_P4REGPTR()->Use256colorIcons( ) )
		if(!GET_P4REGPTR()->SetUse256colorIcons( !UseAllIconColors ) )
			errors++;

//	BOOL SwapButtonPosition = TRUE;
//	if( m_SwapButtonPosition.GetCheck() == 0 )
//		SwapButtonPosition = FALSE;
//	if( SwapButtonPosition != GET_P4REGPTR()->SwapButtonPosition( ) )
//		if(!GET_P4REGPTR()->SetSwapButtonPosition( SwapButtonPosition ) )
//			errors++;

	BOOL enableSubChgIntegFilter = TRUE;
	if( m_EnableSubChgIntegFilter.GetCheck() == 0 )
		enableSubChgIntegFilter = FALSE;
	if( enableSubChgIntegFilter != GET_P4REGPTR()->GetEnableSubChgIntegFilter( ) )
	{
		if(!GET_P4REGPTR()->SetEnableSubChgIntegFilter( enableSubChgIntegFilter ) )
			errors++;
		else
		{
			CMenu *pMenu = MainFrame()->GetMenu();
			if (enableSubChgIntegFilter)
			{
				pMenu->InsertMenu(ID_JOB_REMOVEFILEFILTER, MF_BYCOMMAND | MF_STRING, 
						ID_JOB_SETFILEFILTERINTEG, LoadStringResource(IDS_FILTERINTEG_JOBVIEW));
			}
			else
			{
				pMenu->DeleteMenu(ID_JOB_SETFILEFILTERINTEG, MF_BYCOMMAND);
			}
		}
	}
	if( !enableSubChgIntegFilter )	// FALSE here => turns off Annotate- & TLVIncInteg as well
	{
		if(!GET_P4REGPTR()->SetTLVIncInteg( FALSE ) )
			errors++;
		if(!GET_P4REGPTR()->SetAnnotateIncInteg( FALSE ) )
			errors++;
	}

	if(errors)
		AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
}
