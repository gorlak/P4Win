// spec-dlgs\P4SpecSheet.cpp : implementation file
//

#include "stdafx.h"
#include "P4win.h"
#include "P4SpecSheet.h"
#include "P4Command.h"
#include "MainFrm.h"

// CP4SpecSheet dialog

IMPLEMENT_DYNAMIC(CP4SpecSheet, CDialog)
CP4SpecSheet::CP4SpecSheet(CWnd* pParent /*=NULL*/)
	: CDialog(CP4SpecSheet::IDD, pParent)
{
	m_pParent  = pParent;
	m_LastFocus= 0;
	m_BtnId[0] = ID_UPDATE;
	m_BtnId[1] = ID_EDITOR;
	m_BtnId[2] = ID_CANCEL;
	m_BtnId[3] = ID_HELPNOTES;
	m_BtnId[4] = ID_HELP;
	m_bShowReopen = m_bShowRevertUnchg = m_IsMinimized = FALSE;
	m_MainFRmSignaled = TRUE;
	m_Pad = 0;
	m_UnchangedFlag = 0;
}

CP4SpecSheet::~CP4SpecSheet()
{
	if (m_pParent && !m_MainFRmSignaled)
		::PostMessage(MainFrame()->m_hWnd, WM_P4DLGDESTROY, 0, (LPARAM)this);
}

void CP4SpecSheet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CP4SpecSheet)
	DDX_Control(pDX, ID_HELP, m_bHelpHelp);
	DDX_Control(pDX, ID_HELPNOTES, m_bHelp);
	DDX_Control(pDX, ID_EDITOR, m_bEditor);
	DDX_Control(pDX, ID_UPDATE, m_bOK);
	DDX_Control(pDX, ID_ACTION_1, m_bAlternate);
	DDX_Control(pDX, ID_CANCEL, m_bCancel);
	DDX_Control(pDX, IDC_REOPEN, m_bReopen);
	DDX_Radio(pDX, IDC_RADIOSUBMIT, m_UnchangedFlag);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CP4SpecSheet, CDialog)
	//{{AFX_MSG_MAP(CP4SpecSheet)
	ON_BN_CLICKED(IDOK, OnEnter)
	ON_BN_CLICKED(ID_UPDATE, On_OK)
	ON_BN_CLICKED(ID_ACTION_1, OnAlternate)
	ON_BN_CLICKED(ID_CANCEL, On_Cancel)
	ON_BN_CLICKED(ID_HELPNOTES, OnHelpnotes)
	ON_BN_CLICKED(ID_EDITOR, OnEditor)
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_BN_CLICKED(IDC_REOPEN, OnReopen)
	ON_BN_CLICKED(IDC_RADIOSUBMIT, OnRadioUnchg)
	ON_BN_CLICKED(IDC_RADIOREVERT, OnRadioUnchg)
	ON_BN_CLICKED(IDC_RADIOLEAVE, OnRadioUnchg)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_GETMINMAXINFO()
	ON_WM_SHOWWINDOW()
	ON_WM_HELPINFO()
 	ON_WM_CLOSE()
  	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MODIFYSTYLE, OnModifyStyle)
    ON_MESSAGE(WM_P4ENDDESCRIBE, OnP4EndHelpnotes )
	ON_MESSAGE(WM_QUITTING, OnQuitting )
	ON_MESSAGE(WM_ENABLEDISABLE, OnEnableDisable )
	ON_MESSAGE(WM_FORCEFOCUS, OnForceFocus )
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CP4SpecSheet message handlers

BOOL CP4SpecSheet::OnInitDialog() 
{
	CDialog::OnInitDialog();

 	if (m_pParent)
	{
  		MainFrame()->SetModelessWnd(this);
		m_MainFRmSignaled = FALSE;	// indicate that we need to let MainFrame know when we end
	}
	else
	{
		long style = GetWindowLong(m_hWnd, GWL_STYLE);
		style ^= WS_MINIMIZEBOX;
		SetWindowLong(m_hWnd, GWL_STYLE, style);
	}

	// For Submit forms, we need the value of m_UnchangedFlag before we call m_P4SpecDlg.Create();
	// so compute it here and show/hide the Reopen checkbox & Unchanged radio buttons.
	if( m_P4SpecDlg.m_SpecType == P4CHANGE_SPEC && m_P4SpecDlg.m_AllowSubmit )
	{
		if (GET_SERVERLEVEL() >= 13)
		{
			m_bReopen.SetCheck(GET_P4REGPTR()->GetAutoReopen());
			m_bShowReopen = TRUE;
			if (GET_SERVERLEVEL() >= 21)	// 2006.1 or later?
			{
				if (GET_P4REGPTR()->UseClientSpecSubmitOpts() // do they want to use cli spec opts?
				 && TheApp( )->m_ClientSubOpts && GET_SERVERLEVEL() >= 22)	// 2006.2 or later?
				{
					switch(TheApp( )->m_ClientSubOpts)
					{
					case SUBMITUNCHANGED:
					case SUBMITUNCHANGED_REOPEN:
						m_UnchangedFlag = 0;
						break;

					case REVERTUNCHANGED:
					case REVERTUNCHANGED_REOPEN:
						m_UnchangedFlag = 1;
						break;

					default:
					case LEAVEUNCHANGED:
					case LEAVEUNCHANGED_REOPEN:
						m_UnchangedFlag = 2;
						break;
					}
					m_bReopen.SetCheck(TheApp( )->m_ClientSubOpts > REOPEN_MASK);
				}
				else
				{
					if (!GET_P4REGPTR()->GetSubmitOnlyChged())
						m_UnchangedFlag = 0;
					else
						m_UnchangedFlag = GET_P4REGPTR()->GetRevertUnchged() ? 1 : 2;
				}
				m_bShowRevertUnchg = TRUE;
			}
			else
			{
				GetDlgItem( IDC_STATICUNCHANGED )->ShowWindow(SW_HIDE);
				GetDlgItem( IDC_RADIOSUBMIT )->EnableWindow(FALSE);
				GetDlgItem( IDC_RADIOSUBMIT )->ShowWindow(SW_HIDE);
				GetDlgItem( IDC_RADIOREVERT )->EnableWindow(FALSE);
				GetDlgItem( IDC_RADIOREVERT )->ShowWindow(SW_HIDE);
				GetDlgItem( IDC_RADIOLEAVE )->EnableWindow(FALSE);
				GetDlgItem( IDC_RADIOLEAVE )->ShowWindow(SW_HIDE);
				m_UnchangedFlag = GET_P4REGPTR()->GetSubmitOnlyChged() ? 2 : 0;
			}
		}
		else
		{
			m_bReopen.SetCheck(FALSE);
			GetDlgItem( IDC_REOPEN )->EnableWindow(FALSE);
			GetDlgItem( IDC_REOPEN )->ShowWindow(SW_HIDE);
			GetDlgItem( IDC_STATICUNCHANGED )->ShowWindow(SW_HIDE);
			GetDlgItem( IDC_RADIOSUBMIT )->EnableWindow(FALSE);
			GetDlgItem( IDC_RADIOSUBMIT )->ShowWindow(SW_HIDE);
			GetDlgItem( IDC_RADIOREVERT )->EnableWindow(FALSE);
			GetDlgItem( IDC_RADIOREVERT )->ShowWindow(SW_HIDE);
			GetDlgItem( IDC_RADIOLEAVE )->EnableWindow(FALSE);
			GetDlgItem( IDC_RADIOLEAVE )->ShowWindow(SW_HIDE);
			m_UnchangedFlag = GET_P4REGPTR()->GetSubmitOnlyChged() ? 2 : 0;
		}
	}
	else
	{
		GetDlgItem( IDC_REOPEN )->EnableWindow(FALSE);
		GetDlgItem( IDC_REOPEN )->ShowWindow(SW_HIDE);
		GetDlgItem( IDC_STATICUNCHANGED )->ShowWindow(SW_HIDE);
		GetDlgItem( IDC_RADIOSUBMIT )->EnableWindow(FALSE);
		GetDlgItem( IDC_RADIOSUBMIT )->ShowWindow(SW_HIDE);
		GetDlgItem( IDC_RADIOREVERT )->EnableWindow(FALSE);
		GetDlgItem( IDC_RADIOREVERT )->ShowWindow(SW_HIDE);
		GetDlgItem( IDC_RADIOLEAVE )->EnableWindow(FALSE);
		GetDlgItem( IDC_RADIOLEAVE )->ShowWindow(SW_HIDE);
		GetDlgItem( ID_ACTION_1 )->EnableWindow(FALSE);
		GetDlgItem( ID_ACTION_1 )->ShowWindow(SW_HIDE);
		SendMessage(DM_SETDEFID, ID_UPDATE, 0);
	}

	CRect clientRect, newClientRect, windowRect, newWindowRect;

	// Create the SpecDlg property page - this runs CP4SpecDlg::OnInitDialog()
	m_P4SpecDlg.m_UnchangedFlag = m_UnchangedFlag;
	m_P4SpecDlg.m_WinPos.SetWindowPtr(this);
	m_P4SpecDlg.Create(m_P4SpecDlg.m_psp.pszTemplate, this);
	m_P4SpecDlg.SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOSIZE);

	// Change the font in the dialog & on the buttons
	SetFont(&m_P4SpecDlg.m_Font);
	ChgFont(ID_ACTION_1);
	ChgFont(ID_UPDATE);
	ChgFont(ID_CANCEL);
	ChgFont(ID_HELPNOTES);
	ChgFont(ID_EDITOR);
	ChgFont(ID_HELP);
	ChgFont(IDC_REOPEN);
	ChgFont(IDC_STATICUNCHANGED);
	ChgFont(IDC_RADIOSUBMIT);
	ChgFont(IDC_RADIOREVERT);
	ChgFont(IDC_RADIOLEAVE);

	// Position SpecDlg and reposition our buttons
	CRect rect, rectCancel;
	GetClientRect(&rect);
	GetDlgItem( ID_CANCEL )->GetWindowRect( &rectCancel );
	ScreenToClient( &rectCancel );

	// Now that m_P4SpecDlg has been created, we can position the buttons on a submit form
	if( m_P4SpecDlg.m_SpecType == P4CHANGE_SPEC && m_P4SpecDlg.m_AllowSubmit )
	{
		if (m_bShowReopen)
		{
			GetDlgItem( IDC_REOPEN )->GetWindowRect( &rect );
			m_Pad += rect.Height();
		}
		GetDlgItem( ID_UPDATE )->GetWindowRect( windowRect );
		ScreenToClient( windowRect );
		GetDlgItem( m_BtnId[0] = ID_ACTION_1 )->MoveWindow( windowRect );

		GetDlgItem( ID_EDITOR )->GetWindowRect( windowRect );
		ScreenToClient( windowRect );
		GetDlgItem( m_BtnId[1] = ID_UPDATE )->MoveWindow( windowRect );

		GetDlgItem( ID_EDITOR )->EnableWindow(FALSE);
		GetDlgItem( ID_EDITOR )->ShowWindow(SW_HIDE);
		m_P4SpecDlg.m_EditorBtnDisabled = TRUE;

		SendMessage(DM_SETDEFID, ID_ACTION_1, 0);
	}

	OnReopen();

	CWnd *pButton=GetDlgItem(ID_HELP);
	pButton->GetWindowRect(&rect);
	int w=rect.Width();
	int xtra = w/16;
	GetWindowRect(&rect);
	rect.right= max( rect.right, rect.left + 5 * w + 8 * xtra );
	// Yeah, this is stupid, but it seems to
	// be necessary to make the buttons show
	rect.top += 1;
	MoveWindow(&rect);
	rect.top -= 1;
	MoveWindow(&rect);
	UpdateData(FALSE);

	return FALSE;  // return TRUE unless you set the focus to a control
}

void CP4SpecSheet::ChgFont(int id)
{
	CRect windowRect;

	CWnd *pChildWnd = GetDlgItem( id );
	pChildWnd->SetFont( &m_P4SpecDlg.m_Font );
	pChildWnd->GetWindowRect( windowRect );
	ScreenToClient( windowRect );
	windowRect.left= windowRect.left * m_P4SpecDlg.m_NewAveCharWidth / m_P4SpecDlg.m_OldAveCharWidth;
	windowRect.right= windowRect.right * m_P4SpecDlg.m_NewAveCharWidth / m_P4SpecDlg.m_OldAveCharWidth;
	windowRect.top= windowRect.top * m_P4SpecDlg.m_NewHeight / m_P4SpecDlg.m_OldHeight;
	windowRect.bottom= windowRect.bottom * m_P4SpecDlg.m_NewHeight / m_P4SpecDlg.m_OldHeight;
	pChildWnd->MoveWindow( windowRect );
}

void CP4SpecSheet::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_ACTIVE)
	{
		if (m_LastFocus)
		{
			if (!IsBadWritePtr(m_LastFocus, sizeof(CWnd))
			 && IsWindow(m_LastFocus->m_hWnd))
				PostMessage(WM_FORCEFOCUS, 2, (LPARAM)m_LastFocus->m_hWnd);
			m_LastFocus = 0;
		}
	}
	else if (nState == WA_INACTIVE)
	{
		if (!m_LastFocus)
		{
			CWnd *w = GetFocus();
			if (!w || w->GetParent()->m_hWnd == m_P4SpecDlg.m_hWnd)
				m_LastFocus = w;
		}
	}
}

LRESULT CP4SpecSheet::OnForceFocus(WPARAM wParam, LPARAM lParam)
{
	if (wParam)
		PostMessage(WM_FORCEFOCUS, --wParam, lParam);
	else
	{
		HWND hWnd = (HWND)lParam;
		::SetForegroundWindow(m_P4SpecDlg.m_hWnd);
		::SetFocus(m_P4SpecDlg.m_hWnd);
		::SetForegroundWindow(hWnd);
		::SetFocus(hWnd);
	}
	return wParam;
}

void CP4SpecSheet::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
}

INT_PTR CP4SpecSheet::DoModal() 
{
	m_P4SpecDlg.SetIsModal(TRUE);
	INT_PTR retcode=CDialog::DoModal();
	m_P4SpecDlg.DoCleanup();
	return retcode;
}

void CP4SpecSheet::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	m_ScreenHeight = rect.bottom - rect.top;

	lpMMI->ptMinTrackSize.y = 240;
	lpMMI->ptMaxTrackSize.y = m_ScreenHeight;
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CP4SpecSheet::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CDialog::OnSizing(fwSide, pRect);
	
	pRect->bottom= max( pRect->bottom, pRect->top + 240 );
	pRect->bottom= min( pRect->bottom, m_ScreenHeight );

	CWnd *pButton=GetDlgItem(ID_HELP);
	if (!pButton || !IsWindow(pButton->m_hWnd))
		return;
	CRect rect;
	pButton->GetWindowRect(&rect);
	int w=rect.Width();
	int xtra = w/16;
	pRect->right= max( pRect->right, pRect->left + 5 * w + 8 * xtra );
}

void CP4SpecSheet::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if (nType == SIZE_MINIMIZED)
	{
		m_IsMinimized = TRUE;
		return;
	}
	else if (m_IsMinimized)
	{
		m_IsMinimized = FALSE;
		if (nType != SIZE_MAXIMIZED)
			return;
	}

	CWnd *pButton=GetDlgItem(ID_HELP);
	if (!pButton || !IsWindow(pButton->m_hWnd))
		return;

	CRect rect, rect2, rectCancel;

	pButton->InvalidateRect(NULL, TRUE);
	pButton->GetWindowRect(&rect);
	int x;
	int w=rect.Width();
	int xtra = w/16;
	int end = cx - xtra;
	int y = cy - xtra - rect.Height();
	pButton->SetWindowPos(NULL, end - w, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pButton=GetDlgItem(m_BtnId[3]);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->SetWindowPos(NULL, end - (w*2 + xtra), y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pButton=GetDlgItem(m_BtnId[2]);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->SetWindowPos(NULL, end - (w*3 + xtra*2), y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pButton=GetDlgItem(m_BtnId[1]);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->SetWindowPos(NULL, end - (w*4 + xtra*3), y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pButton=GetDlgItem(m_BtnId[0]);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->SetWindowPos(NULL, end - (w*5 + xtra*4), y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	GetClientRect(&rect);
	GetDlgItem( ID_CANCEL )->GetWindowRect( &rectCancel );
	ScreenToClient( &rectCancel );

	if (!m_Pad)
		 m_Pad = (rect.bottom - rectCancel.bottom) * 2;

	m_P4SpecDlg.SetWindowPos(&CWnd::wndBottom, 0, 0, 
					rect.Width(), rect.Height() - rectCancel.Height() - m_Pad, 0);

	// Place the bottom of the Reopen Checkbox 3 pixels above the top of the buttons
	pButton=GetDlgItem(IDC_REOPEN);
	pButton->GetClientRect(&rect);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->SetWindowPos(NULL, end - (w*5 + xtra*4), 
					rectCancel.top - rect.Height() - 3, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pButton=GetDlgItem(IDC_STATICUNCHANGED);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->SetWindowPos(NULL, x = end - (w*5 + xtra*4) + rect.Width(), 
					rectCancel.top - rect.Height() - 3 + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pButton->GetWindowRect(&rect2);

	pButton=GetDlgItem(IDC_RADIOSUBMIT);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->SetWindowPos(NULL, x = x + rect2.Width(), 
					rectCancel.top - rect.Height() - 3, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pButton->GetWindowRect(&rect2);

	pButton=GetDlgItem(IDC_RADIOREVERT);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->SetWindowPos(NULL, x = x + rect2.Width(), 
					rectCancel.top - rect.Height() - 3, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pButton->GetWindowRect(&rect2);

	pButton=GetDlgItem(IDC_RADIOLEAVE);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->SetWindowPos(NULL, x = x + rect2.Width(), 
					rectCancel.top - rect.Height() - 3, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

// The user pressed the Enter key and we got a button click for IDOK
void CP4SpecSheet::OnEnter() 
{
	m_P4SpecDlg.OnEnter();
}

void CP4SpecSheet::On_OK() 
{
	m_P4SpecDlg.On_OK();
}

void CP4SpecSheet::OnAlternate() 
{
	if (GET_SERVERLEVEL() >= 21)	// 2006.1 or later?
		OnRadioUnchg();
	m_P4SpecDlg.OnAlternate();
}

void CP4SpecSheet::On_Cancel() 
{
	m_P4SpecDlg.On_Cancel();
}

void CP4SpecSheet::OnCancel() 
{
	// Eat ESC while sending spec, so the server's reply can
	// be properly processed.  Don't confuse this with On_Cancel(),
	// which is called when the cancel button is hit.
	// m_P4SpecDlg.OnCancel() does all the work
	m_P4SpecDlg.OnCancel();
}

void CP4SpecSheet::OnHelpnotes() 
{
	m_P4SpecDlg.OnHelpnotes();
}

void CP4SpecSheet::OnEditor() 
{
	m_P4SpecDlg.OnEditor();
}

void CP4SpecSheet::OnHelp()
{
	m_P4SpecDlg.OnHelp();
}

void CP4SpecSheet::OnClose() 
{
 	On_Cancel();
}

LRESULT CP4SpecSheet::OnQuitting(WPARAM wParam, LPARAM lParam) 
{
	if (m_pParent)
	{
		CP4Command *pCmd = (CP4Command *)m_P4SpecDlg.GetCallingCommand();
		m_pParent->SendMessage(WM_P4ENDSPECEDIT, (WPARAM)pCmd, IDABORT);
	}
	m_P4SpecDlg.DoCleanup();
	return 0;
}

// This signals the closing of a modeless dialog
// to MainFrame which will delete the 'this' object
void CP4SpecSheet::OnDestroy()
{
	if (m_pParent)
	{
		::PostMessage(MainFrame()->m_hWnd, WM_P4DLGDESTROY, 0, (LPARAM)this);
		m_MainFRmSignaled = TRUE;
	}
}

BOOL CP4SpecSheet::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	return m_P4SpecDlg.OnHelpInfo(pHelpInfo);
}

void CP4SpecSheet::OnReopen()
{
	m_P4SpecDlg.m_bReopen = m_bReopen.GetCheck();
}

void CP4SpecSheet::OnRadioUnchg()
{
	UpdateData();
	m_P4SpecDlg.m_CheckOnlyChgedFiles = (m_UnchangedFlag == 2);
	m_P4SpecDlg.ResetFileChecks((m_P4SpecDlg.m_UnchangedFlag = m_UnchangedFlag) != 2);
}

LRESULT CP4SpecSheet::OnEnableDisable( WPARAM wParam, LPARAM lParam )
{
	GetDlgItem( IDC_STATICUNCHANGED )->EnableWindow(lParam);
	GetDlgItem( IDC_RADIOREVERT )->EnableWindow(lParam);
	GetDlgItem( IDC_RADIOSUBMIT )->EnableWindow(lParam);
	GetDlgItem( IDC_RADIOLEAVE )->EnableWindow(lParam);
	return 0;
}

LRESULT CP4SpecSheet::OnModifyStyle( WPARAM wParam, LPARAM lParam )
{
	for (int i = -1; ++i < 5; )
		GetDlgItem(m_BtnId[i])->ModifyStyle((DWORD)wParam, (DWORD)lParam, 0);
	if (m_bShowReopen)
		GetDlgItem(IDC_REOPEN)->ModifyStyle((DWORD)wParam, (DWORD)lParam, 0);
	if (m_bShowRevertUnchg)
	{
		GetDlgItem( IDC_STATICUNCHANGED )->ModifyStyle((DWORD)wParam, (DWORD)lParam, 0);
		GetDlgItem( IDC_RADIOREVERT )->ModifyStyle((DWORD)wParam, (DWORD)lParam, 0);
		GetDlgItem( IDC_RADIOSUBMIT )->ModifyStyle((DWORD)wParam, (DWORD)lParam, 0);
		GetDlgItem( IDC_RADIOLEAVE )->ModifyStyle((DWORD)wParam, (DWORD)lParam, 0);
	}
	return 0;
}

LRESULT CP4SpecSheet::OnP4EndHelpnotes(WPARAM wParam, LPARAM lParam)
{
	return m_P4SpecDlg.OnP4EndHelpnotes(wParam, lParam);
}

void CP4SpecSheet::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch(nID)
	{
	case SC_MINIMIZE:
		GetDesktopWindow()->ArrangeIconicWindows();
		break;
	}

	CDialog::OnSysCommand(nID, lParam);
}
