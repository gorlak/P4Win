//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// DepotPage.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "DepotPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDepotPage property page

IMPLEMENT_DYNCREATE(CDepotPage, CPropertyPage)

CDepotPage::CDepotPage() : CPropertyPage(CDepotPage::IDD)
{
	//{{AFX_DATA_INIT(CDepotPage)
	m_ExpandPath = _T("");
	m_ExpandFlag = -1;
	m_ExtSortMax = _T("1000");
	m_BusyWaitTime = _T("250");
	//}}AFX_DATA_INIT
	m_Inited = FALSE;
}

CDepotPage::~CDepotPage()
{
}

void CDepotPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDepotPage)
	DDX_Control(pDX, IDC_P4SHOWFTYPES, m_ShowTypes);
	DDX_Control(pDX, IDC_P4SHOWDELETED, m_ShowDeleted);
	DDX_Control(pDX, IDC_SHOWHIDDEN, m_ShowHidden);
	DDX_Control(pDX, IDC_SHOWHIGHLITE, m_ShowHighLite);
	DDX_Control(pDX, IDC_SHOWCLIENTPATH, m_ShowClientPath);
	DDX_Control(pDX, IDC_DIFF2TODLG, m_Diff2InDialog);
	DDX_Control(pDX, IDC_AUTOTREEEXPAND, m_AutoTreeExpand);
	DDX_Text(pDX, IDC_EXPANDPATH, m_ExpandPath);
	DDX_Radio(pDX, IDC_NODEPOTEXPAND, m_ExpandFlag);
	DDX_Text(pDX, IDC_EXTSORTMAX, m_ExtSortMax);
	DDV_MaxChars(pDX, m_ExtSortMax, 5);
	DDX_Text(pDX, IDC_BUSYWAITTIME, m_BusyWaitTime);
	DDV_MaxChars(pDX, m_BusyWaitTime, 4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDepotPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDepotPage)
	ON_BN_CLICKED(IDC_SHOWCLIENTPATH, OnShowclientpath)
	ON_BN_CLICKED(IDC_EXPAND_TO, OnFetchRadio)
	ON_BN_CLICKED(IDC_EXPAND_TO_PREV, OnFetchRadio)
	ON_BN_CLICKED(IDC_NODEPOTEXPAND, OnFetchRadio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDepotPage message handlers

BOOL CDepotPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Init all data members from the registry
	m_ShowDeleted.SetCheck(GET_P4REGPTR()->ShowDeleted()); 
	m_ShowHidden.SetCheck(GET_P4REGPTR()->ShowHiddenFilesNotInDepot()); 
	m_ShowTypes.SetCheck(GET_P4REGPTR()->ShowFileType()); 
    m_ShowClientPath.SetCheck(GET_P4REGPTR()->ShowClientPath()); 
    m_Diff2InDialog.SetCheck(GET_P4REGPTR()->Diff2InDialog()); 
    m_AutoTreeExpand.SetCheck(GET_P4REGPTR()->AutoTreeExpand()); 
	m_ShowHighLite.SetCheck(GET_P4REGPTR()->ShowDepotPathHiLite()); 
	m_ExpandFlag = GET_P4REGPTR()->GetExpandFlag();
	m_ExpandPath = GET_P4REGPTR()->GetExpandPath();
	m_ExtSortMax = GET_P4REGPTR()->GetExtSortMax();
	m_BusyWaitTime.Format(_T("%ld"), GET_P4REGPTR()->BusyWaitTime());

	// Update input fields
	UpdateData(FALSE);
    OnShowclientpath();
	OnFetchRadio();

	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDepotPage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	m_ErrorCount=0;

	if(UpdateData(TRUE))
	{
		BOOL showDeleted=TRUE;
		if(m_ShowDeleted.GetCheck() == 0)
			showDeleted=FALSE;
		if( showDeleted != GET_P4REGPTR()->ShowDeleted() )
			if(!GET_P4REGPTR()->SetShowDeleted( showDeleted ) )
				m_ErrorCount++;

		BOOL showHidden=TRUE;
		if(m_ShowHidden.GetCheck() == 0)
			showHidden=FALSE;
		if( showHidden != GET_P4REGPTR()->ShowHiddenFilesNotInDepot() )
			if(!GET_P4REGPTR()->SetShowHiddenFilesNotInDepot( showHidden ) )
				m_ErrorCount++;

		BOOL showTypes=TRUE;
		if(m_ShowTypes.GetCheck() == 0)
			showTypes=FALSE;
		if( showTypes != GET_P4REGPTR()->ShowFileType() )
			if(!GET_P4REGPTR()->SetShowFileType( showTypes ) )
				m_ErrorCount++;
		
		// Disable this because it involved too much overhead
		BOOL showClientPath4Chgs = FALSE;
		if( showClientPath4Chgs != GET_P4REGPTR()->ShowClientPath4Chgs( ) )
			if(!GET_P4REGPTR()->SetShowClientPath4Chgs( showClientPath4Chgs ) )
				m_ErrorCount++;

		BOOL showClientPath = TRUE;
		if( m_ShowClientPath.GetCheck() == 0 )
			showClientPath = FALSE;
		if( showClientPath != GET_P4REGPTR()->ShowClientPath( ) )
			if(!GET_P4REGPTR()->SetShowClientPath( showClientPath ) )
				m_ErrorCount++;

		BOOL diff2InDialog = TRUE;
		if( m_Diff2InDialog.GetCheck() == 0 )
			diff2InDialog = FALSE;
		if( diff2InDialog != GET_P4REGPTR()->Diff2InDialog( ) )
			if(!GET_P4REGPTR()->SetDiff2InDialog( diff2InDialog ) )
				m_ErrorCount++;

		BOOL autoTreeExpand = TRUE;
		if( m_AutoTreeExpand.GetCheck() == 0 )
			autoTreeExpand = FALSE;
		if( autoTreeExpand != GET_P4REGPTR()->AutoTreeExpand( ) )
			if(!GET_P4REGPTR()->SetAutoTreeExpand( autoTreeExpand ) )
				m_ErrorCount++;

		BOOL showHiLite = TRUE;
		if( m_ShowHighLite.GetCheck() == 0 )
			showHiLite = FALSE;
		if( showHiLite != GET_P4REGPTR()->ShowDepotPathHiLite( ) )
			if(!GET_P4REGPTR()->SetShowDepotPathHiLite( showHiLite ) )
				m_ErrorCount++;

		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetExpandFlag( m_ExpandFlag ) )
				m_ErrorCount++;
		}
		if (m_ExpandFlag == 2)
		{
			if (!m_ErrorCount)
			{
				if(!GET_P4REGPTR()->SetExpandPath( m_ExpandPath ) )
					m_ErrorCount++;
			}
			if (!m_ErrorCount)
			{
				if(!GET_P4REGPTR()->AddMRUPcuPath( m_ExpandPath ) )
					m_ErrorCount++;
			}
		}
		if (!m_ErrorCount)
		{
			m_ExtSortMax.TrimRight();
			m_ExtSortMax.TrimLeft();
			if ((m_ExtSortMax.GetLength() > 5) && (m_ExtSortMax > _T("99999")))
				m_ExtSortMax = _T("99999");
			if(!GET_P4REGPTR()->SetExtSortMax( m_ExtSortMax ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			m_BusyWaitTime.TrimRight();
			m_BusyWaitTime.TrimLeft();
			int t = _tstoi(m_BusyWaitTime);
			if (t < 100)
				t = 100;
			else if (t > 9999)
				t = 9999;
			if(!GET_P4REGPTR()->SetBusyWaitTime( t ) )
				m_ErrorCount++;
		}
		
		if(m_ErrorCount)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
	}		
}

void CDepotPage::OnFetchRadio() 
{
	UpdateData();
	CWnd *pwnd=GetDlgItem(IDC_EXPANDPATH);
	if( m_ExpandFlag == 2 )
	{
		pwnd->EnableWindow(TRUE);
		if (m_Inited)
			GotoDlgCtrl(pwnd);
	}
	else
		pwnd->EnableWindow(FALSE);
}

void CDepotPage::OnShowclientpath() 
{
    m_ShowHighLite.EnableWindow( m_ShowClientPath.GetCheck() );
}
