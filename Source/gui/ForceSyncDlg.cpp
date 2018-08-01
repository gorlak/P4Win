// ForceSyncDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "ForceSyncDlg.h"
#include "MainFrm.h"
#include "cmd_diff.h"
#include "cmd_fstat.h"
#include "cmd_history.h"
#include "hlp\p4win.hh"
#include "strops.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static HWND hWndThis = 0;
static int  adjX = 0;
static int  adjY = 0;

/////////////////////////////////////////////////////////////////////////////
// CForceSyncDlg dialog


CForceSyncDlg::CForceSyncDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CForceSyncDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CForceSyncDlg)
	m_Action = -1;
	//}}AFX_DATA_INIT
	m_InitRect.SetRect(0,0,0,0);
	m_InitDialogDone=FALSE;
	m_WinPos.SetWindow( this, _T("ForceSyncDlg") );
}


void CForceSyncDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CForceSyncDlg)
	DDX_Control(pDX, IDC_EDITCHANGNUM, m_ChangeCombo);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_DELETESELECTED, m_Delete);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Radio(pDX, IDC_OPEN, m_Action);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CForceSyncDlg, CDialog)
	//{{AFX_MSG_MAP(CForceSyncDlg)
	ON_BN_CLICKED(IDC_DELETESELECTED, OnDeleteSelected)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_OPEN, OnOpenFiles)
	ON_BN_CLICKED(IDC_FORCERESYNC, OnForceResync)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_RBUTTONUP()
	ON_WM_HELPINFO()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_FILE_DIFFHEAD, OnDiffHead)
	ON_COMMAND(ID_FILE_DIFFHAVE, OnDiffHave)
	ON_COMMAND(ID_POSITIONDEPOT, OnPositionDepot)
	ON_COMMAND(ID_FILE_REVISIONHISTORY, OnFileRevisionhistory)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_P4DIFF, OnP4Diff )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CForceSyncDlg message handlers

BOOL CForceSyncDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetWindowRect(&m_InitRect);
	m_LastRect = m_InitRect;

	CString filename;
	CString filenmok;
	POSITION pos;
	
	BOOL bWeird = FALSE;
	CStringList strlist;
	for(pos = m_lpCstrListC->GetHeadPosition(); pos != NULL; )
	{
		filename = m_lpCstrListC->GetNext(pos);
		if (filename.FindOneOf(_T("@#%")) != -1)
		{
			StrBuf b;
			StrBuf f;
			f << CharFromCString(filename);
			StrPtr *p = &f;
			StrOps::WildToStr(*p, b);
			filenmok = CharToCString(b.Value());
			bWeird = TRUE;
		}
		m_List.AddString(filename);
		strlist.AddTail(filenmok);
	}
	if (bWeird)
	{
		m_lpCstrListC->RemoveAll();
		for(pos = strlist.GetHeadPosition(); pos != NULL; )
			m_lpCstrListC->AddTail(strlist.GetNext(pos));
	}
	bWeird = FALSE;
	strlist.RemoveAll();
	for(pos = m_lpCstrListD->GetHeadPosition(); pos != NULL; )
	{
		filename = m_lpCstrListD->GetNext(pos);
		if (filename.FindOneOf(_T("@#%")) != -1)
		{
			StrBuf b;
			StrBuf f;
			f << CharFromCString(filename);
			StrPtr *p = &f;
			StrOps::WildToStr(*p, b);
			filenmok = CharToCString(b.Value());
			bWeird = TRUE;
		}
		m_List.AddString(filename);
		strlist.AddTail(filenmok);
	}
	if (bWeird)
	{
		m_lpCstrListD->RemoveAll();
		for(pos = strlist.GetHeadPosition(); pos != NULL; )
			m_lpCstrListD->AddTail(strlist.GetNext(pos));
	}

	/////////////////////////////
	// Fill in the change list, if present, and select the suggested
	// change number
	if(!m_pChangeList.IsEmpty())
	{
		ASSERT(m_pChangeList.GetCount() > 0);
		
		for(pos=m_pChangeList.GetHeadPosition(); pos != NULL; )
		{
			m_ChangeCombo.AddString(m_pChangeList.GetNext(pos));
		}	
	}

	int index;
	ASSERT(!m_SelChange.IsEmpty());
	if( (index=m_ChangeCombo.FindStringExact(-1, m_SelChange))==CB_ERR)
	{
		ASSERT(0);  //Why wasnt default value in list?
		index=0;
	}
	m_ChangeCombo.SetCurSel(index);

	m_Delete.ShowWindow(SW_SHOWNOACTIVATE);
	m_Delete.EnableWindow(FALSE);
	m_OK.EnableWindow(FALSE);
	m_ChangeCombo.EnableWindow(FALSE);

	hWndThis = m_hWnd;
	RECT rect;
	m_List.GetWindowRect(&rect);
	adjX = rect.left;
	adjY = rect.top;
	adjX -= m_InitRect.left;
	adjY -= m_InitRect.top;
	m_List.SetRightClkCallback((PTRFUNC)OnRightClickList);

	// Restore window position
	m_InitDialogDone = TRUE;
	m_WinPos.RestoreWindowPosition();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CForceSyncDlg::OnDeleteSelected() 
{
	CString filename;
	POSITION pos;
	POSITION oldPos;

	for(int i=m_List.GetCount()-1; i>=0; i--)
	{
		if(m_List.GetSel(i))
		{
			m_List.GetText(i, filename);
			if (filename.FindOneOf(_T("@#%")) != -1)
			{
				StrBuf b;
				StrBuf f;
				f << CharFromCString(filename);
				StrPtr *p = &f;
				StrOps::WildToStr(*p, b);
				filename = CharToCString(b.Value());
			}
			for(pos = m_lpCstrListC->GetHeadPosition(); pos != NULL; )
			{
				oldPos = pos;
				if (filename == m_lpCstrListC->GetNext(pos))
				{
					m_lpCstrListC->RemoveAt(oldPos);
					m_List.DeleteString(i);
					continue;
				}
			}
			for(pos = m_lpCstrListD->GetHeadPosition(); pos != NULL; )
			{
				oldPos = pos;
				if (filename == m_lpCstrListD->GetNext(pos))
				{
					m_lpCstrListD->RemoveAt(oldPos);
					m_List.DeleteString(i);
					break;
				}
			}
		}
	}
	m_Delete.EnableWindow(FALSE);
}

void CForceSyncDlg::OnSelchangeList() 
{
	if(m_List.GetSelCount() > 0)
		m_Delete.EnableWindow(TRUE);
	else
		m_Delete.EnableWindow(FALSE);
}

void CForceSyncDlg::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_COMPARING_CLIENT_AND_DEPOT_FILES);
}

BOOL CForceSyncDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void CForceSyncDlg::OnOK() 
{
	m_WinPos.SaveWindowPosition();
	UpdateData( );
	if (m_Action == 1)
	{
		if (IDYES != AfxMessageBox(IDS_ASK_FORCE_RESYNC, MB_YESNO | MB_ICONQUESTION))
			return;
	}
	else
	{
		int index=m_ChangeCombo.GetCurSel();
		CString txt;
		m_ChangeCombo.GetLBText(index, txt);
		if(txt.Compare(LoadStringResource(IDS_DEFAULTCHANGELISTNAME)) == 0)
			m_SelectedChange= 0;
		else if(txt.Compare(LoadStringResource(IDS_NEWCHANGELISTNAME)) == 0)
		{
			m_SelectedChange= MainFrame()->CreateNewChangeList(m_Key);
			if (m_SelectedChange == -1)	// the user bailed
				return;
		}
		else
			m_SelectedChange=_ttol(txt);
	}
	CDialog::OnOK();
}

void CForceSyncDlg::OnCancel() 
{
	m_WinPos.SaveWindowPosition();
	CDialog::OnCancel();
}

void CForceSyncDlg::OnOpenFiles() 
{
	m_OK.EnableWindow(TRUE);
	m_ChangeCombo.EnableWindow(TRUE);
}

void CForceSyncDlg::OnForceResync() 
{
	m_OK.EnableWindow(TRUE);
	m_ChangeCombo.EnableWindow(FALSE);
}

void CForceSyncDlg::OnSize(UINT nType, int cx, int cy) 
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
	m_List.GetWindowRect(&rect);
	m_List.SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								    rect.bottom - rect.top + dy, SWP_NOMOVE | SWP_NOZORDER);

	// Widen the group and combo boxes by the change in width
	// Slide them down by the change in height
	CWnd *pWnd = GetDlgItem(IDC_STATIC1);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOZORDER);
	pWnd = GetDlgItem(IDC_EDITCHANGNUM);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	// Slide radio buttons and the static text at the bottom
	// down by the change in height
	pWnd = GetDlgItem(IDC_OPEN);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDC_FORCERESYNC);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDC_STATIC2);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	// Slide the buttons to the right by the change in width
	// and down by the change in height
	pWnd = GetDlgItem(IDOK);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDC_DELETESELECTED);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDCANCEL);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDHELP);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);
}

void CForceSyncDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_InitRect.Height())
	{
		lpMMI->ptMinTrackSize.x= m_InitRect.Width();
		lpMMI->ptMinTrackSize.y= m_InitRect.Height();
	}
}

void CForceSyncDlg::OnEditCopy() 
{
	if (!m_List.GetCount())
		return;

    CString txt;
	CString filename;
	for(int i=-1; ++i < m_List.GetCount(); )
	{
		if(!m_List.GetSel(i))
			continue;
		m_List.GetText(i, filename);
		txt += filename + _T("\r\n");
	}

    CopyTextToClipboard(txt);
}

void CForceSyncDlg::OnDiffHead()
{
	OnDiffFile(FALSE);
}

void CForceSyncDlg::OnDiffHave()
{
	OnDiffFile(TRUE);
}

void CForceSyncDlg::OnDiffFile(BOOL bHave)
{
	CString filename;

	m_StringList.RemoveAll();
	for(int i=m_List.GetCount()-1; i>=0; i--)
	{
		if(!m_List.GetSel(i))
			continue;
		m_List.GetText(i, filename);
		if (filename.FindOneOf(_T("@#%")) != -1)
		{
			StrBuf b;
			StrBuf f;
			f << CharFromCString(filename);
			StrPtr *p = &f;
			StrOps::WildToStr(*p, b);
			filename = CharToCString(b.Value());
		}
		BOOL b = TRUE;
		for(POSITION pos = m_lpCstrListD->GetHeadPosition(); pos != NULL; )
		{
			if (filename == m_lpCstrListD->GetNext(pos))
			{
				CString txt;
				txt.FormatMessage(IDS_s_IS_MISSING, filename);
				AddToStatus(txt, SV_MSG);
				b = FALSE;
				break;
			}
		}
		if (b)
			m_StringList.AddTail(filename);
	}

	if (!m_StringList.GetCount())
	{
		if (m_List.GetSelCount() > 1)
			AddToStatus(LoadStringResource(IDS_ALLSELFILESAREMISSING_NODIFFRUN), SV_COMPLETION);
		return;
	}

	if (bHave)
	{
		INT_PTR cnt = m_StringList.GetCount();
		CCmd_Fstat *pCmd2= new CCmd_Fstat;
		pCmd2->Init(NULL, RUN_SYNC);
		if( pCmd2->Run( FALSE, &m_StringList, TRUE, 0 ) && !pCmd2->GetError() )
		{
			m_StringList.RemoveAll();
			CObList *coblist2 = pCmd2->GetFileList( );
			ASSERT_KINDOF( CObList, coblist2 );
			ASSERT( coblist2->GetCount() == cnt );
			for (POSITION pos = coblist2->GetHeadPosition( ); pos != NULL; )
			{
				CP4FileStats *stats = ( CP4FileStats * )coblist2->GetNext( pos );
				ASSERT_KINDOF( CP4FileStats, stats );
				long headRev = stats->GetHeadRev();
				long haveRev = stats->GetHaveRev();
				CString filename = stats->GetFullDepotPath();
				if (headRev != haveRev)
				{
					CString revnbr;
					revnbr.Format(_T("#%ld"), haveRev);
					filename += revnbr;
				}
				m_StringList.AddTail(filename);
				delete stats;
			}		
		}
		delete pCmd2;
	}

	CCmd_Diff *pCmd= new CCmd_Diff;
	pCmd->Init( m_hWnd, RUN_ASYNC);
	if( pCmd->Run( &m_StringList, _T("-f") ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_DIFFING_FILE) );
	else
		delete pCmd;
}

LRESULT CForceSyncDlg::OnP4Diff(WPARAM wParam, LPARAM lParam)
{
	CCmd_Diff *pCmd= (CCmd_Diff *) wParam;
    
	if(!pCmd->GetError())
	{
		int cnt;
		if ((cnt = pCmd->GetDiffRunCount()) == 0)
		{
			if (pCmd->GetDiffNbrFiles() == 1)
            {
                CString msg;
                msg.FormatMessage(IDS_CLIENT_FILE_s_DOES_NOT_DIFFER_FROM_DEPOT_FILE,
						pCmd->GetDiffFileName());
				AddToStatus(msg, SV_COMPLETION);
            }
			else if (pCmd->GetDiffErrCount() == 0)
				AddToStatus(LoadStringResource(IDS_NONE_OF_THE_SELECTED_CLIENT_FILES_DIFFER), SV_COMPLETION);
		}
		else if (cnt < pCmd->GetDiffNbrFiles())
		{
			CString txt;
			int i = pCmd->GetDiffNbrFiles() - cnt;
            if(i == 1)
                txt.FormatMessage(IDS_ONECLIENTFILEDOESNOTDIFFER);
            else
                txt.FormatMessage(IDS_SEVERALCLIENTFILESDONOTDIFFER_n, i);
			AddToStatus(txt, SV_COMPLETION);
		}
	}
    
	delete pCmd;
	MainFrame()->ClearStatus();
	return 0;
}

void CForceSyncDlg::OnPositionDepot()
{
	CString filename;

	for(int i=m_List.GetCount()-1; i>=0; i--)
	{
		if(!m_List.GetSel(i))
			continue;
		m_List.GetText(i, filename);
		((CMainFrame *) AfxGetMainWnd())->ExpandDepotString( filename, TRUE );
		break;
	}
}

void CForceSyncDlg::OnFileRevisionhistory() 
{
	CString filename;

	for(int i=m_List.GetCount()-1; i>=0; i--)
	{
		if(!m_List.GetSel(i))
			continue;
		m_List.GetText(i, filename);
	
		CCmd_History *pCmd= new CCmd_History;
		pCmd->Init( MainFrame()->GetDepotWnd(), RUN_ASYNC );
		pCmd->SetCallingWnd(m_hWnd);
		if( pCmd->Run( LPCTSTR(filename)) )
		{
			MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_HISTORY) );
		}
		else
			delete pCmd;
	}
}

void CForceSyncDlg::OnEditSelectAll() 
{
	for(int i=m_List.GetCount()-1; i>=0; i--)
		m_List.SetSel( i, TRUE );
	OnSelchangeList();
}

void CForceSyncDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// Are we below the top of the listbox?
	// Are we to the right of left edge of the listbox?
	if (point.y < adjY || point.x < adjX)
		return;

	// Are we within the listbox?
	RECT rect;
	m_List.GetWindowRect(&rect);
	if (point.y > rect.bottom - rect.top + adjY || point.x > rect.right - rect.left + adjX)
		return;

	// select the item right clicked upon (unless it is already selected)
	CPoint pt;
	GetCursorPos(&pt);	// make sure we have the real cursor coordinates
	m_List.ScreenToClient(&pt);
	for(int i=m_List.GetCount()-1; i>=0; i--)
	{
		if (m_List.GetItemRect(i, &rect) == LB_ERR)
			continue;
		if (pt.x >= rect.left && pt.x <= rect.right
		 && pt.y >= rect.top  && pt.y <= rect.bottom)	// was click within this item?
		{
			if(!m_List.GetSel(i))		// if not already selected,
			{
				m_List.SetSel(-1, FALSE);	// clear all selections
				m_List.SetSel(i, TRUE);		// and select this one
				OnSelchangeList();
			}
			break;
		}
	}
	
	CP4Menu popMenu;
	popMenu.CreatePopupMenu();
	if (m_List.GetSelCount())
	{
		popMenu.AppendMenu(MF_ENABLED|MF_STRING, ID_EDIT_COPY, LoadStringResource(IDS_EDIT_COPY));
		popMenu.AppendMenu(MF_SEPARATOR);
		popMenu.AppendMenu(MF_ENABLED|MF_STRING, ID_FILE_DIFFHEAD, LoadStringResource(IDS_DIFFAGAINSTDEPOT));
		popMenu.AppendMenu(MF_ENABLED|MF_STRING, ID_FILE_DIFFHAVE, LoadStringResource(IDS_DIFFAGAINSTHAVEREV));
		if (m_List.GetSelCount() == 1)
		{
			popMenu.AppendMenu(MF_ENABLED|MF_STRING, ID_POSITIONDEPOT, LoadStringResource(IDS_POSITIONDEPOT));
			popMenu.AppendMenu(MF_ENABLED|MF_STRING, ID_FILE_REVISIONHISTORY, LoadStringResource(IDS_REVISIONHISTORY));
		}
		popMenu.AppendMenu(MF_SEPARATOR);
	}
	popMenu.AppendMenu(MF_ENABLED|MF_STRING, ID_EDIT_SELECT_ALL, LoadStringResource(IDS_EDIT_SELECT_ALL));

    GetCursorPos(&point);	// make sure we have the real cursor coordinates
	popMenu.TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void OnRightClickList(UINT flags, CPoint pt)
{
	PostMessage(hWndThis, WM_RBUTTONUP, flags, MAKELPARAM(pt.x + adjX, pt.y + adjY));
}
