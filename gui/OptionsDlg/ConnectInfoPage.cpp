//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// ConnectInfoPage.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "ConnectInfoPage.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectInfoPage property page

IMPLEMENT_DYNCREATE(CConnectInfoPage, CPropertyPage)

CConnectInfoPage::CConnectInfoPage() : CPropertyPage(CConnectInfoPage::IDD)
{
	//{{AFX_DATA_INIT(CConnectInfoPage)
	m_AutoPollTime = 0;
	m_ReloadUncoverTime = 1;
	m_P4Port = _T("");
	m_P4Charset = _T("");
	//}}AFX_DATA_INIT
	m_Inited = FALSE;
}

CConnectInfoPage::~CConnectInfoPage()
{
}

void CConnectInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectInfoPage)
	DDX_Control(pDX, IDC_RELOADUNCOVER, m_ReloadOnUncover);
	DDX_Control(pDX, IDC_POLLJOBS, m_PollJobs);
	DDX_Control(pDX, IDC_OKICONIC, m_PollIconic);
	DDX_Control(pDX, IDC_P4AUTOPOLL, m_AutoPoll);
	DDX_Text(pDX, IDC_P4AUTOPOLLTIME, m_AutoPollTime);
	DDV_MinMaxInt(pDX, m_AutoPollTime, 0, 1440);
	DDX_Text(pDX, IDC_RELOADUNCOVERTIME, m_ReloadUncoverTime);
	DDV_MinMaxInt(pDX, m_ReloadUncoverTime, 0, 1440);
	DDX_Control(pDX, IDC_P4PORT, m_PortCombo);
	DDX_Control(pDX, IDC_P4CHARSET, m_CharsetCombo);
	DDX_Control(pDX, IDC_P4SHOWCONNECT, m_ShowConnectSettings);
	DDX_Control(pDX, IDC_SHOWPORT1ST, m_ShowConnectPort1st);
	DDX_Control(pDX, IDC_LOGOUTATEXIT, m_LogoutOnExit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConnectInfoPage, CPropertyPage)
	//{{AFX_MSG_MAP(CConnectInfoPage)
	ON_BN_CLICKED(IDC_P4SHOWCONNECT, OnShowConnectSettings)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_P4AUTOPOLL, OnP4autopoll)
	ON_BN_CLICKED(IDC_RELOADUNCOVER, OnP4ReloadOnUncover)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectInfoPage message handlers


BOOL CConnectInfoPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CString str;

	// Init all data members from the registry
    m_AutoPollTime = GET_P4REGPTR()->GetAutoPollTime();
    m_ReloadUncoverTime = GET_P4REGPTR()->GetReloadUncoverTime();


    // Get the current port setting
    m_OrigPort= m_P4Port = GET_P4REGPTR()->GetP4Port();
    
	// Make sure we have the current port setting first in the combo box
	if (m_P4Port.GetLength() > 0)
		GET_P4REGPTR()->AddMRUPort( m_P4Port, GET_P4REGPTR()->GetP4Charset() );

	// Make sure the combo is empty
	m_PortCombo.ResetContent();

	// Load all MRU ports into list box
	for(int i=0; i < MAX_MRU_PORTS; i++)
	{
		if(GET_P4REGPTR()->GetMRUPort(i).GetLength() > 0)
		{
			str = GET_P4REGPTR()->GetMRUPort(i);
			if( str.Find(_T(" ")) != -1 )
				str = _T("\"") + str + _T("\"");
			m_PortCombo.AddString(str);
		}
	}

	// Select the first item
	m_PortCombo.SetCurSel(0);

	
    // Get the current charset setting
    m_OrigCharset= m_P4Charset = GET_P4REGPTR()->GetP4Charset();
    
	// Make sure the combo is empty
	m_CharsetCombo.ResetContent();

	// Make sure we have the current charset setting first in the combo box
	if (m_P4Charset.GetLength() > 0)
		GET_P4REGPTR()->AddMRUCharset( m_P4Charset );
	else
		m_CharsetCombo.AddString(_T(""));

	// Load all MRU charsets into list box
	for(int i=0; i < MAX_MRU_CHARSETS; i++)
	{
		if(GET_P4REGPTR()->GetMRUCharset(i).GetLength() > 0)
		{
			str = GET_P4REGPTR()->GetMRUCharset(i);
			if( str.Find(_T(" ")) != -1 )
				str = _T("\"") + str + _T("\"");
			m_CharsetCombo.AddString(str);
		}
	}

	// Select the first item
	m_CharsetCombo.SetCurSel(0);

	
	// Enable/disable poll time per m_AutoPoll state
	m_AutoPoll.SetCheck(GET_P4REGPTR()->GetAutoPoll()); 
	m_PollJobs.SetCheck(GET_P4REGPTR()->GetAutoPollJobs());
	m_PollIconic.SetCheck(GET_P4REGPTR()->GetAutoPollIconic()); 
	m_ReloadOnUncover.SetCheck(GET_P4REGPTR()->GetReloadOnUncover()); 
	OnP4autopoll();
	OnP4ReloadOnUncover();

	m_ShowConnectSettings.SetCheck(GET_P4REGPTR()->ShowConnectSettings()); 
	m_ShowConnectPort1st.SetCheck(GET_P4REGPTR()->ShowConnectPort1st()); 
	OnShowConnectSettings();

	m_LogoutOnExit.SetCheck(GET_P4REGPTR()->GetLogoutOnExit()); 

	// Update input fields
	UpdateData(FALSE);
	
	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConnectInfoPage::OnP4autopoll() 
{
	CWnd *pwnd1=GetDlgItem(IDC_P4AUTOPOLLTIME);
	CWnd *pwnd2=GetDlgItem(IDC_OKICONIC);
	CWnd *pwnd3=GetDlgItem(IDC_POLLJOBS);

	pwnd1->EnableWindow(m_AutoPoll.GetCheck()==1);
	pwnd2->EnableWindow(m_AutoPoll.GetCheck()==1);
	pwnd3->EnableWindow(m_AutoPoll.GetCheck()==1);
}

void CConnectInfoPage::OnP4ReloadOnUncover() 
{
	CWnd *pwnd=GetDlgItem(IDC_RELOADUNCOVERTIME);
	pwnd->EnableWindow(m_ReloadOnUncover.GetCheck()==1);
}


void CConnectInfoPage::OnShowConnectSettings() 
{
    m_ShowConnectPort1st.EnableWindow( m_ShowConnectSettings.GetCheck() );
}

/*
	_________________________________________________________________
*/

void CConnectInfoPage::OnOK() 
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
		if ( EditPort( ) == FALSE )
			return;

		//		did port change  or  is this to be the new permanent port?
		//
		if( m_OrigPort != m_P4Port || IsDlgButtonChecked(IDC_PERMANENT))
		{
			if (GET_P4REGPTR()->GetExpandFlag() == 1)
				GET_P4REGPTR()->AddMRUPcuPath(MainFrame()->GetCurrentItemPath());

			// We always update the temp value and optionally update the perm value
			if(!GET_P4REGPTR()->SetP4Port(m_P4Port,TRUE,IsDlgButtonChecked(IDC_PERMANENT),TRUE))
			{
				AfxMessageBox( IDS_UNABLE_TO_WRITE_P4PORT_TO_THE_REGISTRY, MB_ICONEXCLAMATION);
				m_ErrorCount++;
			}
		}

		if ( EditCharset( ) == FALSE )
			return;

		//		did Charset change  or  is this to be the new permanent Charset?
		//
		if( m_OrigCharset != m_P4Charset || IsDlgButtonChecked(IDC_PERMANENT))
		{
			if (GET_P4REGPTR()->GetExpandFlag() == 1)
				GET_P4REGPTR()->AddMRUPcuPath(MainFrame()->GetCurrentItemPath());

			// We always update the temp value and optionally update the perm value
			if(!GET_P4REGPTR()->SetP4Charset(m_P4Charset,TRUE,IsDlgButtonChecked(IDC_PERMANENT),TRUE))
			{
				AfxMessageBox( IDS_UNABLE_TO_WRITE_P4CHARSET_TO_THE_REGISTRY, MB_ICONEXCLAMATION);
				m_ErrorCount++;
			}
		}

		BOOL autoState=TRUE;
		if(m_AutoPoll.GetCheck() == 0)
			autoState=FALSE;
		if( autoState != GET_P4REGPTR()->GetAutoPoll() )
			if(!GET_P4REGPTR()->SetAutoPoll( autoState) )
				m_ErrorCount++;

		if( m_AutoPollTime < 1)
			m_AutoPollTime = 1;
		if( m_AutoPollTime != GET_P4REGPTR()->GetAutoPollTime() )
			if(!GET_P4REGPTR()->SetAutoPollTime( m_AutoPollTime) )
				m_ErrorCount++;

		BOOL jobsState=TRUE;	
		if(m_PollJobs.GetCheck() == 0)
			jobsState=FALSE;
		if( jobsState != GET_P4REGPTR()->GetAutoPollJobs() )
			if(!GET_P4REGPTR()->SetAutoPollJobs( jobsState) )
				m_ErrorCount++;

		BOOL iconicState=TRUE;	
		if(m_PollIconic.GetCheck() == 0)
			iconicState=FALSE;
		if( iconicState != GET_P4REGPTR()->GetAutoPollIconic() )
			if(!GET_P4REGPTR()->SetAutoPollIconic( iconicState) )
				m_ErrorCount++;

		BOOL reloadOnUncover=TRUE;
		if(m_ReloadOnUncover.GetCheck() == 0)
			reloadOnUncover=FALSE;
		if( reloadOnUncover != GET_P4REGPTR()->GetReloadOnUncover() )
			if(!GET_P4REGPTR()->SetReloadOnUncover( reloadOnUncover) )
				m_ErrorCount++;

		if( m_ReloadUncoverTime < 0)
			m_ReloadUncoverTime = 1;
		if( m_ReloadUncoverTime != GET_P4REGPTR()->GetReloadUncoverTime() )
			if(!GET_P4REGPTR()->SetReloadUncoverTime( m_ReloadUncoverTime) )
				m_ErrorCount++;

		BOOL showConnect=TRUE;
		if(m_ShowConnectSettings.GetCheck() == 0)
			showConnect=FALSE;
		if( showConnect != GET_P4REGPTR()->ShowConnectSettings() )
			if(!GET_P4REGPTR()->SetShowConnectSettings( showConnect ) )
				m_ErrorCount++;

		BOOL showConnectPort1st=TRUE;
		if(m_ShowConnectPort1st.GetCheck() == 0)
			showConnectPort1st=FALSE;
		if( showConnectPort1st != GET_P4REGPTR()->ShowConnectPort1st() )
			if(!GET_P4REGPTR()->SetShowConnectPort1st( showConnectPort1st ) )
				m_ErrorCount++;

		BOOL logoutOnExit=TRUE;
		if(m_LogoutOnExit.GetCheck() == 0)
			logoutOnExit=FALSE;
		if( logoutOnExit != GET_P4REGPTR()->GetLogoutOnExit() )
			if(!GET_P4REGPTR()->SetLogoutOnExit( logoutOnExit) )
				m_ErrorCount++;

		if(m_ErrorCount)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
	}		
}

BOOL CConnectInfoPage::EditPort()
{
	if(m_PortCombo.GetCurSel() != CB_ERR)
		m_PortCombo.GetLBText(m_PortCombo.GetCurSel(), m_P4Port);
	else
		m_PortCombo.GetWindowText( m_P4Port );

	m_P4Port.TrimRight();
	m_P4Port.TrimLeft();

	if (!m_P4Port.GetLength())	// if the port field is empty, put back the original port
		m_P4Port = m_OrigPort;

	if( m_P4Port.GetAt(0) == _T('\"'))
	{
		m_P4Port.TrimRight(_T('\"'));
		m_P4Port.TrimLeft(_T('\"'));
	}
	if (m_P4Port.GetLength() > 0)
		GET_P4REGPTR()->AddMRUPort( m_P4Port, GET_P4REGPTR()->GetP4Charset() );
	return TRUE;
}

BOOL CConnectInfoPage::EditCharset()
{
	if(m_CharsetCombo.GetCurSel() != CB_ERR)
		m_CharsetCombo.GetLBText(m_CharsetCombo.GetCurSel(), m_P4Charset);
	else
		m_CharsetCombo.GetWindowText( m_P4Charset );

	m_P4Charset.TrimRight();
	m_P4Charset.TrimLeft();

	if( m_P4Charset.GetAt(0) == _T('\"'))
	{
		m_P4Charset.TrimRight(_T('\"'));
		m_P4Charset.TrimLeft(_T('\"'));
	}
	else if( m_P4Charset.Find(_T(" ")) != -1 )
	{	
		m_ErrorCount++;
		AfxMessageBox( IDS_EMBEDDED_SPACES_NOT_ALLOWED_IN_P4CHARSET_ENTRY, MB_ICONSTOP );
		return FALSE;
	}
	if (m_P4Charset.GetLength() > 0)
		GET_P4REGPTR()->AddMRUCharset( m_P4Charset );
	return TRUE;
}
