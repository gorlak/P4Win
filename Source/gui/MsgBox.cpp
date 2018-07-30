// MsgBox.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "MsgBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgBox dialog


CMsgBox::CMsgBox(CString text, UINT nType /*=1*/, int cancelButton /*=0*/, 
				 CWnd* pParent /*=NULL*/, BOOL *lpBdontShow /*= NULL*/, BOOL bShowVscroll /*= FALSE*/)
	: CDialog(CMsgBox::IDD, pParent), m_cancelButton(cancelButton)
{

	//{{AFX_DATA_INIT(CMsgBox)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_InitRect = CRect(0,0,0,0);

	// the string provided should be delimited by tabs, with button labels
    // first, followed by the prompt string.  First, extract the button labels
    m_numButtons = 0;
	m_DontShowAgain = 0;
	m_ShowVscroll = bShowVscroll;
	m_lpDontShowAgain = lpBdontShow;
    for(int nextTab = text.Find(_T('\t')); nextTab != -1; nextTab = text.Find(_T('\t')))
    {
        switch(m_numButtons)
        {
        case 0:
            m_Button1 = text.Left(nextTab);
            m_numButtons++;
            break;
        case 1:
            m_Button2 = text.Left(nextTab);
            m_numButtons++;
            break;
        case 2:
            m_Button3 = text.Left(nextTab);
            m_numButtons++;
            break;
        default:
            ASSERT(0);
            // too many buttons specified - ignore them
        }
        text = text.Mid(nextTab+1);
    }

    // what's left in string 'labels' now is just the prompt string

	if (text.GetLength() < 150)
		 text = "\n" + text;
    m_Text = text;

    switch(nType & MB_DEFMASK)
    {
    case MB_DEFBUTTON2:
        m_DefButton = IDC_BUTTON2;
        break;
    case MB_DEFBUTTON3:
		m_DefButton = IDC_BUTTON3;
        break;
    default:
		m_DefButton = m_numButtons == 1 ? IDC_BUTTON2 : IDC_BUTTON1;
    }

    int nIDCaption = 0;
    switch(nType & MB_ICONMASK)
    {
    case MB_ICONHAND:
        m_Icon = IDI_HAND;
        nIDCaption = IDS_ERROR;
        break;
    case MB_ICONQUESTION:
        m_Icon = IDI_QUESTION;
        break;
    case MB_ICONEXCLAMATION:
        m_Icon = IDI_EXCLAMATION;
        nIDCaption = IDS_WARNING;
        break;
    case MB_ICONASTERISK:
        m_Icon = IDI_ASTERISK;
        break;
    default:
        m_Icon = 0;
    }

    m_Caption = TheApp()->m_pszAppName;
    if(nIDCaption)
        m_Caption += _T(" ") + LoadStringResource(nIDCaption);
}


void CMsgBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMsgBox)
	DDX_Check(pDX, IDC_CHECK_DONT_SHOW_AGAIN, m_DontShowAgain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMsgBox, CDialog)
	//{{AFX_MSG_MAP(CMsgBox)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(ID_CANCEL, OnCancel)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsgBox message handlers

BOOL CMsgBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetWindowRect(&m_InitRect);
	m_LastRect = m_InitRect;

	if (m_ShowVscroll)
	{
		DWORD dwStyle = GetWindowLong(GetDlgItem(IDC_MESSAGE)->m_hWnd, GWL_STYLE);
		SetWindowLong(GetDlgItem(IDC_MESSAGE)->m_hWnd, GWL_STYLE, dwStyle | WS_VSCROLL);
		GetDlgItem(IDC_MESSAGE)->SetWindowPos(0,0,0,0,0, 
			SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

		dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | WS_SYSMENU | WS_MAXIMIZEBOX | WS_THICKFRAME);
		SetWindowPos(0,0,0,0,0, SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
	}

	m_Text.Replace(_T("\n"), _T("\r\n"));
	SetDlgItemText( IDC_MESSAGE, m_Text );
	SetWindowText( m_Caption );

	if (m_Icon)
	{
		HICON hIcon = AfxGetApp()->LoadStandardIcon(m_Icon);
		CStatic * pStatic = (CStatic *)GetDlgItem(IDC_MSGICON);
		pStatic->SetIcon( hIcon );
	}

	switch (m_numButtons)
	{
	default:
	case 1:
		SetDlgItemText( IDC_BUTTON2, m_Button1 );
		GetDlgItem(IDC_BUTTON1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON3)->ShowWindow(SW_HIDE);
		GotoDlgCtrl( GetDlgItem(IDC_BUTTON2) );
		break;

	case 2:
		SetDlgItemText( IDC_BUTTON1, m_Button1 );
		SetDlgItemText( IDC_BUTTON3, m_Button2 );
		GetDlgItem(IDC_BUTTON2)->ShowWindow(SW_HIDE);
		GotoDlgCtrl( GetDlgItem(m_DefButton == IDC_BUTTON1 ? IDC_BUTTON1 : IDC_BUTTON3) );
		break;

	case 3:
		SetDlgItemText( IDC_BUTTON1, m_Button1 );
		SetDlgItemText( IDC_BUTTON2, m_Button2 );
		SetDlgItemText( IDC_BUTTON3, m_Button3 );
		GotoDlgCtrl( GetDlgItem(m_DefButton) );
		break;
	}

	if (m_lpDontShowAgain)
	{
		GetDlgItem(IDC_CHECK_DONT_SHOW_AGAIN)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_CHECK_DONT_SHOW_AGAIN)->EnableWindow(TRUE);
	}
	
	UpdateData(FALSE);
	return FALSE;
}

void CMsgBox::OnButton1() 
{
	UpdateData();
	CDialog::EndDialog(IDC_BUTTON1);
}

void CMsgBox::OnButton2() 
{
    // IDC_BUTTON2 is only used for 1 or 3-button box
    ASSERT(m_numButtons == 3 || m_numButtons == 1);
	UpdateData();
	CDialog::EndDialog(IDC_BUTTON2);
}

void CMsgBox::OnButton3() 
{
    // IDC_BUTTON3 is used for button2 with 2-button box
    // and for button3 with 3-button box
    // but not at with 1-button box
    ASSERT(m_numButtons != 1);
	UpdateData();
    CDialog::EndDialog((m_numButtons == 2) ? IDC_BUTTON2 : (m_numButtons == 3 ? IDC_BUTTON3 : IDC_BUTTON1));
}

void CMsgBox::OnCancel()
{
	if (m_cancelButton)
		CDialog::EndDialog(m_cancelButton);
	else
		MessageBeep(0);
}

void CMsgBox::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_InitRect.Height())
	{
		int w = m_InitRect.Width();
		lpMMI->ptMinTrackSize.x= w;
		lpMMI->ptMaxTrackSize.x= w * 3 / 2;
		lpMMI->ptMinTrackSize.y= m_InitRect.Height();
	}
}

void CMsgBox::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	CWnd *pWnd=GetDlgItem(IDC_MESSAGE);
	if (!pWnd || !IsWindow(pWnd->m_hWnd))
		return;

	// Compute the change in width (dx) & height (dy)
	CRect rect;
	GetWindowRect(&rect);
	int dx = rect.Width()  - m_LastRect.Width();
	int dy = rect.Height() - m_LastRect.Height();
	// Save the new size
	m_LastRect = rect;

	// Widen the message box by the change in width (dx)
	// Lengthen the message box by the change in height (dy)
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top + dy, SWP_NOMOVE | SWP_NOZORDER);

	// Slide the chkbox & buttons right by 1/2 the change in width
	// & down by the change in height
	pWnd = GetDlgItem(IDC_CHECK_DONT_SHOW_AGAIN);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx/2, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDC_BUTTON1);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx/2, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDC_BUTTON2);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx/2, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDC_BUTTON3);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx/2, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);
}
