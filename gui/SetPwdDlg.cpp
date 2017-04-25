// SetPwdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "SetPwdDlg.h"
#include "cmd_password.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetPwdDlg dialog


CSetPwdDlg::CSetPwdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetPwdDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetPwdDlg)
	m_NewPwd = _T("");
	m_NewPwd2 = _T("");
	m_OldPwd = _T("");
	m_RememberPSW = TRUE;
	//}}AFX_DATA_INIT
	m_bLogin = FALSE;
	m_Key = 0;
}


void CSetPwdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetPwdDlg)
	DDX_Text(pDX, IDC_PWDNEW, m_NewPwd);
	DDV_MaxChars(pDX, m_NewPwd, 32);
	DDX_Text(pDX, IDC_PWDNEW2, m_NewPwd2);
	DDV_MaxChars(pDX, m_NewPwd2, 32);
	DDX_Text(pDX, IDC_PWDOLD, m_OldPwd);
	DDV_MaxChars(pDX, m_OldPwd, 32);
	DDX_Check(pDX, IDC_REMBRPSW, m_RememberPSW);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetPwdDlg, CDialog)
	//{{AFX_MSG_MAP(CSetPwdDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetPwdDlg message handlers

BOOL CSetPwdDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_bLogin || GET_SECURITYLEVEL() >= 2)
	{
		m_RememberPSW = FALSE;
		GetDlgItem(IDC_REMBRPSW)->EnableWindow(FALSE);
		GetDlgItem(IDC_REMBRPSW)->ShowWindow(SW_HIDE);
	}

	if (!m_Caption.IsEmpty())
		SetWindowText(m_Caption);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetPwdDlg::OnOK() 
{
	UpdateData(TRUE);

    if( !CCmd_Password::PrepPassword( m_OldPwd ) ||
        !CCmd_Password::PrepPassword( m_NewPwd ) ||
        !CCmd_Password::PrepPassword( m_NewPwd2 ))
	{
		AfxMessageBox(LoadStringResource(IDS_PASSWORD_MAY_NOT_CONTAIN__SPACES_OR_POUNDSIGN) + 
                      LoadStringResource(IDS_LF_PLEASE_TRY_AGAIN), 
					  MB_ICONEXCLAMATION);
		return;
	}
	else if( m_NewPwd.Compare(m_NewPwd2) != 0 )
	{
		AfxMessageBox(LoadStringResource(IDS_NEW_PASSWORD_DOES_NOT_MATCH_CONFIRMATION_PASSWORD) + 
                      LoadStringResource(IDS_LF_PLEASE_TRY_AGAIN), 
					  MB_ICONEXCLAMATION);
		return;
	}

	BOOL b = FALSE;
	if (m_OldPwd != m_NewPwd)
	{
		CCmd_Password cmd;
		cmd.Init( NULL, FALSE, m_Key ? TRUE : FALSE, m_Key );
		if( cmd.Run( m_OldPwd, m_NewPwd ) )
		{
			CString txt= cmd.GetErrorText();
			if( !txt.IsEmpty() && txt.Find(_T("Password deleted.")) != 0)
			{
				AfxMessageBox(txt + LoadStringResource(IDS_LF_PLEASE_TRY_AGAIN), 
					MB_ICONEXCLAMATION);
				return;
			}
			// Make sure our temporary copy is updated;
			// bug don't get encrypted version for p4 login
			if (!m_bLogin)
				m_NewPwd = GET_P4REGPTR()->GetPasswordfromReg( );	// get the encrypted version
			GET_P4REGPTR()->SetP4Password( m_NewPwd, TRUE, FALSE, FALSE );
		}
		b = TRUE;
	}
	if (b || (m_OldPwd == m_NewPwd))
	{
		if (!m_RememberPSW)
		{
			HKEY	hkKEY;

			RegOpenKeyEx(HKEY_CURRENT_USER,_T("Software\\Perforce\\environment"),0,KEY_WRITE,&hkKEY);
			RegDeleteValue(hkKEY, _T("P4PASSWD"));
			RegCloseKey(hkKEY);
			if (!m_bLogin)
				AddToStatus(LoadStringResource(IDS_PSWD_FORGOTTEN));
			b = TRUE;
		}

		// And get out of here
		if (b)
			CDialog::OnOK();
	}
	if (!b)
	{
		AfxMessageBox(LoadStringResource(IDS_OLD_NEW_PSWD_SAME)
					+ LoadStringResource(IDS_LF_PLEASE_TRY_AGAIN), 
						MB_ICONEXCLAMATION);
	}
}

