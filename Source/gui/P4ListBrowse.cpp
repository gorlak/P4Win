// P4ListBrowse.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "P4Object.h"
#include "P4ListBrowse.h"
#include "StringUtil.h"
#include "RegKeyEx.h"
#include "cmd_describe.h"
#include "hlp\p4win.hh"
#include "P4ImageList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static LPCTSTR sRegKey = _T("Software\\Perforce\\P4Win\\Layout\\");
static LPCTSTR sRegValue_ColumnWidths = _T("Column Widths");
static LPCTSTR sRegValue_SortColumns = _T("Sort Columns");

#define UPDATE_STATUS(x) ((CMainFrame *)AfxGetMainWnd())->UpdateStatus(x)
int CALLBACK P4ListBrowseSort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
#define IMG_INDEX(x) (x-IDB_PERFORCE)


// Module global for use in sort callback
CP4ListBrowse *pDlg;
int viewType;


/////////////////////////////////////////////////////////////////////////////
// CP4ListBrowse dialog


CP4ListBrowse::CP4ListBrowse(CWnd* pParent, BOOL bWiz/*=FALSE*/, BOOL bBranchInteg/*=FALSE*/)
	: CDialog(CP4ListBrowse::IDD, pParent)
{
	//{{AFX_DATA_INIT(CP4ListBrowse)
	//}}AFX_DATA_INIT
	viewType = m_viewType;
	m_pParent = pParent;
	m_Wiz = bWiz;
	m_BranchInteg = bBranchInteg;
	m_SortAscending = m_FilterByHost = m_IsFiltered = FALSE;
	m_LastSortColumn= 0;
	pDlg=this;
	m_InitRect.SetRect(0,0,100,100);
	m_WinPos.SetWindow( this, bBranchInteg ? _T("IntegDlg") : _T("P4ListBrowse") );
	for (int i = -1; ++i < MAX_SORT_COLUMNS; )
		m_SortColumns[i] = 0;
}


void CP4ListBrowse::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CP4ListBrowse)
	DDX_Control(pDX, IDC_P4LIST, m_P4ListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CP4ListBrowse, CDialog)
	//{{AFX_MSG_MAP(CP4ListBrowse)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_P4LIST, OnColumnclickP4list)
	ON_NOTIFY(NM_DBLCLK, IDC_P4LIST, OnDblclickP4list)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_DESCRIBE, OnDescribe)
	ON_BN_CLICKED(IDC_BACK, OnBack)
//	ON_BN_CLICKED(IDHELP, OnHelp)
//	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CP4ListBrowse message handlers

BOOL CP4ListBrowse::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// Record the initial window size, then see if there is a registry preference
	GetWindowRect(&m_InitRect);
	m_LastRect = m_InitRect;
	if (!m_Wiz)
		m_WinPos.RestoreWindowPosition();

	CString str;

	SetWindowText(m_caption);
	SetFont(m_Font);
	
	// Modify the list control style so that the entire selected row is highlighted
	LRESULT dwStyle = ::SendMessage(m_P4ListCtrl.m_hWnd,LVM_GETEXTENDEDLISTVIEWSTYLE,0,0);
	dwStyle |= LVS_EX_FULLROWSELECT;
	::SendMessage(m_P4ListCtrl.m_hWnd,LVM_SETEXTENDEDLISTVIEWSTYLE,0,dwStyle);

	// Make sure list control shows selection when not the focused control
	m_P4ListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS, 0);

	// Set the imagelist
	m_P4ListCtrl.SetImageList(TheApp()->GetImageList(), LVSIL_SMALL);

	if (m_Wiz)
	{
		GetDlgItem(IDC_BACK)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BACK)->EnableWindow(TRUE);
		GetDlgItem(IDOK)->SetWindowText(LoadStringResource(IDS_FINISH));
		m_Hostname = GET_P4REGPTR()->GetHostname();
		m_FilterByHost = TRUE;
		if (!m_IsFiltered)
		{
			GetDlgItem(IDC_REFRESH)->EnableWindow(FALSE);
			GetDlgItem(IDC_REFRESH)->ShowWindow(SW_HIDE);
		}
	}
	else if (m_BranchInteg)
	{
		GetDlgItem(IDC_BACK)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BACK)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->SetWindowText(LoadStringResource(IDS_NEXT));
	}

	if (m_IsFiltered)
		GetDlgItem(IDC_REFRESH)->SetWindowText(LoadStringResource(IDS_CLEARFILTERR));

	// Get original size of control
	CRect rect;
	m_P4ListCtrl.GetWindowRect(&rect);

	int colwidth[MAX_P4OBJECTS_COLUMNS]={90,90,90,90,90,90,90,90,90,90};

	// make sure OnSize gets called to reposition controls
	// if restored position is default, this won't happen unless
	// we force it
	GetClientRect(&rect);
	SendMessage(WM_SIZE, 0, MAKELONG(rect.Width(), rect.Height()));

	// Get new size of control after resized as specified in the registry
	m_P4ListCtrl.GetWindowRect(&rect);

	// Get any saved column widths from registry
	RestoreSavedWidths(colwidth, MAX_P4OBJECTS_COLUMNS);

	// Make sure no column completely disappeared (because you can't get it back then)
	for (int i=-1; ++i < MAX_P4OBJECTS_COLUMNS; )
	{
		if (colwidth[i] < 5)
			colwidth[i] = 5;
	}
	// Use the same font as the calling window
	m_P4ListCtrl.SetFont(m_Font);

	// Insert the columns 
	INT_PTR maxcols = m_ColNames->GetSize();
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
		retval=m_P4ListCtrl.InsertColumn(subItem, &lvCol);
	}
 
	AddTheListData();

	EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CP4ListBrowse::AddTheListData()
{
	// Add the data
	INT_PTR maxcols = m_ColNames->GetSize();
	CP4Object *p4Object;
	LV_ITEM lvItem;
	int iActualItem = -1;
	CString txt;
	POSITION pos= m_pP4List->GetHeadPosition();
	int iItem;
	for(iItem=0; iItem < m_pP4List->GetCount(); iItem++)
	{
		p4Object= (CP4Object *) m_pP4List->GetNext(pos);

		if (m_FilterByHost)
		{
			txt = p4Object->GetField(2);
			if (!txt.IsEmpty() && txt.CompareNoCase(m_Hostname))
				continue;
		}
		
		for(int subItem=0; subItem < maxcols; subItem++)
		{
			lvItem.mask=LVIF_TEXT | 
					((subItem==0) ? LVIF_IMAGE : 0) |
					((subItem==0) ? LVIF_PARAM : 0);
			lvItem.iItem= (subItem==0) ? iItem : iActualItem;
            ASSERT(lvItem.iItem != -1);
			lvItem.iSubItem= subItem;
			lvItem.iImage = m_iImage;
			lvItem.lParam=(LPARAM) p4Object;
			txt=PadCRs(p4Object->GetField(subItem));
			lvItem.pszText = const_cast<LPTSTR>( (LPCTSTR ) txt);

			if(subItem==0)
				iActualItem=m_P4ListCtrl.InsertItem(&lvItem);
			else
				m_P4ListCtrl.SetItem(&lvItem);
		}
	}

	if(m_pP4List->GetCount())
	{
		// Sort the P4Objects list the same way the P4ListView is sorted
		viewType = m_viewType;
		m_P4ListCtrl.SortItems( P4ListBrowseSort, m_LastSortColumn );

		// Find the p4Object currently selected in the P4Objects pane
		iItem=0;
		if (!m_CurrP4Object->IsEmpty())
		{
			TCHAR cur[ LISTVIEWNAMEBUFSIZE + 1 ];
			TCHAR str[ LISTVIEWNAMEBUFSIZE + 1 ];

			lstrcpy(cur, m_CurrP4Object->GetBuffer(m_CurrP4Object->GetLength()+1));
			m_CurrP4Object->ReleaseBuffer(-1);
			for (int cnt = ListView_GetItemCount( m_P4ListCtrl.m_hWnd ); iItem < cnt; iItem++ )
			{
				ListView_GetItemText( m_P4ListCtrl.m_hWnd, iItem, 0, str, LISTVIEWNAMEBUFSIZE );
				if( !Compare(cur, str) )
					break;
			}
			if (iItem >= ListView_GetItemCount( m_P4ListCtrl.m_hWnd ))
				iItem = 0;
		}

		// Make sure the same p4Object is selected and visible
		m_P4ListCtrl.SetItemState(iItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_P4ListCtrl.EnsureVisible( iItem, FALSE );
	}

	// And finally, set focus to the list control so that the first 'down'
	// keystroke can be used to scroll down
	m_P4ListCtrl.SetFocus();
}

void CP4ListBrowse::SetP4ObjectList(CObList *P4list)
{
	ASSERT_KINDOF(CObList,P4list);
	m_pP4List= P4list;
}

void CP4ListBrowse::SetP4ObjectCols(CStringArray *P4Cols)
{
	m_ColNames= P4Cols;
}

void CP4ListBrowse::SetP4ObjectCurr(CString *P4Name)
{
	m_CurrP4Object= P4Name;
}

void CP4ListBrowse::OnOK() 
{
	if (!m_Wiz)
		m_WinPos.SaveWindowPosition();

	int nItem=m_P4ListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
	if (nItem != -1)
	{
		TCHAR str[ LISTVIEWNAMEBUFSIZE + 1 ];
		ListView_GetItemText( m_P4ListCtrl.m_hWnd, nItem, 0, str, LISTVIEWNAMEBUFSIZE );
		m_SelectedP4Object = str;
	}
	else
		m_SelectedP4Object.Empty();
	
	EndDialog(IDOK);
}

void CP4ListBrowse::OnCancel() 
{
	if (!m_Wiz)
		m_WinPos.SaveWindowPosition();	
	CDialog::OnCancel();
}

void CP4ListBrowse::OnBack() 
{
	EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, FALSE);
	EndDialog(IDC_BACK);
}

void CP4ListBrowse::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// Slide the OK and Cancel buttons to the bottom of dlg
	CWnd *pRefresh = GetDlgItem(IDC_REFRESH);
	CWnd *pBack = GetDlgItem(IDC_BACK);
	CWnd *pOK   = GetDlgItem(IDOK);
	CWnd *pCancel = GetDlgItem(IDCANCEL);
	CWnd *pDescribe = GetDlgItem(IDC_DESCRIBE);

	if(!pOK)
		return;

	CRect rect;
	GetWindowRect(&rect);
	int dx = rect.Width() - m_LastRect.Width();
	int dy = rect.Height() - m_LastRect.Height();
	// Save the new size
	m_LastRect = rect;

	// Move down
	pRefresh->GetWindowRect(&rect);
	ScreenToClient(rect);
	pRefresh->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Move down and slide right
	pBack->GetWindowRect(&rect);
	ScreenToClient(rect);
	pBack->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pOK->GetWindowRect(&rect);
	ScreenToClient(rect);
	pOK->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pCancel->GetWindowRect(&rect);
	ScreenToClient(rect);
	pCancel->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pDescribe->GetWindowRect(&rect);
	ScreenToClient(rect);
	pDescribe->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Increase the size of the list both horiz and vert
	CWnd *pList = GetDlgItem(IDC_P4LIST);
	pList->GetWindowRect(&rect);
	pList->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								    rect.bottom - rect.top + dy, SWP_NOMOVE | SWP_NOZORDER);
	RedrawWindow();
}

void CP4ListBrowse::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize.x= m_InitRect.Width();
	lpMMI->ptMinTrackSize.y= m_InitRect.Height();
}

void CP4ListBrowse::OnColumnclickP4list(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
    if(pNMListView->iSubItem != m_LastSortColumn)
		m_LastSortColumn=pNMListView->iSubItem;
	else
		m_SortAscending= !m_SortAscending;

	viewType = m_viewType;
	if(pNMListView->iItem == -1)
		m_P4ListCtrl.SortItems( P4ListBrowseSort,(DWORD)pNMListView->iSubItem);
	
	*pResult = 0;
}

// Since a sort column can only appear once in our saved column array,
// given the current column, we can determine the next column.
// Note that incoming 'colnbr' is a non-negative 0-relative number and must be incremented
// The return value is a signed 1-relative number; negative for descending, positive for ascending
int CP4ListBrowse::NextSortColumn(int lastcol)
{
	if (lastcol == 0)
		return 0;		// 0 => no next column
	lastcol++;
	for (int i = -1; ++i < (MAX_SORT_COLUMNS-1); )
	{
		if (abs(m_SortColumns[i]) == lastcol)
			return m_SortColumns[i+1];
	}
	return 0;			// 0 => no next column
}

int CALLBACK P4ListBrowseSort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int rc;
	int nextcol;
	CString txt1;
	CString txt2;

	if (viewType == P4CHANGE_SPEC && !lParamSort)
	{
		txt1.Format(_T("%09d"), _tstoi(((CP4Object const *)lParam1)->GetField( static_cast<int>(lParamSort) )));
		txt2.Format(_T("%09d"), _tstoi(((CP4Object const *)lParam2)->GetField( static_cast<int>(lParamSort) )));
	}
	else
	{
		txt1= ((CP4Object const *)lParam1)->GetField( static_cast<int>(lParamSort) );
		txt2= ((CP4Object const *)lParam2)->GetField( static_cast<int>(lParamSort) );
	}

	if(pDlg->IsSortAscending())
		rc = txt1.CompareNoCase(txt2);
	else
		rc = txt2.CompareNoCase(txt1);

	// check for duplicate keys; if so, sort on next sort columns
	if (!rc && lParamSort && ((nextcol = pDlg->NextSortColumn( static_cast<int>(lParamSort) )) != 0))
	{
		// nextcol now contains a value like 1 for col-0-ascending or like -1 for col-0-decending
		BOOL saveSortAscending = pDlg->IsSortAscending();
		if (nextcol < 0)
		{
			nextcol = 0 - nextcol;
			pDlg->SetSortAscending(FALSE);
		}
		else
			pDlg->SetSortAscending(TRUE);
		rc = P4ListBrowseSort(lParam1, lParam2, (LPARAM)(nextcol-1));
		pDlg->SetSortAscending(saveSortAscending);
	}
	return rc;
}

// Check the registry to see if we have recorded the
// column widths lastused for the p4Object pane's list view.
void CP4ListBrowse::RestoreSavedWidths(int *width, int numcols)
{
    CRegKeyEx key;

    CString theKey = sRegKey + m_SubKey;
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
        if(!sortCols.IsEmpty())
        {
			for(int i=0; i< MAX_SORT_COLUMNS; i++)
            {
				m_SortColumns[i]= GetANumber(sortCols);
                if(!i && !m_SortColumns[i])
                    m_SortColumns[i] = 1;
            }
			m_LastSortColumn= abs(m_SortColumns[0]) - 1; // SortColumns are signed & 1-relative
			m_SortAscending = m_SortColumns[0] >= 0 ? TRUE : FALSE;
        }
    }
}

void CP4ListBrowse::OnRefresh() 
{
	EndDialog(IDC_REFRESH);
}

void CP4ListBrowse::OnDblclickP4list(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	OnOK();
}

void CP4ListBrowse::OnDescribe( void )
{
	int nItem=m_P4ListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
	if (nItem != -1)
	{
		TCHAR str[ LISTVIEWNAMEBUFSIZE + 1 ];
		ListView_GetItemText( m_P4ListCtrl.m_hWnd, nItem, 0, str, LISTVIEWNAMEBUFSIZE );
		m_SelectedP4Object = str;
		if ( m_SelectedP4Object.IsEmpty( ) )
			return;
	}
	else
		return;

	CCmd_Describe *pCmd = new CCmd_Describe;
	pCmd->Init( m_pParent->m_hWnd, RUN_ASYNC );
	pCmd->SetCaller(this);
	pCmd->SetListCtrl(&m_P4ListCtrl);
	if( pCmd->Run( m_viewType, m_SelectedP4Object ) )
	{
		MainFrame()->UpdateStatus( LoadStringResource(IDS_FETCHING_SPEC) );	
		return;
	}
	else
	{
		delete pCmd;
		return;
	}
}

#ifdef	WANTTOFILTERCLIENTSBYHOSTNAME
void CP4ListBrowse::OnFilter() 
{
	m_FilterByHost = !m_FilterByHost;
	m_P4ListCtrl.DeleteAllItems();
	AddTheListData();
}
#endif

#ifdef	HELPWANTEDFORP4LISTBROPWSE
void CP4ListBrowse::OnHelp() 
{
	AfxGetApp()->WinHelp(0);
}

BOOL CP4ListBrowse::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
#endif
