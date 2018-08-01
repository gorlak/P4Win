//		NewClientDlg.cpp : implementation file
//
//		this is a very badly named file, since it puts up a dialogue
//		for new clients AND NEW USERS.
//
//		my humble apologies.
//
//		(signed) Francesca Nudo, who cannot plan ahead.
//

#include "stdafx.h"
#include "p4win.h"
#include "NewClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewClientDlg dialog



CNewClientDlg::CNewClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewClientDlg)
	m_Name = _T("");
	m_RunWizard = FALSE;
	//}}AFX_DATA_INIT
	m_Type = DLG_NEW;
}


void CNewClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewClientDlg)
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDV_MaxChars(pDX, m_Name, 1024);
	DDX_Check(pDX, IDC_WIZARD, m_RunWizard);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewClientDlg, CDialog)
	//{{AFX_MSG_MAP(CNewClientDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewClientDlg message handlers

CString CNewClientDlg::GetName()
{
	m_Name.TrimRight( );
	m_Name.TrimLeft( );
	return m_Name;
}


/*
	_________________________________________________________________
	
	don't allow embedded spaces in the client or user name.
	_________________________________________________________________
*/

void CNewClientDlg::OnOK() 
{
	UpdateData( );
	
	if( (m_Name.Find(_T(" ")) != -1) && (m_What != NEWFILESPEC) )
	{	
		CString msg = m_What;
		msg.MakeLower( );
        msg.FormatMessage(IDS_PLEASE_ENTER_A_s_NAME_WITH_NO_EMBEDDED_SPACES, msg);
		AfxMessageBox( msg, MB_ICONSTOP );
	}
	else if( m_Name == m_Active && !m_Active.IsEmpty() && (m_What != NEWFILESPEC) )
	{	
		CString msg = m_What;
		msg.MakeLower( );
        msg.FormatMessage(IDS_CANNOT_RECREATE_ACTIVE_s_s_s, m_Active, msg, msg);
		AfxMessageBox( msg, MB_ICONSTOP );
	}
	else	
		CDialog::OnOK();
}


/*
	_________________________________________________________________

	doctor the dialogue, depending on whether the user wants a new
	client or a new user. 
	_________________________________________________________________
*/

BOOL CNewClientDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    CString txt;
	if (m_Type == DLG_NEW)
		txt.FormatMessage(IDS_CREATE_NEW_s, m_What);
	else if (m_Type == DLG_FILTER)
		txt.FormatMessage(IDS_ENTER_s_TO_USE_AS_FILTER, m_What);
	else
		txt = m_What;
	SetWindowText( txt );	
	GetDlgItem( IDC_STATICNAME )->SetWindowText( CString ( _T("&") ) + m_What  + _T(":"));
	if (m_What == NEWCLIENT)
	{
		GetDlgItem(IDC_WIZARD)->EnableWindow(TRUE);
		GetDlgItem(IDC_WIZARD)->ShowWindow(SW_NORMAL);
	}
	GotoDlgCtrl( GetDlgItem( IDC_NAME ) );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/*
	_________________________________________________________________

	since creating new clients and new users come through this 
	dialogue (see apology supra) we have to know which of the two
	we've got. the strings are #defined in newclientdlg.h.
	_________________________________________________________________
*/

void CNewClientDlg::SetNew( const CString &type )
{
	m_What = type;
	ASSERT ( m_What == NEWCLIENT || m_What == NEWUSER  || m_What == NEWBRANCH 
								 || m_What == NEWLABEL || m_What == NEWFILESPEC);
}


/*
	_________________________________________________________________

	make sure i call SetNew( ) before i call domodal by asserting.
	_________________________________________________________________
*/

INT_PTR CNewClientDlg::DoModal() 
{
	ASSERT ( !m_What.IsEmpty( ) );
	return CDialog::DoModal();
}

BOOL CNewClientDlg::SwitchTo()
{
	return TRUE;
}


