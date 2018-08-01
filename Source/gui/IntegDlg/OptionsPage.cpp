//
// 
// Copyright 1999 Perforce Software.  All rights reserved.
//
// This file is part of Perforce - the FAST SCM System.
//
//


// OptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\p4win.h"
#include "OptionsPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIntegOptionsPage property page

IMPLEMENT_DYNCREATE(CIntegOptionsPage, CPropertyPage)

CIntegOptionsPage::CIntegOptionsPage() : CPropertyPage(CIntegOptionsPage::IDD)
{
	//{{AFX_DATA_INIT(CIntegOptionsPage)
	m_IsNoCopy = FALSE;
	m_IsForceInteg = FALSE;
	m_IsForceDirect = FALSE;
	m_IsDeleteSource = FALSE;
	m_IsMappingReverse = FALSE;
	m_FileSpecs = _T("");
	m_IsPermitDelReadd = FALSE;
	m_DelReaddType = 2;
	m_IsBaselessMerge = FALSE;
	m_IsPropagateTypes = FALSE;
	m_IsBaseViaDelReadd = FALSE;
	m_UseNewChglist = m_UseNewChglist2 = FALSE;
	m_IsSyncFirst = TRUE;
	//}}AFX_DATA_INIT

	m_IsDefOpts = TRUE;
	m_IsBranch = FALSE;
	m_Initialized = FALSE;
}

CIntegOptionsPage::~CIntegOptionsPage()
{
}

void CIntegOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIntegOptionsPage)
	DDX_Check(pDX, IDC_NOCOPY, m_IsNoCopy);
	DDX_Check(pDX, IDC_FORCEINTEG, m_IsForceInteg);
	DDX_Check(pDX, IDC_FORCEDIRECT, m_IsForceDirect);
	DDX_Check(pDX, IDC_DELETESOURCE, m_IsDeleteSource);
	DDX_Check(pDX, IDC_REVERSE, m_IsMappingReverse);
	DDX_Text(pDX, IDC_FILESPECS, m_FileSpecs);
	DDX_Check(pDX, IDC_DELREADD, m_IsPermitDelReadd);
	DDX_Radio(pDX, IDC_RADIO_DEL, m_DelReaddType);
	DDX_Check(pDX, IDC_BASELESS, m_IsBaselessMerge);
	DDX_Check(pDX, IDC_PROPAGATETYPES, m_IsPropagateTypes);
	DDX_Check(pDX, IDC_BASEVIADELREADD, m_IsBaseViaDelReadd);
	DDX_Radio(pDX, IDC_USEDEFCHGLIST, m_UseNewChglist);
	DDX_Radio(pDX, IDC_USEDEFCHGLIST2, m_UseNewChglist2);
	DDX_Check(pDX, IDC_SYNCFIRST, m_IsSyncFirst);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIntegOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CIntegOptionsPage)
	ON_BN_CLICKED(IDC_ALLFILES, OnAllfiles)
	ON_BN_CLICKED(IDC_FILESUBSET, OnFilesubset)
	ON_BN_CLICKED(IDC_DELREADD, OnDelReadd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIntegOptionsPage message handlers

INT_PTR CIntegOptionsPage::DoModal() 
{
	return CPropertyPage::DoModal();
}

BOOL CIntegOptionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	if (m_IsDefOpts)
	{
		m_IsNoCopy = GET_P4REGPTR()->GetIsNoCopy();
		m_IsForceInteg = GET_P4REGPTR()->GetIsForceInteg();
		m_IsForceDirect = GET_P4REGPTR()->GetIsForceDirect();
		m_IsPermitDelReadd = GET_P4REGPTR()->GetIsPermitDelReadd();
		m_DelReaddType = GET_P4REGPTR()->GetDelReaddType();
		m_IsBaselessMerge = GET_P4REGPTR()->GetIsBaselessMerge();
		m_IsPropagateTypes = GET_P4REGPTR()->GetIsPropagateTypes();
		m_IsBaseViaDelReadd = GET_P4REGPTR()->GetIsBaseViaDelReadd();
		m_UseNewChglist = GET_P4REGPTR()->GetUseNewChglist();
		m_UseNewChglist2 = GET_P4REGPTR()->GetUseNewChglist2();
		m_IsSyncFirst = GET_P4REGPTR()->GetSyncFirstDefault();
		GetDlgItem( IDC_INTEGTOLABEL )->ModifyStyle( WS_VISIBLE, 0 );
		GetDlgItem( IDC_ALLFILES   )->ModifyStyle( WS_VISIBLE, 0 );
		GetDlgItem( IDC_FILESUBSET )->ModifyStyle( WS_VISIBLE, 0 );
		GetDlgItem( IDC_FILESPECS  )->ModifyStyle( WS_VISIBLE, 0 );
	}
	else 
	{
		GetDlgItem( IDC_USEGRPCHGLIST )->ModifyStyle( WS_VISIBLE, 0 );
		GetDlgItem( IDC_USEDEFCHGLIST )->ModifyStyle( WS_VISIBLE, 0 );
		GetDlgItem( IDC_USENEWCHGLIST )->ModifyStyle( WS_VISIBLE, 0 );
		GetDlgItem( IDC_USEGRPCHGLIST2 )->ModifyStyle( WS_VISIBLE, 0 );
		GetDlgItem( IDC_USEDEFCHGLIST2 )->ModifyStyle( WS_VISIBLE, 0 );
		GetDlgItem( IDC_USENEWCHGLIST2 )->ModifyStyle( WS_VISIBLE, 0 );
		if (GET_SERVERLEVEL() < 17)
		{
			m_DelReaddType = 2;
			GetDlgItem( IDC_RADIO_DEL )->EnableWindow(FALSE);
			GetDlgItem( IDC_RADIO_READD )->EnableWindow(FALSE);
			if (!m_IsPermitDelReadd)
				GetDlgItem( IDC_RADIO_BOTH )->EnableWindow(FALSE);
		}
		else
		{
			if (!m_IsPermitDelReadd)
			{
				GetDlgItem( IDC_RADIO_DEL )->EnableWindow(FALSE);
				GetDlgItem( IDC_RADIO_READD )->EnableWindow(FALSE);
				GetDlgItem( IDC_RADIO_BOTH )->EnableWindow(FALSE);
			}
		}
	}
	m_Initialized = TRUE;

	if( (m_BranchFlag != INTEG_USING_BRANCH_SPEC) || GET_SERVERLEVEL() >= LEVEL_NEWINTEG )
	{
		// Hide Mapping Reverse if not a branch-from-file-specs or if new integ support
		GetDlgItem( IDC_REVERSE )->EnableWindow(FALSE);
		GetDlgItem( IDC_REVERSE )->ModifyStyle( WS_VISIBLE, 0 );
	}

	if( m_IsBranch || m_IsDefOpts)
	{
		// Hide Delete Source if is a branch or default options
		GetDlgItem( IDC_DELETESOURCE )->EnableWindow(FALSE);
		GetDlgItem( IDC_DELETESOURCE )->ModifyStyle( WS_VISIBLE, 0 );
		m_IsDeleteSource = FALSE;
	}

	if ( GET_SERVERLEVEL() < 18 )
	{
		GetDlgItem( IDC_BASEVIADELREADD )->ModifyStyle( WS_VISIBLE, 0 );
		m_IsBaseViaDelReadd = FALSE;
		GetDlgItem( IDC_FORCEDIRECT )->ModifyStyle( WS_VISIBLE, 0 );
		m_IsBaseViaDelReadd = FALSE;
		if ( GET_SERVERLEVEL() < 10 )
		{
			GetDlgItem( IDC_PROPAGATETYPES )->ModifyStyle( WS_VISIBLE, 0 );
			if (!m_IsDefOpts)
				m_IsPropagateTypes = FALSE;
			if ( GET_SERVERLEVEL() < 8 )
			{
				// Hide Permit Delete/Re-add & Bassless Merges if server is older than 99.2
				GetDlgItem( IDC_DELREADD )->ModifyStyle( WS_VISIBLE, 0 );
				GetDlgItem( IDC_BASELESS )->ModifyStyle( WS_VISIBLE, 0 );
				if (!m_IsDefOpts)
				{
					m_IsPermitDelReadd = FALSE;
					m_IsBaselessMerge = FALSE;
				}
			}
		}
	}

	if (m_BranchFlag != INTEG_USING_BRANCH)
	{
		GetDlgItem(IDC_INTEGTOLABEL)->ModifyStyle( WS_VISIBLE, 0 );
		GetDlgItem( IDC_ALLFILES   )->ModifyStyle( WS_VISIBLE, 0 );
		GetDlgItem( IDC_FILESUBSET )->ModifyStyle( WS_VISIBLE, 0 );
		GetDlgItem( IDC_FILESPECS  )->ModifyStyle( WS_VISIBLE, 0 );
	}
	else
	{
		((CButton *) GetDlgItem(m_FileSpecs.IsEmpty() 
								? IDC_ALLFILES : IDC_FILESUBSET))->SetCheck( 1 );
		GetDlgItem( IDC_FILESPECS )->EnableWindow( m_FileSpecs.IsEmpty() ? FALSE : TRUE );
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIntegOptionsPage::OnAllfiles() 
{
	UpdateData();
	GetDlgItem(IDC_FILESPECS)->EnableWindow( FALSE );
	m_FileSpecs.Empty();
	UpdateData(FALSE);
}

void CIntegOptionsPage::OnFilesubset() 
{
	GetDlgItem(IDC_FILESPECS)->EnableWindow( TRUE );
	GotoDlgCtrl(GetDlgItem(IDC_FILESPECS));
}

void CIntegOptionsPage::OnDelReadd() 
{
	if (!m_IsDefOpts)
	{
		UpdateData();
		if (GET_SERVERLEVEL() > 16)
		{
			GetDlgItem(IDC_RADIO_DEL)->EnableWindow( m_IsPermitDelReadd );
			GetDlgItem(IDC_RADIO_READD)->EnableWindow( m_IsPermitDelReadd );
		}
		GetDlgItem(IDC_RADIO_BOTH)->EnableWindow( m_IsPermitDelReadd );
	}
}

void CIntegOptionsPage::OnOK() 
{
	if (!m_Initialized || !m_IsDefOpts)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	m_ErrorCount=0;

	if(UpdateData(TRUE))
	{
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetIsNoCopy( m_IsNoCopy ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetIsForceInteg( m_IsForceInteg ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetIsForceDirect( m_IsForceDirect ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetIsDeleteSource( FALSE ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetIsPermitDelReadd( m_IsPermitDelReadd ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetDelReaddType( m_DelReaddType ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetIsPropagateTypes( m_IsPropagateTypes ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetIsBaseViaDelReadd( m_IsBaseViaDelReadd ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetUseNewChglist( m_UseNewChglist ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetUseNewChglist2( m_UseNewChglist2 ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetIsBaselessMerge( m_IsBaselessMerge ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetSyncFirstDefault( m_IsSyncFirst ) )
				m_ErrorCount++;
		}
		if(m_ErrorCount)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
	}
}
