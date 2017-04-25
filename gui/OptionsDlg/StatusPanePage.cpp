//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// StatusPanePage.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "StatusPanePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusPanePage property page

IMPLEMENT_DYNCREATE(CStatusPanePage, CPropertyPage)

CStatusPanePage::CStatusPanePage() : CPropertyPage(CStatusPanePage::IDD)
{
	//{{AFX_DATA_INIT(CStatusPanePage)
	//}}AFX_DATA_INIT
	m_Inited = FALSE;
}

CStatusPanePage::~CStatusPanePage()
{
}

void CStatusPanePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatusPanePage)
	DDX_Control(pDX, IDC_SHOWSTATUSTIME, m_ShowStatusTime);
	DDX_Control(pDX, IDC_USE24HOURCLOCK, m_Use24hourClock);
	DDX_Control(pDX, IDC_P4SHOWCOMMANDTRACE, m_ShowCommandTrace);
	DDX_Control(pDX, IDC_SHOW_STATUSMSGS, m_ShowStatusMsgs);
	DDX_Control(pDX, IDC_SHOWTRUNCTOOLTIP, m_ShowTruncTooltip);
	DDX_Control(pDX, IDC_USE_NOTEPAD, m_UseNotepad);
	DDX_Text(pDX, IDC_MAXSTATUSLINES, m_MaxStatusLines);
	DDX_Control(pDX, IDC_ALLOW_ER, m_AllowExceptionReporting);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatusPanePage, CPropertyPage)
	//{{AFX_MSG_MAP(CStatusPanePage)
	ON_BN_CLICKED(IDC_SETTODEFAULT, OnSettodefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusPanePage message handlers

void CStatusPanePage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	int errors=0;

	if(UpdateData(TRUE))
	{
		BOOL showCommandTrace = TRUE;
		if( m_ShowCommandTrace.GetCheck() == 0 )
			showCommandTrace = FALSE;
		if( showCommandTrace != GET_P4REGPTR()->ShowCommandTrace( ) )
			if(!GET_P4REGPTR()->SetShowCommandTrace( showCommandTrace ) )
				errors++;

		BOOL showStatusTime = TRUE;
		if( m_ShowStatusTime.GetCheck() == 0 )
			showStatusTime = FALSE;
		if( showStatusTime != GET_P4REGPTR()->ShowStatusTime( ) )
			if(!GET_P4REGPTR()->SetShowStatusTime( showStatusTime ) )
				errors++;

		BOOL use24hourClock = TRUE;
		if( m_Use24hourClock.GetCheck() == 0 )
			use24hourClock = FALSE;
		if( use24hourClock != GET_P4REGPTR()->Use24hourClock( ) )
			if(!GET_P4REGPTR()->SetUse24hourClock( use24hourClock ) )
				errors++;

		BOOL showStatusMsgs = TRUE;
		if( m_ShowStatusMsgs.GetCheck() == 0 )
			showStatusMsgs = FALSE;
		if( showStatusMsgs != GET_P4REGPTR()->GetShowStatusMsgs( ) )
			if(!GET_P4REGPTR()->SetShowStatusMsgs( showStatusMsgs ) )
				errors++;

		BOOL showTruncTooltip = TRUE;
		if( m_ShowTruncTooltip.GetCheck() == 0 )
			showTruncTooltip = FALSE;
		if( showTruncTooltip != GET_P4REGPTR()->GetShowTruncTooltip( ) )
			if(!GET_P4REGPTR()->SetShowTruncTooltip( showTruncTooltip ) )
				errors++;

		BOOL useNotepad = TRUE;
		if( m_UseNotepad.GetCheck() == 0 )
			useNotepad = FALSE;
		if( useNotepad != GET_P4REGPTR()->UseNotepad4WarnAndErr( ) )
			if(!GET_P4REGPTR()->SetUseNotepad4WarnAndErr( useNotepad ) )
				errors++;

		if(m_MaxStatusLines != GET_P4REGPTR()->GetMaxStatusLines())
			if(!GET_P4REGPTR()->SetMaxStatusLines( m_MaxStatusLines ) )
				errors++;

		BOOL allowExceptionReporting = TRUE;
		if( m_AllowExceptionReporting.GetCheck() == 0 )
			allowExceptionReporting = FALSE;
		if( allowExceptionReporting != GET_P4REGPTR()->AllowExceptionReporting( ) )
			if(!GET_P4REGPTR()->SetAllowExceptionReporting( allowExceptionReporting ) )
				errors++;

		if(errors)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
	}
}

BOOL CStatusPanePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_ShowCommandTrace.SetCheck(GET_P4REGPTR()->ShowCommandTrace()); 
	m_ShowStatusTime.SetCheck(GET_P4REGPTR()->ShowStatusTime()); 
	m_Use24hourClock.SetCheck(GET_P4REGPTR()->Use24hourClock()); 
	m_ShowStatusMsgs.SetCheck(GET_P4REGPTR()->GetShowStatusMsgs());
	m_ShowTruncTooltip.SetCheck(GET_P4REGPTR()->GetShowTruncTooltip());
	m_UseNotepad.SetCheck(GET_P4REGPTR()->UseNotepad4WarnAndErr());
	m_MaxStatusLines = GET_P4REGPTR()->GetMaxStatusLines();
	m_AllowExceptionReporting.SetCheck(GET_P4REGPTR()->AllowExceptionReporting());

	// Update input fields
	UpdateData(FALSE);

	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CStatusPanePage::OnSettodefault() 
{
	UpdateData(TRUE);
	m_MaxStatusLines = 5000;
	UpdateData(FALSE);
	GotoDlgCtrl(GetDlgItem(IDC_MAXSTATUSLINES));
}
