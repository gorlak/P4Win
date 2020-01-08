/*
 * Copyright 1997, 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

// SpecDescDlg.cpp : implementation file
//

#include "stdafx.h"
// don't really need mfc 7 for this.  just need platform sdk installed
// but this will prevent breaking the build for right now
#if _MFC_VER >= 0x0700
#define USE_THEMES
#include <uxtheme.h>
#endif
#include "p4win.h"
#include "WinPos.h"
#include "SpecDescDlg.h"
#include "MainFrm.h"
#include "FileInfoDlg.h"
#include "Historydlg.h"
#include "ViewerDlg.h"
#include "P4Fix.h"

#include "cmd_diff2.h"
#include "cmd_get.h"
#include "cmd_fixes.h"
#include "cmd_fstat.h"
#include "cmd_history.h"
#include "cmd_opened.h"
#include "cmd_prepbrowse.h"

#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define HOLD_LOCK_IF_HAVE_KEY (m_Key ? HOLD_LOCK : LOSE_LOCK)
#define UPDATE_STATUS(x) ((CMainFrame *)AfxGetMainWnd())->UpdateStatus(x)
#define	ID_EMAIL ID_EMAIL_PERFORCE
#define ID_URL   ID_WWW_PERFORCE_COM

static bool sbHasWingDings = false;

/////////////////////////////////////////////////////////////////////////////
// a subclass of CButton to pass Ctrl+F, F3 and Shift F3 to the parent window
BEGIN_MESSAGE_MAP(CKeyDownButton, CButton)
	//{{AFX_MSG_MAP(CMainFrame)
	//}}AFX_MSG_MAP
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CKeyDownButton::CKeyDownButton(CWnd* pParent) 
	: CButton()
{
}

CKeyDownButton::~CKeyDownButton()
{
}

void CKeyDownButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == 'F' && GetKeyState(VK_CONTROL) & 0x8000)	// ^F == Find
		GetParent()->PostMessage(WM_COMMAND, ID_POSITIONTOPATTERN, 0);
	else if (nChar == VK_F3)
		GetParent()->PostMessage(WM_COMMAND, GetKeyState(VK_SHIFT) & 0x8000 
											 ? ID_FINDPREV : ID_FINDNEXT, 0);
	else
		CButton::OnChar(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
// a subclass of CButton to draw arrows to the left of the text
BEGIN_MESSAGE_MAP(CArrowButton, CButton)
	//{{AFX_MSG_MAP(CMainFrame)
	//}}AFX_MSG_MAP
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover )
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave )
	ON_WM_GETDLGCODE()
	ON_MESSAGE(BM_SETSTYLE, OnSetStyle)
END_MESSAGE_MAP()

CArrowButton::CArrowButton() 
	: CButton()
	, m_bUp(false)
	, m_bOverControl(false)
	, m_bTracking(false)
	, m_bDefault(false)
{
	m_il.Create(IDB_UPDOWNARROWS, 16, 1, RGB(0xff, 0xff, 0xff));
	m_themeLib = LoadLibrary(_T("UxTheme.dll"));
}

CArrowButton::~CArrowButton()
{
	if(m_themeLib)
		FreeLibrary(m_themeLib);
}

void CArrowButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == 'F' && GetKeyState(VK_CONTROL) & 0x8000)	// ^F == Find
		GetParent()->PostMessage(WM_COMMAND, ID_POSITIONTOPATTERN, 0);
	else if (nChar == VK_F3)
		GetParent()->PostMessage(WM_COMMAND, GetKeyState(VK_SHIFT) & 0x8000 
											 ? ID_FINDPREV : ID_FINDNEXT, 0);
	else
		CButton::OnChar(nChar, nRepCnt, nFlags);
}

void CArrowButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme) != FALSE;		
	}	
	CButton::OnMouseMove(nFlags, point);
}
LRESULT CArrowButton::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	m_bOverControl=TRUE;
	Invalidate();
	return 1;
}

LRESULT CArrowButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;
	m_bOverControl = FALSE;
	Invalidate(FALSE);
	return 0;
}
UINT CArrowButton::OnGetDlgCode() 
{
	UINT nCode = CButton::OnGetDlgCode();
	nCode |= (m_bDefault ? DLGC_DEFPUSHBUTTON : DLGC_UNDEFPUSHBUTTON);
	return nCode;
}

// mask for control's type
#undef  BS_TYPEMASK
#define BS_TYPEMASK SS_TYPEMASK

LRESULT CArrowButton::OnSetStyle(WPARAM wParam, LPARAM lParam)
{
	m_bDefault = wParam & BS_DEFPUSHBUTTON;
	// can't change control type after owner-draw is set.
	// let the system process changes to other style bits
	// and redrawing, while keeping owner-draw style
	return DefWindowProc(BM_SETSTYLE,
		(wParam & ~BS_TYPEMASK) | BS_OWNERDRAW, lParam);
}

#ifdef USE_THEMES
typedef HTHEME(__stdcall *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT(__stdcall *PFNCLOSETHEMEDATA)(HTHEME hTheme);
typedef HRESULT(__stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc, 
	int iPartId, int iStateId, const RECT *pRect, OPTIONAL const RECT *pClipRect);
typedef HRESULT(__stdcall *PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME hTheme, HDC hdc, 
	int iPartId, int iStateId, const RECT *pRect, RECT *pContentRect);
#endif

void CArrowButton::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
#ifdef USE_THEMES
	HTHEME hTheme = 0;
	if(m_themeLib)
	{
		PFNOPENTHEMEDATA pOpenThemeData = (PFNOPENTHEMEDATA)
			GetProcAddress(m_themeLib, "OpenThemeData");
		if(pOpenThemeData)
			hTheme = (pOpenThemeData)(m_hWnd, L"Button");
	}
	if(hTheme)
	{
		// attempt to map owner draw states to theme pushbutton states
		int iState = PBS_NORMAL;
		if(m_bOverControl)//lpDrawItemStruct->itemState & ODS_HOTLIGHT)
			iState = PBS_HOT;
		else if(m_bDefault)//lpDrawItemStruct->itemState & ODS_DEFAULT)
			iState = PBS_DEFAULTED;
		else if(lpDrawItemStruct->itemState & ODS_SELECTED)
			iState = PBS_PRESSED;
		HRESULT hr;

		HDC hDC = lpDrawItemStruct->hDC;
		RECT rc = lpDrawItemStruct->rcItem;

		// let theme draw background
		PFNDRAWTHEMEBACKGROUND pDrawThemeBackground = (PFNDRAWTHEMEBACKGROUND)
			GetProcAddress(m_themeLib, "DrawThemeBackground");
		if(!pDrawThemeBackground)
			return;
		hr = (pDrawThemeBackground)(hTheme, hDC, 
			BP_PUSHBUTTON, iState, &rc, 0);
		if(hr != S_OK)
			return;

		// get rect to put content in
		RECT rcContent;
		PFNGETTHEMEBACKGROUNDCONTENTRECT pGetThemeBackgroundContentRect = 
			(PFNGETTHEMEBACKGROUNDCONTENTRECT)
			GetProcAddress(m_themeLib, "GetThemeBackgroundContentRect");
		if(!pGetThemeBackgroundContentRect)
			return;
		hr = (pGetThemeBackgroundContentRect)(hTheme, hDC,
			BP_PUSHBUTTON, iState, &rc, &rcContent);
		if(hr != S_OK)
			return;

		// draw arrow as content
		CDC dc;
		dc.Attach(hDC);
		CPoint pos;
		pos.x = (rcContent.left + rcContent.right)/2 - 8;
		pos.y = (rcContent.top + rcContent.bottom)/2 - 8;
		m_il.Draw(&dc, m_bUp ? 0 : 1, pos, ILD_NORMAL);

		if (lpDrawItemStruct->itemState & ODS_FOCUS)
		{
			::DrawFocusRect(lpDrawItemStruct->hDC, &rcContent);
		}
		PFNCLOSETHEMEDATA pCloseThemeData = (PFNCLOSETHEMEDATA)GetProcAddress(m_themeLib, "CloseThemeData");
		if(!pCloseThemeData)
			return;
		(pCloseThemeData)(hTheme);
		return;
	}
	else
#endif
	{
		// if the button is defaulted, draw a black frame first
		// and then shring the rect by 1 pixel all around
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		if (m_bDefault)//lpDrawItemStruct->itemState & ODS_DEFAULT)
		{
			CRect r(lpDrawItemStruct->rcItem);
			CPen *pOldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
			CBrush *pOldBrush= (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
			pDC->Rectangle(&lpDrawItemStruct->rcItem);
			pDC->SelectObject( pOldPen );
			pDC->SelectObject( pOldBrush );
			r.DeflateRect(1,1);
			lpDrawItemStruct->rcItem = r;
		}

		// This code only works with buttons.
		ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

		// If drawing selected, add the pushed style to DrawFrameControl.
		UINT uStyle = DFCS_BUTTONPUSH;
		if (lpDrawItemStruct->itemState & ODS_SELECTED)
			uStyle |= DFCS_PUSHED;

		// Make DrawFrameControl give back the content rect
		uStyle |= DFCS_ADJUSTRECT;

		// Draw the button frame.
		::DrawFrameControl(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, DFC_BUTTON, uStyle);

		// Draw the button's bitmap
		CDC dc;
		dc.Attach(lpDrawItemStruct->hDC);
		CPoint pos;
		pos.x = (lpDrawItemStruct->rcItem.right + lpDrawItemStruct->rcItem.left) / 2 - 8;
		pos.y = (lpDrawItemStruct->rcItem.bottom + lpDrawItemStruct->rcItem.top) / 2 - 8;
		m_il.Draw(&dc, m_bUp ? 0 : 1, pos, ILD_NORMAL);

		// draw a focus rect if we have focus
		if (lpDrawItemStruct->itemState & ODS_FOCUS)
		{
			CRect focusRect = lpDrawItemStruct->rcItem;
			focusRect.DeflateRect(1, 1);
			::DrawFocusRect(lpDrawItemStruct->hDC, focusRect);
		}
	}

}

/////////////////////////////////////////////////////////////////////////////
// CSpecDescDlg dialog


CSpecDescDlg::CSpecDescDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpecDescDlg::IDD, pParent)
    , m_SkipLines(0)
    , m_ScrollPastComments(false)
    , m_numHotSpots(0)
    , m_DescriptionW(0)
{
	//{{AFX_DATA_INIT(CSpecDescDlg)
	//}}AFX_DATA_INIT
	MainFrame()->WaitAWhileToPoll( );
	m_pParent = pParent;
	m_Caption=LoadStringResource(IDS_PERFORCE_SPECIFICATION);
	m_ReportedByTitle = "";
	m_Grey= RGB(200,200,200);
	m_GreyBrush.CreateSolidBrush( m_Grey );
	m_WinPos.SetWindow( this, _T("BrowseDlg") );
	m_Modeless     = FALSE;
	m_ShowNextPrev = FALSE;
	m_ShowShowDiffs= FALSE;
	m_ShowShowFixes= FALSE;
	m_ShowShowFiles= FALSE;
	m_ShowEditBtn  = FALSE;
	m_TurnOnReDraw = FALSE;
	m_DoNotActivate= FALSE;
	m_HasBeenMinimized= FALSE;
	m_LButtonDownTime = 0;
	m_MoreThan256Colors = FALSE;
	m_viewType = m_DiffFlag = 0;
	m_pToolTip = NULL;
	// Are we running as a Rev Hist dialog only?
	m_RevHistEnable = (TheApp()->m_RevHistPath.IsEmpty()) ? TRUE : FALSE;
	CString temp = LoadStringResource(IDS_RENAME_TO);
#ifndef UNICODE
    int numChars = MultiByteToWideChar(CP_ACP, 0, temp, -1, 0, 0);
    m_TO_ = new WCHAR[numChars];
    MultiByteToWideChar(CP_ACP, 0, temp, -1, const_cast<LPWCH>(m_TO_), numChars);
#else
    m_TO_ = new WCHAR[temp.GetLength() + 1];
	lstrcpy(const_cast<WCHAR*>(m_TO_), temp);
#endif
	m_NextBtn.m_bUp = false;
	m_PrevBtn.m_bUp = true;
	m_InitRect.SetRect(0,0,0,0);
	m_ChkServerBusy = !SERVER_BUSY();	// if busy at init, it'll always be busy
	m_Key = 0;
	m_pFRDlg = NULL;
	m_FindWhatStr = MainFrame()->GetFindWhatStr();
	m_FindWhatFlags = (MainFrame()->GetFindWhatFlags() | FR_DOWN) & ~FR_HIDEWHOLEWORD;
}

CSpecDescDlg::~CSpecDescDlg()
{
	// can't use MainFrame()-> construct
	// because mainfram might have closed.
	CMainFrame * mainWnd = MainFrame();
	if (mainWnd)
	{
		mainWnd->SetGotUserInput( );
		mainWnd->WaitAWhileToPoll( );
	}
#ifndef UNICODE
    // for UNICODE build, m_DescriptionW just points at m_Description
    delete const_cast<LPWCH>(m_DescriptionW);
#endif
    delete const_cast<LPWCH>(m_TO_);
	if (m_pToolTip)
		delete m_pToolTip;
}

void CSpecDescDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpecDescDlg)
	DDX_Control(pDX, IDC_DESCRIPTION, m_Text);
	DDX_Control(pDX, IDC_PREVITEM, m_PrevBtn);
	DDX_Control(pDX, IDC_NEXTITEM, m_NextBtn);
	DDX_Control(pDX, IDC_SHOWDIFFS, m_btShowDiffs);
	DDX_Control(pDX, IDOK, m_CloseBtn);
	DDX_Control(pDX, IDC_PRINT, m_PrintBtn);
	DDX_Control(pDX, IDC_EDITIT, m_EditBtn);
	DDX_Control(pDX, IDC_SHOWFIXES, m_ShowFixesBtn);
	DDX_Control(pDX, IDC_SHOWFILES, m_ShowFilesBtn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpecDescDlg, CDialog)
	ON_NOTIFY(EN_MSGFILTER, IDC_DESCRIPTION, OnMsgfilterDescription)
	//{{AFX_MSG_MAP(CSpecDescDlg)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_NEXTITEM, OnNextitem)
	ON_BN_CLICKED(IDC_PREVITEM, OnPrevitem)
	ON_BN_CLICKED(IDC_SHOWDIFFS, OnShowDiffsNormal)
	ON_BN_CLICKED(IDC_SHOWDIFFSMENU, OnShowDiffsBtn)
	ON_COMMAND(ID_SHOWDIFFS_NORMAL, OnShowDiffsNormal)
	ON_COMMAND(ID_SHOWDIFFS_SUMMARY, OnShowDiffsSummary)
	ON_COMMAND(ID_SHOWDIFFS_UNIFIED, OnShowDiffsUnified)
	ON_COMMAND(ID_SHOWDIFFS_CONTEXT, OnShowDiffsContext)
	ON_COMMAND(ID_SHOWDIFFS_RCS, OnShowDiffsRCS)
	ON_COMMAND(ID_SHOWDIFFS_NONE, OnShowDiffsNone)
	ON_UPDATE_COMMAND_UI(ID_SHOWDIFFS_NORMAL, OnUpdateShowDiffsNormal)
	ON_UPDATE_COMMAND_UI(ID_SHOWDIFFS_SUMMARY, OnUpdateShowDiffsSummary)
	ON_UPDATE_COMMAND_UI(ID_SHOWDIFFS_UNIFIED, OnUpdateShowDiffsUnified)
	ON_UPDATE_COMMAND_UI(ID_SHOWDIFFS_CONTEXT, OnUpdateShowDiffsContext)
	ON_UPDATE_COMMAND_UI(ID_SHOWDIFFS_RCS, OnUpdateShowDiffsRCS)
	ON_UPDATE_COMMAND_UI(ID_SHOWDIFFS_NONE, OnUpdateShowDiffsNone)
	ON_BN_CLICKED(IDC_PRINT, OnPrint)
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_FILE_GETCUSTOM, OnSync)
	ON_UPDATE_COMMAND_UI(ID_FILE_GETCUSTOM, OnUpdateSync)
	ON_COMMAND(ID_POSITIONDEPOT, OnPositionDepot)
	ON_UPDATE_COMMAND_UI(ID_POSITIONDEPOT, OnUpdatePositionDepot)
	ON_COMMAND(ID_FILE_DIFFHEAD, OnDiffHead)
	ON_UPDATE_COMMAND_UI(ID_FILE_DIFFHEAD, OnUpdateDiffHead)
	ON_COMMAND(IDB_DIFFREVISIONS, OnDiffPrev)
	ON_UPDATE_COMMAND_UI(IDB_DIFFREVISIONS, OnUpdateDiffPrev)
	ON_COMMAND(IDB_DIFFCLIFILE, OnDiffCliFile)
	ON_UPDATE_COMMAND_UI(IDB_DIFFCLIFILE, OnUpdateDiffCliFile)
	ON_COMMAND(IDB_BROWSE, OnFileAutobrowse)
	ON_UPDATE_COMMAND_UI(IDB_BROWSE, OnUpdateFileAutobrowse)
	ON_COMMAND(ID_FILE_ANNOTATE, OnFileAnnotate)
	ON_UPDATE_COMMAND_UI(ID_FILE_ANNOTATE, OnUpdateFileAnnotate)
	ON_COMMAND(ID_FILE_PROPERTIES, OnFileInformation)
	ON_UPDATE_COMMAND_UI(ID_FILE_PROPERTIES, OnUpdateFileInformation)
	ON_COMMAND(ID_FILE_REVISIONHISTORY, OnFileRevisionhistory)
	ON_UPDATE_COMMAND_UI(ID_FILE_REVISIONHISTORY, OnUpdateFileRevisionhistory)
	ON_COMMAND(ID_FILE_REVISIONTREE, OnFileRevisionTree)
	ON_UPDATE_COMMAND_UI(ID_FILE_REVISIONTREE, OnUpdateFileInformation)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_PERFORCE_OPTIONS, OnOptions)
    ON_UPDATE_COMMAND_UI(ID_PERFORCE_OPTIONS, OnUpdateOptions)
	ON_COMMAND(ID_CHANGE_DESCRIBE, OnDescChg)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_DESCRIBE, OnUpdateDescChg)
	ON_COMMAND(ID_BRANCH_DESCRIBE, OnDescBranch)
	ON_UPDATE_COMMAND_UI(ID_BRANCH_DESCRIBE, OnUpdateDescBranch)
	ON_COMMAND(ID_LABEL_DESCRIBE, OnDescLabel)
	ON_UPDATE_COMMAND_UI(ID_LABEL_DESCRIBE, OnUpdateDescLabel)
	ON_COMMAND(ID_CLIENT_DESCRIBE, OnDescClient)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_DESCRIBE, OnUpdateDescClient)
	ON_COMMAND(ID_USER_DESCRIBE, OnDescUser)
	ON_UPDATE_COMMAND_UI(ID_USER_DESCRIBE, OnUpdateDescUser)
	ON_COMMAND(ID_JOB_DESCRIBE, OnDescJob)
	ON_UPDATE_COMMAND_UI(ID_JOB_DESCRIBE, OnUpdateDescJob)
	ON_COMMAND(ID_EMAIL, OnEmail)
	ON_UPDATE_COMMAND_UI(ID_EMAIL, OnUpdateEmail)
	ON_COMMAND(ID_URL, OnURL)
	ON_UPDATE_COMMAND_UI(ID_URL, OnUpdateURL)
	ON_COMMAND(ID_DIFF2, OnDiff2)
	ON_UPDATE_COMMAND_UI(ID_DIFF2, OnUpdateDiff2)
	ON_COMMAND(ID_CURRENTTASK, OnCallTrack)
	ON_UPDATE_COMMAND_UI(ID_CURRENTTASK, OnUpdateCallTrack)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_POSITIONTOPATTERN, OnUpdatePositionToPattern)
	ON_COMMAND(ID_POSITIONTOPATTERN, OnPositionToPattern)
	ON_UPDATE_COMMAND_UI(ID_FINDNEXT, OnUpdatePositionToNext)
	ON_COMMAND(ID_FINDNEXT, OnPositionToNext)
	ON_UPDATE_COMMAND_UI(ID_FINDPREV, OnUpdatePositionToPrev)
	ON_COMMAND(ID_FINDPREV, OnPositionToPrev)
	ON_BN_CLICKED(IDC_SHOWFIXES, OnShowfixes)
	ON_BN_CLICKED(IDC_SHOWFILES, OnShowfiles)
	ON_WM_HELPINFO()
	ON_COMMAND(ID_HELPNOTES, OnQuickHelp)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(IDCANCEL, OnClose)
	ON_BN_CLICKED(IDC_EDITIT, OnEditButton)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_P4DIFF2, OnP4Diff2 )
	ON_MESSAGE(WM_P4PREPBROWSE, OnP4ViewFile )
	ON_MESSAGE(WM_P4FIXES, OnP4Fixes )
	ON_MESSAGE(WM_P4DESCRIBE, OnP4Describe )
	ON_MESSAGE(WM_P4FILEINFORMATION, OnP4FileInformation )
	ON_MESSAGE(WM_P4ENDFILEINFORMATION, OnP4EndFileInformation )
	ON_MESSAGE(WM_P4FSTAT, OnP4LabelContents )
    ON_MESSAGE(WM_P4ENDDESCRIBE, OnP4EndDescribe )
	ON_MESSAGE(WM_NEWCLIENT, OnNewClient )
	ON_MESSAGE(WM_NEWUSER, OnNewUser )
	ON_MESSAGE(WM_QUITTING, OnQuitting )
	ON_MESSAGE(WM_FINDPATTERN, OnFindPattern )
    ON_WM_INITMENUPOPUP()
    ON_REGISTERED_MESSAGE( WM_FINDREPLACE, OnFindReplace )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecDescDlg message handlers

BOOL CSpecDescDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_Modeless)
	{
		DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | WS_MINIMIZEBOX);
		MainFrame()->SetModelessWnd(this);
	}

	m_bDiffOutput = m_Caption.Find(_T(" <> ")) > 0;

	GetWindowRect(&m_InitRect);
	if (m_ShowNextPrev)
	{
		GetDlgItem(IDC_PREVITEM)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_NEXTITEM)->ShowWindow(SW_SHOWNORMAL);
	}

	if (m_ShowEditBtn)
		GetDlgItem(IDC_EDITIT)->ShowWindow(SW_SHOWNORMAL);

	if (m_ShowShowDiffs && !m_Key)
	{
		m_btShowDiffs.ShowWindow(SW_SHOWNORMAL);
		m_btShowDiffs.EnableWindow(TRUE);

		// Set the 2 IDs for the Diff button
		m_btShowDiffs.SetIDs(IDC_SHOWDIFFS, IDC_SHOWDIFFSMENU);
		m_btShowDiffs.SetSplit(m_DiffFlag != ID_SHOWDIFFS_NORMAL);

		//Set up the tooltip
		m_pToolTip = new CToolTipCtrl;
		if (!m_pToolTip->Create(this))
		{
		   TRACE("Unable To create ToolTip\n");
		}
		else if (!m_pToolTip->AddTool(&m_btShowDiffs, LoadStringResource(IDS_SHOWDIFFS_BTN_TOOLTIP)))
		{
		   TRACE("Unable to add Show Diffs button to the tooltip\n");
		}
		m_pToolTip->Activate(TRUE);
	}
	else
	{
		GetMenu()->DeleteMenu(3, MF_BYPOSITION);
	}

	if (m_ShowShowFixes && GET_P4REGPTR()->GetJobsEnabled())
	{
		GetDlgItem(IDC_SHOWFIXES)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_SHOWFIXES)->EnableWindow(TRUE);
	}

	if (m_ShowShowFiles)
	{
		GetDlgItem(IDC_SHOWFILES)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_SHOWFILES)->EnableWindow(TRUE);
	}

	if (!MainFrame()->HaveP4QTree())
		GetMenu()->DeleteMenu(ID_FILE_REVISIONTREE, MF_BYCOMMAND);

	// Set the font to the fixed dialog font
	CreateTheFont();

	// And then initialize the window position and edit control
	m_WinPos.RestoreWindowPosition();

    SetEditText();
    m_Text.SetEventMask(ENM_KEYEVENTS|ENM_LINK);
	m_Text.SetBackgroundColor(FALSE, GetSysColor(COLOR_BTNFACE));	// set background color to gray

    CHARFORMAT cf;
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR;
    cf.crTextColor = GetSysColor(COLOR_BTNTEXT);

	m_Text.SetSel(0, m_Description.GetLength());
	m_Text.SetSelectionCharFormat(cf);
	m_Text.SetSel(0, 0);

	if( m_ScrollPastComments )
		ScrollPastComments();

	SetHotSpots();

	SetWindowText(m_Caption);

	CMenu* hSysMenu = GetSystemMenu( FALSE );
	UINT uWhere = hSysMenu->GetMenuItemCount() - 2;
	// insert new items in reverse order
	hSysMenu->InsertMenu( uWhere, MF_STRING | MF_BYPOSITION, ID_HELP, LoadStringResource(IDS_SPECDESC_HELP) );
	hSysMenu->InsertMenu( uWhere, MF_STRING | MF_BYPOSITION, IDC_COMMAND, LoadStringResource(IDS_SPECDESC_MORECOMMANDS) );
	hSysMenu->InsertMenu( uWhere, MF_SEPARATOR | MF_BYPOSITION );
	hSysMenu->InsertMenu( uWhere, MF_STRING | MF_BYPOSITION, IDC_PRINT, LoadStringResource(IDS_SPECDESC_PRINT) );	
	hSysMenu->InsertMenu( uWhere, MF_STRING | MF_BYPOSITION, ID_PAGE_SETUP, LoadStringResource(IDS_SPECDESC_PAGESETUP) );
	hSysMenu->InsertMenu( uWhere, MF_SEPARATOR | MF_BYPOSITION );

	// remove the calltrack menu item if the super-secret reg setting isn't present:
	if (!AfxGetApp()->GetProfileInt(_T("Settings"), _T("CallTrack"), 0))
		GetMenu()->DeleteMenu(ID_CURRENTTASK,MF_BYCOMMAND);

	m_TurnOnReDraw = TRUE;

	// Always turn repainting back on - it might still be off and we currently can't
	// reliably detect those cases when we don't have to turn it back on.
	EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSpecDescDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
		SetWindowPos(&wndTop, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
}
	
void CSpecDescDlg::CreateTheFont()
{
	CString face= GET_P4REGPTR()->GetFontFace();
	int size= GET_P4REGPTR()->GetFontSize();
	int weight= GET_P4REGPTR()->GetFontWeight();
	BOOL isItalic= GET_P4REGPTR()->GetFontItalic();

	LOGFONT logFont;
	CWindowDC dc(this);
	int i = GetDeviceCaps(dc.m_hDC, NUMCOLORS);
	m_MoreThan256Colors = ((i < 0) || (i > 256)) ? TRUE : FALSE;
	memset( &logFont, 0, sizeof(LOGFONT) );

	// create the regular dialog font
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfPitchAndFamily= FIXED_PITCH | FF_DONTCARE;
	lstrcpy(logFont.lfFaceName, face.GetBuffer(face.GetLength()));
	logFont.lfHeight= -abs(size);	 
	logFont.lfWeight = weight;			//Regular	
	logFont.lfItalic = (BYTE) isItalic;
	logFont.lfCharSet = DEFAULT_CHARSET;

	m_Font.CreateFontIndirect( &logFont );
	m_Text.SetFont(&m_Font);
}

void CSpecDescDlg::SetEditText()
{
#ifndef UNICODE
    int numChars = MultiByteToWideChar(CP_ACP, 0, m_Description, -1, 0, 0);
    delete const_cast<LPWCH>(m_DescriptionW);
    m_DescriptionW = new WCHAR[numChars];
    MultiByteToWideChar(CP_ACP, 0, m_Description, -1, const_cast<LPWCH>(m_DescriptionW), numChars);
#else
    m_DescriptionW = m_Description;
#endif
	m_Text.SetWindowText(m_Description);
}

void CSpecDescDlg::ScrollPastComments()
{
    if(m_ScrollPastComments)
    {
        if(m_SkipLines == 0)
        {
            CString line;

            // Most specification output begins with a huge block of comment text
            // scroll past all that rot
            for( int i=0; i < m_Text.GetLineCount(); i++ )
            {
                const int MAX_LINE_LEN = 1024;
                LPTSTR buf= line.GetBuffer( MAX_LINE_LEN + 1);
                int size = m_Text.GetLine( i, buf, MAX_LINE_LEN );
                buf[size] = 0;
                line.ReleaseBuffer();
                line.TrimLeft(_T(" \t"));
                if( line.GetLength() > 0 && line[0] == _T('#') )
                    m_SkipLines++;
            }
        }
        m_Text.LineScroll(m_SkipLines);
        m_Text.SetSel(m_Text.LineIndex(m_SkipLines), m_Text.LineIndex(m_SkipLines));
    }
}

void CSpecDescDlg::SetDescription(LPCTSTR txt, BOOL scrollPastComments /*=TRUE*/)
{
    // copy to m_Description, replacing "\r\n" with "\n"
    // equivalent to but much, much faster than: 
    // m_Description = txt;
    // m_Description.Replace(_T("\r\n"), _T("\n"));
	LPTSTR pDesc = m_Description.GetBufferSetLength(lstrlen(txt)+1);
    while(*txt)
    {
        if(*txt == _T('\r') && txt[1] == _T('\n'))
        {
            txt++;
            *pDesc++ = *txt++;
        }
        else
        {
#ifdef UNICODE
			*pDesc++ = *txt++;
#else
            _tccpy(pDesc, txt);
            pDesc = CharNext(pDesc);
            txt = CharNext(txt);
#endif
        }
    }
    *pDesc = 0;
    m_Description.ReleaseBuffer();

	m_ScrollPastComments= scrollPastComments;
    m_SkipLines = 0;

	int lgth = m_Description.GetLength();
    if(lgth > 32000)
    {
	    // Find out if we're running NT or win95
	    OSVERSIONINFO osVer;
	    osVer.dwOSVersionInfoSize= sizeof(OSVERSIONINFO);
	    GetVersionEx(&osVer);
	    BOOL brittleWare= (osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);

		int trunc = 0;
    	if(brittleWare)
			trunc = 32000;
		else if (lgth > 60020)
		{
			if (osVer.dwMajorVersion < 5)
				trunc = 60000;
			else if (lgth > 256000)
				trunc = 256000;
		}
		if (trunc)
		{
			CString txt;
			txt.FormatMessage(IDS_DESCRIPTION_TRUNCATED_n, trunc);
	    	m_Description = m_Description.Left(trunc) + txt;
		}
    }
}

void CSpecDescDlg::OnSize(UINT nType, int cx, int cy) 
{
	CRect rect;

	CDialog::OnSize(nType, cx, cy);

	if (nType == SIZE_MINIMIZED)
	{
		if (!m_Item.IsEmpty())
			SetWindowText(m_Item);
	}
	else if (m_HasBeenMinimized)
		SetWindowText(m_Caption);

	GetClientRect(&rect);
	int x=rect.Width();
	int y=rect.Height();
	
	// Slide the OK button to the right of dlg
	CWnd *pOK=GetDlgItem(IDOK);
	if(pOK != NULL && IsWindow(pOK->m_hWnd))
	{
		pOK->GetWindowRect(&rect);
		ScreenToClient(&rect);
		pOK->MoveWindow(x-rect.Width()-4, y-rect.Height()-4, rect.Width(), rect.Height(), TRUE); 
		pOK->RedrawWindow();
		CWnd *pBtn = GetDlgItem(IDC_PRINT);
		pBtn->MoveWindow(4, y-rect.Height()-4, rect.Width(), rect.Height(), TRUE); 
		pBtn->RedrawWindow();
		if (m_ShowNextPrev)
		{
			pBtn = GetDlgItem(IDC_PREVITEM);
			pBtn->MoveWindow(4+rect.Width()+4, 
				y-rect.Height()-4, rect.Width(), rect.Height(), TRUE); 
			pBtn->RedrawWindow();
			pBtn = GetDlgItem(IDC_NEXTITEM);
			pBtn->MoveWindow(4+rect.Width()+4+rect.Width()+4, 
				y-rect.Height()-4, rect.Width(), rect.Height(), TRUE); 
			pBtn->RedrawWindow();
		}
		if (m_ShowEditBtn)
		{
			pBtn = GetDlgItem(IDC_EDITIT);
			pBtn->MoveWindow(4+rect.Width()+4+rect.Width()+4+rect.Width()+4, 
				y-rect.Height()-4, rect.Width(), rect.Height(), TRUE); 
			pBtn->RedrawWindow();
		}
		if (m_ShowShowDiffs)
		{
			pBtn = GetDlgItem(IDC_SHOWDIFFS);
			pBtn->MoveWindow(4+rect.Width()+4+rect.Width()+4+rect.Width()+4+rect.Width()+4, 
				y-rect.Height()-4, rect.Width()+18, rect.Height(), TRUE); 
			pBtn->RedrawWindow();
		}
		if (m_ShowShowFixes)
		{
			pBtn = GetDlgItem(IDC_SHOWFIXES);
			pBtn->MoveWindow(4+rect.Width()+4+rect.Width()+4+rect.Width()+4+rect.Width()+4, 
				y-rect.Height()-4, rect.Width(), rect.Height(), TRUE); 
			pBtn->RedrawWindow();
		}
		if (m_ShowShowFiles)
		{
			pBtn = GetDlgItem(IDC_SHOWFILES);
			pBtn->MoveWindow(4+rect.Width()+4+rect.Width()+4+rect.Width()+4+rect.Width()+4, 
				y-rect.Height()-4, rect.Width(), rect.Height(), TRUE); 
			pBtn->RedrawWindow();
		}

		// Increase the size of the edit control above the button
		m_Text.MoveWindow(4, 4, x-8, y-rect.Height()-14, TRUE);
	}
}

LRESULT CSpecDescDlg::OnQuitting(WPARAM wParam, LPARAM lParam)
{
	// P4Win is terminating, so close this dialog.
	CDialog::OnOK();
	return 0;
}

void CSpecDescDlg::OnClose() 
{
	if (m_pParent && m_Modeless)
		m_pParent->PostMessage(WM_P4ENDDESCRIBE, 0, (LPARAM)this);
	CDialog::OnOK();
}

void CSpecDescDlg::OnOK() 
{
	// If server is busy, wait a bit before exiting
	if( m_ChkServerBusy && SERVER_BUSY() )
	{
		Sleep(0);
		SET_BUSYCURSOR();
		// wait a bit in 1/10 sec intervals to see if the server request finishes
		int t=GET_P4REGPTR()->BusyWaitTime();
		do
		{
			Sleep(50);
			t -= 50;
		} while (SERVER_BUSY() && t > 0);
		if( SERVER_BUSY() )
		{
			::PostMessage(MainFrame()->m_hWnd, WM_COMMAND, ID_CANCEL_BUTTON, 0);
			return;
		}
	}

	m_WinPos.SaveWindowPosition();

	if (m_pParent && m_Modeless)
		m_pParent->PostMessage(WM_P4ENDDESCRIBE, 0, (LPARAM)this);
	CDialog::OnOK();
}

// These 2 routines handle the Next and Prev buttons.
//
// Note that we have to turn off the painting for all child windows of the main P4Win window
// to prevent flashing.  This works because the describe dialogbox is not a child of the
// P4win main window - it's a top level window of its own.
// We turn the painting back on in the caller of this dialogbox when they pess OK, Enter or ESC

void CSpecDescDlg::OnNextitem() 
{
	if ((!m_Key && SERVER_BUSY()) || !m_ShowNextPrev)
	{
		MessageBeep(0);
		return;
	}
	m_WinPos.SaveWindowPosition();
	SetWindowText( LoadStringResource(IDS_PERFORCE_DESCRIPTION) );
	m_Text.SetWindowText( LoadStringResource(IDS_LOADING) );
	UpdateWindow( );

	// Turn off all painting in children of main window to prevent flashing
	EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, FALSE);

	if (m_pParent && m_Modeless)
		m_pParent->PostMessage(WM_P4ENDDESCRIBE, IDC_NEXTITEM, (LPARAM)this);
	CDialog::EndDialog(IDC_NEXTITEM);
}

void CSpecDescDlg::OnPrevitem() 
{
	if ((!m_Key && SERVER_BUSY()) || !m_ShowNextPrev)
	{
		MessageBeep(0);
		return;
	}
	m_WinPos.SaveWindowPosition();
	SetWindowText( LoadStringResource(IDS_PERFORCE_DESCRIPTION) );
	m_Text.SetWindowText( LoadStringResource(IDS_LOADING) );
	UpdateWindow( );

	// Turn off all painting in children of main window to prevent flashing	
	EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, FALSE);

	if (m_pParent && m_Modeless)
		m_pParent->PostMessage(WM_P4ENDDESCRIBE, IDC_PREVITEM, (LPARAM)this);
	CDialog::EndDialog(IDC_PREVITEM);
}

void CSpecDescDlg::OnUpdateShowDiffsNormal(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ShowShowDiffs && m_DiffFlag != ID_SHOWDIFFS_NORMAL);
}

void CSpecDescDlg::OnUpdateShowDiffsSummary(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ShowShowDiffs && m_DiffFlag != ID_SHOWDIFFS_SUMMARY);
}

void CSpecDescDlg::OnUpdateShowDiffsUnified(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ShowShowDiffs && m_DiffFlag != ID_SHOWDIFFS_UNIFIED);
}

void CSpecDescDlg::OnUpdateShowDiffsContext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ShowShowDiffs && m_DiffFlag != ID_SHOWDIFFS_CONTEXT);
}

void CSpecDescDlg::OnUpdateShowDiffsRCS(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ShowShowDiffs && m_DiffFlag != ID_SHOWDIFFS_RCS);
}

void CSpecDescDlg::OnUpdateShowDiffsNone(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_ShowShowDiffs && m_DiffFlag);
}

void CSpecDescDlg::OnShowDiffsBtn() 
{
	CPoint point;
	CRect rect;
	GetDlgItem(IDC_SHOWDIFFS)->GetWindowRect(&rect);
	point.x = rect.left;
	point.y = rect.bottom + 1;
	OnContextMenu(GetDlgItem(IDC_SHOWDIFFS), point);
	m_btShowDiffs.ClearButtonPushed();
}

void CSpecDescDlg::OnShowDiffsNormal() 
{
	if (m_DiffFlag == ID_SHOWDIFFS_NORMAL)
		OnShowDiffsBtn();
	else
		OnShowDiffs(ID_SHOWDIFFS_NORMAL);
}

void CSpecDescDlg::OnShowDiffsSummary() 
{
	OnShowDiffs(ID_SHOWDIFFS_SUMMARY);
}

void CSpecDescDlg::OnShowDiffsUnified() 
{
	OnShowDiffs(ID_SHOWDIFFS_UNIFIED);
}

void CSpecDescDlg::OnShowDiffsContext() 
{
	OnShowDiffs(ID_SHOWDIFFS_CONTEXT);
}

void CSpecDescDlg::OnShowDiffsRCS() 
{
	OnShowDiffs(ID_SHOWDIFFS_RCS);
}

void CSpecDescDlg::OnShowDiffsNone() 
{
	OnShowDiffs(ID_SHOWDIFFS_NONE);
}

void CSpecDescDlg::OnShowDiffs(int flag) 
{
	if ((!m_Key && SERVER_BUSY()) || !m_ShowShowDiffs)
	{
		MessageBeep(0);
		return;
	}
	m_WinPos.SaveWindowPosition();
	SetWindowText( LoadStringResource(IDS_LOADING_DIFFS) );
	UpdateWindow( );

	// Turn off all painting in children of main window to prevent flashing	
	EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, FALSE);

	if (m_pParent && m_Modeless)
		m_pParent->PostMessage(WM_P4ENDDESCRIBE, flag, (LPARAM)this);
	CDialog::EndDialog(flag);
}

void CSpecDescDlg::OnEditButton() 
{
	if (SERVER_BUSY() || !m_ShowEditBtn)
	{
		MessageBeep(0);
		return;
	}
	m_WinPos.SaveWindowPosition();
	if (m_pParent && m_Modeless)
		m_pParent->PostMessage(WM_P4ENDDESCRIBE, IDC_EDITIT, (LPARAM)this);
	CDialog::EndDialog(IDC_EDITIT);
}

void CSpecDescDlg::OnPrint() 
{
	CString outBuf = m_Description;

	// remove comments from description
	int i;
	while (outBuf.GetAt(0) == _T('#'))
	{
		if ((i = outBuf.Find(_T('\n'))) == -1)
			break;
		outBuf = outBuf.Right(outBuf.GetLength() - i - 1);
	}
	outBuf.TrimLeft(_T("\r\n"));
	outBuf.TrimRight(_T("\r\n\t "));

	// if the whole spec is comments, they must be printing the spec def itself.
	if (outBuf.IsEmpty())
		outBuf = m_Description;

	MainFrame()->PrintString(outBuf, m_Caption);
}

void CSpecDescDlg::OnPageSetup() 
{
	MainFrame()->PageSetup();
}

void CSpecDescDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch(nID)
	{
	case ID_PAGE_SETUP:
		OnPageSetup();
		return;

	case IDC_PRINT:
		OnPrint();
		return;

	case ID_HELP:
		OnHelp();
		return;

	case SC_MINIMIZE:
		GetDesktopWindow()->ArrangeIconicWindows();
		break;
	}

	CDialog::OnSysCommand(nID, lParam);
}

void CSpecDescDlg::SetMenuFlags()
{
	long nStartChar;
	long nEndChar;
	m_fPopup = MF_GRAYED | MF_DISABLED | MF_POPUP;
	m_fMarked= MF_GRAYED | MF_STRING;
	m_fFile = MF_GRAYED | MF_STRING;
	m_fHist = MF_GRAYED | MF_STRING;
	m_fRev  = MF_GRAYED | MF_STRING;
	m_fProp = MF_GRAYED | MF_STRING;
	m_fChg  = MF_GRAYED | MF_STRING;
	m_fClTk = MF_GRAYED | MF_STRING;
	m_fItem = MF_GRAYED | MF_STRING;
	m_fCli  = MF_GRAYED | MF_STRING;
	m_fUser = MF_GRAYED | MF_STRING;
	m_fJob  = MF_GRAYED | MF_STRING;
	m_fEmail= MF_GRAYED | MF_STRING;
	m_fURL  = MF_GRAYED | MF_STRING;
	m_fDiff2= MF_GRAYED | MF_STRING;

	m_Text.GetSel( nStartChar,  nEndChar );
	if ( nStartChar < nEndChar )
	{
		int i = IsItaHotSpot(nStartChar, nEndChar);
		UINT fHotSpot = (i == -1) ? 0 : m_HotSpotType.GetAt(i);
		BOOL b1line = m_Text.LineFromChar(nStartChar) == m_Text.LineFromChar(nEndChar);
		m_fMarked= MF_ENABLED | MF_STRING;
		CString selText = m_Text.GetSelText();
		selText.TrimRight();
		selText.TrimLeft();
		switch(fHotSpot)
		{
		case HS_ISAFILE:
			m_fFile = MF_ENABLED | MF_STRING;
			break;
		case HS_ISACHG:
			m_fChg = MF_ENABLED | MF_STRING;
			break;
		case HS_ISAUSER:
			m_fUser = MF_ENABLED | MF_STRING;
			break;
		case HS_ISACLIENT:
			m_fCli = MF_ENABLED | MF_STRING;
			break;
		case HS_ISAJOB:
			m_fJob = MF_ENABLED | MF_STRING;
			break;
		case HS_ISAEMAIL:
			m_fEmail = MF_ENABLED | MF_STRING;
			break;
		case HS_ISAURL:
			m_fURL = MF_ENABLED | MF_STRING;
			break;
		case HS_ISDIFF2:
			m_fDiff2 = MF_ENABLED | MF_STRING;
			break;
		default:
			m_fFile = ((((selText.GetAt(0) == _T('/')) && (selText.GetAt(1) == _T('/')))
					 || ((selText.GetAt(0) == _T('"')) && (selText.GetAt(1) == _T('/')) && (selText.GetAt(2) == _T('/'))))
					&& (selText.Find(_T("//"), 2) == -1))
					? MF_ENABLED | MF_STRING : MF_GRAYED | MF_STRING;
			if (m_fFile == (MF_ENABLED | MF_STRING))
				break;
			if ((i = selText.Find(_T('@'))) != -1)
			{
				if (b1line && (selText.Find(_T(' ')) == -1))
				{
					if (selText.Find(_T('.'), i) != -1)
						m_fEmail = MF_ENABLED | MF_STRING;
					else
					{
						m_fPopup = MF_POPUP;
						m_fCli = m_fUser = MF_ENABLED | MF_STRING;
					}
				}
			}
			else if (b1line && (selText.Find(_T(' ')) == -1))
			{
				m_fPopup = MF_POPUP;
				if (_istdigit(selText.GetAt(0)))
				{
					m_fChg = m_fClTk = MF_ENABLED | MF_STRING;
					for (int i = 0; ++i < selText.GetLength(); )
					{
						if (!_istdigit(selText.GetAt(i)))
						{
							m_fChg = m_fClTk = MF_GRAYED | MF_STRING;
							break;
						}
					}
				}
				m_fItem = m_fCli = m_fUser = m_fJob = (m_fChg == (MF_GRAYED | MF_STRING)) 
						? MF_ENABLED | MF_STRING : MF_GRAYED | MF_STRING;
			}
			break;
		}
		if (m_fFile == (MF_ENABLED | MF_STRING))
		{
			m_fRev = selText.Find(_T('#')) > 2 ? MF_ENABLED | MF_STRING : MF_GRAYED | MF_STRING;
			if ((selText.Find(_T("...")) == -1) && (selText.FindOneOf(_T("*?")) == -1))
			{
				m_fProp = MF_ENABLED | MF_STRING;
				if (m_RevHistEnable)
					m_fHist = MF_ENABLED | MF_STRING;
			}
		}
	}
}

int CSpecDescDlg::IsItaHotSpot(int nStartChar, int nEndChar)
{
	int i;
	for (i = -1; ++i < m_numHotSpots; )
	{
		int	b = m_HotSpotBgn.GetAt(i);
		int	e = m_HotSpotEnd.GetAt(i);
		if ((nStartChar == b) && (nEndChar == e))
			return i;
		else if (nStartChar < b)
			break;
	}
	return -1;
}

void CSpecDescDlg::OnMsgfilterDescription(NMHDR* pNMHDR, LRESULT* pResult) 
{
	MSGFILTER *pMsgFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);
	*pResult = 0;

    if(pMsgFilter->nmhdr.idFrom == IDC_DESCRIPTION)
    {
		if(pMsgFilter->msg == WM_KEYDOWN)
		{
			// trap ctrl-c keydown message
			if(GetKeyState(VK_CONTROL) & 0x8000)
			{
				if (pMsgFilter->wParam == 0x43)			// ^C
				{
					// richedit should not process this message
					*pResult = 1;   
					// translate to a copy command so we can modify the copy process
					PostMessage(WM_COMMAND, ID_EDIT_COPY, 0);
				}
				else if (pMsgFilter->wParam == 0x46)	// ^F
				{
					// richedit should not process this message
					*pResult = 1;   
					// translate to a find command
					PostMessage(WM_COMMAND, ID_POSITIONTOPATTERN, 0);
				}
				else if (pMsgFilter->wParam == 0x54)	// ^T
				{
					// richedit should not process this message
					*pResult = 1;   
					OnOptions();
				}
			}
			else if(GetKeyState(VK_SHIFT) & 0x8000)
			{
				if (pMsgFilter->wParam == VK_F3)			// Shift+F3
				{
					// richedit should not process this message
					*pResult = 1;   
					// translate to a find previous command
					PostMessage(WM_COMMAND, ID_FINDPREV, 0);
				}
			}
			else
			{
				if (pMsgFilter->wParam == VK_F3)			// F3
				{
					// richedit should not process this message
					*pResult = 1;   
					// translate to a find next command
					PostMessage(WM_COMMAND, ID_FINDNEXT, 0);
				}
			}
		}
		else if(pMsgFilter->msg == WM_KEYUP)
		{
			if (pMsgFilter->wParam == VK_TAB)			// TAB key coming up == Desc got focus
				MakeSmartSelection();
		}
    }
}

void CSpecDescDlg::MakeSmartSelection()
{
	CHARRANGE cr, crtmp;
	m_Text.GetSel(cr);
	if (!cr.cpMin)
	{
		m_Text.LineScroll(0 - m_Text.GetFirstVisibleLine() + m_SkipLines);
		int nFirstVisChar = m_Text.LineIndex(m_SkipLines);
		crtmp.cpMin = 0;
		crtmp.cpMax = -1;
		m_Text.GetSel(crtmp);
		if (cr.cpMax >= crtmp.cpMax)
		{
			CPoint pt;
			pt.x = pt.y = 0;
			cr.cpMin = cr.cpMax = nFirstVisChar;
		}
		m_Text.SetSel(cr);
	}
}

void CSpecDescDlg::OnEditCopy() 
{
    CString selText= MakeLFs(m_Text.GetSelText());
    CopyTextToClipboard(selText);
}

void CSpecDescDlg::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
    // this should always be enabled
	pCmdUI->Enable(TRUE);
}

void CSpecDescDlg::OnEditSelectAll() 
{
	m_Text.SetSel( 0, -1 );
}

void CSpecDescDlg::OnUpdateOptions(CCmdUI* pCmdUI) 
{
    // this should always be enabled
	pCmdUI->Enable(TRUE);
}

void CSpecDescDlg::OnOptions() 
{
	CString face = GET_P4REGPTR()->GetFontFace();
	int size     = GET_P4REGPTR()->GetFontSize();
	int weight   = GET_P4REGPTR()->GetFontWeight();
	BOOL isItalic= GET_P4REGPTR()->GetFontItalic();

	MainFrame()->OnPerforceOptions(TRUE, FALSE, 0, IDS_PAGE_SPECIFICATIONS);

	if (face != GET_P4REGPTR()->GetFontFace()
	 || size != GET_P4REGPTR()->GetFontSize()
	 || weight != GET_P4REGPTR()->GetFontWeight()
	 || isItalic != GET_P4REGPTR()->GetFontItalic())
	{
		m_Font.DeleteObject();
		CreateTheFont();
	}
}

void CSpecDescDlg::OnFileRevisionhistory() 
{
	int i;
	int rev = -1;
	CString itemStr= m_Text.GetSelText();
	if ((i = itemStr.Find(_T('#'))) != -1)
	{
		rev = _ttoi(itemStr.Right(itemStr.GetLength() - i - 1));
		if (!rev)
			 rev = -1;
		itemStr = itemStr.Left(i);
	}
	
	CCmd_History *pCmd= new CCmd_History;
	pCmd->Init( MainFrame()->GetDepotWnd(), RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	pCmd->SetCallingWnd(m_hWnd);
	pCmd->SetInitialRev(rev, itemStr);
	if( pCmd->Run( LPCTSTR(itemStr)) )
	{
		UPDATE_STATUS( LoadStringResource(IDS_REQUESTING_HISTORY) );
	}
	else
		delete pCmd;
}

void CSpecDescDlg::OnFileRevisionTree() 
{
	int i;
	int rev = -1;
	CString itemStr= m_Text.GetSelText();
	if ((i = itemStr.Find(_T('#'))) != -1)
	{
		rev = _ttoi(itemStr.Right(itemStr.GetLength() - i - 1));
		if (!rev)
			 rev = -1;
		itemStr = itemStr.Left(i);
	}
	TheApp()->CallP4RevisionTree(itemStr);	
}

void CSpecDescDlg::OnFileInformation() 
{
	int i;
	CString itemStr= m_Text.GetSelText();
	if ((i = itemStr.Find(_T('#'))) != -1)
		itemStr = itemStr.Left(i);

	m_StringList.RemoveAll();
	m_StringList.AddHead(m_ItemStr = itemStr);
	
	CCmd_Opened *pCmd= new CCmd_Opened;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, m_Key);
	pCmd->SetAlternateReplyMsg( WM_P4FILEINFORMATION );

	if( pCmd->Run( TRUE, FALSE, -1, &m_StringList ) )
		UPDATE_STATUS( LoadStringResource(IDS_REQUESTING_FILE_INFORMATION) );
	else
		delete pCmd;
}

LRESULT CSpecDescDlg::OnP4FileInformation( WPARAM wParam, LPARAM lParam )
{
	CCmd_Opened *pCmd= (CCmd_Opened *) wParam;
	
	m_StringList.RemoveAll();
	if(!pCmd->GetError())
	{
		CString thisuser=GET_P4REGPTR()->GetMyID();
				
		// Initialize the file info dialog
		CFileInfoDlg *dlg = new CFileInfoDlg(this);

		dlg->m_DepotPath = m_ItemStr;

		int key= pCmd->GetServerKey();
		CCmd_Fstat *pCmd2= new CCmd_Fstat;
		
		pCmd2->Init(NULL, RUN_SYNC, HOLD_LOCK, key);
		if ( !PumpMessages( ) )
			goto CantGetFStat;

		pCmd2->SetIncludeAddedFiles( TRUE );
		if( pCmd2->Run( FALSE, m_ItemStr, TRUE, 0 ) && !pCmd2->GetError() )
		{
			CObList *list = pCmd2->GetFileList ( );
			ASSERT_KINDOF( CObList, list );
			ASSERT( list->GetCount() <= 1 );
			POSITION pos = list->GetHeadPosition( );
			if( pos != NULL )
			{
				CP4FileStats *stats = ( CP4FileStats * )list->GetNext( pos );
				ASSERT_KINDOF( CP4FileStats, stats );
				dlg->m_ClientPath = stats->GetFullClientPath( );
				if(dlg->m_ClientPath.GetLength() == 0)
					dlg->m_ClientPath= LoadStringResource(IDS_NOT_IN_CLIENT_VIEW);
				
				dlg->m_HeadRev.Format(_T("%ld"), stats->GetHeadRev());
				dlg->m_HaveRev.Format(_T("%ld"), stats->GetHaveRev());
				
				dlg->m_HeadAction= stats->GetActionStr(stats->GetHeadAction());
				dlg->m_HeadChange.Format(_T("%ld"), stats->GetHeadChangeNum());
				dlg->m_HeadType= stats->GetHeadType();
				dlg->m_ModTime= stats->GetFormattedHeadTime();
				dlg->m_FileSize= stats->GetFileSize();

				// Check for open/lock by this user
				if(stats->IsMyLock())
					dlg->m_LockedBy= thisuser;
				
				delete stats;
			}		
			else dlg->m_ClientPath= LoadStringResource(IDS_NOT_IN_CLIENT_VIEW);
		}

CantGetFStat:
		if (!m_Key)
			RELEASE_SERVER_LOCK(key);
		delete pCmd2;

		CObList *list= pCmd->GetList();
		ASSERT_KINDOF(CObList, list);

        POSITION pos= list->GetHeadPosition();
		while(pos != NULL)
		{
			CP4FileStats *fs= (CP4FileStats *) list->GetNext(pos);
			
			CString str;
			CString strUser;
			CString strChange;
			CString strAction;

			if( fs->GetOpenChangeNum() == 0 )
				strChange= LoadStringResource(IDS_DEFAULT_CHANGE);
			else
				strChange.FormatMessage(IDS_CHANGE_n, fs->GetOpenChangeNum()); 

			strUser= fs->GetOtherUsers();
			if( fs->IsMyOpen() && strUser.IsEmpty() )
			{
				strUser= thisuser;
				strAction= fs->GetActionStr(fs->GetMyOpenAction());
			}
			else
				strAction= fs->GetActionStr(fs->GetOtherOpenAction());

			str.Format(_T("%s - %s (%s)"), strUser, strChange, strAction);
			
			if( fs->IsOtherLock() )
				str += " " + LoadStringResource(IDS_STAR_LOCKED);
			
			dlg->m_StrList.AddHead( str );
			
			delete fs;
		}
		delete pCmd;		// no longer needed - delete it now before the dialog goes up
		// Display the info
		if (!dlg->Create(IDD_FILE_INFORMATION, this))	// display the description dialog box
		{
			dlg->DestroyWindow();	// some error! clean up
			delete dlg;
		}
	}
	else
		delete pCmd;

	UPDATE_STATUS(_T(""));
	
	return 0;
}

LRESULT CSpecDescDlg::OnP4EndFileInformation( WPARAM wParam, LPARAM lParam )
{
	CFileInfoDlg *dlg = (CFileInfoDlg *)lParam;
	dlg->DestroyWindow();
	return TRUE;
}

void CSpecDescDlg::OnPositionDepot()
{
	int i;
	CString itemStr = m_Text.GetSelText();
	if ((i = itemStr.Find(_T('#'))) != -1)
		itemStr = itemStr.Left( i );  // trim off rev# info
	else if ((i = itemStr.Find(_T("/..."))) != -1)
		itemStr = itemStr.Left( i );  // trim off "/..."
	((CMainFrame *) AfxGetMainWnd())->ExpandDepotString( itemStr, TRUE );
}

void CSpecDescDlg::OnDiffHead()
{
	int i;
	int rev = -1;
	int headRev = -1;
	CString fileType = _T("text");
	CString name = m_Text.GetSelText();
	if ((i = name.Find(_T('#'))) != -1)
	{
		rev = _ttoi(name.Right(name.GetLength() - i - 1));
		if (!rev)
			 rev = -1;
		name = name.Left( i );  // trim off rev# info
	}

	if ( !PumpMessages( ) )
		return;		// 0 -> quitting

	CCmd_Fstat *pCmd2= new CCmd_Fstat;
	pCmd2->Init(NULL, RUN_SYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	pCmd2->SetIncludeAddedFiles( TRUE );
	if( pCmd2->Run( FALSE, name, TRUE, 0 ) && !pCmd2->GetError() )
	{
		CObList *coblist2 = pCmd2->GetFileList( );
		ASSERT_KINDOF( CObList, coblist2 );
		ASSERT( coblist2->GetCount() <= 1 );
		POSITION pos = coblist2->GetHeadPosition( );
		if( pos != NULL )
		{
			CP4FileStats *stats = ( CP4FileStats * )coblist2->GetNext( pos );
			ASSERT_KINDOF( CP4FileStats, stats );
			fileType= stats->GetHeadType();
			headRev = stats->GetHeadRev();
			delete stats;
		}		
	}
	delete pCmd2;

	if ((rev == -1) || (headRev == -1))
	{
		CString txt = LoadStringResource(IDS_UNABLE_TO_DETERMINE_REV_NUMBERS_DIFF_FAILS);
		if (headRev == -1)
			txt += LoadStringResource(IDS_SPECDESC_FILE_NOT_IN_CLIENT_VIEW);
		AfxMessageBox(txt, MB_OK | MB_ICONHAND);
		return;
	}
	if (rev == headRev)
	{
		AfxMessageBox(IDS_THIS_IS_THE_HEAD_REV, MB_OK);
		return;
	}

	CCmd_Diff2 *pCmd= new CCmd_Diff2;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	if( pCmd->Run( name, name, rev, headRev, fileType, fileType) )
	{
		UPDATE_STATUS( LoadStringResource(IDS_DIFFING_FILES) );
	}
	else
		delete pCmd;
}

void CSpecDescDlg::OnDiffPrev()
{
	int i;
	int rev = -1;
	int prevRev = -1;
	CString fileType = _T("text");
	CString name = m_Text.GetSelText();
	if ((i = name.Find(_T('#'))) != -1)
	{
		rev = _ttoi(name.Right(name.GetLength() - i - 1));
		if (!rev)
			 rev = -1;
		name = name.Left( i );  // trim off rev# info
	}
	if (rev < 2)
	{
		AfxMessageBox(rev == 1 ? 
            IDS_THERE_IS_NO_PREVIOUS_REV : 
            IDS_UNABLE_TO_DETERMINE_REV_NUMBER_DIFF_FAILS,
			MB_OK);
		return;
	}

	prevRev = rev - 1;

	if ( !PumpMessages( ) )
		return;		// 0 -> quitting

	CCmd_Fstat *pCmd2= new CCmd_Fstat;
	pCmd2->Init(NULL, RUN_SYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	pCmd2->SetIncludeAddedFiles( TRUE );
	if( pCmd2->Run( FALSE, name, TRUE, 0 ) && !pCmd2->GetError() )
	{
		CObList *coblist2 = pCmd2->GetFileList( );
		ASSERT_KINDOF( CObList, coblist2 );
		ASSERT( coblist2->GetCount() <= 1 );
		POSITION pos = coblist2->GetHeadPosition( );
		if( pos != NULL )
		{
			CP4FileStats *stats = ( CP4FileStats * )coblist2->GetNext( pos );
			ASSERT_KINDOF( CP4FileStats, stats );
			fileType= stats->GetHeadType();
			delete stats;
		}		
	}
	delete pCmd2;

	CCmd_Diff2 *pCmd= new CCmd_Diff2;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	if( pCmd->Run( name, name, prevRev, rev, fileType, fileType) )
	{
		UPDATE_STATUS( LoadStringResource(IDS_DIFFING_FILES) );
	}
	else
		delete pCmd;
}

void CSpecDescDlg::OnDiffCliFile()
{
	int i;
	int rev = -1;
	CString fileType = _T("text");
	CString name = m_Text.GetSelText();
	CString clifile;
	if ((i = name.Find(_T('#'))) != -1)
	{
		rev = _ttoi(name.Right(name.GetLength() - i - 1));
		if (!rev)
			 rev = -1;
		name = name.Left( i );  // trim off rev# info
	}

	if ( !PumpMessages( ) )
		return;		// 0 -> quitting

	CCmd_Fstat *pCmd2= new CCmd_Fstat;
	pCmd2->Init(NULL, RUN_SYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	pCmd2->SetIncludeAddedFiles( TRUE );
	if( pCmd2->Run( FALSE, name, 0 ) && !pCmd2->GetError() )
	{
		CObList *coblist2 = pCmd2->GetFileList( );
		ASSERT_KINDOF( CObList, coblist2 );
		ASSERT( coblist2->GetCount() <= 1 );
		POSITION pos = coblist2->GetHeadPosition( );
		if( pos != NULL )
		{
			CP4FileStats *stats = ( CP4FileStats * )coblist2->GetNext( pos );
			ASSERT_KINDOF( CP4FileStats, stats );
			fileType= stats->GetHeadType();
			clifile = stats->GetFullClientPath();
			delete stats;
		}		
	}
	delete pCmd2;

	if (clifile.IsEmpty())
	{
		CString msg = name + _T(" ") + LoadStringResource(IDS_NOT_IN_CLIENT_VIEW);
		AfxMessageBox(msg, MB_ICONINFORMATION);
		return;
	}

	CCmd_Diff2 *pCmd= new CCmd_Diff2;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	if( pCmd->Run( name, clifile, rev, rev, fileType, fileType, FALSE, TRUE) )
	{
		UPDATE_STATUS( LoadStringResource(IDS_DIFFING_FILES) );
	}
	else
		delete pCmd;
}

LRESULT CSpecDescDlg::OnP4Diff2(WPARAM wParam, LPARAM lParam)
{
	CCmd_Diff2 *pCmd= (CCmd_Diff2 *) wParam;
	CString msg= pCmd->GetInfoText();
	if( ! msg.IsEmpty() )
	{
		AfxMessageBox( msg, MB_ICONINFORMATION);
	}
	
	UPDATE_STATUS(_T(""));
	delete pCmd;
	return 0;
}

void CSpecDescDlg::OnUpdateSync(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fRev & (MF_GRAYED|MF_DISABLED)));
}


void CSpecDescDlg::OnUpdatePositionDepot(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fFile & (MF_GRAYED|MF_DISABLED)) && !m_Key);
}


void CSpecDescDlg::OnUpdateDiffHead(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fRev & (MF_GRAYED|MF_DISABLED)));
}


void CSpecDescDlg::OnUpdateDiffPrev(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fRev & (MF_GRAYED|MF_DISABLED)));
}

void CSpecDescDlg::OnUpdateDiffCliFile(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fRev & (MF_GRAYED|MF_DISABLED)));
}

void CSpecDescDlg::OnUpdateFileAutobrowse(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fRev & (MF_GRAYED|MF_DISABLED)));
}                                        

void CSpecDescDlg::OnUpdateFileAnnotate(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(GET_SERVERLEVEL() >= 14 && !(m_fRev & (MF_GRAYED|MF_DISABLED)));
}

void CSpecDescDlg::OnUpdateFileInformation(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fProp & (MF_GRAYED|MF_DISABLED)));
}


void CSpecDescDlg::OnUpdateFileRevisionhistory(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fHist & (MF_GRAYED|MF_DISABLED)));
}

void CSpecDescDlg::OnUpdateDescChg(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fChg & (MF_GRAYED|MF_DISABLED)));
}

void CSpecDescDlg::OnUpdateDescBranch(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fItem & (MF_GRAYED|MF_DISABLED)));
}


void CSpecDescDlg::OnUpdateDescLabel(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fItem & (MF_GRAYED|MF_DISABLED)));
}


void CSpecDescDlg::OnUpdateDescClient(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fCli & (MF_GRAYED|MF_DISABLED)));
}


void CSpecDescDlg::OnUpdateDescUser(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fUser & (MF_GRAYED|MF_DISABLED)));
}


void CSpecDescDlg::OnUpdateDescJob(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fJob & (MF_GRAYED|MF_DISABLED)));
}

void CSpecDescDlg::OnUpdateCallTrack(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fClTk & (MF_GRAYED|MF_DISABLED)));
}

void CSpecDescDlg::OnUpdateEmail(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fEmail & (MF_GRAYED|MF_DISABLED)));
}

void CSpecDescDlg::OnUpdateURL(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fURL & (MF_GRAYED|MF_DISABLED)));
}

void CSpecDescDlg::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
    long nStartChar, nEndChar;
    m_Text.GetSel(nStartChar, nEndChar);
	pCmdUI->Enable(nStartChar < nEndChar);
}

void CSpecDescDlg::OnSync() 
{
	int i;
	CString rev;
	CString itemStr= m_Text.GetSelText();
	if ((i = itemStr.Find(_T('#'))) != -1)
	{
		rev = itemStr.Right(itemStr.GetLength() - i - 1);
		itemStr = itemStr.Left(i);
	}
	if (rev.GetLength() == 0)
	{
		AfxMessageBox(IDS_UNABLE_TO_DETERMINE_REV_NUMBER_SYNC_FAILS,
			MB_ICONSTOP);
		return;
	}
	itemStr += _T("#") + rev;

	m_StringList.RemoveAll();
	m_StringList.AddHead(itemStr);
	CCmd_Get *pCmd= new CCmd_Get;
	pCmd->Init( MainFrame()->GetDepotWnd(), RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	if( pCmd->Run( &m_StringList, FALSE ) )
		UPDATE_STATUS( LoadStringResource(IDS_FILE_SYNC) );
	else
		delete pCmd;
}

void CSpecDescDlg::OnFileAutobrowse() 
{
	int i;
	int rev = -1;
	CString fileType = _T("text");
	CString name = m_Text.GetSelText();
	if ((i = name.Find(_T('#'))) != -1)
	{
		rev = _ttoi(name.Right(name.GetLength() - i - 1));
		if (!rev)
			 rev = -1;
		name = name.Left( i );  // trim off rev# info
	}

	if ( !PumpMessages( ) )
		return;		// 0 -> quitting

	CCmd_Fstat *pCmd2= new CCmd_Fstat;
	pCmd2->Init(NULL, RUN_SYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	pCmd2->SetIncludeAddedFiles( TRUE );
	if( pCmd2->Run( FALSE, name, TRUE, 0 ) && !pCmd2->GetError() )
	{
		CObList *coblist2 = pCmd2->GetFileList( );
		ASSERT_KINDOF( CObList, coblist2 );
		ASSERT( coblist2->GetCount() <= 1 );
		POSITION pos = coblist2->GetHeadPosition( );
		if( pos != NULL )
		{
			CP4FileStats *stats = ( CP4FileStats * )coblist2->GetNext( pos );
			ASSERT_KINDOF( CP4FileStats, stats );
			fileType= stats->GetHeadType();
			delete stats;
		}
		else
		{
			AfxMessageBox(IDS_UNABLE_TO_DETERMINE_FILE_TYPE_VIEW_FAILS, MB_ICONSTOP);
			rev = -2;
		}
	}
	else
	{
		AfxMessageBox(IDS_UNABLE_TO_DETERMINE_FILE_TYPE_VIEW_FAILS, MB_ICONSTOP);
		rev = -2;
	}
	delete pCmd2;
	if (rev == -2)
		return;

	if (rev == -1)
	{
		AfxMessageBox(IDS_UNABLE_TO_DETERMINE_REV_NUMBER_VIEW_FAILS,
			MB_ICONSTOP);
		return;
	}

	// Ask the user to pick a viewer
	CViewerDlg dlg;
	SET_APP_HALTED(TRUE);
	if(dlg.DoModal() == IDCANCEL)
	{
		SET_APP_HALTED(FALSE);
		return;
	}
	SET_APP_HALTED(FALSE);
	m_Viewer=dlg.GetViewer();
	if(m_Viewer != _T("SHELLEXEC"))
		GET_P4REGPTR()->AddMRUViewer(m_Viewer);

	m_ViewFileIsText = ((fileType.Find(_T("text")) != -1) 
					 || (fileType.Find(_T("symlink")) != -1)) ? TRUE : FALSE;

	// Fetch the selected revision of the file to a temp filename
	CCmd_PrepBrowse *pCmd= new CCmd_PrepBrowse;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	if( pCmd->Run( name, fileType, rev ) )
	{
		UPDATE_STATUS( LoadStringResource(IDS_FETCHING_FILE) );
	}
	else
		delete pCmd;
}

void CSpecDescDlg::OnFileAnnotate() 
{
	int i;
	int rev = -1;
	CString fileType = _T("text");
	CString name = m_Text.GetSelText();
	if ((i = name.Find(_T('#'))) != -1)
	{
		rev = _ttoi(name.Right(name.GetLength() - i - 1));
		if (!rev)
			 rev = -1;
		name = name.Left( i );  // trim off rev# info
	}

	if ( !PumpMessages( ) )
		return;		// 0 -> quitting

	if (MainFrame()->HaveTLV())
	{
		TheApp()->CallP4A(name, _T(""), 0);	// use p4v.exe for annotate
		return;
	}

	CCmd_Fstat *pCmd2= new CCmd_Fstat;
	pCmd2->Init(NULL, RUN_SYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	pCmd2->SetIncludeAddedFiles( TRUE );
	if( pCmd2->Run( FALSE, name, TRUE, 0 ) && !pCmd2->GetError() )
	{
		CObList *coblist2 = pCmd2->GetFileList( );
		ASSERT_KINDOF( CObList, coblist2 );
		ASSERT( coblist2->GetCount() <= 1 );
		POSITION pos = coblist2->GetHeadPosition( );
		if( pos != NULL )
		{
			CP4FileStats *stats = ( CP4FileStats * )coblist2->GetNext( pos );
			ASSERT_KINDOF( CP4FileStats, stats );
			fileType= stats->GetHeadType();
			delete stats;
		}
		else
		{
			AfxMessageBox(IDS_UNABLE_TO_DETERMINE_FILE_TYPE_VIEW_FAILS, MB_ICONSTOP);
			rev = -2;
		}
	}
	else
	{
		AfxMessageBox(IDS_UNABLE_TO_DETERMINE_FILE_TYPE_VIEW_FAILS, MB_ICONSTOP);
		rev = -2;
	}
	delete pCmd2;
	if (rev == -2)
		return;

	if (rev == -1)
	{
		AfxMessageBox(IDS_UNABLE_TO_DETERMINE_REV_NUMBER_VIEW_FAILS,
			MB_ICONSTOP);
		return;
	}

	m_ViewFileIsText = ((fileType.Find(_T("text")) != -1) 
					 || (fileType.Find(_T("symlink")) != -1)) ? TRUE : FALSE;

	// Fetch the annotated file to a temp filename
	CCmd_PrepBrowse *pCmd= new CCmd_PrepBrowse;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	if( pCmd->Run( FALSE, name, fileType, FALSE, FALSE, FALSE, rev, 
				GET_P4REGPTR()->GetAnnotateWhtSpace()) )
	{
		UPDATE_STATUS( LoadStringResource(IDS_FETCHING_FILE) );
		m_Viewer=GET_P4REGPTR()->GetEditApp();
	}
	else
		delete pCmd;
}

// TODO: This code is pretty much a copy of the code in CDepotView::RunViewer()
// Might want to craft a single file viewing class that can be instantiated from
// anywhere, or perhaps make CMainFrame be responsible for all file viewing.
LRESULT CSpecDescDlg::OnP4ViewFile(WPARAM wParam, LPARAM lParam)
{
	UPDATE_STATUS(_T(""));
	CString tempName;
	CString msg;

	CCmd_PrepBrowse *pCmd= (CCmd_PrepBrowse *) wParam;

	if(!pCmd->GetError())
	{
		CString viewFilePath= pCmd->GetTempName();

		// First, get the file extension, if any, and find out if
		// its a text file
		CString extension;
		int slash= viewFilePath.ReverseFind(_T('\\'));
		if(slash != -1)
			extension=viewFilePath.Mid(slash+1);
		else
			extension=viewFilePath;

		int dot= extension.ReverseFind(_T('.'));
		if(dot == -1)
			extension.Empty();
		else
			extension=extension.Mid(dot+1);

		// We have the file, viewFilePath, try to display it
		while(1)
		{
			if(m_Viewer == _T("SHELLEXEC"))
			{
				CString assocViewer;

				// First, see if there a P4win file association
				if(!extension.IsEmpty())
					assocViewer= GET_P4REGPTR()->GetAssociatedApp(extension);
			
				// If we still havent found a viewer, set viewer to default text app
				// if user wishes to ignore windows associations
				if(assocViewer.IsEmpty() && m_ViewFileIsText && GET_P4REGPTR()->GetIgnoreWinAssoc())
					assocViewer= GET_P4REGPTR()->GetEditApp();
			
				// Let windows take a crack at finding a viewer
				if(assocViewer.IsEmpty() && !extension.IsEmpty())
				{
					// Quick check for executeable extension, which will make ShellExec try to run the file
					HINSTANCE hinst=0;
					if( extension.CompareNoCase(_T("com")) != 0 && extension.CompareNoCase(_T("exe")) != 0 &&
						extension.CompareNoCase(_T("bat")) != 0 && extension.CompareNoCase(_T("cmd")) != 0)
					{										// give VS .NET 7.1 (non-standard!) a try
						hinst= ShellExecute( m_hWnd, _T("Open.VisualStudio.7.1"), viewFilePath, NULL, NULL, SW_SHOWNORMAL);
						if( (INT_PTR) hinst > 32)
						{
							break;  // successfull viewer launch
						}
						if( (INT_PTR) hinst == SE_ERR_NOASSOC)	// give MSDEV (non-standard!) a try
						{
							hinst= ShellExecute( m_hWnd, _T("&Open with MSDEV"), viewFilePath, NULL, NULL, SW_SHOWNORMAL);
							if( (INT_PTR) hinst > 32 ) 
								break;  // successfull MSDEV viewer launch
						}
						if( (INT_PTR) hinst == SE_ERR_NOASSOC)	// give standard "open" a try
						{
							hinst= ShellExecute( m_hWnd, _T("open"), viewFilePath, NULL, NULL, SW_SHOWNORMAL);
							if( (INT_PTR) hinst > 32 ) 
								break;  // successfull MSDEV viewer launch
						}
					}
				}

				// If windows doesnt have an associated viewer for a text file, we use the 
				// default text editor
				if(assocViewer.IsEmpty() && m_ViewFileIsText)
					assocViewer= GET_P4REGPTR()->GetEditApp();
				

				if ( TheApp()->RunViewerApp( assocViewer, viewFilePath ) )
					break;  // successfull viewer launch
			}
			else
			{
				if ( TheApp()->RunViewerApp( m_Viewer, viewFilePath ) )
					break;  // successfull viewer launch
			}

			CString msg;
			msg.FormatMessage(IDS_UNABLE_TO_LAUNCH_VIEWER_s, viewFilePath);
			if(AfxMessageBox(msg, MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
				break;

			// Try to find an alternate viewer
			CViewerDlg dlg;
			SET_APP_HALTED(TRUE);
			if(dlg.DoModal() == IDCANCEL)
			{
				SET_APP_HALTED(FALSE);
				break;
			}

			SET_APP_HALTED(FALSE);
			m_Viewer=dlg.GetViewer();
			if(m_Viewer != _T("SHELLEXEC"))
				GET_P4REGPTR()->AddMRUViewer(m_Viewer);
		} // while
	} // no command error
	
	delete pCmd;
	UPDATE_STATUS(_T(""));
	return 0;
}

void CSpecDescDlg::OnDescItem(HWND hWnd, int viewType, int flag /*=0*/)
{
	m_SelItem.TrimRight();
	m_SelItem.TrimLeft();
	CCmd_Describe *pCmd = new CCmd_Describe;
	// if a command is in progress, the new dialogs are modeless but their parent
	// needs to be this dialog for proper cleanup if the user does something stupid.
	pCmd->Init( SERVER_BUSY() ? m_hWnd : hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key );
	if( pCmd->Run( m_SelType = viewType, m_SelItem, NULL, FALSE, flag ) )
	{
		UPDATE_STATUS( LoadStringResource(IDS_FETCHING_SPEC) );	
		return;
	}
	else
	{
		delete pCmd;
		return;
	}
}

void CSpecDescDlg::OnDescChg()
{
	m_SelItem = m_Text.GetSelText();
	OnDescItem(MainFrame()->OldChgsWnd(), P4DESCRIBE);
}

void CSpecDescDlg::OnDescChgLong(int flag)
{
	OnDescItem(MainFrame()->OldChgsWnd(), P4DESCRIBELONG, flag);
}

void CSpecDescDlg::OnDescBranch()
{
	m_SelItem = m_Text.GetSelText();
	OnDescItem(MainFrame()->BranchWnd(), P4BRANCH_SPEC);
}

void CSpecDescDlg::OnDescLabel()
{
	m_SelItem = m_Text.GetSelText();
	OnDescItem(MainFrame()->LabelWnd(), P4LABEL_SPEC);
}

void CSpecDescDlg::OnDescClient()
{
	int i;
	m_SelItem = m_Text.GetSelText();
	if ((i = m_SelItem.Find(_T('@'))) != -1)
		m_SelItem = m_SelItem.Right(m_SelItem.GetLength() - i - 1);
	OnDescItem(MainFrame()->ClientWnd(), P4CLIENT_SPEC);
}

void CSpecDescDlg::OnDescUser()
{
	int i;
	m_SelItem = m_Text.GetSelText();
	if ((i = m_SelItem.Find(_T('@'))) != -1)
		m_SelItem = m_SelItem.Left(i);
	OnDescItem(MainFrame()->UserWnd(), P4USER_SPEC);
}

void CSpecDescDlg::OnDescJob()
{
	m_SelItem = m_Text.GetSelText();
	OnDescItem(MainFrame()->JobWnd(), P4JOB_SPEC);
}

void CSpecDescDlg::OnUpdateDiff2(CCmdUI* pCmdUI) 
{
    SetMenuFlags();
	pCmdUI->Enable(!(m_fDiff2 & (MF_GRAYED|MF_DISABLED)));
}

void CSpecDescDlg::OnDiff2()
{
	TCHAR szBuffer[4096];
	LPTSTR lpszBuffer = szBuffer;
	CString curLine;
	long nStartChar;
	long nEndChar;

	m_Text.GetSel(nStartChar, nEndChar);
	long lineNbr = m_Text.LineFromChar(-1);
	int lgth = m_Text.GetLine(lineNbr, lpszBuffer, sizeof(szBuffer)/sizeof(TCHAR) - 6);
	szBuffer[lgth] = _T('\0');
	curLine = &szBuffer[0];
	while (curLine && curLine.GetAt(0) != _T('=') || curLine.Find(_T("//")) == -1)
	{
		CString tempLine = curLine;		// temp variable needed becuase of MFC bug
		lgth = m_Text.GetLine(--lineNbr, lpszBuffer, sizeof(szBuffer)/sizeof(TCHAR) - 6);
		szBuffer[lgth] = _T('\0');
		curLine = &szBuffer[0] + tempLine;
	}
	curLine.TrimLeft(_T(" ="));
	ASSERT(curLine.GetAt(0) == _T('/'));

	int i;
	if ((i = curLine.Find(_T('#'))) > 0)
	{
		CString file1 = curLine.Left(i);
		curLine = curLine.Mid(i+1);
		int rev1 = _tstoi(curLine);
		if ((i = curLine.Find(_T('('))) > 0)
		{
			CString ft1 = curLine = curLine.Mid(i+1);
			if ((i = ft1.Find(_T(')'))) > 0)
			{
				ft1 = ft1.Left(i);
				if ((i = curLine.Find(_T('/'))) > 0)
				{
					curLine = curLine.Mid(i);
					if ((i = curLine.Find(_T('#'))) > 0)
					{
						CString file2 = curLine.Left(i);
						curLine = curLine.Mid(i+1);
						int rev2 = _tstoi(curLine);
						if ((i = curLine.Find(_T('('))) > 0)
						{
							CString ft2 = curLine.Mid(i+1);
							if ((i = ft2.Find(_T(')'))) > 0)
							{
								ft2 = ft2.Left(i);
								CCmd_Diff2 *pCmd= new CCmd_Diff2;
								pCmd->Init( MainFrame()->GetDepotWnd(), RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key );
								if( pCmd->Run( file1, file2, rev1, rev2, ft1, ft2, FALSE, FALSE) )
									MainFrame()->UpdateStatus( LoadStringResource(IDS_DIFFING_FILES) );
								else
									delete pCmd;
								return;
							}
						}
					}
				}
			}
		}
	}
	AfxMessageBox(_T("Error parsing line\nUnable to determine files to diff"), MB_ICONSTOP);
}

LRESULT CSpecDescDlg::OnP4Describe( WPARAM wParam, LPARAM lParam )
{
	// Modal SpecDescDlgs are only called when the server is busy
	if (!SERVER_BUSY())
		ASSERT(0);

	MSG  msg;
	BOOL ret = FALSE;
	BOOL bOK = TRUE;
	CString txt;

	CCmd_Describe *pCmd = ( CCmd_Describe * )wParam;

	if(!pCmd->GetError() && !MainFrame()->IsQuitting())
	{
		CString desc = MakeCRs( pCmd->GetDescription( ) );

		if (m_SelType == P4JOB_SPEC)
		{
			int i, j = -2;
	        CString specblankdesc = _T("<enter description here>");
			if ((i = desc.Find(specblankdesc)) > 0)
			{
				j = lstrlen(_T("\r\nDescription:\r\n\t"));
				j = desc.Find(_T("\r\nDescription:\r\n\t"), i - j*2) + j;
			}
			if ((i == j) 
			 && (desc.GetAt(i-1) == _T('\t'))
			 && ((i+specblankdesc.GetLength() >= desc.GetLength()) 
			  || (desc.GetAt(i+specblankdesc.GetLength()) < _T(' '))))
			{
				txt.FormatMessage(IDS_THERE_IS_NO_s_TO_DESCRIBE, m_SelItem);
 				bOK = FALSE;
			}
		}
		else if (((m_SelType == P4BRANCH_SPEC) || (m_SelType == P4LABEL_SPEC) 
			   || (m_SelType == P4CLIENT_SPEC) || (m_SelType == P4USER_SPEC)) 
			  && (desc.Find(_T("\nUpdate:")) == -1))
		{
			txt = m_SelItem + _T(" - no such ");
			switch(m_SelType)
			{
			case P4BRANCH_SPEC:
				txt += _T("branch");
				break;
			case P4LABEL_SPEC:
				txt += _T("label");
				break;
			case P4CLIENT_SPEC:
				txt += _T("client");
				break;
			case P4USER_SPEC:
				txt += _T("user");
				break;
			}
			bOK = FALSE;
		}
		if (bOK)
		{
			CSpecDescDlg *dlg = new CSpecDescDlg(this);
			dlg->SetIsModeless(TRUE);
			dlg->SetKey(m_Key);
			dlg->SetDescription( desc );
			dlg->SetItemName( m_SelItem );
            CString caption;
            caption.FormatMessage(IDS_PERFORCE_DESCRIPTION_FOR_s, m_SelItem);
			dlg->SetCaption( caption );
			dlg->SetShowNextPrev(FALSE);
			dlg->SetShowShowDiffs(m_SelType == P4DESCRIBE || m_SelType == P4DESCRIBELONG);
			dlg->SetDiffFlag(pCmd->GetFlag());
			dlg->SetShowEditBtn(m_SelType == P4JOB_SPEC);
			dlg->SetShowShowFixes(m_SelType == P4JOB_SPEC);
			dlg->SetShowShowFiles(m_SelType == P4LABEL_SPEC);
			dlg->SetFindStrFlags(&m_FindWhatStr, m_FindWhatFlags);
			dlg->SetViewType(m_SelType);
			while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )	// clear the message queue
			{
				if ( msg.message == WM_QUIT )	//	get out if app is terminating
					break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (!dlg->Create(IDD_SPECDESC, this))	// display the description dialog box
			{
				dlg->DestroyWindow();	// some error! clean up
				delete dlg;
			}
		}
		else
		{
			EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
			AfxMessageBox(txt, MB_ICONWARNING);
		}
	}
	else	// had an error - need to turn painting back on
	{
		EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
		::InvalidateRect(NULL, NULL, TRUE);	// Make sure ALL windows get updated.
	}
	
	delete pCmd;
	UPDATE_STATUS( _T("") );
	return ret;
}

LRESULT CSpecDescDlg::OnP4EndDescribe(WPARAM wParam, LPARAM lParam)
{
	CSpecDescDlg *dlg = (CSpecDescDlg *)lParam;

	switch(wParam)				// which button did they click to close the box?
	{
	case ID_SHOWDIFFS_NORMAL:
	case ID_SHOWDIFFS_SUMMARY:
	case ID_SHOWDIFFS_UNIFIED:
	case ID_SHOWDIFFS_CONTEXT:
	case ID_SHOWDIFFS_RCS:
	case ID_SHOWDIFFS_NONE:
	{
		OnDescChgLong(int(wParam));
		break;
	}
	case IDC_EDITIT:
		if (!m_Key && wParam == IDC_EDITIT)
		{
			if (m_SelType == P4JOB_SPEC)
				MainFrame()->EditJobSpec(&m_SelItem);
			else
				ASSERT(0);
		}
	default:	// clicked OK, pressed ESC or ENTER - need to turn painting back on
		EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
		break;
	}
	dlg->DestroyWindow();
	return TRUE;
}

void CSpecDescDlg::OnEmail()
{
	CString seltxt = m_Text.GetSelText();
	seltxt.TrimRight(_T('>'));
	seltxt.TrimLeft(_T('<'));
	seltxt.TrimLeft();
	CString txt = _T("mailto:") + seltxt;
	INT_PTR s = (INT_PTR)ShellExecute(m_hWnd,_T("open"), txt,NULL,NULL,SW_SHOWNORMAL);
	if (s <= 32)
	{
		txt.FormatMessage(IDS_UNABLE_TO_SEND_EMAIL_TO_s, m_Text.GetSelText());
		AfxMessageBox(txt, MB_ICONSTOP);
	}
}

void CSpecDescDlg::OnURL()
{
	CString seltxt = m_Text.GetSelText();
	seltxt.TrimRight(_T(">\""));
	seltxt.TrimLeft(_T("<\""));
	seltxt.TrimLeft();
	INT_PTR s = (INT_PTR)ShellExecute(m_hWnd, _T("open"), seltxt,NULL,NULL,SW_SHOWNORMAL);
	if (s <= 32)
	{
		seltxt.FormatMessage(IDS_UNABLE_TO_BROWSE_s, m_Text.GetSelText());
		AfxMessageBox(seltxt, MB_ICONSTOP);
	}
}

void CSpecDescDlg::OnCallTrack()
{
	CString host = AfxGetApp()->GetProfileString(_T("Settings"), _T("CallTrackHost"), _T("calltrack"));
	CString txt = "http://" + host + _T("/cgi-bin/detail?call=") + m_Text.GetSelText();
	INT_PTR s = (INT_PTR)ShellExecute(m_hWnd,_T("open"), txt,NULL,NULL,SW_SHOWNORMAL);
	if (s <= 32)
	{
		txt = _T("Unable to display CallTrack number ") + m_Text.GetSelText();
		AfxMessageBox(txt, MB_ICONSTOP);
	}
}

void CSpecDescDlg::OnShowfixes() 
{
	CCmd_Fixes *pCmdFixes= new CCmd_Fixes;
	pCmdFixes->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	if( pCmdFixes->Run(0, 0, TRUE, m_Item) )
	{
		UPDATE_STATUS( LoadStringResource(IDS_UPDATING_JOB_FIXES) );
	}
	else
	{
		delete pCmdFixes;
		RedrawWindow();
		UPDATE_STATUS(_T(""));
	}
}

LRESULT CSpecDescDlg::OnP4Fixes(WPARAM wParam, LPARAM lParam)
{
	CCmd_Fixes *pCmd= (CCmd_Fixes *) wParam;

	if(!pCmd->GetError())
	{
		CObList *fixes = pCmd->GetList();
		CString theFixes;
		if (fixes->IsEmpty())
		{
			theFixes = LoadStringResource(IDS_EMPTY_FIXLIST);
		}
		else
		{
			CP4Fix *fix;
            CString buf;

			UpdateData(TRUE);
			theFixes = LoadStringResource(IDS_FIXLIST_HEADER);

			POSITION pos;
			for(pos= fixes->GetHeadPosition(); pos != NULL; )
			{
				fix=(CP4Fix *) fixes->GetNext(pos);
				ASSERT(fix->IsKindOf(RUNTIME_CLASS(CP4Fix)));
				
				buf.FormatMessage(IDS_FIXLIST_ITEM_n_ON_s_BY_s, fix->GetChangeNum(), 
								fix->GetFixDate(), fix->GetUser());
				theFixes += buf;
				delete fix;
			} //for
		}

        theFixes.Replace(_T("\r\n"), _T("\n"));
		m_Description += theFixes;
        SetEditText();
		m_Text.LineScroll(0 - m_Text.GetFirstVisibleLine());
		if( m_ScrollPastComments )
			ScrollPastComments();
		SetHotSpots();
		UpdateData(FALSE);
		GetDlgItem(IDC_SHOWFIXES)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SHOWFIXES)->EnableWindow(FALSE);
	}
	
	UPDATE_STATUS(_T(""));
		
	delete pCmd;
	return 0;
}

void CSpecDescDlg::OnShowfiles() 
{
	if (SERVER_BUSY())
	{
		MessageBeep(0);
		return;
	}

	m_LabelFileCount=0;
	m_LabelFiles.Empty();

	CString spec;
	spec.Format(_T("//...@%s"), m_Item);

	// Call Fstat, w/ suppress==FALSE
	CCmd_Fstat *pCmd= new CCmd_Fstat;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);

	//		okay, this is weird, but let's set show entire depot
	//		to true, since we want this command to 
	//		read 'p4 fstat //...@mynumber WITHOUT the -C
	//		that would run otherwise. 
	//		after all, we all the files to show, not just
	//		the ones on the client view.
	//
	BOOL bshowEntireDepot = TRUE;
	if( pCmd->Run( FALSE, spec, bshowEntireDepot, 0 ) )
	{
		MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_LABEL_CONTENTS) );
	}
	else
	{
		delete pCmd;
		MainFrame()->ClearStatus();
	}
}

LRESULT CSpecDescDlg::OnP4LabelContents(WPARAM wParam, LPARAM lParam)
{
	CString tmp;
    CCmd_Fstat *pCmd;

	if(lParam == 0)   // completion
	{
		pCmd= (CCmd_Fstat *) wParam;
		ASSERT_KINDOF(CCmd_Fstat,pCmd);

		if(!pCmd->GetError())
		{
			tmp.FormatMessage(IDS_LABEL_s_POINTS_TO_n_FILES, m_Item, m_LabelFileCount);
			AddToStatus(tmp, SV_COMPLETION);

			MainFrame()->ClearStatus();
			delete pCmd;
			if (m_LabelFileCount)
			{
				m_LabelFiles += _T('\n') + tmp;
				CSpecDescDlg *dlg = new CSpecDescDlg(this);
				dlg->SetIsModeless(TRUE);
				dlg->SetKey(m_Key);
				dlg->SetDescription( m_LabelFiles );
				dlg->SetItemName( m_Item );
                CString caption;
                caption.FormatMessage(IDS_FILE_LIST_FOR_LABEL_s, m_Item);
				dlg->SetCaption( caption );
				dlg->SetFindStrFlags(&m_FindWhatStr, m_FindWhatFlags);
				dlg->SetViewType(P4LABEL_SPEC);
				if (!dlg->Create(IDD_SPECDESC, this))	// display the description dialog box
				{
					dlg->DestroyWindow();	// some error! clean up
					delete dlg;
				}
				m_LabelFiles.Empty();
			}
		}
		return 0;
	}
	else
	{
        // Pull a ptr to the command, as well as a batch of CP4FileStats
        // out of the wrapper
        CFstatWrapper *pWrap= (CFstatWrapper *) wParam;
        pCmd= (CCmd_Fstat *) pWrap->pCmd;
	    ASSERT_KINDOF(CCmd_Fstat, pCmd);
		CObList *list= (CObList *) pWrap->pList;
		ASSERT_KINDOF(CObList, list);
        
		POSITION pos= list->GetHeadPosition();
		while(pos != NULL)
		{
			// Get the filestats
			CP4FileStats *stats= (CP4FileStats *) list->GetNext(pos);
			ASSERT_KINDOF(CP4FileStats, stats);
		
			// Increment the counter
			m_LabelFileCount++;

			// Format the file, rev and type
			tmp.FormatMessage(IDS_s_n_s_CHANGELIST_n_s, 
				stats->GetFullDepotPath(),
				stats->GetHeadRev(),
				stats->GetHeadType(),
				stats->GetHeadChangeNum(),
				stats->GetActionStr(stats->GetHeadAction()));

			// And add to Description
			m_LabelFiles += tmp;

			delete stats;

		} // while row batch not done

		delete list;
		delete pWrap;
		return 0;
	} // a batch of rows, we'll be called again so dont delete pCmd
}

BOOL CSpecDescDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return(FALSE);
}

void CSpecDescDlg::OnQuickHelp()
{
	AfxMessageBox(IDS_SPECDESC_QUICKHELP, MB_ICONINFORMATION);
}

void CSpecDescDlg::OnHelp()
{
	AfxGetApp()->WinHelp(TASK_GETTING_INFORMATION_ABOUT_FORMS);
}

int
CSpecDescDlg::AddHotSpotWord(int idx, int offset, int lineStart, int lgth, BOOL bAtSign)
{
    LPCWSTR line = m_DescriptionW + lineStart;

    // scan backwards from offset until a space or line ending is found
    LPCWSTR pBegin = line + offset - lineStart;
    while(*pBegin > ' ' && pBegin > line)
	{
		if (bAtSign && ((*pBegin == '\\') || (*pBegin == '/') || (*pBegin == ':')))
			return offset;	// bail because users, clients and email addrs don't have /, \ or :
		if (bAtSign && (*pBegin == '<'))
			break;	// because '<' terminates email and can't appear in users & clients
        pBegin--;
	}
    if(*pBegin <= ' ')
        pBegin++;

    // scan forwards from offset until space of line ending is found
    LPCWSTR pEnd = line + offset - lineStart;
    while(*pEnd > ' ')
	{
		if (bAtSign && ((*pEnd == '\\') || (*pEnd == '/') || (*pEnd == ':')))
			return offset;	// bail because users, clients and email addrs don't have /, \ or :
		if (bAtSign && (*pEnd == '>'))
			break;	// because '>' terminates email and can't appear in users & clients
        pEnd++;
	}

    // strip off angle brackets, if present
    if(*pBegin == '<')
    {
        pBegin++;
        if(*(pEnd-1) == '>')
            pEnd--;
    }

    // strip off parens, even if only left or right is present
    if(*pBegin == '(')
        pBegin++;
    if(*(pEnd-1)  == ')')
        pEnd--;

    // calculate indexes, and add hotspot
    int b = int(pBegin - line) + lineStart;
    int e = int(pEnd - line) + lineStart;
	m_HotSpotBgn.InsertAt(idx, b);
	m_HotSpotEnd.InsertAt(idx, e);
	return(e);
}

int
CSpecDescDlg::AddHotSpotFile(int idx, int offset, int lineStart, int lgth, BOOL bQuoted)
{
    // b points to either "// or -// or //

    int b = offset;
	size_t n = wcslen(m_TO_);
    if(m_DescriptionW[b] != '/')
        b++;

    LPCWSTR pEnd;
    if(bQuoted)
    {
        // scan forwards until first of: end of buffer, ending quote, or newline
        pEnd = m_DescriptionW + b + 2;
        while(*pEnd && *pEnd != '\"' && *pEnd >= ' ')
            pEnd++;
    }
    else
    {
    	BOOL bGotStop = FALSE;
        // start with pEnd after slashes
        for(pEnd = m_DescriptionW + b + 2; *pEnd; pEnd++)
	    {
            LPCWSTR pPrev = pEnd - 1;
			if (*pEnd == '#')
				bGotStop = TRUE;
			else if (*pEnd < ' ')
			{
                while(*pPrev == ' ' && pPrev >= m_DescriptionW + lineStart)
                    pEnd = pPrev--;
				break;
			}
			else if ((pEnd[0] == '.')
				  && (pEnd[1] == '.')
				  && (pEnd[2] == '.'))
			{
				bGotStop = TRUE;
                pEnd += 2;
			}
			else if ((pEnd[0] == '/')
				  && (pEnd[1] == '/'))
			{
                while(*pPrev == ' ' && pPrev >= m_DescriptionW + lineStart)
                    pEnd = pPrev--;
				break;
			}
			else if ((*pEnd == ' ') && !wcsncmp( pEnd, m_TO_, n ) 
				  && ((pEnd[n] == '/') || (pEnd[n] <= ' ')))
			{
				while(*pPrev == ' ' && pPrev >= m_DescriptionW + lineStart)
                    pEnd = pPrev--;
				break;
			}
			else if (bGotStop && (*pEnd == ' '))
				break;
    	}
    }
    int e = int(pEnd - m_DescriptionW);
	m_HotSpotBgn.InsertAt(idx, b);
	m_HotSpotEnd.InsertAt(idx, e);
	return(e);
}
#define STRLENW(s) (sizeof(s)/sizeof(WCHAR)-1)

void CSpecDescDlg::SetHotSpots()
{
    int lgth = int(wcslen(m_DescriptionW));
    LPCWSTR pLastMinus1 = m_DescriptionW + (lgth ? lgth - 1 : 0);
	BOOL bInJobs = FALSE;
	BOOL bInFixes = FALSE;
	BOOL bInDiffs = FALSE;

    // preallocate arrays, and set grow-by size
    m_HotSpotBgn.RemoveAll();
    m_HotSpotEnd.RemoveAll();
    m_HotSpotType.RemoveAll();
	m_HotSpotBgn.SetSize(m_Text.GetLineCount() - m_Text.GetFirstVisibleLine(), 10);
	m_HotSpotEnd.SetSize(m_Text.GetLineCount() - m_Text.GetFirstVisibleLine(), 10);
	m_HotSpotType.SetSize(m_Text.GetLineCount() - m_Text.GetFirstVisibleLine(), 10);
    m_numHotSpots = 0;

    int beginChar = m_Text.LineIndex();

	for (LPCWSTR pAt = m_DescriptionW + beginChar; *pAt; pAt++ )
	{
        // some strings we look for:
        static const WCHAR strModifiedBy[] = L"ModifiedBy:\t";
        static const WCHAR strUser[] = L"User:\t";
        static const WCHAR strJobsFixed[] = L"Jobs fixed ...";
        static const WCHAR strAffectedFilesEnglish[] = L"Affected files ...";
        static WCHAR strAffectedFiles[100] = L"";
        static WCHAR strFixes[100] = L"";
        static WCHAR strChange[100] = L"";
        static WCHAR strReportedBy[100] = L"";
        static const WCHAR strDifferences[] = L"Differences ...";
        static const WCHAR strChangelist[] = L"Changelist ";
		static const WCHAR strEqContent[] = L"==== content";

        static const int strModifiedByLen = STRLENW(strModifiedBy);
        static const int strUserLen = STRLENW(strUser);
        static const int strJobsFixedLen = STRLENW(strJobsFixed);
        static const int strAffectedFilesEnglishLen = STRLENW(strAffectedFilesEnglish);
        static int strAffectedFilesLen = STRLENW(strAffectedFiles) + 1;
        static int strFixesLen = STRLENW(strFixes) + 1;
        static int strChangeLen = STRLENW(strChange) + 1;
        static int strReportedByLen = STRLENW(strReportedBy) + 1;
        static const int strDifferencesLen = STRLENW(strDifferences);
        static const int strChangelistLen = STRLENW(strChangelist);
        static const int strEqContentLen = STRLENW(strEqContent);

        // load resource dependent tags first time through:
        static bool bResStringsLoaded = false;
        if(!bResStringsLoaded)
        {
            bResStringsLoaded = true;
            CString strAffectedFilesR = LoadStringResource(IDS_AFFECTEDFILES);
            strAffectedFilesR.TrimLeft(_T("\r\n"));
            strAffectedFilesR.TrimRight(_T("\r\n"));
            CString strFixesR = LoadStringResource(IDS_FIXLIST_HEADER);
            strFixesR.TrimLeft(_T('\n'));
            strFixesR.TrimRight(_T('\n'));
            CString strChangeR = LoadStringResource(IDS_FIXLIST_ITEM_CHANGE_TAG);
			CString strReportedByR;
			if (m_ReportedByTitle.IsEmpty())
				strReportedByR = _T("Owner:\t");
			else
				strReportedByR = m_ReportedByTitle + _T(":\t");
#ifdef UNICODE
            lstrcpy(strAffectedFiles, strAffectedFilesR);
            lstrcpy(strFixes, strFixesR);
            lstrcpy(strChange, strChangeR);
            lstrcpy(strReportedBy, strReportedByR);
#else
            ASSERT(strAffectedFilesR.GetLength() < strAffectedFilesLen);
            MultiByteToWideChar(CP_ACP, 0, strAffectedFilesR, -1, strAffectedFiles, strAffectedFilesLen);
            ASSERT(strFixesR.GetLength() < strFixesLen);
            MultiByteToWideChar(CP_ACP, 0, strFixesR, -1, strFixes, strFixesLen);
            ASSERT(strChangeR.GetLength() < strChangeLen);
            MultiByteToWideChar(CP_ACP, 0, strChangeR, -1, strChange, strChangeLen);
            ASSERT(strReportedByR.GetLength() < strReportedByLen);
            MultiByteToWideChar(CP_ACP, 0, strReportedByR, -1, strReportedBy, strReportedByLen);
#endif
            strAffectedFilesLen = int(wcslen(strAffectedFiles));
            strFixesLen = int(wcslen(strFixes));
            strChangeLen = int(wcslen(strChange));
			strReportedByLen = int(wcslen(strReportedBy));
        }

        // if at end of line, skip to start of next line
        if(*pAt == '\n')
        {
            while(*pAt == '\n')
                pAt++;
            if(!*pAt)
                break;

            beginChar = int(pAt - m_DescriptionW);
        }

        int i = int(pAt - m_DescriptionW);

        if(bInDiffs && i == beginChar)
        {
            if(wcsncmp(pAt, L"==== ", 5))
            {
                // skip line if not starting a diff
                LPCWSTR pChar = pAt;
                while ((++pChar < pLastMinus1) && (*pChar != '\n'))
                    ;
                pAt = pChar - 1;
                continue;
            }
            i += 4;
            pAt += 4;
        }
        // an '@' that's not at the start of a line?
		if (i > beginChar && (*pAt == '@'))
		{
			// Don't put hotspots on @@
			if (*(pAt+1) == '@')
			{
				pAt++;
				continue;
			}

            // make sure there's something before the '@'
            LPCWSTR pBeforeAt = pAt-1;
            if(*pBeforeAt <= ' ')
                continue;

			int newi = AddHotSpotWord(m_numHotSpots, i, beginChar, lgth, TRUE);
			if (newi == i)
				pAt++;	// word contained a / \ or : which is not in a user, client or email addr - so skip it
			else
			{
				LPCWSTR pDot = wcschr(m_DescriptionW+i+1, '.');
				if(pDot && pDot < m_DescriptionW+newi)
				{
					// found '.' following '@', so assume email address
					m_HotSpotType.InsertAt(m_numHotSpots++, HS_ISAEMAIL);
				}
				else
				{
					// no '.' following '@', so assume user@client
					m_HotSpotEnd.SetAt(m_numHotSpots, i);
					m_HotSpotType.InsertAt(m_numHotSpots++, HS_ISAUSER);
					m_HotSpotBgn.InsertAt(m_numHotSpots, i+1);
					m_HotSpotEnd.InsertAt(m_numHotSpots, newi);
					m_HotSpotType.InsertAt(m_numHotSpots++, HS_ISACLIENT);
				}
				pAt += newi - i - 1;
			}
		}
        else if(!wcsncmp(pAt, L"-//", 3) || 
                !wcsncmp(pAt, L"\"//", 3) ||
                !wcsncmp(pAt, L" //", 3) ||
                !wcsncmp(pAt, L"\t//", 3) ||
                ((i == beginChar) && !wcsncmp(pAt, L"//", 2)))
        {
			if (*(pAt+3) <= ' ')	// depot names don't have a white space after the //
				continue;
            bool bQuoted = *pAt == L'\"';
   			int newi = AddHotSpotFile(m_numHotSpots, i, beginChar, lgth, bQuoted);
    		m_HotSpotType.InsertAt(m_numHotSpots++, HS_ISAFILE);
            // don't lose trailing delimiter
            pAt += newi - i - 1;
        }
        else if(!wcsncmp(pAt, L"http://", 7) || !wcsncmp(pAt, L"https://", 8))
        {
    		int newi = AddHotSpotWord(m_numHotSpots, i, beginChar, lgth, FALSE) - 1;
	    	m_HotSpotType.InsertAt(m_numHotSpots++, HS_ISAURL);
            pAt += newi - i;
        }
        else if(i == beginChar)
        {
            int newi = i;
            if(!wcsncmp(pAt, strReportedBy, strReportedByLen))
            {
                newi = AddHotSpotWord(m_numHotSpots, i + strReportedByLen, beginChar, lgth, FALSE);
                m_HotSpotType.InsertAt(m_numHotSpots++, HS_ISAUSER);
            }
            else if(!wcsncmp(pAt, strModifiedBy, strModifiedByLen))
            {
                newi = AddHotSpotWord(m_numHotSpots, i + strModifiedByLen, beginChar, lgth, FALSE);
                m_HotSpotType.InsertAt(m_numHotSpots++, HS_ISAUSER);
            }
            else if((m_viewType != P4USER_SPEC) && !wcsncmp(pAt, strUser, strUserLen))
            {
                newi = AddHotSpotWord(m_numHotSpots, i + strUserLen, beginChar, lgth, FALSE);
                m_HotSpotType.InsertAt(m_numHotSpots++, HS_ISAUSER);
            }
            else if(!wcsncmp(pAt, strJobsFixed, strJobsFixedLen))
            {
                bInJobs = TRUE;
                newi = i + strJobsFixedLen;
            }
            // note: depending on whether a submitted or unsubmitted change is 
            // being described, the 'Affected files...' string comes either
            // from the server (always English) or from our resource 
            // stringtable.  We don't know which it is here, so try both.
            else if(!wcsncmp(pAt, strAffectedFiles, strAffectedFilesLen))
            {
                bInJobs = FALSE;
                newi = i + strAffectedFilesLen;
            }
            else if(!wcsncmp(pAt, strAffectedFilesEnglish, strAffectedFilesEnglishLen))
            {
                bInJobs = FALSE;
                newi = i + strAffectedFilesEnglishLen;
            }
			else if (bInJobs && (*pAt > ' '))
			{
				newi = AddHotSpotWord(m_numHotSpots, i, beginChar, lgth, FALSE);
				m_HotSpotType.InsertAt(m_numHotSpots++, HS_ISAJOB);
                pAt += newi - i;
                i = newi;

                static const WCHAR strBy[] = L" by ";
                static const WCHAR strDate[] = L" on yyyy/mm/dd";
                static const int strByLen = STRLENW(strBy);
                static const int strDateLen = STRLENW(strDate);
				if (!wcsncmp(pAt + strDateLen, strBy, strByLen))
				{
					newi = AddHotSpotWord(m_numHotSpots, i + strDateLen + strByLen, beginChar, lgth, FALSE);
					m_HotSpotType.InsertAt(m_numHotSpots++, HS_ISAUSER);
				}
			}
            else if(!wcsncmp(pAt, strFixes, strFixesLen))
            {
                bInFixes = TRUE;
                newi = i + strFixesLen;
                if(pAt[strFixesLen] == '\n')
                {
                    // if there are fixes, make sure we don't miss the newline
                    newi--;
                }
            }
			else if (bInFixes && !wcsncmp(pAt, strChange, strChangeLen))
			{
                newi = AddHotSpotWord(m_numHotSpots, i + strChangeLen, beginChar, lgth, FALSE);
				m_HotSpotType.InsertAt(m_numHotSpots++, HS_ISACHG);
			}
			else if (!wcsncmp(pAt, strDifferences, strDifferencesLen))
			{
                bInDiffs = TRUE;
                newi = i + strDifferencesLen;
			}
            pAt += newi - i;
        }
		else if (!wcsncmp(pAt, strChangelist, strChangelistLen))
		{
            pAt += strChangelistLen + 1;
            i += strChangelistLen + 1;
            LPCWSTR pChar = pAt;
            while(iswdigit(*pChar))
                pChar++;

            if(pChar > pAt)
            {
			    i = AddHotSpotWord(m_numHotSpots, i, beginChar, int(pChar - pAt), FALSE);
                pAt = pChar;
			    m_HotSpotType.InsertAt(m_numHotSpots++, HS_ISACHG);
            }
		}
		else if (m_bDiffOutput && !wcsncmp(pAt, strEqContent, strEqContentLen))
		{
			i = AddHotSpotWord(m_numHotSpots, i+5, beginChar, strEqContentLen-5, FALSE);
            pAt += strChangelistLen;
			m_HotSpotType.InsertAt(m_numHotSpots++, HS_ISDIFF2);
		}
        // if at end of line, note start of next line
        if(*pAt == '\n')
            beginChar = int((pAt + 1) - m_DescriptionW);
	}
	if (m_numHotSpots == 0)
		return;

    CHARFORMAT cf;
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR | CFM_UNDERLINE | CFM_LINK;
    cf.dwEffects = CFE_UNDERLINE|CFE_LINK;
	DWORD txtcolor = GetSysColor(COLOR_BTNTEXT);
	if (!txtcolor)
		cf.crTextColor = m_MoreThan256Colors ? RGB(0,0,0xFF) : RGB(0,128,0);
	else if (txtcolor == 0xFFFFFF)
		cf.crTextColor = RGB(0xFF,0,0xFF);
	else
	{
		if (GetBValue(txtcolor) + 0xFF > 0xFF)
			txtcolor ^= 0xFFFF00;
		cf.crTextColor = txtcolor;
	}

	long orgb, orge;
	m_Text.GetSel(orgb, orge);
	for (int i = -1; ++i < m_numHotSpots; )
	{
		int	b = m_HotSpotBgn.GetAt(i);
		int	e = m_HotSpotEnd.GetAt(i);
		m_Text.SetSel(b, e);
		m_Text.SetSelectionCharFormat(cf);
	}
	m_Text.SetSel(orgb, orge);
}

/*  _________________________________________________________________

	for commands that will run synchronously.
	_________________________________________________________________
*/

BOOL CSpecDescDlg::PumpMessages( )
{
	if (MainFrame()->IsQuitting())
		return FALSE;

	MSG msg;

	while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
	{
		//		get out if app is terminating
		//
		if ( msg.message == WM_QUIT )
			return FALSE;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////
// CDialog doesn't support UI updating the way CFrameWnd does, so we have to
// handle WM_INITPOPOPMENU here.  See MSDN article Q242577.

void CSpecDescDlg::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)
{
    ASSERT(pPopupMenu != NULL);
    // Check the enabled state of various menu items.

    CCmdUI state;
    state.m_pMenu = pPopupMenu;
    ASSERT(state.m_pOther == NULL);
    ASSERT(state.m_pParentMenu == NULL);

    // Determine if menu is popup in top-level menu and set m_pOther to
    // it if so (m_pParentMenu == NULL indicates that it is secondary popup).
    HMENU hParentMenu;
    if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
        state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
    else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
    {
        CWnd* pParent = this;
           // Child windows don't have menus--need to go to the top!
        if (pParent != NULL &&
           (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
        {
           int nIndexMax = ::GetMenuItemCount(hParentMenu);
           for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
           {
            if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
            {
                // When popup is found, m_pParentMenu is containing menu.
                state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
                break;
            }
           }
        }
    }

    state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
    for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
      state.m_nIndex++)
    {
        state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
        if (state.m_nID == 0)
           continue; // Menu separator or invalid cmd - ignore it.

        ASSERT(state.m_pOther == NULL);
        ASSERT(state.m_pMenu != NULL);
        if (state.m_nID == (UINT)-1)
        {
           // Possibly a popup menu, route to first item of that popup.
           state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
           if (state.m_pSubMenu == NULL ||
            (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
            state.m_nID == (UINT)-1)
           {
            continue;       // First item of popup can't be routed to.
           }
           state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
        }
        else
        {
           // Normal menu item.
           // Auto enable/disable if frame window has m_bAutoMenuEnable
           // set and command is _not_ a system command.
           state.m_pSubMenu = NULL;
           state.DoUpdate(this, FALSE);
        }

        // Adjust for menu deletions and additions.
        UINT nCount = pPopupMenu->GetMenuItemCount();
        if (nCount < state.m_nIndexMax)
        {
           state.m_nIndex -= (state.m_nIndexMax - nCount);
           while (state.m_nIndex < nCount &&
            pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
           {
            state.m_nIndex++;
           }
        }
        state.m_nIndexMax = nCount;
    }
} 



BOOL CSpecDescDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
    NMHDR* pNMHDR = (NMHDR*)lParam;
    if(pNMHDR->idFrom == IDC_DESCRIPTION && pNMHDR->code == EN_LINK)
    {
        ENLINK *link = (ENLINK*)lParam;
        if(link->msg == WM_LBUTTONUP)
        {
	        int i = IsItaHotSpot(link->chrg.cpMin, link->chrg.cpMax);
	        if (i != -1)
	        {
		        {
			        switch (m_HotSpotType.GetAt(i))
			        {
			        case HS_ISACHG:
                        m_Text.SetSel(link->chrg);
				        OnDescChg();
				        break;
			        case HS_ISAUSER:
                        m_Text.SetSel(link->chrg);
				        OnDescUser();
				        break;
			        case HS_ISACLIENT:
                        m_Text.SetSel(link->chrg);
				        OnDescClient();
				        break;
			        case HS_ISAJOB:
                        m_Text.SetSel(link->chrg);
				        OnDescJob();
				        break;
			        case HS_ISAEMAIL:
                        m_Text.SetSel(link->chrg);
				        OnEmail();
				        break;
			        case HS_ISAURL:
                        m_Text.SetSel(link->chrg);
				        OnURL();
				        break;
			        case HS_ISDIFF2:
                        m_Text.SetSel(link->chrg);
				        OnDiff2();
				        break;
			        }
		        }
	        }
            *pResult = 0;
            return TRUE;
        }
        else if(link->msg == WM_LBUTTONUP || link->msg == WM_RBUTTONUP)
        {
            m_Text.SetSel(link->chrg);
            *pResult = 0;
            return TRUE;
        }
    }
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}


void CSpecDescDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CP4Menu popMenu;

	SetMenuFlags();

    if(point.x == -1 && point.y == -1)
    {
        GetCursorPos(&point);
    }
	if (pWnd == GetDlgItem(IDC_SHOWDIFFS))
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		point.x = rect.left;
		point.y = rect.bottom + 1;
		popMenu.LoadMenu(IDR_SPECDESC_DIFF);
	}
	else
	{
	    popMenu.LoadMenu(IDR_SPECDESC_RICH);
		if (!MainFrame()->HaveP4QTree())
			popMenu.DeleteMenu(ID_FILE_REVISIONTREE, MF_BYCOMMAND);
		if (!AfxGetApp()->GetProfileInt(_T("Settings"), _T("CallTrack"), 0))
			popMenu.DeleteMenu(ID_CURRENTTASK,MF_BYCOMMAND);
		if (m_bDiffOutput && !(m_fDiff2 & (MF_GRAYED|MF_DISABLED)))
			popMenu.InsertMenu(IDB_BROWSE, MF_BYCOMMAND, ID_DIFF2, 
								LoadStringResource(IDS_DIFFTHE2FILES));
	}

	popMenu.GetSubMenu(0)->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	if (pWnd == GetDlgItem(IDC_SHOWDIFFS))
		m_btShowDiffs.ClearButtonPushed();
}

BOOL CSpecDescDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (NULL != m_pToolTip)
            m_pToolTip->RelayEvent(pMsg);
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CSpecDescDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_InitRect.Height())
	{
		lpMMI->ptMinTrackSize.x= m_InitRect.Width();
		lpMMI->ptMinTrackSize.y= m_InitRect.Height();
	}
}

// This signals the closing of a modeless dialog to
// MainFrame which will delete the 'this' object
void CSpecDescDlg::OnDestroy()
{
	if (m_Modeless)
		::PostMessage(MainFrame()->m_hWnd, WM_P4DLGDESTROY, 0, (LPARAM)this);
}

LRESULT CSpecDescDlg::OnNewClient(WPARAM wParam, LPARAM lParam)
{
	switch(m_viewType)
	{
	case P4CHANGE_SPEC:
	case P4CLIENT_SPEC:
		if (m_ShowEditBtn)
		{
			GetDlgItem(IDC_EDITIT)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDITIT)->ShowWindow(SW_HIDE);
		}
		break;
	}
	return TRUE;
}

LRESULT CSpecDescDlg::OnNewUser(WPARAM wParam, LPARAM lParam)
{
	switch(m_viewType)
	{
	case P4CHANGE_SPEC:
	case P4USER_SPEC:
		if (m_ShowEditBtn)
		{
			GetDlgItem(IDC_EDITIT)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDITIT)->ShowWindow(SW_HIDE);
		}
		break;
	}
	return TRUE;
}

void CSpecDescDlg::OnUpdatePositionToPattern(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( NULL == m_pFRDlg );
}

void CSpecDescDlg::OnPositionToPattern() 
{
	if (GetFocus()->m_hWnd != m_Text.m_hWnd)
	{
		MakeSmartSelection();
		CHARRANGE cr;
		m_Text.GetSel(cr);
		GotoDlgCtrl(GetDlgItem(IDC_DESCRIPTION));
		m_Text.SetSel(cr);
	}
	if ( NULL == m_pFRDlg )
	{
		m_pFRDlg = new CFindReplaceDialog();  // Must be created on the heap

		m_pFRDlg->m_fr.lStructSize = sizeof(FINDREPLACE);
		m_pFRDlg->m_fr.hwndOwner = this->m_hWnd;
		m_pFRDlg->Create( TRUE, m_FindWhatStr, _T(""), m_FindWhatFlags, this ); 
	}
}

void CSpecDescDlg::OnUpdatePositionToNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_FindWhatStr.IsEmpty());
}

void CSpecDescDlg::OnPositionToNext() 
{
	if (GetFocus()->m_hWnd != m_Text.m_hWnd)
	{
		MakeSmartSelection();
		CHARRANGE cr;
		m_Text.GetSel(cr);
		GotoDlgCtrl(GetDlgItem(IDC_DESCRIPTION));
		m_Text.SetSel(cr);
	}
	if (!m_FindWhatStr.IsEmpty())
	{
		PostMessage(WM_FINDPATTERN, (WPARAM)m_FindWhatFlags | 0x80000000, 
									(LPARAM)m_FindWhatStr.GetBuffer(0));
	}
}

void CSpecDescDlg::OnUpdatePositionToPrev(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_FindWhatStr.IsEmpty());
}

void CSpecDescDlg::OnPositionToPrev() 
{
	if (GetFocus()->m_hWnd != m_Text.m_hWnd)
	{
		MakeSmartSelection();
		CHARRANGE cr;
		m_Text.GetSel(cr);
		GotoDlgCtrl(GetDlgItem(IDC_DESCRIPTION));
		m_Text.SetSel(cr);
	}
	if (!m_FindWhatStr.IsEmpty())
	{
		PostMessage(WM_FINDPATTERN, ((WPARAM)m_FindWhatFlags | 0x80000000) ^ FR_DOWN, 
									 (LPARAM)m_FindWhatStr.GetBuffer(0));
	}
}

LRESULT CSpecDescDlg::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
	LPFINDREPLACE lpfp = (LPFINDREPLACE)lParam;
	if (m_pFRDlg->FindNext() || m_pFRDlg->IsTerminating())
	{
		m_FindWhatStr = lpfp->lpstrFindWhat;
		m_FindWhatFlags = lpfp->Flags;
		if (m_pFRDlg->FindNext())
		{
			PostMessage(WM_FINDPATTERN, (WPARAM)(lpfp->Flags), 
										(LPARAM)m_FindWhatStr.GetBuffer(0));
			delete m_pFRDlg;
		}
		m_pFRDlg = NULL;
	}
	return 0;
}

LRESULT CSpecDescDlg::OnFindPattern(WPARAM wParam, LPARAM lParam)
{
	FINDTEXTEX ft;
	CHARRANGE cr;
	int flags = (int)wParam;
	CString what = (TCHAR *)lParam;

	m_Text.GetSel(cr);
	if (flags & FR_DOWN)
	{
		ft.chrg.cpMin = cr.cpMax;
		ft.chrg.cpMax = -1;
	}
	else
	{
		ft.chrg.cpMax = 0;
		ft.chrg.cpMin = cr.cpMin;
	}
	ft.lpstrText = what.GetBuffer();
	if (-1 != m_Text.FindText(flags, &ft))
		m_Text.SetSel(ft.chrgText);

	return 0;
}
