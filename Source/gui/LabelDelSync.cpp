// LabelDelSync.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "reviewlist.h"
#include "LabelDelSync.h"
#include "MainFrm.h"
#include "P4Command.h"
#include "cmd_fstat.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLabelDelSync dialog


CLabelDelSync::CLabelDelSync(CWnd* pParent /*=NULL*/)
	: CDialog(CLabelDelSync::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLabelDelSync)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_LabelWnd = pParent->m_hWnd;
	m_InitRect.SetRect(0,0,0,0);
	m_InitDialogDone=FALSE;
	m_WinPos.SetWindow( this, _T("LabelDelSync") );
	m_LabelFileCount = 0;
	m_SortFlag = 0;
}

CLabelDelSync::~CLabelDelSync()
{
	delete m_List;
}


void CLabelDelSync::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLabelDelSync)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLabelDelSync, CDialog)
	//{{AFX_MSG_MAP(CLabelDelSync)
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_BN_CLICKED(ID_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	ON_BN_CLICKED(IDC_UNSELECT_ALL, OnUnselectAll)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_P4FSTAT, OnP4LabelContents )
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLabelDelSync message handlers


BOOL CLabelDelSync::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetWindowRect(&m_InitRect);
	m_LastRect = m_InitRect;

	SetWindowText(m_Caption);
	CWnd *pCtrl=GetDlgItem(IDOK);
	pCtrl->SetWindowText(m_BtnText);

	CRect rect;
	pCtrl=GetDlgItem(IDC_LISTRECT);
	pCtrl->GetWindowRect(&rect);
	ScreenToClient(&rect);

	DWORD style= WS_CHILD | WS_BORDER | WS_VISIBLE | LBS_HASSTRINGS | WS_HSCROLL
		 | LBS_EXTENDEDSEL | LBS_OWNERDRAWFIXED | WS_VSCROLL | WS_TABSTOP | LBS_NOINTEGRALHEIGHT;

	m_List = new CReviewList;
	
	m_List->CreateEx(WS_EX_CLIENTEDGE, MainFrame()->m_ReviewListClass, _T(""),style,
                 rect.left, rect.top, rect.Width(), rect.Height(),
				 this->m_hWnd, (HMENU) IDC_LIST);

	// Now fire off the request for all the files in the label
	CString spec;
	spec.Format(_T("//...@%s"), m_Active);

	// Call Fstat, w/ suppress==FALSE
	CCmd_Fstat *pCmd= new CCmd_Fstat;
	pCmd->Init( this->m_hWnd, RUN_ASYNC, LOSE_LOCK);

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

	// Restore window position
	m_InitDialogDone = TRUE;
	m_WinPos.RestoreWindowPosition();

	ShowWindow(SW_SHOW);
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLabelDelSync::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	// Create the user font for the list control
    LOGFONT logFont;
	CFont* cfont = GetFont();
	cfont->GetLogFont(&logFont);
    logFont.lfPitchAndFamily= FIXED_PITCH | FF_DONTCARE;
    lstrcpy(logFont.lfFaceName, LoadStringResource(IDS_DEFAULTFONTNAME));
    m_Font.CreateFontIndirect( &logFont );
	m_List->SetFont(&m_Font);

	m_List->SetFocus();
}

LRESULT CLabelDelSync::OnP4LabelContents(WPARAM wParam, LPARAM lParam)
{
	CString tmp;
    CCmd_Fstat *pCmd;

	if(lParam == 0)   // completion
	{
		pCmd= (CCmd_Fstat *) wParam;
		ASSERT_KINDOF(CCmd_Fstat,pCmd);
		SET_BUSYCURSOR();

		if(!pCmd->GetError())
		{
			int i;
			int maxlgth = 0;
			tmp.FormatMessage(IDS_LABEL_s_POINTS_TO_n_FILES, m_Active, m_LabelFileCount);
			AddToStatus(tmp, SV_COMPLETION);

			MainFrame()->ClearStatus();
			delete pCmd;
			int index;
			for (i = 0; i < m_LabelFiles.GetSize ( ); i++ )
			{
				CString s = m_LabelFiles.GetAt( i );

				if( !s.IsEmpty() )
				{
					maxlgth = max(maxlgth, s.GetLength());

					BOOL bSetCheck = MainFrame()->IsFileInList(&s, m_SelectedList);
					if (bSetCheck)
					{
						index = m_List->AddString( s );
						m_List->SetCheck( index, 1 );
						m_SortFlag |= 1;
					}
				}
			}
			for (i = 0; i < m_LabelFiles.GetSize ( ); i++ )
			{
				CString s = m_LabelFiles.GetAt( i );

				if( !s.IsEmpty() )
				{
					maxlgth = max(maxlgth, s.GetLength());

					BOOL bSetCheck = MainFrame()->IsFileInList(&s, m_SelectedList);
					if (!bSetCheck)
					{
						index = m_List->AddString( s );
						m_SortFlag |= 2;
					}
				}
			}
			// Get text metrics for font so we can figure scroll extent
			//
			TEXTMETRIC tm;
			CDC *pDC= GetDC();
			CFont *pOldFont= pDC->SelectObject( &m_Font );
			pDC->GetTextMetrics( &tm );
			pDC->SelectObject( pOldFont );
			ReleaseDC( pDC );

			m_List->SetHorizontalExtent( tm.tmAveCharWidth * maxlgth + 40 );
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

			tmp.TrimRight(_T("\r\n"));
			// And add to Description
			m_LabelFiles.Add(tmp);
			delete stats;

		} // while row batch not done

		delete list;
		delete pWrap;
		return 0;
	} // a batch of rows, we'll be called again so don't delete pCmd
}

void CLabelDelSync::ReloadList()
{
	m_SortFlag = 0;
	m_List->ResetContent();
	for (int i = 0; i < m_LabelFiles.GetSize ( ); i++ )
	{
		CString s = m_LabelFiles.GetAt( i );

		if( !s.IsEmpty() )
			m_List->AddString( s );
	}
}

void CLabelDelSync::OnSelectAll() 
{
	if (m_SortFlag == 3)
		ReloadList();
}

void CLabelDelSync::OnUnselectAll() 
{
	if (m_SortFlag == 3)
		ReloadList();
}

void CLabelDelSync::OnCancel() 
{
	::PostMessage(m_LabelWnd, m_ReturnMsg, (WPARAM)IDCANCEL, (LPARAM)0);
}

void CLabelDelSync::OnPreview() 
{
	OnFinish();
	::PostMessage(m_LabelWnd, m_ReturnMsg, (WPARAM)IDOK, (LPARAM)TRUE);
}

void CLabelDelSync::OnOK() 
{
	OnFinish();
	::PostMessage(m_LabelWnd, m_ReturnMsg, (WPARAM)IDOK, (LPARAM)FALSE);
}

void CLabelDelSync::OnFinish() 
{
	int hash;
	int space;
	CString txt;

	m_WinPos.SaveWindowPosition();
	m_OutputList->RemoveAll();
	for ( int i = 0; i < m_List->GetCount ( ); i++ )
	{
		if (m_List->GetCheck(i))
		{
			m_List->GetText( i, txt );
			if ((hash = txt.Find(_T('#'))) != -1)
			{
				if ((space = txt.Find(_T(' '), hash)) != -1)
					hash = space;
				txt = txt.Left(hash);
			}
			m_OutputList->AddTail( txt );
		}
	}
}

void CLabelDelSync::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_MANAGING_LABELS);
}

BOOL CLabelDelSync::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

BOOL CLabelDelSync::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(SERVER_BUSY())
		return SET_BUSYCURSOR();
	else
		return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CLabelDelSync::OnSize(UINT nType, int cx, int cy) 
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

	// Widen the list box by the change in width (dx) and
	// heighten the list box by the change in height (dy)
	CWnd *pWnd=GetDlgItem(IDC_LIST);
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								    rect.bottom - rect.top + dy, SWP_NOMOVE | SWP_NOZORDER);

	// Slide the buttons to the right by the change in width
	// and down by the change in height
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

void CLabelDelSync::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_InitRect.Height())
	{
		lpMMI->ptMinTrackSize.x= m_InitRect.Width();
		lpMMI->ptMinTrackSize.y= m_InitRect.Height();
	}
}

// This signals the closing of a modeless dialog
// to MainFrame which will delete the 'this' object
void CLabelDelSync::OnDestroy()
{
	::PostMessage(MainFrame()->m_hWnd, WM_P4DLGDESTROY, 0, (LPARAM)this);
}
