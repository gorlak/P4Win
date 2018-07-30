#include "stdafx.h"
#include "p4win.h"
#include "GetPwdDlg.h"
#include "MainFrm.h"
#include "cmd_password.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetPwdDlg dialog


CGetPwdDlg::CGetPwdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGetPwdDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetPwdDlg)
	m_IsPermanent = FALSE;
	m_Password = _T("");
	//}}AFX_DATA_INIT
	m_InitRect.SetRect(0,0,0,0);
	m_InitDialogDone = FALSE;
}


void CGetPwdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetPwdDlg)
	DDX_Check(pDX, IDC_CHECK1, m_IsPermanent);
	DDX_Text(pDX, IDC_PASSWORD, m_Password);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetPwdDlg, CDialog)
	//{{AFX_MSG_MAP(CGetPwdDlg)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetPwdDlg message handlers

void CGetPwdDlg::OnOK() 
{
	UpdateData( );

    if(!CCmd_Password::PrepPassword(m_Password))
	{
		AfxMessageBox(LoadStringResource(IDS_PASSWORD_MAY_NOT_CONTAIN__SPACES_OR_POUNDSIGN) + 
                      LoadStringResource(IDS_LF_PLEASE_TRY_AGAIN), 
					  MB_ICONEXCLAMATION);
		m_Password.Empty( );
		UpdateData( FALSE );
		return;
	}

	// Put the trimmed password back, so OnOK doesnt hose it out
	// by calling UpdateData(TRUE)
	UpdateData(FALSE);

	if (m_Timer)
		KillTimer(MISC_TIMER);
	CDialog::OnOK();
}


void CGetPwdDlg::OnCancel() 
{
	if (m_Timer)
		KillTimer(MISC_TIMER);
	CDialog::OnCancel();
}


CString CGetPwdDlg::GetPassword()
{
	return m_Password;
}

BOOL CGetPwdDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetWindowRect(&m_InitRect);
	m_LastRect = m_InitRect;

	while (!MainFrameCWnd)			// wait until MainFrame is intitialzed
		Sleep(100);
	CMainFrame * mainWnd = (CMainFrame *)AfxGetMainWnd();
	mainWnd->UpdateCaption();

	CString msg;
    msg.FormatMessage(IDS_PASSWORD_FOR_USER_s, GET_P4REGPTR()->GetP4User( ));
	GetDlgItem( IDC_STATICMSG )->SetWindowText( msg );

	if (GET_SERVERLEVEL() >= 18)
	{
		m_IsPermanent = FALSE;
		GetDlgItem(IDC_CHECK1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK1)->ShowWindow(SW_HIDE);
		// Set a timer so we can check for Caps Lock toggles
		m_Timer= SetTimer( MISC_TIMER, 200, NULL);
	}
	
	UpdateData(FALSE);
	return m_InitDialogDone = TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGetPwdDlg::OnTimer(UINT nIDEvent) 
{
	CDialog::OnTimer(nIDEvent);

	BYTE keystates[256];
	if (GetKeyboardState((PBYTE)&keystates))
		GetDlgItem(IDC_CAPSLOCK_MSG)->ShowWindow(keystates[VK_CAPITAL] ? SW_SHOW : SW_HIDE);
}

void CGetPwdDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if (!m_InitDialogDone)
		return;

	// Compute the change in width (dx)
	CRect rect;
	GetWindowRect(&rect);
	int dx = rect.Width() - m_LastRect.Width();
	// Save the new size
	m_LastRect = rect;

	// Widen the text fields by the change in width (dx) and
	GetDlgItem( IDC_STATICMSG )->GetWindowRect(&rect);
	GetDlgItem( IDC_STATICMSG )->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								    rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	GetDlgItem( IDC_STATICMSG )->InvalidateRect(NULL);

	GetDlgItem( IDC_PASSWORD )->GetWindowRect(&rect);
	GetDlgItem( IDC_PASSWORD )->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								    rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	GetDlgItem( IDC_PASSWORD )->InvalidateRect(NULL);

	// Slide the buttons to the right by the change in width
	CWnd *pWnd = GetDlgItem(IDOK);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDCANCEL);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);
}

void CGetPwdDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_InitRect.Height())
	{
		lpMMI->ptMinTrackSize.x = m_InitRect.Width();
		lpMMI->ptMinTrackSize.y = lpMMI->ptMaxTrackSize.y = m_InitRect.Height();
	}
}
