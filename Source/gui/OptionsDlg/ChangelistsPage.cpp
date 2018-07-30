//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// ChangelistsPage.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "ChangelistsPage.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangelistsPage property page

IMPLEMENT_DYNCREATE(CChangelistsPage, CPropertyPage)

CChangelistsPage::CChangelistsPage() : CPropertyPage(CChangelistsPage::IDD)
{
	//{{AFX_DATA_INIT(CChangelistsPage)
	m_WarnAlreadyOpened = FALSE;
	m_SubmitOnlyChged = m_RevertUnchged = FALSE;
	m_AutoReopen = FALSE;
	m_WarnLimit = _T("");
	m_OldChangeCount = 0;
	m_UseLongChglistDesc = 31;
	//}}AFX_DATA_INIT
	m_Inited = FALSE;
}

CChangelistsPage::~CChangelistsPage()
{
}

void CChangelistsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangelistsPage)
	DDX_Check(pDX, IDC_WARNALREADYOPENED, m_WarnAlreadyOpened);
	DDX_Check(pDX, IDC_DESEL_UNCHGED, m_SubmitOnlyChged);
	DDX_Check(pDX, IDC_REVERT, m_RevertUnchged);
	DDX_Check(pDX, IDC_AUTO_REOPEN, m_AutoReopen);
	DDX_Text(pDX, IDC_WARNLIMIT, m_WarnLimit);
	DDX_Text(pDX, IDC_WARNLIMITOPEN, m_WarnLimitOpen);
	DDX_Text(pDX, IDC_WARNLIMITDIFF, m_WarnLimitDiff);
	DDV_MaxChars(pDX, m_WarnLimit, 10);
	DDV_MaxChars(pDX, m_WarnLimitOpen, 10);
	DDV_MaxChars(pDX, m_WarnLimitDiff, 5);
	DDX_Control(pDX, IDC_EXPANDCHGLIST, m_ExpandChgLists);
	DDX_Control(pDX, IDC_REEXPAND, m_ReExpandChgs);
	DDX_Control(pDX, IDC_P4SHOWACTIONS, m_ShowOpenActions);
	DDX_Control(pDX, IDC_P4SHOWCHANGEDESC, m_ShowChangeDescs);
	DDX_Control(pDX, IDC_ENABLE_PEND_CHG_OTHER_CLI, m_EnablePendingChgsOtherClients);
	DDX_Control(pDX, IDC_FETCHALL_RAD, m_ChangeCountRadio);
	DDX_Text(pDX, IDC_CHANGECOUNT, m_OldChangeCount);
	DDV_MinMaxInt(pDX, m_OldChangeCount, 0, 10000000);
	DDX_Text(pDX, IDC_USELONGCHGLISTDESC, m_UseLongChglistDesc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangelistsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CChangelistsPage)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_FETCHX_RAD, OnFetchRadio)
	ON_BN_CLICKED(IDC_FETCHALL_RAD, OnFetchRadio)
	ON_BN_CLICKED(IDC_P4SHOWCHANGEDESC, OnShowChgDesc)
	ON_BN_CLICKED(IDC_DESEL_UNCHGED, OnDeselUnchanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangelistsPage message handlers


BOOL CChangelistsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CString str;

	// Init all data members from the registry
	m_WarnAlreadyOpened = GET_P4REGPTR()->GetWarnAlreadyOpened();
	m_SubmitOnlyChged = GET_P4REGPTR()->GetSubmitOnlyChged();
	m_RevertUnchged = m_SubmitOnlyChged ? GET_P4REGPTR()->GetRevertUnchged() : FALSE;
	m_AutoReopen = GET_P4REGPTR()->GetAutoReopen();
	m_WarnLimit = GET_P4REGPTR()->GetWarnLimit();
	m_WarnLimitOpen = GET_P4REGPTR()->GetWarnLimitOpen();
	m_WarnLimitDiff = GET_P4REGPTR()->GetWarnLimitDiff();
	m_ShowOpenActions.SetCheck(GET_P4REGPTR()->ShowOpenAction()); 
	m_ShowChangeDescs.SetCheck(GET_P4REGPTR()->ShowChangeDesc()); 
	m_ExpandChgLists.SetCheck(GET_P4REGPTR()->ExpandChgLists());
	m_ReExpandChgs.SetCheck(GET_P4REGPTR()->ReExpandChgs());
	m_EnablePendingChgsOtherClients.SetCheck(GET_P4REGPTR()->GetEnablePendingChgsOtherClients());
	m_OldChangeCount = GET_P4REGPTR()->GetFetchChangeCount();
	m_UseLongChglistDesc = GET_P4REGPTR()->GetUseLongChglistDesc();

    m_ChangeCountRadio.SetCheck(GET_P4REGPTR()->GetFetchAllChanges());
	if(!GET_P4REGPTR()->GetFetchAllChanges())
	{	
		CButton otherButton;
		otherButton.Attach(GetDlgItem(IDC_FETCHX_RAD)->m_hWnd);
		otherButton.SetCheck(1);
		otherButton.Detach();
	}

	// Enable/disable fetch change count per radio button
	OnFetchRadio();
	OnShowChgDesc();

	// Update input fields
	UpdateData(FALSE);
	OnDeselUnchanged();	// must call after UpdateData() has loaded widgets
	
	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CChangelistsPage::OnFetchRadio() 
{
	CWnd *pwnd=GetDlgItem(IDC_CHANGECOUNT);
	if(m_ChangeCountRadio.GetCheck()==0) 
		pwnd->EnableWindow(TRUE);
	else
		pwnd->EnableWindow(FALSE);
}

void CChangelistsPage::OnShowChgDesc() 
{
	CWnd *pwnd=GetDlgItem(IDC_USELONGCHGLISTDESC);
	if(m_ShowChangeDescs.GetCheck()!=0) 
		pwnd->EnableWindow(TRUE);
	else
		pwnd->EnableWindow(FALSE);
}

void CChangelistsPage::OnDeselUnchanged()
{
	UpdateData();
	CWnd *pwnd=GetDlgItem(IDC_REVERT);
	if(m_SubmitOnlyChged)
		pwnd->EnableWindow(TRUE);
	else
	{
		m_RevertUnchged = FALSE;
		UpdateData(FALSE);
		pwnd->EnableWindow(FALSE);
	}
}

void CChangelistsPage::OnOK() 
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
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetWarnAlreadyOpened( m_WarnAlreadyOpened ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetSubmitOnlyChged( m_SubmitOnlyChged ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetRevertUnchged( m_RevertUnchged ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetAutoReopen( m_AutoReopen ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			m_WarnLimit.TrimRight();
			m_WarnLimit.TrimLeft();
			if ((m_WarnLimit.GetLength() > 9) && (m_WarnLimit > _T("2000000000")))
				m_WarnLimit = _T("2000000000");
			if(!GET_P4REGPTR()->SetWarnLimit( m_WarnLimit ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			m_WarnLimitOpen.TrimRight();
			m_WarnLimitOpen.TrimLeft();
			if ((m_WarnLimitOpen.GetLength() > 5) && (m_WarnLimitOpen > _T("2000000000")))
				m_WarnLimitOpen = _T("2000000000");
			if(!GET_P4REGPTR()->SetWarnLimitOpen( m_WarnLimitOpen ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			m_WarnLimitDiff.TrimRight();
			m_WarnLimitDiff.TrimLeft();
			if ((m_WarnLimitDiff.GetLength() > 5) && (m_WarnLimitDiff > _T("99999")))
				m_WarnLimitDiff = _T("99999");
			if(!GET_P4REGPTR()->SetWarnLimitDiff( m_WarnLimitDiff ) )
				m_ErrorCount++;
		}

		BOOL showActions=TRUE;
		if(m_ShowOpenActions.GetCheck() == 0)
			showActions=FALSE;
		if( showActions != GET_P4REGPTR()->ShowOpenAction() )
			if(!GET_P4REGPTR()->SetShowOpenAction( showActions ) )
				m_ErrorCount++;

		BOOL showDescs=TRUE;
		if(m_ShowChangeDescs.GetCheck() == 0)
			showDescs=FALSE;
		if( showDescs != GET_P4REGPTR()->ShowChangeDesc() )
			if(!GET_P4REGPTR()->SetShowChangeDesc( showDescs ) )
				m_ErrorCount++;

		BOOL expandChgLists = TRUE;
		if( m_ExpandChgLists.GetCheck() == 0 )
			expandChgLists = FALSE;
		if( expandChgLists != GET_P4REGPTR()->ExpandChgLists( ) )
			if(!GET_P4REGPTR()->SetExpandChgLists( expandChgLists ) )
				m_ErrorCount++;

		BOOL reExpandChgs = TRUE;
		if( m_ReExpandChgs.GetCheck() == 0 )
			reExpandChgs = FALSE;
		if( reExpandChgs != GET_P4REGPTR()->ReExpandChgs( ) )
			if(!GET_P4REGPTR()->SetReExpandChgs( reExpandChgs ) )
				m_ErrorCount++;

		BOOL enablePendingChgsOtherClients = TRUE;
		if( m_EnablePendingChgsOtherClients.GetCheck() == 0 )
			enablePendingChgsOtherClients = FALSE;
		if( enablePendingChgsOtherClients != GET_P4REGPTR()->GetEnablePendingChgsOtherClients( ) )
			if(!GET_P4REGPTR()->SetEnablePendingChgsOtherClients( enablePendingChgsOtherClients ) )
				m_ErrorCount++;

		BOOL allState=TRUE;
		if(m_ChangeCountRadio.GetCheck() == 0)
            allState=FALSE;
        if( allState != GET_P4REGPTR()->GetFetchAllChanges() )
			if(!GET_P4REGPTR()->SetFetchAllChanges( allState) )
				m_ErrorCount++;

		if ((m_OldChangeCount != GET_P4REGPTR()->GetFetchChangeCount()) 
		 || (!m_OldChangeCount && !allState))
		{
			if (!m_OldChangeCount && !allState)
				 m_OldChangeCount = 1;
			if(!GET_P4REGPTR()->SetFetchChangeCount(m_OldChangeCount))
                m_ErrorCount++;
		}

		if ((m_UseLongChglistDesc != GET_P4REGPTR()->GetUseLongChglistDesc()) 
		 || (m_UseLongChglistDesc < 31) || (m_UseLongChglistDesc > 250))
		{
			if (m_UseLongChglistDesc < 31)
				m_UseLongChglistDesc = 31;
			else if (m_UseLongChglistDesc > 250)
				m_UseLongChglistDesc = 250;
			if(!GET_P4REGPTR()->SetUseLongChglistDesc(m_UseLongChglistDesc))
                m_ErrorCount++;
		}

		if(m_ErrorCount)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
	}		
}
