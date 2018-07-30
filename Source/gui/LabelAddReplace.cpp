// LabelAddReplace.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "reviewlist.h"
#include "LabelAddReplace.h"
#include "MainFrm.h"
#include "P4Command.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLabelAddReplace dialog


CLabelAddReplace::CLabelAddReplace(CWnd* pParent /*=NULL*/)
	: CDialog(CLabelAddReplace::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLabelAddReplace)
	m_RevFlag = -1;
	m_FilesFlag = -1;
	m_DefaultFlag = 0;
	m_RevNbr = _T("");
	m_Symbol = _T("");
	//}}AFX_DATA_INIT

	m_LabelWnd = pParent->m_hWnd;
	m_InitRect.SetRect(0,0,0,0);
	m_InitDialogDone=FALSE;
	m_WinPos.SetWindow( this, _T("LabelAddReplace") );
}


void CLabelAddReplace::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLabelAddReplace)
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Radio(pDX, IDC_USECLIENTREV, m_RevFlag);
	DDX_Radio(pDX, IDC_RADIO1, m_FilesFlag);
	DDX_Radio(pDX, IDC_RADIO4, m_DefaultFlag);
	DDX_Text(pDX, IDC_F_REVNBR, m_RevNbr);
	DDX_Text(pDX, IDC_F_SYMBOL, m_Symbol);
	DDX_Control(pDX, IDC_F_COMBO, m_TypeCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLabelAddReplace, CDialog)
	//{{AFX_MSG_MAP(CLabelAddReplace)
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_BN_CLICKED(ID_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_USECLIENTREV, OnUseclientrev)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio2)
	ON_BN_CLICKED(IDC_F_ISREVNBR, OnFIsrevnbr)
	ON_BN_CLICKED(IDC_F_ISSYMBOL, OnFIssymbol)
	ON_BN_CLICKED(IDC_F_BROWSE, OnBrowse)
	ON_CBN_SELCHANGE(IDC_F_COMBO, OnComboValueChg)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
	ON_WM_HELPINFO()
	ON_MESSAGE(WM_BROWSECALLBACK1, OnBrowseCallBack)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLabelAddReplace message handlers

BOOL CLabelAddReplace::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetWindowRect(&m_InitRect);
	m_LastRect = m_InitRect;

	CString caption;
	GetWindowText(caption);
	caption = caption + _T(' ') + m_Active;
	SetWindowText(caption);
	
	int maxlgth = 0;
	for (POSITION pos=m_SelectedList->GetHeadPosition(); pos != NULL; )
	{
		CString s = m_SelectedList->GetNext( pos );

		if( !s.IsEmpty() )
		{
			maxlgth = max(maxlgth, s.GetLength());
			m_List.AddString( s );
		}
	}
	// Get text metrics for font so we can figure scroll extent
	//
	TEXTMETRIC tm;
	CDC *pDC= GetDC();
	CFont* font = m_List.GetFont( );
	CFont *pOldFont= pDC->SelectObject( font );
	pDC->GetTextMetrics( &tm );
	pDC->SelectObject( pOldFont );
	ReleaseDC( pDC );

	m_List.SetHorizontalExtent( tm.tmAveCharWidth * maxlgth + 40 );

	m_FilesFlag ? OnRadio2() : OnRadio1();		// Enable/Disable controls

	GetDlgItem(IDC_F_STATIC)->ShowWindow(SW_HIDE);
	m_TypeCombo.AddString(LoadStringResource(IDS_COMBO_CHGNBR));
	m_TypeCombo.AddString(LoadStringResource(IDS_COMBO_LABEL));
	m_TypeCombo.AddString(LoadStringResource(IDS_COMBO_DATE));
	m_TypeCombo.AddString(LoadStringResource(IDS_COMBO_CLIENT));
	m_TypeCombo.SetCurSel(0);

	UpdateData(FALSE);

	// Restore window position
	m_InitDialogDone = TRUE;
	m_WinPos.RestoreWindowPosition();

	ShowWindow(SW_SHOW);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLabelAddReplace::OnRadio1() 
{
	GetDlgItem(IDC_LIST)->EnableWindow( FALSE );
	GetDlgItem(IDC_REMOVE)->EnableWindow( FALSE );
	GetDlgItem(IDC_USECLIENTREV)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_ISREVNBR)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_REVNBR)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_ISSYMBOL)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_SYMBOL)->EnableWindow( FALSE );
}

void CLabelAddReplace::OnRadio2() 
{
	UpdateData();
	GetDlgItem(IDC_LIST)->EnableWindow( TRUE );
	GetDlgItem(IDC_REMOVE)->EnableWindow( TRUE );
	GetDlgItem(IDC_USECLIENTREV)->EnableWindow( TRUE );
	GetDlgItem(IDC_F_ISREVNBR)->EnableWindow( TRUE );
	GetDlgItem(IDC_F_REVNBR)->EnableWindow( m_RevFlag == 1 );
	GetDlgItem(IDC_F_ISSYMBOL)->EnableWindow( TRUE );
	GetDlgItem(IDC_F_SYMBOL)->EnableWindow( m_RevFlag == 2 );
	if (!m_List.GetCount())
		AfxMessageBox(LoadStringResource(IDS_NO_FILES_SELECTED_FOR_ADD_REPLACE), MB_ICONINFORMATION);
}


void CLabelAddReplace::OnRemove() 
{
	for ( int i = m_List.GetCount ( ); i--; )
	{
		if (m_List.GetSel(i))
			m_List.DeleteString(i);
	}
}

void CLabelAddReplace::OnUseclientrev() 
{
	GetDlgItem(IDC_F_REVNBR)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_SYMBOL)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_COMBO)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_BROWSE)->EnableWindow( FALSE );
}

void CLabelAddReplace::OnFIsrevnbr() 
{
	GetDlgItem(IDC_F_REVNBR)->EnableWindow( TRUE );
	GetDlgItem(IDC_F_SYMBOL)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_COMBO)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_BROWSE)->EnableWindow( FALSE );
	GotoDlgCtrl(GetDlgItem(IDC_F_REVNBR));
}

void CLabelAddReplace::OnFIssymbol() 
{
	GetDlgItem(IDC_F_REVNBR)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_SYMBOL)->EnableWindow( TRUE );
	GetDlgItem(IDC_F_COMBO)->EnableWindow( TRUE );
	OnComboValueChg();
	GotoDlgCtrl(GetDlgItem(IDC_F_COMBO));
}

void CLabelAddReplace::OnComboValueChg() 
{
	switch(m_TypeCombo.GetCurSel())
	{
	default:
	case COMBO_CHGNBR:
		GetDlgItem(IDC_F_BROWSE)->EnableWindow( TRUE );
		GetDlgItem(IDC_F_STATIC)->ShowWindow(SW_HIDE);
		break;
	case COMBO_LABEL:
		GetDlgItem(IDC_F_BROWSE)->EnableWindow( TRUE );
		GetDlgItem(IDC_F_STATIC)->ShowWindow(SW_HIDE);
		break;
	case COMBO_CLIENT:
		GetDlgItem(IDC_F_BROWSE)->EnableWindow( TRUE );
		GetDlgItem(IDC_F_STATIC)->ShowWindow(SW_HIDE);
		break;
	case COMBO_DATE:
		GetDlgItem(IDC_F_BROWSE)->EnableWindow( FALSE );
		GetDlgItem(IDC_F_STATIC)->ShowWindow(SW_SHOW);
		break;
	}
}

void CLabelAddReplace::OnBrowse()
{
	HWND hWnd;
	switch(m_TypeCombo.GetCurSel())
	{
	case COMBO_CHGNBR:
		hWnd = MainFrame()->OldChgsWnd();
		break;
	case COMBO_CLIENT:
		hWnd = MainFrame()->ClientWnd();
		break;
	case COMBO_LABEL:
		hWnd = MainFrame()->LabelWnd();
		break;
	default:
		hWnd = 0;
		break;
	}
	::SendMessage(hWnd, WM_FETCHOBJECTLIST, (WPARAM)(this->m_hWnd), WM_BROWSECALLBACK1);
}

LRESULT CLabelAddReplace::OnBrowseCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_Symbol = *str;
	UpdateData(FALSE);
	return 0;
}

void CLabelAddReplace::OnCancel() 
{
	::PostMessage(m_LabelWnd, WM_DOLABELSYNC, (WPARAM)IDCANCEL, (LPARAM)0);
}

void CLabelAddReplace::OnPreview() 
{
	OnFinish();
	::PostMessage(m_LabelWnd, WM_DOLABELSYNC, (WPARAM)IDOK, (LPARAM)TRUE);
}

void CLabelAddReplace::OnOK() 
{
	OnFinish();
	::PostMessage(m_LabelWnd, WM_DOLABELSYNC, (WPARAM)IDOK, (LPARAM)FALSE);
}

void CLabelAddReplace::OnFinish() 
{
	m_WinPos.SaveWindowPosition();
	UpdateData();
	m_OutputList->RemoveAll();
	if (m_FilesFlag)	// load up the list of files
	{
		int hash;
		int space;
		CString txt;
		CString rev;
		if (m_RevFlag == 1)
		{
			m_RevNbr.TrimLeft(_T("# "));
			rev = _T("#") + m_RevNbr;
		}
		else if (m_RevFlag == 2)
		{
			m_Symbol.TrimLeft(_T("@ "));
			rev = _T("@") + m_Symbol;
		}
		for ( int i = 0; i < m_List.GetCount ( ); i++ )
		{
			m_List.GetText( i, txt );
			if ((hash = txt.Find(_T('#'))) != -1)
			{
				if ((space = txt.Find(_T(' '), hash)) != -1)
					hash = space;
					txt = txt.Left(hash);
			}
			if (m_RevFlag)
				txt += rev;
			m_OutputList->AddTail( txt );
		}
	}
	else
		m_OutputList->AddHead(_T("//..."));
}

void CLabelAddReplace::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_MANAGING_LABELS);
}

BOOL CLabelAddReplace::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}


void CLabelAddReplace::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if (!m_InitDialogDone)
		return;

	// Compute the change in width (dx)
	CRect rect;
	GetWindowRect(&rect);
	int dx = rect.Width() - m_LastRect.Width();
	int dy = rect.Height() - m_LastRect.Height();
	// Save the new size
	m_LastRect = rect;

	// Widen the list and group boxes by the change in width (dx)
	// Widen the list box by the change in width (dx) and
	// heighten the list box by the change in height (dy)
	CWnd *pWnd=GetDlgItem(IDC_LIST);
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								    rect.bottom - rect.top + dy, SWP_NOMOVE | SWP_NOZORDER);
	// Widen the group by the change in width
	// Slide it down by the change in height
	pWnd = GetDlgItem(IDC_STATIC1);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOZORDER);
	// Slide revision radio buttons and other revision fields
	// down by the change in height
	pWnd = GetDlgItem(IDC_USECLIENTREV);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_F_ISREVNBR);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_F_ISSYMBOL);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_F_REVNBR);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_F_COMBO);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_F_SYMBOL);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_F_BROWSE);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_F_STATIC);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Slide the buttons to the right by the change in width
	// and down by the change in height
	pWnd = GetDlgItem(IDC_REMOVE);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDOK);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(ID_PREVIEW);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDCANCEL);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(ID_HELP);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	InvalidateRect(NULL, TRUE);
}

void CLabelAddReplace::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_InitRect.Height())
	{
		lpMMI->ptMinTrackSize.x= m_InitRect.Width();
		lpMMI->ptMinTrackSize.y= m_InitRect.Height();
	}
}

// This signals the closing of a modeless dialog
// to MainFrame which will delete the 'this' object
void CLabelAddReplace::OnDestroy()
{
	::PostMessage(MainFrame()->m_hWnd, WM_P4DLGDESTROY, 0, (LPARAM)this);
}

