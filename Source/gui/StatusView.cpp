// StatusView.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "StatusView.h"
#include "MainFrm.h"
#include "MsgBox.h"
#include "ImageList.h"
#include "cmd_where.h"

#pragma warning (push)
#pragma warning (disable: 4201)
#include <mmsystem.h>
#pragma warning (pop)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CStatusView

IMPLEMENT_DYNCREATE(CStatusView, CListView)

CStatusView::CStatusView()
{
	m_HeadIndex=0;
	m_ErrFound = m_RowAdded = FALSE;
	m_ShowStatusMsgs = GET_P4REGPTR()->GetShowStatusMsgs();
	OnMaxStatusLines();

    CString tempPath= GET_P4REGPTR()->GetTempDir();
	CreateDirectory(tempPath, NULL);	// make sure the directory exists
	for (int i = -1; ++i < 256; )
	{
		m_ErrFile.Format(_T("%s\\P4Werr%02x.txt"), tempPath, i);
		if ((m_hErrFile=CreateFile(m_ErrFile, GENERIC_READ | GENERIC_WRITE, 
					FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0)) != INVALID_HANDLE_VALUE)
		{
#ifdef UNICODE
			TCHAR msg[] = {0xFEFF};
			DWORD NumberOfBytesWritten;
			WriteFile(m_hErrFile, msg, 2, &NumberOfBytesWritten, NULL);
#endif
			break;
		}
	}
	EnableToolTips();
}

CStatusView::~CStatusView()
{
	CloseHandle(m_hErrFile);
}


BEGIN_MESSAGE_MAP(CStatusView, CListView)
	//{{AFX_MSG_MAP(CStatusView)
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_EX( TTN_NEEDTEXTW, 0, OnToolTipText )
	ON_NOTIFY_EX( TTN_NEEDTEXTA, 0, OnToolTipText )
	ON_COMMAND(ID_WINDOW_CLEAR, OnWindowClear)
	ON_COMMAND(ID_SHOW_COMMAND_TRACE, OnShowCommandTrace )
	ON_COMMAND(ID_SHOW_TIMESTAMP, OnShowTimestamp)
	ON_COMMAND(ID_SHOW_STATUSMSGS, OnShowStatusMsgs)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WARNANDERR, OnUpdateViewWarnAndErrs)
	ON_COMMAND(ID_VIEW_WARNANDERR, OnViewWarnAndErrs)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_COPYSELECTED, OnUpdateWindowCopyselected)
	ON_COMMAND(ID_EDIT_COPY, OnWindowCopyselected)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_POSITIONDEPOT, OnUpdateFindInDepot)
	ON_COMMAND(ID_POSITIONDEPOT, OnFindInDepot)
	ON_UPDATE_COMMAND_UI(ID_POSITIONCHGS, OnUpdateFindInChgs)
	ON_COMMAND(ID_POSITIONCHGS, OnFindInChgs)
	ON_COMMAND(ID_PERFORCE_OPTIONS, OnPerforceOptions)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_FINDPATTERN, OnFindPattern )
	ON_WM_SYSCOLORCHANGE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CStatusView diagnostics

#ifdef _DEBUG
void CStatusView::AssertValid() const
{
	CListView::AssertValid();
}

void CStatusView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CStatusView message handlers

void CStatusView::Clear()
{
	ListView_DeleteAllItems(m_hWnd);
	m_HeadIndex=0;
}


void CStatusView::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();
	
	// Set image shared list
	CListCtrl& list= (CListCtrl&) GetListCtrl();
	list.SetImageList(TheApp()->GetImageList(), LVSIL_SMALL);
	
	LV_COLUMN lvCol;
	lvCol.mask= LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH;
	lvCol.fmt=LVCFMT_LEFT;
	lvCol.iSubItem=0;
	lvCol.cx=1800;  
			
	list.InsertColumn(0, &lvCol);
}

void CStatusView::AddItem(LPCTSTR text, StatusView level, bool showDialog, BOOL ensureVisible /*=TRUE */) 
{
	ASSERT(	text != NULL);
	ASSERT( level >= SV_MSG && level < SV_MAX);

	int len= lstrlen(text);
	BOOL didFirstRow= FALSE;
	BOOL write2file = (showDialog==true	|| level==SV_WARNING
		|| level==SV_ERROR || level==SV_WARNSUMMARY) ? TRUE : FALSE;

	if (write2file && TheApp()->m_RunClientWizOnly)
		showDialog = TRUE;

	// Alloc a large enough buffer and initialize cleanText
	CString cleanText(_T(' '), len + 10);   
	cleanText.Empty();
	if (GET_P4REGPTR()->ShowStatusTime())
	{
		SYSTEMTIME	st;

		GetLocalTime(&st);
		if (GET_P4REGPTR()->Use24hourClock())
		{
			cleanText.Format(_T("%02d:%02d:%02d "), st.wHour, st.wMinute, st.wSecond);
		}
		else
		{
			cleanText.Format(_T("%d:%02d:%02d "), 
				(st.wHour > 12) ? st.wHour - 12 : st.wHour, 
				st.wMinute, st.wSecond);
		}
	}

	for(int i=0; i < len; i++)
	{
		// Split out multiple rows as required
		if(text[i] == _T('\n') && cleanText.GetLength() > 0)
		{
			if(!didFirstRow)
			{
				AddOneRow(cleanText, level, ensureVisible, write2file);
				didFirstRow= TRUE;
			}
			else
			{
				// Dont show bitmap for continuation lines
				AddOneRow(cleanText, SV_BLANK, ensureVisible, write2file);
			}

			cleanText.Empty();
		}

		// Get rid of misc control chars
		else if(text[i] != _T('\r') && text[i] != _T('\t') && text[i] != _T('\n'))	
			cleanText+= text[i];
	}

	if(cleanText.GetLength() > 0)
	{
		if(!didFirstRow)
		{
			AddOneRow(cleanText, level, ensureVisible, write2file);
			didFirstRow=TRUE;
		}
		else
		{
			// Dont show bitmap for continuation lines
			AddOneRow(cleanText, SV_BLANK, ensureVisible, write2file);
		}
	}

	if(didFirstRow)
	{
		// Play a sound.  The nodefault flag is used so PlaySound is silent when the
		// sound is not defined.  
		if(level== SV_WARNING)
			PlaySound(_T("PerforceWarning"), NULL, SND_ALIAS | SND_ASYNC | SND_NOWAIT | SND_NODEFAULT);
			
		else if(level== SV_COMPLETION)
			PlaySound(_T("PerforceCompleted"), NULL, SND_ALIAS | SND_ASYNC | SND_NOWAIT | SND_NODEFAULT);
	
		else if(level== SV_ERROR)
		{
			// Check to see if the main menu or Status pane context menu is open
			BOOL bMenuOpen = FALSE;
			MENUBARINFO mbi;
			mbi.cbSize = sizeof(MENUBARINFO);
			if (MainFrame()->m_InPopUpMenu
			 || (MainFrame()->GetMenuBarInfo(OBJID_MENU, 0, &mbi) && mbi.fBarFocused))
				bMenuOpen = TRUE;
			// If the mouse is captured - release it and pump the message loop.
			// This can happen if the user is dragging the toolbar to a new
			// location when we get here. We only do this however if all menus
			// are closed - if one is open and we mess with the Capture, it
			// makes another mess.  Things are still a mess cuz MS still doesn't
			// clean up from the drag and leaves the main window on top with the
			// upcoming messagebox hidden.  Yuck - and I still can't figure a way
			// out of that mess.
			while (!bMenuOpen && ::GetCapture())
			{
				MSG msg;

				::ReleaseCapture();
				if (!MainFrame()->IsQuitting())
				{
					while ( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) 
					{ 
						if (!::GetMessage(&msg, NULL, NULL, NULL))
						{ 
							::PostQuitMessage(0); 
							break; 
						} 
						if (!(TheApp( )->PreTranslateMessage(&msg)))
						{
							::TranslateMessage(&msg);
							::DispatchMessage(&msg);
						}
					} 
				}
    		}
			// Note: there is no real app modal message box, so set a flag to
			// kill the autopoll timer, thus preventing a billion error boxes
			// if the machine is unattended
			if (!showDialog)
			{
				SET_APP_HALTED(TRUE);
				PlaySound(_T("PerforceError"), NULL, SND_ALIAS | SND_SYNC | SND_NOWAIT | SND_NODEFAULT);
				AfxMessageBox(text, MB_ICONSTOP | MB_SYSTEMMODAL); 
				SET_APP_HALTED(FALSE);
			}
		}
	}

	if(didFirstRow && showDialog)
	{
		CString str = CString(_T("OK\t")) + text;
		str.Replace('\t', ' ');
		str.SetAt(2, '\t');
		int i, j;
		for (i = j = 0; (i = str.Find('\n', i)) != -1; i++)
			j++;
		BOOL b = (j > 5 || str.GetLength() > 200) ? TRUE : FALSE;
		switch(level)
		{
		case SV_WARNING:
			MsgBox( str, MB_ICONEXCLAMATION, IDC_BUTTON2, this, NULL, b );
			break;
		default:
			MsgBox( str, MB_ICONSTOP, IDC_BUTTON2, this, NULL, b );
			break;
		}
		MainFrame()->SetStatusBarLevel(level);
	}
}

void CStatusView::AddOneRow(LPCTSTR text, StatusView level, BOOL ensureVisible, BOOL write2file, BOOL bSave/*=TRUE*/) 
{
	DWORD NumberOfBytesWritten;

	ASSERT(	text != NULL);
	ASSERT( level >= SV_MSG && level < SV_MAX);

	// Add the image and text
	LV_ITEM lvItem;
			
	lvItem.mask=LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvItem.iItem= m_HeadIndex;
	lvItem.iSubItem= 0;
	lvItem.iImage = CP4ViewImageList::VI_STATUS_MSG + level;
	lvItem.pszText= const_cast<LPTSTR>(text); 
	lvItem.lParam= level;

	if (write2file || m_ShowStatusMsgs 
	 || level== SV_COMPLETION || level== SV_WARNING || level== SV_ERROR || level== SV_WARNSUMMARY)
	{
		// If the output window has too many rows, delete oldest row
		if(ListView_GetItemCount(m_hWnd) >= m_MaxStatusLines)
			ListView_DeleteItem(m_hWnd, 0);
		// Add the item to the status listview
		m_HeadIndex=ListView_InsertItem(m_hWnd, &lvItem);
		if( ensureVisible )
			ListView_EnsureVisible(m_hWnd, m_HeadIndex, FALSE);
		m_HeadIndex++;
		m_RowAdded = TRUE;
	}

	CString msg;
	msg.Format(_T("%d %s\r\n"), level, text);
	if (bSave)
	{
		if (m_StatusRows.GetCount() > m_MaxSaveLines)
			m_StatusRows.RemoveHead();
		m_StatusRows.AddTail(msg);
	}

	if (write2file)
	{
		m_ErrFound = TRUE;
		WriteFile(m_hErrFile, msg, msg.GetLength()*sizeof(TCHAR), &NumberOfBytesWritten, NULL);
		MainFrame()->SetStatusBarLevel(level);
	}
	if (GET_P4REGPTR()->GetStatusUpdateInterval() && m_ShowStatusMsgs)
		SendMessage(WM_SETREDRAW, FALSE, 0);
}

BOOL CStatusView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style|= LVS_NOCOLUMNHEADER | LVS_SHAREIMAGELISTS | LVS_ALIGNLEFT | LVS_REPORT;
	return CListView::PreCreateWindow(cs);
}

void CStatusView::OnSize(UINT nType, int cx, int cy) 
{
	CListView::OnSize(nType, cx, cy);
}


BOOL CStatusView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(SERVER_BUSY())
		return SET_BUSYCURSOR();
	else
		return CListView::OnSetCursor(pWnd, nHitTest, message);
}

void CStatusView::OnWindowClear() 
{
	Clear();
}

void CStatusView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu popMenu;
	MainFrame()->m_InPopUpMenu = TRUE;

	if (point.x <= 0 && point.y <= 0)	// Position NOT actually in status window?
	{
		point.x = point.y = 0;
		ClientToScreen(&point);			// re-position to upper left corner of status window
	}

    popMenu.LoadMenu(IDR_STATUS);
    popMenu.GetSubMenu(0)->CheckMenuItem(ID_SHOW_COMMAND_TRACE, MF_BYCOMMAND | 
        (GET_P4REGPTR()->ShowCommandTrace( ) ? MF_CHECKED : MF_UNCHECKED));
    popMenu.GetSubMenu(0)->CheckMenuItem(ID_SHOW_TIMESTAMP, MF_BYCOMMAND | 
        (GET_P4REGPTR()->ShowStatusTime( ) ? MF_CHECKED : MF_UNCHECKED));
    popMenu.GetSubMenu(0)->CheckMenuItem(ID_SHOW_STATUSMSGS, MF_BYCOMMAND | 
        (GET_P4REGPTR()->GetShowStatusMsgs( ) ? MF_CHECKED : MF_UNCHECKED));

	popMenu.GetSubMenu(0)->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd( ));
	MainFrame()->m_InPopUpMenu = FALSE;
}


/*
	_________________________________________________________________

	toggle the show command trace flag in the registry.
	_________________________________________________________________
*/

void CStatusView::OnShowCommandTrace() 
{
	GET_P4REGPTR()->SetShowCommandTrace( ! GET_P4REGPTR()->ShowCommandTrace() );
}

void CStatusView::OnUpdateViewWarnAndErrs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_ErrFound );	
}

void CStatusView::CallOnUpdateViewWarnAndErrs(CCmdUI* pCmdUI)
{
	OnUpdateViewWarnAndErrs(pCmdUI);
}

void CStatusView::OnMaxStatusLines() 
{
	m_MaxStatusLines = GET_P4REGPTR()->GetMaxStatusLines();
	m_MaxSaveLines = min(2000, max(m_MaxStatusLines >> 4, 250));
	if (m_MaxSaveLines > m_MaxStatusLines)
		m_MaxSaveLines = m_MaxStatusLines;
}

void CStatusView::OnViewWarnAndErrs() 
{
	if (GET_P4REGPTR()->UseNotepad4WarnAndErr())
	{
#ifdef UNICODE
		DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT; 
#else
		DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS;
#endif
		PROCESS_INFORMATION procInfo;
		STARTUPINFO startInfo;
		GetStartupInfo(&startInfo);
		startInfo.lpReserved= startInfo.lpDesktop= NULL; 
		startInfo.dwFlags |= STARTF_USESHOWWINDOW;
		startInfo.wShowWindow = SW_SHOWNORMAL;
		TCHAR commandLine[_MAX_PATH+1];
		CString cmdline = _T("notepad.exe ") + m_ErrFile;
		lstrcpy(commandLine, cmdline);
		CreateProcess( NULL,	// pointer to name of executable module 
						commandLine,	// pointer to command line string
						NULL, NULL,  // default security rot
						FALSE,	// handle inheritance flag 
						dwCreationFlags,	// creation flags 
						NULL, // env
						NULL, //default dir
						&startInfo, &procInfo);					
	}
	else
		ShellExecute( AfxGetApp( )->m_pMainWnd->m_hWnd, _T("open"), m_ErrFile, NULL, NULL, SW_SHOWNORMAL);
	MainFrame()->SetStatusBarLevel(SV_MSG);
}

void CStatusView::CallOnViewWarnAndErrs() 
{
	OnViewWarnAndErrs();
}

void CStatusView::OnUpdateWindowCopyselected(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( GetSelectedCount() > 0 );	
}

void CStatusView::OnWindowCopyselected() 
{
	CString text;  

	GetPaneText( text, TRUE );
	CopyTextToClipboard( text );
}

void CStatusView::GetPaneText( CString &txt, BOOL onlySelectedText) 
{
	TCHAR buf[1024];
	txt.Empty();
	
	for( int i = 0;  i < ListView_GetItemCount( m_hWnd ); i++ )
	{
		if( !onlySelectedText || 
			ListView_GetItemState( m_hWnd, i, LVIS_SELECTED ) == LVIS_SELECTED )
		{
			ListView_GetItemText( m_hWnd, i, 0, buf, 1023 );

			switch( GetListCtrl().GetItemData( i ) )
			{
			case SV_TOOL:
				txt+= LoadStringResource(IDS_STATUS_TOOL); break;
			case SV_DEBUG:
				txt+= LoadStringResource(IDS_STATUS_DEBUG); break;
			case SV_WARNING:
				txt+= LoadStringResource(IDS_STATUS_WARNING); break;
			case SV_ERROR:
				txt+= LoadStringResource(IDS_STATUS_ERROR); break;
			case SV_MSG:
				txt+= LoadStringResource(IDS_STATUS_STATUS); break;
			case SV_COMPLETION:
			case SV_WARNSUMMARY:
				txt+= LoadStringResource(IDS_STATUS_COMPLETION); break;
			case SV_BLANK:
				// Indent rows following lead row
				txt+= _T("    "); break;
			default:
				break;
			}
			
			txt+= buf;
			txt+= _T("\r\n");
		}
	}
}


int CStatusView::GetSelectedCount()
{
	int selected= 0;
	for( int i = 0;  i < ListView_GetItemCount( m_hWnd ); i++ )
		if( ListView_GetItemState( m_hWnd, i, LVIS_SELECTED ) == LVIS_SELECTED )
			selected++;
	
	return selected;
}

void CStatusView::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);	
}

void CStatusView::OnEditSelectAll() 
{
	for( int i = 0;  i < ListView_GetItemCount( m_hWnd ); i++ )
		ListView_SetItemState( m_hWnd, i, LVIS_SELECTED, LVIS_SELECTED );
	
}

void CStatusView::OnShowTimestamp() 
{
	BOOL showStatusTime = !GET_P4REGPTR()->ShowStatusTime( );
	GET_P4REGPTR()->SetShowStatusTime( showStatusTime );
}

void CStatusView::OnShowStatusMsgs() 
{
	m_ShowStatusMsgs = !GET_P4REGPTR()->GetShowStatusMsgs( );
	GET_P4REGPTR()->SetShowStatusMsgs( m_ShowStatusMsgs );

	int i;
	LPTSTR p, q;
	OnWindowClear();
	if (m_ShowStatusMsgs)
	{
		CString row;
		for (POSITION pos = m_StatusRows.GetHeadPosition(); pos != NULL; )
		{
			row = m_StatusRows.GetNext(pos);
			p = row.GetBuffer(row.GetLength()+1);
			i = _tstoi(p);
			while (*++p != _T(' '))
				;
			q = p++;
			while(*++q != _T('\r'))
				;
			*q = _T('\0');
			AddOneRow(p, (StatusView)i, TRUE, FALSE, FALSE);
		}
	}
	else
	{
		DWORD dw = GetFileSize(m_hErrFile, 0);
		CString buf(_T('\0'), dw+1);
		if (dw)
		{
			SetFilePointer(m_hErrFile, 0, 0, FILE_BEGIN);
			if (ReadFile(m_hErrFile, p = buf.GetBuffer(dw+1), dw, &dw, 0))
			{
				while(*p)
				{
					i = _tstoi(p);
					while (*++p != _T(' '))
						;
					q = p++;
					while(*++q != _T('\r'))
						;
					*q = _T('\0');
					AddOneRow(p, (StatusView)i, TRUE, FALSE, FALSE);
					for (p = q; *++p && *p < _T(' '); )
						;
				}
			}
			SetFilePointer(m_hErrFile, 0, 0, FILE_END);
		}
	}
}

CString CStatusView::Extract1stFilename(CString &str) 
{
	int i;
	CString itemStr = str;
	if (itemStr.GetAt(0) != _T('/') && itemStr.GetAt(1) != _T(':'))
	{
		if ((i = itemStr.Find(_T("//"))) > 0)
		{
			// we have something like "STATUS: 3:33:26 Sync preview: //depot/main/www/perforce/customers/bycountry.html#16 - updating c:\workspac\main\www\perforce\customers\bycountry.html"
			// so throw away everything before the "//"
			itemStr = itemStr.Right(itemStr.GetLength() - i);
		}
		else if ((i = itemStr.Find(_T(":\\"))) > 0)
		{
			// we have something like "STATUS: 11:48:04 Executing p4 where c:\workspac\P4CONFIG"
			// so throw away everything before the "c:\"
			itemStr = itemStr.Right(itemStr.GetLength() - i + 1);
		}
		itemStr.TrimRight();
	}
	// if we find a 2nd "//", throw it and the following chars away
	if ((i = itemStr.Find(_T("//"), 2)) > 0)
		itemStr = itemStr.Left(i);
	// if we find a 2nd "c:\", throw it and the following chars away
	if ((i = itemStr.Find(_T(":\\"), 3)) > 0)
		itemStr = itemStr.Left(i-1);
	return itemStr;
}

void CStatusView::OnUpdateFindInDepot(CCmdUI* pCmdUI) 
{
	BOOL b = FALSE;
	CString text;

	GetPaneText( text, TRUE );

	if ((GetSelectedCount() == 1)
	 && ((text.Find(_T("//")) != -1) || (text.Find(_T(":\\")) != -1)))
	{
		b = TRUE;
	}
	pCmdUI->Enable(b);	
	if (pCmdUI->m_pParentMenu == MainFrame()->GetMenu())
		pCmdUI->SetText(LoadStringResource(IDS_FINDSELFILEINDEPOT));
}

void CStatusView::OnFindInDepot() 
{
	CString itemStr;

	GetPaneText( itemStr, TRUE );
	itemStr = Extract1stFilename(itemStr);
	itemStr.TrimRight(_T("*/\\"));
	((CMainFrame *)AfxGetMainWnd())->ExpandDepotString( itemStr, TRUE );
}

void CStatusView::OnUpdateFindInChgs(CCmdUI* pCmdUI) 
{
	CString text;
	GetPaneText( text, TRUE );
	BOOL b = ((GetSelectedCount() == 1)
		   && ((text.Find(_T("//")) != -1) || (text.Find(_T(":\\")) != -1)));
	if (b)
	{
		text = Extract1stFilename(text);
		b = ((text.Find(_T("...")) == -1) && (text.Find(_T("*")) == -1));
	}
	pCmdUI->Enable(b);	
}

void CStatusView::OnFindInChgs() 
{
	int i;
	CString itemStr;

	GetPaneText( itemStr, TRUE );
	itemStr = Extract1stFilename(itemStr);
	if ((itemStr.Find(_T("...")) == -1) && (itemStr.Find(_T("*")) == -1))
	{
		if ((i = itemStr.FindOneOf(_T("#@"))) != -1)
			itemStr = itemStr.Left(i);
		itemStr.TrimRight(_T("*/\\ "));
		if (itemStr.GetAt(0) != _T('/'))
		{
			// have to convert to depot syntax
			CCmd_Where *pCmd1 = new CCmd_Where;
			pCmd1->Init(NULL, RUN_SYNC);
			if ( pCmd1->Run(itemStr) && !pCmd1->GetError() 
				&& pCmd1->GetDepotFiles()->GetCount() )
			{
				itemStr = pCmd1->GetDepotSyntax();
				itemStr.TrimRight();
			}
			delete pCmd1;
		}
		if (((CMainFrame *)AfxGetMainWnd())->PositionChgs( itemStr, TRUE, TRUE ) != -1)
			return;	// found it
	}
	MainFrame()->SetMessageText(LoadStringResource(SERVER_BUSY() ? IDS_FILE_NOT_FOUND_YOUR_CHGS 
																 : IDS_FILE_NOT_FOUND_IN_CHGS));
}

void CStatusView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	int i;

	if (bActivate)
	{
		SendMessage(WM_SETREDRAW, TRUE, 0);
		((CMainFrame *)AfxGetMainWnd())->SetStatusBarLevel(SV_MSG);
		if (m_RowAdded || (ListView_GetSelectedCount( m_hWnd ) == 0))
		{
			m_RowAdded = FALSE;
			i = ListView_GetItemCount( m_hWnd );
			if (i--)
			{
				ListView_SetItemState( m_hWnd, i, LVIS_SELECTED, LVIS_SELECTED );
				ListView_EnsureVisible( m_hWnd, i, FALSE );
			}
		}
		else
		{
			for( i =  ListView_GetItemCount( m_hWnd ); i--; )
				if( ListView_GetItemState( m_hWnd, i, LVIS_SELECTED ) == LVIS_SELECTED )
					break;
			ListView_EnsureVisible( m_hWnd, i, FALSE );
		}
	}
	
	CListView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CStatusView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch ( nChar )
	{
	case VK_TAB:
		if ( ::GetKeyState(VK_CONTROL) & 0x8000 )	// Ctrl+TAB switches to depot pane
			MainFrame()->SwitchPanes((CView *)this, FALSE);
		break;
	}
	
	CListView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CStatusView::OnSysColorChange()
{
	CListView::OnSysColorChange();

	CListCtrl& list= (CListCtrl&) GetListCtrl();
	list.SetImageList(TheApp()->GetImageList(), LVSIL_SMALL);
}

void CStatusView::OnPerforceOptions()
{
	MainFrame()->OnPerforceOptions(TRUE, FALSE, IDS_PAGE_STATUS);
}

LRESULT CStatusView::OnFindPattern(WPARAM wParam, LPARAM lParam)
{
	TCHAR str[ 1024 ];
	CString text;
	CString what = (TCHAR *)lParam;
	int flags = (int)wParam;
	int prev = -1;

	if (!(flags & FR_MATCHCASE))
		what.MakeLower();

	int i, j;
	CListCtrl& list= (CListCtrl&) GetListCtrl();
	int columns = list.GetHeaderCtrl()->GetItemCount();

	if (flags & FR_DOWN)
	{
		for( i = 0;  i < ListView_GetItemCount(m_hWnd); i++ )
		{
			if( ListView_GetItemState( m_hWnd, i, LVIS_SELECTED ) == LVIS_SELECTED )
			{
				ListView_SetItemState( m_hWnd, prev = i, 0, LVIS_SELECTED|LVIS_FOCUSED );
				while (ListView_GetSelectedCount(m_hWnd))	// clear remaining selections
				{
					if( ListView_GetItemState( m_hWnd, ++i, LVIS_SELECTED ) == LVIS_SELECTED )
						ListView_SetItemState( m_hWnd, i, 0, LVIS_SELECTED|LVIS_FOCUSED );
				}
				i = prev;
				break;
			}
		}
		if (flags < 0)
			i++;
		while( i < ListView_GetItemCount(m_hWnd) )
		{
			for (j =-1; ++j < columns; )
			{
				ListView_GetItemText( m_hWnd, i, j, str, sizeof(str)/sizeof(TCHAR) );
				text = _T("");
				switch( GetListCtrl().GetItemData( i ) )
				{
				case SV_ERROR:
					text = LoadStringResource(IDS_STATUS_ERROR);
				case SV_WARNING:
				case SV_WARNSUMMARY:
					text+= LoadStringResource(IDS_STATUS_WARNING) + str;
					break;
				case SV_COMPLETION:
					text = LoadStringResource(IDS_STATUS_COMPLETION) + str;
					break;
				case SV_TOOL:
					text = LoadStringResource(IDS_STATUS_TOOL) + str;
					break;
				default:
					text = str;
					break;
				}
				if (!(flags & FR_MATCHCASE))
					text.MakeLower();
				if (text.Find(what) != -1)
				{
					ListView_SetItemState( m_hWnd, i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
					list.EnsureVisible(i, FALSE);
					MainFrame()->SetMessageText(LoadStringResource(IDS_FOUND));
					return 0;
				}
			}
			i++;
		}
	}
	else
	{
		for( i = ListView_GetItemCount(m_hWnd); --i >= 0; )
		{
			if( ListView_GetItemState( m_hWnd, i, LVIS_SELECTED ) == LVIS_SELECTED )
			{
				ListView_SetItemState( m_hWnd, prev = i, 0, LVIS_SELECTED|LVIS_FOCUSED );
				while (ListView_GetSelectedCount(m_hWnd))	// clear remaining selections
				{
					if( ListView_GetItemState( m_hWnd, --i, LVIS_SELECTED ) == LVIS_SELECTED )
						ListView_SetItemState( m_hWnd, i, 0, LVIS_SELECTED|LVIS_FOCUSED );
				}
				i = prev;
				break;
			}
		}
		if (flags < 0)
			i--;
		while( i >= 0 )
		{
			for (j =-1; ++j < columns; )
			{
				ListView_GetItemText( m_hWnd, i, j, str, sizeof(str)/sizeof(TCHAR) );
				text = _T("");
				switch( GetListCtrl().GetItemData( i ) )
				{
				case SV_ERROR:
					text = LoadStringResource(IDS_STATUS_ERROR);
				case SV_WARNING:
				case SV_WARNSUMMARY:
					text+= LoadStringResource(IDS_STATUS_WARNING) + str;
					break;
				case SV_COMPLETION:
					text = LoadStringResource(IDS_STATUS_COMPLETION) + str;
					break;
				case SV_TOOL:
					text = LoadStringResource(IDS_STATUS_TOOL) + str;
					break;
				default:
					text = str;
					break;
				}
				if (!(flags & FR_MATCHCASE))
					text.MakeLower();
				if (text.Find(what) != -1)
				{
					ListView_SetItemState( m_hWnd, i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
					list.EnsureVisible(i, FALSE);
					MainFrame()->SetMessageText(LoadStringResource(IDS_FOUND));
					return 0;
				}
			}
			i--;
		}
	}

	MessageBeep(0);
	MainFrame()->SetMessageText(LoadStringResource(IDS_NOT_FOUND));
	if (prev != -1)
		ListView_SetItemState( m_hWnd, prev, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
	return 0;
}

static	int	indexNbr = -1;

INT_PTR CStatusView::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	// CWnd implementation won't do us any good, so we have to handle this.
	// watch out!  sometimes get called with null pointer
	if (pTI == 0 || !GET_P4REGPTR()->GetShowTruncTooltip( ))
		return -1;

	// don't want to show tooltip if nothing is hidden
	// the tooltip is only there to show truncated text
	int index;
	LVHITTESTINFO info;
	info.pt = point;
	ListView_HitTest(m_hWnd, &info);
	if ((index = info.iItem) == -1)
		return -1;	// retun because not on a line with text
	TCHAR buf[1024];
	ListView_GetItemText( m_hWnd, index, 0, buf, 1023 );
	int lgth = lstrlen(buf);
	if (lgth < 50)
		return -1;	// return - line has less than 50 characters
	CRect rect;
	GetWindowRect(&rect);
	HDC hDC = ::GetDC(m_hWnd);
	SIZE size;
	GetTextExtentPoint32(hDC, buf, lgth, &size);
	::ReleaseDC(m_hWnd, hDC);
	if ((size.cx < rect.Width()) && (size.cx < 1700))
		return -1;	// return - line is fully visible

	if (indexNbr != index)
	{
		// If this is the 1st time for this line, just return
		// in order to remove any current tooltip; we will be
		// called again and will put up the tooltip then.
		indexNbr = index;
		return -1;
	}

	// we want to show a tooltip, so set up for the normal callback
	pTI->hwnd = m_hWnd;					// us
	pTI->uId = (UINT_PTR)m_hWnd;		// us
	pTI->uFlags |= TTF_IDISHWND;		// uId is HWND, not ID
	pTI->lpszText = LPSTR_TEXTCALLBACK;
	indexNbr = index;					// save in a static since this function Must be const!
	return (int)pTI->uId;
}

BOOL CStatusView::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	// first, check for my tooltip text request
	// and handle it here
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;    
    UINT_PTR nID =pNMHDR->idFrom;
    if (pTTT->uFlags & TTF_IDISHWND)    
    {
		if(nID == (UINT_PTR)m_hWnd)
		{
			TCHAR buf[1024];
			CRect rect;
			GetClientRect(&rect);
			::SendMessage(pTTT->hdr.hwndFrom, TTM_SETMAXTIPWIDTH, 0, max(480, 2*rect.Width()/3));
			ListView_GetItemText( m_hWnd, indexNbr, 0, buf, 1023 );
			m_ToolTipText = buf;
			pTTT->lpszText = (LPTSTR)(LPCTSTR)m_ToolTipText;
            pTTT->hinst = NULL;
			return TRUE;
		}
	}
	// for other cases, just pass it along to mainframe
    return MainFrame()->OnToolTipText(id, pNMHDR, pResult);
}
