//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// HistoryPage.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "HistoryPage.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHistoryPage property page

IMPLEMENT_DYNCREATE(CHistoryPage, CPropertyPage)

CHistoryPage::CHistoryPage() : CPropertyPage(CHistoryPage::IDD)
{
	//{{AFX_DATA_INIT(CHistoryPage)
	m_HistCount = 0;
	//}}AFX_DATA_INIT
	m_Inited = FALSE;
}

CHistoryPage::~CHistoryPage()
{
}

void CHistoryPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHistoryPage)
	DDX_Control(pDX, IDC_FETCHALLHIST_RAD, m_HistCountRadio);
	DDX_Text(pDX, IDC_HISTCOUNT, m_HistCount);
	DDV_MinMaxInt(pDX, m_HistCount, 0, 1000000);
	DDX_Control(pDX, IDC_ENABLE_REVHIST_SHOW_INTEGS, m_EnableRevHistShowIntegs);
	DDX_Control(pDX, IDC_USE_SHORT_REVHIST_DESC, m_UseShortRevHistDesc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHistoryPage, CPropertyPage)
	//{{AFX_MSG_MAP(CHistoryPage)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_FETCHALLHIST_RAD, OnFetchRadio)
	ON_BN_CLICKED(IDC_FETCHXHIST_RAD, OnFetchRadio)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHistoryPage message handlers


BOOL CHistoryPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CString str;

	// Init all data members from the registry
	m_HistCount = GET_P4REGPTR()->GetFetchHistCount();

	m_HistCountRadio.SetCheck(GET_P4REGPTR()->GetFetchAllHist());
	if(!GET_P4REGPTR()->GetFetchAllHist())
	{	
		CButton otherButton;
		otherButton.Attach(GetDlgItem(IDC_FETCHXHIST_RAD)->m_hWnd);
		otherButton.SetCheck(1);
		otherButton.Detach();
	}

	// Enable/disable fetch record count per radio button
	OnFetchRadio();
	
	m_EnableRevHistShowIntegs.SetCheck(GET_P4REGPTR()->GetEnableRevHistShowIntegs());
	m_UseShortRevHistDesc.SetCheck(GET_P4REGPTR()->GetUseShortRevHistDesc());

	// Update input fields
	UpdateData(FALSE);
	
	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CHistoryPage::OnFetchRadio() 
{
	CWnd *pwnd=GetDlgItem(IDC_HISTCOUNT);
	if(m_HistCountRadio.GetCheck()==0) 
		pwnd->EnableWindow(TRUE);
	else
		pwnd->EnableWindow(FALSE);
}


void CHistoryPage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	m_ErrorCount=0;

	Error e;
	
	if( UpdateData( TRUE ) )
	{
		BOOL allState=TRUE;
		if(m_HistCountRadio.GetCheck() == 0)
            allState=FALSE;
        if( allState != GET_P4REGPTR()->GetFetchAllHist() )
			if(!GET_P4REGPTR()->SetFetchAllHist( allState) )
				m_ErrorCount++;

		if ((m_HistCount != GET_P4REGPTR()->GetFetchHistCount())
		 || (!m_HistCount && !allState))
		{
			if (!m_HistCount && !allState)
				 m_HistCount = 1;
			if(!GET_P4REGPTR()->SetFetchHistCount(m_HistCount))
                m_ErrorCount++;
		}
		
		BOOL enableRevHistShowIntegs = TRUE;
		if( m_EnableRevHistShowIntegs.GetCheck() == 0 )
			enableRevHistShowIntegs = FALSE;
		if( enableRevHistShowIntegs != GET_P4REGPTR()->GetEnableRevHistShowIntegs( ) )
			if(!GET_P4REGPTR()->SetEnableRevHistShowIntegs( enableRevHistShowIntegs ) )
				m_ErrorCount++;

		BOOL useShortRevHistDesc = TRUE;
		if( m_UseShortRevHistDesc.GetCheck() == 0 )
			useShortRevHistDesc = FALSE;
		if( useShortRevHistDesc != GET_P4REGPTR()->GetUseShortRevHistDesc( ) )
			if(!GET_P4REGPTR()->SetUseShortRevHistDesc( useShortRevHistDesc ) )
				m_ErrorCount++;

		if(m_ErrorCount)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
	}		
}
