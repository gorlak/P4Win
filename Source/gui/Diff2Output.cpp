// CDiff2Output.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "Diff2Output.h"
#include "StringUtil.h"
#include "RegKeyEx.h"
#include "FileInfoDlg.h"
#include "ViewerDlg.h"
#include "SpecDescDlg.h"
#include "cmd_diff2.h"
#include "cmd_fstat.h"
#include "cmd_history.h"
#include "cmd_opened.h"
#include "cmd_prepbrowse.h"
#include "cmd_where.h"
//#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static LPCTSTR sRegKey = _T("Software\\Perforce\\P4Win\\Layout\\Diff2Output");
static LPCTSTR sRegValue_ColumnWidths = _T("Column Widths");
static LPCTSTR sRegValue_SortColumns = _T("Sort Columns");

#define UPDATE_STATUS(x) ((CMainFrame *)AfxGetMainWnd())->UpdateStatus(x)
#define HOLD_LOCK_IF_HAVE_KEY (m_Key ? HOLD_LOCK : LOSE_LOCK)


/////////////////////////////////////////////////////////////////////////////
// CDiff2Output dialog


CDiff2Output::CDiff2Output(CWnd* pParent)
	: CDialog(CDiff2Output::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiff2Output)
	//}}AFX_DATA_INIT
	m_pParent = pParent;
	if (m_pParent)
		MainFrame()->SetModelessWnd(this);
	m_InitRect.SetRect(0,0,100,100);
	m_WinPos.SetWindow( this, _T("Diff2Output") );
}

CDiff2Output::~CDiff2Output()
{
	// can't use MainFrame()-> construct
	// because mainfram might have closed.
	CMainFrame * mainWnd = MainFrame();
	if (mainWnd)
		mainWnd->SetGotUserInput( );
}

void CDiff2Output::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiff2Output)
	DDX_Control(pDX, IDC_LIST, m_ListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDiff2Output, CDialog)
	//{{AFX_MSG_MAP(CDiff2Output)
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclickP4list)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedP4List)
	ON_BN_CLICKED(ID_DIFF2, OnDiff2)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
//	ON_BN_CLICKED(IDHELP, OnHelp)
//	ON_WM_HELPINFO()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_POSITIONDEPOT, OnPositionDepot)
	ON_COMMAND(IDB_BROWSE, OnFileAutobrowse)
	ON_COMMAND((UINT)(ID_FILE_PROPERTIES-30000), OnFileInformation)	// should be ID_FILE_PROPERTIES - but bug in M$ complier(!)
	ON_COMMAND((UINT)(ID_FILE_REVISIONTREE-30000), OnFileRevisionTree)
	ON_COMMAND(ID_FILE_ANNOTATE, OnFileAnnotate)
	ON_COMMAND(ID_FILE_REVISIONHISTORY, OnFileRevisionhistory)
	ON_COMMAND(ID_SINGLEPANEVIEW, OnSinglePaneView)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_P4DIFF2, OnP4Diff2 )
	ON_MESSAGE(WM_P4PREPBROWSE, OnP4ViewFile )
	ON_MESSAGE(WM_P4FILEINFORMATION, OnP4FileInformation )
	ON_MESSAGE(WM_P4ENDFILEINFORMATION, OnP4EndFileInformation )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiff2Output message handlers

BOOL CDiff2Output::OnInitDialog() 
{
	int i;

	CDialog::OnInitDialog();
	// Record the initial window size, then see if there is a registry preference
	GetWindowRect(&m_InitRect);
	m_LastRect = m_InitRect;
	m_WinPos.RestoreWindowPosition();

	CString str;

	SetWindowText(m_caption);
	SetFont(m_Font);

	// Modify the list control style so that the entire selected row is highlighted
	LRESULT dwStyle = ::SendMessage(m_ListCtrl.m_hWnd,LVM_GETEXTENDEDLISTVIEWSTYLE,0,0);
	dwStyle |= LVS_EX_FULLROWSELECT;
	::SendMessage(m_ListCtrl.m_hWnd,LVM_SETEXTENDEDLISTVIEWSTYLE,0,dwStyle);

	// Make sure list control shows selection when not the focused control
	m_ListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS, 0);

	// Get original size of control
	CRect rect;
	m_ListCtrl.GetWindowRect(&rect);

	int colwidth[2]={120,3000};

	// make sure OnSize gets called to reposition controls
	// if restored position is default, this won't happen unless
	// we force it
	GetClientRect(&rect);
	SendMessage(WM_SIZE, 0, MAKELONG(rect.Width(), rect.Height()));

	// Get new size of control after resized as specified in the registry
	m_ListCtrl.GetWindowRect(&rect);

	// Get any saved column widths from registry
	RestoreSavedWidths(colwidth, 2);

	// Make sure no column completely disappeared (because you can't get it back then)
	for (i=-1; ++i < 2; )
	{
		if (colwidth[i] < 5)
			colwidth[i] = 5;
	}
	// Use the same font as the calling window
	m_ListCtrl.SetFont(m_Font);

	// Figure out the headers
	int lgthHdr1;
	int lgthHdr2;
	CString txt;
	txt = m_ColNames->GetAt(0);
	if ((i = txt.Find(_T("/..."))) > -1)
		lgthHdr1 = i + 1;
	else
	{
		lgthHdr1 = 0;
	}
	txt = m_ColNames->GetAt(1);
	if ((i = txt.Find(_T("/..."))) > -1)
		lgthHdr2 = i + 1;
	else
	{
		lgthHdr2 = 0;
	}
	if (!lgthHdr1)	// local syntax?
	{
		txt = m_ColNames->GetAt(0);
		int i1 = txt.Find(_T("\\..."));
		if (i1 != -1)	// local syntax!
		{
			m_OrigHdr1 = txt.Left(i1);
			CCmd_Where *pCmd1 = new CCmd_Where;
			pCmd1->Init(NULL, RUN_SYNC);
			if ( pCmd1->Run(m_OrigHdr1) && !pCmd1->GetError() 
			  && pCmd1->GetDepotFiles()->GetCount() )
			{
				CString txt1 = pCmd1->GetDepotSyntax() + _T('/');
				lgthHdr1 = txt1.GetLength();
				txt1 += txt.Mid(i1+1);
				m_ColNames->SetAt(0, txt1);
			}
			else
				ASSERT(0);
			delete pCmd1;
			m_OrigHdr1 += _T('\\');
		}
	}
	if (!lgthHdr2)	// local syntax?
	{
		txt = m_ColNames->GetAt(1);
		int i2 = txt.Find(_T("\\..."));
		if (i2 != -1)	// local syntax!
		{
			m_OrigHdr2 = txt.Left(i2);
			CCmd_Where *pCmd1 = new CCmd_Where;
			pCmd1->Init(NULL, RUN_SYNC);
			if ( pCmd1->Run(m_OrigHdr2) && !pCmd1->GetError() 
			  && pCmd1->GetDepotFiles()->GetCount() )
			{
				CString txt2 = pCmd1->GetDepotSyntax() + _T('/');
				lgthHdr2 = txt2.GetLength();
				txt2 += txt.Mid(i2+1);
				m_ColNames->SetAt(1, txt2);
			}
			else
				ASSERT(0);
			delete pCmd1;
			m_OrigHdr2 += _T('\\');
		}
	}

	// these contain the folders including the last / but chop off the ... and following
	m_Hdr1 = m_ColNames->GetAt(0).Left(lgthHdr1);
	m_Hdr2 = m_ColNames->GetAt(1).Left(lgthHdr2);
	if (m_OrigHdr1.IsEmpty())
		m_OrigHdr1 = m_Hdr1;
	if (m_OrigHdr2.IsEmpty())
		m_OrigHdr2 = m_Hdr1;

	// Insert the columns 
	int maxcols = 2;
	int width=GetSystemMetrics(SM_CXVSCROLL);;
	int retval;
	LV_COLUMN lvCol;
	for(int subItem=0; subItem < maxcols; subItem++)
	{
		lvCol.mask= LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT |LVCF_WIDTH;
		lvCol.fmt=LVCFMT_LEFT;
		lvCol.pszText=( LPTSTR )( LPCTSTR ) m_ColNames->GetAt( subItem );
		lvCol.iSubItem=subItem;
		if(subItem < maxcols-1)
		{
			lvCol.cx=colwidth[subItem];
			width+=lvCol.cx;
		}
		else
			lvCol.cx=max(colwidth[subItem], rect.Width() - width - 4);  // expand last column to fill window
		retval=m_ListCtrl.InsertColumn(subItem, &lvCol);
	}
 
	AddTheListData(lgthHdr1, lgthHdr2);

	ShowWindow(SW_SHOW);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDiff2Output::AddTheListData(int lgthHdr1, int lgthHdr2)
{
	// Add the data
	int i;
	LV_ITEM lvItem;
	LPARAM  lParam;
	int iActualItem = -1;

	int maxwcol2 = m_ListCtrl.GetStringWidth(m_ColNames->GetAt(1)) + 20;

	int iItem = 0;
	CString msg = m_Msg;
	while ((i = msg.Find(_T('\n'))) != -1)
	{
		CString line = msg.Left(i);
		line.TrimLeft(_T(" =\t\r\n"));
		msg = msg.Mid(i+1);

		if (line.GetAt(0) == _T('/'))
		{
			i = line.Find(_T('#'));
			if (i > 0)
				i = line.Find(_T(" - "), i);
		}
		else
			i = line.Find(_T(" - "));
		ASSERT(i != -1);

		CString col1txt = line.Left(i);
		CString col2txt = line.Mid(i+3);
		col2txt.TrimLeft();
		if (col2txt.GetAt(0) == _T('/'))
		{
			i = col2txt.Find(_T('#'));
			if (i > 0)
			{
				if ((i = col2txt.Find(_T(" content"), i)) != -1)
					col2txt = col2txt.Left(i);
			}
		}
		col2txt.TrimRight(_T(" =\t\r\n"));

		lParam = 1;							// 1 -> can diff 2 files
		if (col1txt.GetAt(0) == _T('/'))
		{
			if (col1txt.Find(m_Hdr1) == 0)
				col1txt = col1txt.Mid(lgthHdr1);
		}
		else
		{
			col1txt = _T("   ") + col1txt;
			lParam = 0;						// 0 -> cannot diff 2 files
		}
		if (col2txt.GetAt(0) == _T('/'))
		{
			if (col2txt.Find(m_Hdr2) == 0)
				col2txt = col2txt.Mid(lgthHdr2);
		}
		else
		{
			col2txt = _T("   ") + col2txt;
			lParam = 0;						// 0 -> cannot diff 2 files
		}

		maxwcol2 = max(maxwcol2, m_ListCtrl.GetStringWidth(col2txt)+20);

		for(int subItem=0; subItem < 2; subItem++)
		{
			lvItem.mask=LVIF_TEXT | 
					((subItem==0) ? LVIF_IMAGE : 0) |
					((subItem==0) ? LVIF_PARAM : 0);
			lvItem.iItem = (subItem==0) ? iItem : iActualItem;
            ASSERT(lvItem.iItem != -1);
			lvItem.iSubItem = subItem;
			lvItem.lParam = lParam;
			lvItem.pszText = const_cast<LPTSTR>( subItem==0 ? (LPCTSTR)col1txt : (LPCTSTR)col2txt);

			if(subItem==0)
				iActualItem=m_ListCtrl.InsertItem(&lvItem);
			else
				m_ListCtrl.SetItem(&lvItem);
		}
		iItem++;
	}

	int w = GetSystemMetrics(SM_CXVSCROLL);
	CRect rect;
	m_ListCtrl.GetWindowRect(&rect);
	m_ListCtrl.SetColumnWidth(1, max(maxwcol2 + w, 
									 rect.Width() - m_ListCtrl.GetColumnWidth(0) - 8 - w));

	// finally, set focus to the list control so that the first 'down'
	// keystroke can be used to scroll down
	m_ListCtrl.SetFocus();
}

void CDiff2Output::OnOK() 
{
	OnCancel();
}

void CDiff2Output::OnCancel() 
{
	m_WinPos.SaveWindowPosition();
	SaveColumnWidths();
	if (m_pParent)
		m_pParent->PostMessage(WM_P4ENDDIFF2OUTPUT, 0, (LPARAM)this);
	CDialog::OnCancel();
}

// This signals the closing of a modeless dialog
// to MainFrame which will delete the 'this' object
void CDiff2Output::OnDestroy()
{
	if (m_pParent)
		::PostMessage(MainFrame()->m_hWnd, WM_P4DLGDESTROY, 0, (LPARAM)this);
}

void CDiff2Output::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch(nID)
	{
	case SC_MINIMIZE:
		GetDesktopWindow()->ArrangeIconicWindows();
		break;
	}

	CDialog::OnSysCommand(nID, lParam);
}

void CDiff2Output::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// Slide the buttons to the bottom of dlg
	CWnd *pOldV = GetDlgItem(ID_SINGLEPANEVIEW);
	CWnd *pDiff = GetDlgItem(ID_DIFF2);
	CWnd *pCancel = GetDlgItem(IDCANCEL);

	if(!pCancel)
		return;

	int h, y;
	CRect rect;
	GetWindowRect(&rect);
	int dx = rect.Width() - m_LastRect.Width();
	int dy = rect.Height() - m_LastRect.Height();
	// Save the new size
	m_LastRect = rect;

	pOldV->GetWindowRect(&rect);
	ScreenToClient(rect);
	pOldV->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pDiff->GetWindowRect(&rect);
	ScreenToClient(rect);
	pDiff->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pCancel->GetWindowRect(&rect);
	ScreenToClient(rect);
	h = rect.Height();
	pCancel->SetWindowPos(NULL, rect.left + dx, y = rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Increase the size of the list both horiz and vert
	CWnd *pList = GetDlgItem(IDC_LIST);
	pList->GetWindowRect(&rect);
	pList->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								    y - h, SWP_NOMOVE | SWP_NOZORDER);
	RedrawWindow();
}

void CDiff2Output::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize.x= m_InitRect.Width();
	lpMMI->ptMinTrackSize.y= m_InitRect.Height();
}


// Check the registry to see if we have recorded the
// column widths lastused for the Diff2Output dialog
void CDiff2Output::RestoreSavedWidths(int *width, int numcols)
{
    CRegKeyEx key;

    CString theKey = CString(sRegKey);
    if(ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, theKey, KEY_READ))
    {
        CString result = key.QueryValueString(sRegValue_ColumnWidths);
		CString sortCols = key.QueryValueString(sRegValue_SortColumns);
        if(!result.IsEmpty())
        {
			//		things can go wrong with the registry setting of the 
			//		widths. Use the defaults if the entry is all zeroes.
			//
			if ( result != _T("0,0,0,0,0,0,0,0,0,0") )
				for(int i=0; i< numcols; i++)
					width[i]= GetPositiveNumber(result);
        }
    }
}

void CDiff2Output::SaveColumnWidths() 
{
	// Save the column widths
	CString str;

	for(int i=0; i < 10; i++)
	{
		// Note that GetColumnWidth returns zero if i > numcols
    	CString num;
		num.Format(_T("%d"), m_ListCtrl.GetColumnWidth(i));
		if(i)
			str+=_T(",");
		str+=num;
	}
	
    CRegKeyEx key;
    if(ERROR_SUCCESS == key.Create(HKEY_CURRENT_USER, sRegKey))
        key.SetValueString(str, sRegValue_ColumnWidths);
}

#ifdef	HELPWANTEDFORCDiff2Output
void CDiff2Output::OnHelp() 
{
	AfxGetApp()->WinHelp(0);
}

BOOL CDiff2Output::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
#endif

/*  _________________________________________________________________

	for commands that will run synchronously.
	_________________________________________________________________
*/

BOOL CDiff2Output::PumpMessages( )
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

/*  _________________________________________________________________

	Parse //depot/folder/filename#rev (type) from a column into its parts
	_________________________________________________________________
*/
CString CDiff2Output::ParseFileInfo(CString *itemStr, int *rev, CString *filetype)
{
	CString filename = *itemStr;
	int i;
	if ((i = filename.ReverseFind(_T('('))) != -1)
	{
		if (filetype)
		{
			*filetype = filename.Mid(i);
			filetype->TrimLeft(_T('('));
			filetype->TrimRight(_T(')'));
		}
		filename = filename.Left(i-1);
	}

	if ((i = filename.Find(_T('#'))) != -1)
	{
		if (rev)
		{
			*rev = _ttoi(filename.Right(filename.GetLength() - i - 1));
			if (!*rev)
				*rev = -1;
		}
		filename = filename.Left( i );  // trim off rev# info
	}
	return filename;
}

void CDiff2Output::OnItemchangedP4List(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if (!pos)
		return;
	int index = m_ListCtrl.GetNextSelectedItem(pos);
	CWnd *ctl = GetDlgItem(ID_DIFF2);
	ctl->UpdateWindow();
	BOOL enable = m_ListCtrl.GetItemData(index) != 0;
	if(enable != ctl->IsWindowEnabled())
		ctl->EnableWindow(enable);	// 0 -> cannot diff; 1 -> can diff
}


void CDiff2Output::OnDblclickP4list(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	OnDiff2();
}

void CDiff2Output::OnDiff2()
{
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if (!pos)
		return;
	int index = m_ListCtrl.GetNextSelectedItem(pos);
	if (!m_ListCtrl.GetItemData(index))	// 0 -> cannot diff; 1 -> can diff
	{
		MessageBeep(0);
		return;
	}

	CString col1txt = m_ListCtrl.GetItemText(index, 0);
	CString col2txt = m_ListCtrl.GetItemText(index, 1);
	if (col1txt.GetAt(0) != _T('/'))
		col1txt = m_Hdr1 + col1txt;
	if (col2txt.GetAt(0) != _T('/'))
		col2txt = m_Hdr2 + col2txt;

	int rev1 = -1;
	int rev2 = -1;

	CString filetype1 = _T("text");
	CString filetype2 = _T("text");

	col1txt = ParseFileInfo(&col1txt, &rev1, &filetype1);
	col2txt = ParseFileInfo(&col2txt, &rev2, &filetype2);

	CCmd_Diff2 *pCmd= new CCmd_Diff2;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	if( pCmd->Run( col1txt, col2txt, rev1, rev2, filetype1, filetype2) )
	{
		UPDATE_STATUS( LoadStringResource(IDS_DIFFING_FILES) );
	}
	else
		delete pCmd;
}

LRESULT CDiff2Output::OnP4Diff2(WPARAM wParam, LPARAM lParam)
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

void CDiff2Output::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (SERVER_BUSY())
		return;

	// create an empty context menu
	CMenu popMenu;
	popMenu.CreatePopupMenu();

	CString itemStr;
	RECT rect;
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if (!pos)
	{
		m_ListCtrl.GetWindowRect(&rect);
		if (point.x < rect.left)
		{
			point.x = rect.left + 8;
			point.y = rect.top + 32;
		}
		goto singlepaneview;
	}
	int index = m_ListCtrl.GetNextSelectedItem(pos);

	// make a new selection new if reqd
	if(m_ListCtrl.GetItemState(index,LVIS_SELECTED) != LVIS_SELECTED)
	{
		for(int i=m_ListCtrl.GetItemCount(); i>=0; i-- )
			m_ListCtrl.SetItemState(i, 0, LVIS_SELECTED);
		
		m_ListCtrl.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
	}

	LPARAM b;
	if ((b = m_ListCtrl.GetItemData(index)) > 0)	// 0 -> cannot diff; 1 -> can diff
	{
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_DIFF2, LoadStringResource(IDS_DIFFTHE2FILES));
		popMenu.AppendMenu(MF_SEPARATOR);
	}

	// convert the screen coords in point to m_ListCtrl coords
	POINT clipoint;
	rect.left = rect.top = 0;
	rect.right = point.x;
	rect.bottom = point.y;
	m_ListCtrl.ScreenToClient(&rect);
	clipoint.x = rect.right;
	clipoint.y = rect.bottom;

	// find which column was clicked
	LVHITTESTINFO lvhti;
	lvhti.pt = clipoint;
	int i = m_ListCtrl.SubItemHitTest(&lvhti);
	m_SubItem = (i == index) ? lvhti.iSubItem : 0;

	if (i != index)
	{
		POINT pt;
		m_ListCtrl.GetItemPosition(index, &pt);
		m_ListCtrl.GetWindowRect(&rect);
		point.x = rect.left + pt.x + 8;
		point.y = rect.top + pt.y + 8;
	}

	itemStr = m_ListCtrl.GetItemText(index, m_SubItem);
	if (!b && (itemStr.Find(_T('<')) != -1))
		goto singlepaneview;

	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_EDIT_COPY, LoadStringResource(IDS_EDIT_COPY));
	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_POSITIONDEPOT, LoadStringResource(IDS_POSITIONDEPOT) );
	popMenu.AppendMenu(MF_ENABLED | MF_STRING, IDB_BROWSE, LoadStringResource(IDS_VIEWUSING3DOTS));
	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_FILE_PROPERTIES-30000, LoadStringResource(IDS_PROPERTIES));	// should be ID_FILE_PROPERTIES - but bug in M$ complier(!)
	if (MainFrame()->HaveP4QTree())
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_FILE_REVISIONTREE-30000,LoadStringResource(IDS_REVISIONTREE));
	if (MainFrame()->HaveTLV())
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_FILE_ANNOTATE, LoadStringResource(IDS_ANNOTATIONS));
 	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_FILE_REVISIONHISTORY, LoadStringResource(IDS_REVISIONHISTORY));
	popMenu.AppendMenu(MF_SEPARATOR);

singlepaneview:
 	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_SINGLEPANEVIEW, LoadStringResource(IDS_SINGLEPANEVIEW));

	// Finally blast the menu onto the screen
	if (popMenu.GetMenuItemCount() > 0)
		popMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	point.x, point.y, this);
}

void CDiff2Output::OnEditCopy() 
{
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if (!pos)
		return;
	int index = m_ListCtrl.GetNextSelectedItem(pos);

	CString itemStr = m_ListCtrl.GetItemText(index, m_SubItem);
	if (itemStr.GetAt(0) != _T('/'))
		itemStr = CString(m_SubItem == 0 ? m_Hdr1 : m_Hdr2) + itemStr;
	int rev;
	itemStr = ParseFileInfo(&itemStr, &rev);
	CString selText;
	selText.Format(_T("%s#%d"), itemStr, rev);
    CopyTextToClipboard(selText);
}

void CDiff2Output::OnPositionDepot()
{
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if (!pos)
		return;
	int index = m_ListCtrl.GetNextSelectedItem(pos);

	CString itemStr = m_ListCtrl.GetItemText(index, m_SubItem);
	if (itemStr.GetAt(0) != _T('/'))
		itemStr = CString(m_SubItem == 0 ? m_Hdr1 : m_Hdr2) + itemStr;
	itemStr = ParseFileInfo(&itemStr);
	((CMainFrame *) AfxGetMainWnd())->ExpandDepotString( itemStr, TRUE );
}

void CDiff2Output::OnFileAutobrowse() 
{
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if (!pos)
		return;
	int index = m_ListCtrl.GetNextSelectedItem(pos);

	CString name = m_ListCtrl.GetItemText(index, m_SubItem);
	if (name.GetAt(0) != _T('/'))
		name = CString(m_SubItem == 0 ? m_Hdr1 : m_Hdr2) + name;
	int rev = -1;
	CString fileType = _T("text");
	name = ParseFileInfo(&name, &rev, &fileType);

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

// TODO: This code is pretty much a copy of the code in CDepotView::RunViewer()
// Might want to craft a single file viewing class that can be instantiated from
// anywhere, or perhaps make CMainFrame be responsible for all file viewing.
LRESULT CDiff2Output::OnP4ViewFile(WPARAM wParam, LPARAM lParam)
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
						if( reinterpret_cast<intptr_t>( hinst ) > 32)
						{
							break;  // successfull viewer launch
						}
						if( reinterpret_cast<intptr_t>( hinst ) == SE_ERR_NOASSOC)	// give MSDEV (non-standard!) a try
						{
							hinst= ShellExecute( m_hWnd, _T("&Open with MSDEV"), viewFilePath, NULL, NULL, SW_SHOWNORMAL);
							if( reinterpret_cast<intptr_t>( hinst ) > 32 ) 
								break;  // successfull MSDEV viewer launch
						}
						if( reinterpret_cast<intptr_t>( hinst ) == SE_ERR_NOASSOC)	// give standard "open" a try
						{
							hinst= ShellExecute( m_hWnd, _T("open"), viewFilePath, NULL, NULL, SW_SHOWNORMAL);
							if( reinterpret_cast<intptr_t>( hinst ) > 32 ) 
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

void CDiff2Output::OnFileInformation() 
{
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if (!pos)
		return;
	int index = m_ListCtrl.GetNextSelectedItem(pos);

	CString itemStr = m_ListCtrl.GetItemText(index, m_SubItem);
	if (itemStr.GetAt(0) != _T('/'))
		itemStr = CString(m_SubItem == 0 ? m_Hdr1 : m_Hdr2) + itemStr;
	itemStr = ParseFileInfo(&itemStr);

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

LRESULT CDiff2Output::OnP4FileInformation( WPARAM wParam, LPARAM lParam )
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
		if( pCmd2->Run( FALSE, m_ItemStr, 0 ) && !pCmd2->GetError() )
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

LRESULT CDiff2Output::OnP4EndFileInformation( WPARAM wParam, LPARAM lParam )
{
	CFileInfoDlg *dlg = (CFileInfoDlg *)lParam;
	dlg->DestroyWindow();
	return TRUE;
}

void CDiff2Output::OnFileRevisionTree() 
{
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if (!pos)
		return;
	int index = m_ListCtrl.GetNextSelectedItem(pos);

	CString itemStr = m_ListCtrl.GetItemText(index, m_SubItem);
	if (itemStr.GetAt(0) != _T('/'))
		itemStr = CString(m_SubItem == 0 ? m_Hdr1 : m_Hdr2) + itemStr;
	itemStr = ParseFileInfo(&itemStr);
	TheApp()->CallP4RevisionTree(itemStr);	// use p4v.exe for tree
}

void CDiff2Output::OnFileAnnotate() 
{
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if (!pos)
		return;
	int index = m_ListCtrl.GetNextSelectedItem(pos);

	CString itemStr = m_ListCtrl.GetItemText(index, m_SubItem);
	if (itemStr.GetAt(0) != _T('/'))
		itemStr = CString(m_SubItem == 0 ? m_Hdr1 : m_Hdr2) + itemStr;
	itemStr = ParseFileInfo(&itemStr);
	TheApp()->CallP4A(itemStr, _T(""), 0);	// use p4v.exe for annotate
}

void CDiff2Output::OnFileRevisionhistory() 
{
	int rev = -1;
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	if (!pos)
		return;
	int index = m_ListCtrl.GetNextSelectedItem(pos);

	CString itemStr = m_ListCtrl.GetItemText(index, m_SubItem);
	if (itemStr.GetAt(0) != _T('/'))
		itemStr = CString(m_SubItem == 0 ? m_Hdr1 : m_Hdr2) + itemStr;
	itemStr = ParseFileInfo(&itemStr, &rev);
	
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

void CDiff2Output::OnSinglePaneView()
{
	// Do this first to trigger a repaint of the buttons
	// and before the new dialog is created (so it gets the focus)
	GotoDlgCtrl(GetDlgItem(IDC_LIST));

	CSpecDescDlg *dlg = new CSpecDescDlg(m_pParent);
	dlg->SetIsModeless(TRUE);
	dlg->SetKey(m_Key);
	dlg->SetDescription( m_Msg );
	dlg->SetItemName( m_OrigHdr1 + _T("... <> ") + m_OrigHdr2 + _T("...") );
	dlg->SetCaption( m_caption );
	dlg->SetViewType(P4DESCRIBE);
	if (!dlg->Create(IDD_SPECDESC, m_pParent))	// display the description dialog box
	{
		dlg->DestroyWindow();	// some error! clean up
		delete dlg;
	}
}
