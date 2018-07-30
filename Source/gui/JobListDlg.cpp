// JobListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "JobListDlg.h"
#include "JobView.h"
#include "P4Job.h"
#include "catchalldlg.h"
#include "MainFrm.h"
#include "StringUtil.h"
#include "RegKeyEx.h"
#include "hlp\p4win.hh"
#include "ImageList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static LPCTSTR sRegKey = _T("Software\\Perforce\\P4Win\\Layout\\Job List");
static LPCTSTR sRegValue_ColumnWidths = _T("Column Widths");
static LPCTSTR sRegValue_SortColumns = _T("Sort Columns");

#define UPDATE_STATUS(x) ((CMainFrame *)AfxGetMainWnd())->UpdateStatus(x)
int CALLBACK JobListDlgSort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
#define IMG_INDEX(x) (x-IDB_PERFORCE)


// Module global for use in sort callback
CJobListDlg *pDlg;

/////////////////////////////////////////////////////////////////////////////
// CJobListDlg dialog


CJobListDlg::CJobListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJobListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJobListDlg)
	//}}AFX_DATA_INIT
	m_SortAscending=FALSE;
	m_LastSortColumn=0;
	pDlg=this;
	m_InitRect.SetRect(0,0,100,100);
	m_WinPos.SetWindow( this, _T("JobListDlg") );
	for (int i = -1; ++i < MAX_SORT_COLUMNS; )
		m_SortColumns[i] = 0;
}


void CJobListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJobListDlg)
	DDX_Control(pDX, IDC_JOBSTATUS, m_JobStatus);
	DDX_Control(pDX, IDC_JOBLIST, m_JobListCtrl);
	DDX_Control(pDX, IDC_JOBDESC, m_JobDesc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJobListDlg, CDialog)
	//{{AFX_MSG_MAP(CJobListDlg)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_JOBLIST, OnItemchangedJoblist)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_JOBLIST, OnColumnclickJoblist)
	ON_NOTIFY(NM_DBLCLK, IDC_JOBLIST, OnDblclickJoblist)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_FILTER, OnJobFilter)
	ON_BN_CLICKED(ID_CLEARFILTER, OnClearFilter)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJobListDlg message handlers

BOOL CJobListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CP4Job *job;
	CString str;

	SetFont(m_Font);
	
	// Modify the list control style so that the entire selected row is highlighted
	DWORD dwStyle = ::SendMessage(m_JobListCtrl.m_hWnd,LVM_GETEXTENDEDLISTVIEWSTYLE,0,0);
	dwStyle |= LVS_EX_FULLROWSELECT;
	::SendMessage(m_JobListCtrl.m_hWnd,LVM_SETEXTENDEDLISTVIEWSTYLE,0,dwStyle);

	// Make sure list control shows selection when not the focused control
	m_JobListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS, 0);

	// Set the imagelist
	m_JobListCtrl.SetImageList(TheApp()->GetImageList(), LVSIL_SMALL);

	// Get original size of control
	CRect rect;
	m_JobListCtrl.GetWindowRect(&rect);

	int colwidth[MAX_JOBS_COLUMNS]={90,80,60,90,200};

	// Record the initial window size, and then see if there is a registry preference
	GetWindowRect(&m_InitRect);
	m_WinPos.RestoreWindowPosition();
	// make sure OnSize gets called to reposition controls
	// if restored position is default, this won't happen unless
	// we force it
	GetClientRect(&rect);
	SendMessage(WM_SIZE, 0, MAKELONG(rect.Width(), rect.Height()));

	// Get new size of control after resized as specified in the registry
	m_JobListCtrl.GetWindowRect(&rect);

	// Get any saved column widths from registry
	RestoreSavedWidths(colwidth, MAX_JOBS_COLUMNS);

	// Make sure no column completely disappeared (because you can't get it back then)
	for (int i=-1; ++i < MAX_JOBS_COLUMNS; )
	{
		if (colwidth[i] < 5)
			colwidth[i] = 5;
	}

	// Show the current job filter
	CString txt;
	m_sFilter = PersistentJobFilter(KEY_READ);
	if(m_sFilter.GetLength() > 0)
	{
		txt.FormatMessage(IDS_JOBFILTERSTR, m_sFilter);
	}
	else
	{
		txt = LoadStringResource(IDS_ALLJOBS);
		GetDlgItem(ID_CLEARFILTER)->EnableWindow(FALSE);
	}
	GetDlgItem(IDC_FILTER_TEXT)->SetWindowText(txt);

	// Use the same font as the calling window
	m_JobListCtrl.SetFont(m_Font);

	// Insert the columns 
	int maxcols = m_ColNames->GetSize();
	int width=GetSystemMetrics(SM_CXVSCROLL);;
	int retval;
	LV_COLUMN lvCol;
	int subItem;
	for(subItem=0; subItem < maxcols; subItem++)
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
			lvCol.cx=rect.Width() - width - 4;  // expand last column to fill window
		retval=m_JobListCtrl.InsertColumn(subItem, &lvCol);
	}
 
	// Add the data
	LV_ITEM lvItem;
	int iActualItem = -1;
	POSITION pos= m_pJobList->GetHeadPosition();
	int iItem;
	for(iItem=0; iItem < m_pJobList->GetCount(); iItem++)
	{
		job= (CP4Job *) m_pJobList->GetNext(pos);
		
		for(subItem=0; subItem < maxcols; subItem++)
		{
			lvItem.mask=LVIF_TEXT | 
					((subItem==0) ? LVIF_IMAGE : 0) |
					((subItem==0) ? LVIF_PARAM : 0);
			lvItem.iItem= (subItem==0) ? iItem : iActualItem;
            ASSERT(lvItem.iItem != -1);
			lvItem.iSubItem= subItem;
			lvItem.iImage = CP4ViewImageList::VI_JOB;
			lvItem.lParam=(LPARAM) job;
			txt=PadCRs(job->GetJobField(subItem));
			lvItem.pszText = const_cast<LPTSTR>( (LPCTSTR ) txt);

			if(subItem==0)
				iActualItem=m_JobListCtrl.InsertItem(&lvItem);
			else
				m_JobListCtrl.SetItem(&lvItem);
		}
	}

	if(m_pJobList->GetCount())
	{
		// Sort the jobs list the same way the JobView is sorted
		m_JobListCtrl.SortItems( JobListDlgSort, m_LastSortColumn );

		// Find the job currently selected in the jobs pane
		iItem=0;
		if (!m_CurrJob->IsEmpty())
		{
			TCHAR cur[ LISTVIEWNAMEBUFSIZE + 1 ];
			TCHAR str[ LISTVIEWNAMEBUFSIZE + 1 ];

			lstrcpy(cur, m_CurrJob->GetBuffer(m_CurrJob->GetLength()+1));
			m_CurrJob->ReleaseBuffer(-1);
			for( ; iItem < ListView_GetItemCount( m_JobListCtrl.m_hWnd ); iItem++ )
			{
				ListView_GetItemText( m_JobListCtrl.m_hWnd, iItem, 0, str, LISTVIEWNAMEBUFSIZE );
				if( !Compare(cur, str) )
					break;
			}
			if (iItem >= ListView_GetItemCount( m_JobListCtrl.m_hWnd ))
				iItem = 0;
		}

		// Make sure the same job is selected and visible
		m_JobListCtrl.SetItemState(iItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_JobListCtrl.EnsureVisible( iItem, FALSE );
		
		// Set change description to match selected item in list
		job= (CP4Job *) m_JobListCtrl.GetItemData(iItem);
		str= job->GetDescription();
		str.Replace(_T("\n"), _T("\r\n"));
		m_JobDesc.SetWindowText(str);
	}
	else if( m_sFilter.IsEmpty() )
	{
        AfxMessageBox(IDS_NO_JOBS_AVAILABLE_FOR_FIXING, MB_ICONEXCLAMATION);
		EndDialog(IDCANCEL);
	}
	else
		AddToStatus(LoadStringResource(IDS_USEJOBFILTERBUTTON), SV_MSG);

	if (!LoadJobStatusComboBox())
	{
		GetDlgItem(IDC_JOBSTATUSPROMPT)->ShowWindow( SW_HIDE );
		GetDlgItem(IDC_JOBSTATUS)->ShowWindow( SW_HIDE );
	}

	// And finally, set focus to the list control so that the first 'down'
	// keystroke can be used to scroll down
	m_JobListCtrl.SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CString CJobListDlg::PersistentJobFilter( REGSAM accessmask )
{
	HKEY hKey = NULL;
	const CString sKey = _T("Software\\Perforce\\P4Win\\");
	const CString sEntry = _T("JobFilter");
	CString filter = _T("");
	DWORD disposition;

	if ( RegCreateKeyEx( HKEY_CURRENT_USER, sKey,
							0, NULL,
							REG_OPTION_NON_VOLATILE,
							accessmask, NULL,
							&hKey, &disposition ) == ERROR_SUCCESS )
	{
		const DWORD lenFilter = 512;
		if ( accessmask == KEY_WRITE )
		{
			RegSetValueEx( hKey, sEntry, NULL, REG_SZ
				, (LPBYTE)(LPCTSTR) m_sFilter, m_sFilter.GetLength( ) * sizeof(TCHAR) + 1);
		}
		else
		{
			TCHAR buf[ lenFilter ];
			DWORD cbData = sizeof(buf);
			if ( (RegQueryValueEx( hKey, sEntry, NULL, NULL, 
									(LPBYTE)buf, &cbData ) == ERROR_SUCCESS) && cbData )
			{
				if(!cbData)
					cbData = 1;
				buf[cbData-1] = _T('\0');
				filter = buf;
			}
		}
		RegCloseKey( hKey );
	}
	return filter;
}

BOOL CJobListDlg::LoadJobStatusComboBox()
{
	int i;
	m_JobStatus.AddString(_T("(default)"));
	if (GET_SERVERLEVEL() < 10)
		return FALSE;

	i = m_pJobSpec->Find(_T("\nFields:\n\t"));
	if (i == -1)
		return FALSE;
	i = m_pJobSpec->Find(_T("\n\t102 "), i);
	if (i == -1)
		return FALSE;
	i += sizeof(_T("\n\t102 "))/sizeof(TCHAR) - 1;
	int j = m_pJobSpec->Find(_T(' '), i);
	if (j == -1)
		return FALSE;
	CString name = m_pJobSpec->Mid(i, j-i);
	i = m_pJobSpec->Find(_T("\nValues:"), j);
	if (i == -1)
	{
		CString dashed = _T("\nValues");
		dashed += _T("-") + name;
		i = m_pJobSpec->Find(dashed, j);
		if (i == -1)
			return FALSE;
	}
	i = m_pJobSpec->Find(name, i);
	if (i == -1)
		return FALSE;
	i += name.GetLength();
	while (1)
	{
		TCHAR c = m_pJobSpec->GetAt(++i);
		if ((c != _T(':')) && (c != _T(' ')) && (c != _T('\t')))
			break;
	}
	j = m_pJobSpec->Find(_T('\n'), i);
	if (j == -1)
		return FALSE;
	CString values = m_pJobSpec->Mid(i, j-i);

	while((i = values.Find(_T('/'))) != -1)
	{
		CString value = values.Mid(0,i);
		m_JobStatus.AddString(value);
		values = values.Mid(i+1);
	}
	m_JobStatus.AddString(values);
	return TRUE;
}

void CJobListDlg::SetJobList(CObList *joblist)
{
	ASSERT_KINDOF(CObList,joblist);
	m_pJobList= joblist;
}

void CJobListDlg::SetJobSpec(CString *jobSpec)
{
	m_pJobSpec= jobSpec;
}

void CJobListDlg::SetJobCols(CStringArray *jobCols)
{
	m_ColNames= jobCols;
}

void CJobListDlg::SetJobCurr(CString *jobName)
{
	m_CurrJob= jobName;
}

void CJobListDlg::OnOK() 
{
	int cursel = m_JobStatus.GetCurSel();
	if (cursel != CB_ERR)
		m_JobStatus.GetLBText(cursel, m_JobStatusValue);
	if (m_JobStatusValue == _T("(default)"))
		m_JobStatusValue.Empty();

	m_WinPos.SaveWindowPosition();
	m_SelectedJobs.RemoveAll();

	int nItem=m_JobListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
	while(nItem != -1)
	{
		TCHAR str[ LISTVIEWNAMEBUFSIZE + 1 ];
		ListView_GetItemText( m_JobListCtrl.m_hWnd, nItem, 0, str, LISTVIEWNAMEBUFSIZE );
		m_SelectedJobs.AddHead(str);		 
		nItem=m_JobListCtrl.GetNextItem( nItem, LVNI_ALL | LVNI_SELECTED );
	}
	
	EndDialog(IDOK);
}

void CJobListDlg::OnItemchangedJoblist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CString txt, txt1;

	// Update display of revision description
	if(pNMListView->uNewState==3)
	{
		txt=((CP4Job *) pNMListView->lParam)->GetDescription();
		txt.Replace(_T("\n"), _T("\r\n"));
		m_JobDesc.SetWindowText(txt);
	}

	*pResult = 0;
}

void CJobListDlg::OnCancel() 
{
	m_WinPos.SaveWindowPosition();	
	CDialog::OnCancel();
}

void CJobListDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// Slide the OK and Cancel buttons to the bottom of dlg
	CWnd *pOK = GetDlgItem(IDOK);
	CWnd *pCancel = GetDlgItem(IDCANCEL);
	CWnd *pHelp = GetDlgItem(IDHELP);
	CWnd *pFilter = GetDlgItem(IDC_FILTER);
	CWnd *pClear = GetDlgItem(ID_CLEARFILTER);

	if(!pOK)
		return;

	CRect rect;
	int fw, fh;
	int x = cx - 5;
	int y = cy - 5;
	pFilter->GetClientRect(&rect);
	pFilter->MoveWindow(x - (rect.Width()*2) - 5, 0, 
		fw = rect.Width(), fh = rect.Height(), TRUE);
	pClear->GetClientRect(&rect);
	pClear->MoveWindow(x - rect.Width(), 0, 
		rect.Width(), rect.Height(), TRUE);
	pHelp->GetClientRect(&rect);
	pHelp->MoveWindow(x - rect.Width(), y - rect.Height(), 
		rect.Width(), rect.Height(), TRUE);
	x -= rect.Width() + 5;
	pCancel->GetClientRect(&rect);
	pCancel->MoveWindow(x - rect.Width(), y - rect.Height(), 
		rect.Width(), rect.Height(), TRUE);
	x -= rect.Width() + 5;
	pOK->GetClientRect(&rect);
	pOK->MoveWindow(x - rect.Width(), y - rect.Height(), 
		rect.Width(), rect.Height(), TRUE);

	// leave space between buttons and description edit control
	y -= rect.Height() + 5;	

	// position the filter string
	CWnd *pFltTxt = GetDlgItem(IDC_FILTER_TEXT);
	pFltTxt->GetWindowRect(&rect);
	pFltTxt->MoveWindow(5, (fh-rect.Height())/2+1, cx - 10 - (fw*2) - 6, rect.Height(), TRUE);

	// position the label
	CWnd *pLabel = GetDlgItem(IDC_SUMMARYLABEL);
	pLabel->GetClientRect(&rect);

	int remainder = y - (rect.Height() + 5 + 5);

	// Position the text box
	CWnd *pText = GetDlgItem(IDC_JOBDESC);
	int h = remainder / 4;
	pText->MoveWindow(2, y - h, cx-4, h, TRUE);	
	y -= h + 5;

	// position the label
	pLabel->GetClientRect(&rect);
	pLabel->MoveWindow(2, y - rect.Height(), rect.Width(), rect.Height(), TRUE);

	// Position the Job Status fields
	CWnd *pList = GetDlgItem(IDC_JOBSTATUS);
	pList->GetClientRect(&rect);
	x = cx - 2;
	pList->MoveWindow(x - rect.Width(), y - rect.Height(), 
		rect.Width(), rect.Height(), TRUE);
	x -= rect.Width() + 1;
	pLabel = GetDlgItem(IDC_JOBSTATUSPROMPT);
	pLabel->GetClientRect(&rect);
	pLabel->MoveWindow(x - rect.Width(), y - rect.Height(), 
		rect.Width(), rect.Height(), TRUE);
	y -= rect.Height() + 5;

	// Increase the size of the list 
	pList = GetDlgItem(IDC_JOBLIST);
	pList->MoveWindow(0, fh+5, cx, y-fh-5, TRUE);


	RedrawWindow();
}

void CJobListDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize.x= m_InitRect.Width();
	lpMMI->ptMinTrackSize.y= m_InitRect.Height();
}

void CJobListDlg::OnDblclickJoblist(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	OnOK();
}

void CJobListDlg::OnColumnclickJoblist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
    if(pNMListView->iSubItem != m_LastSortColumn)
		m_LastSortColumn=pNMListView->iSubItem;
	else
		m_SortAscending= !m_SortAscending;

	if(pNMListView->iItem == -1)
		m_JobListCtrl.SortItems( JobListDlgSort,(DWORD)pNMListView->iSubItem);

	*pResult = 0;
}

// Since a sort column can only appear once in our saved column array,
// given the current column, we can determine the next column.
// Note that incoming 'colnbr' is a non-negative 0-relative number and must be incremented
// The return value is a signed 1-relative number; negative for descending, positive for ascending
int CJobListDlg::NextSortColumn(int lastcol)
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

int CALLBACK JobListDlgSort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CString txt1= ((CP4Job const *)lParam1)->GetJobField(lParamSort);
	CString txt2= ((CP4Job const *)lParam2)->GetJobField(lParamSort);

	int rc;
	int nextcol;

	if(pDlg->IsSortAscending())
		rc = txt1.Compare(txt2);
	else
		rc = txt2.Compare(txt1);

	// check for duplicate keys; if so, sort on next sort columns
	if (!rc && lParamSort && ((nextcol = pDlg->NextSortColumn(lParamSort)) != 0))
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
		rc = JobListDlgSort(lParam1, lParam2, (LPARAM)(nextcol-1));
		pDlg->SetSortAscending(saveSortAscending);
	}
	return rc;
}

// Check the registry to see if we have recorded the
// column widths lastused for the job pane's list view.
// Note that "HKEY_CURRENT_USER\Software\perforce\P4win\Layout\JobListDlg\Column Widths"
// is no longer used because we use the "...\Job List\Column Widths" now.
void CJobListDlg::RestoreSavedWidths(int *width, int numcols)
{
    CRegKeyEx key;

    if(ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, sRegKey, KEY_READ))
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

void CJobListDlg::OnJobFilter() 
{
	RECT	rect;

	CJobFilter dlg;
	dlg.SetFilterString ( m_sFilter );
	GetWindowRect(&rect);										// we want to position filter dialog box
	dlg.m_top  = rect.top + GetSystemMetrics(SM_CYCAPTION)*2;	// at the top left of this pane, so pass
	dlg.m_left = rect.left + 2;									// it our current screen location
	dlg.m_right= rect.right;
	MainFrame()->DoNotAutoPoll();
	dlg.DoModal( );
	MainFrame()->ResumeAutoPoll();
	if ( m_sFilter != dlg.GetFilterString ( ) )
	{
		m_sFilter = dlg.GetFilterString ( );
		PersistentJobFilter( KEY_WRITE );
		// Turn off all painting in children of main window to prevent flashing	
		EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, FALSE);
		SET_BUSYCURSOR();
		EndDialog(IDRETRY);
	}
}

void CJobListDlg::OnClearFilter() 
{
	if (!m_sFilter.IsEmpty())
	{
		m_sFilter.Empty();
		PersistentJobFilter( KEY_WRITE );
		// Turn off all painting in children of main window to prevent flashing	
		EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, FALSE);
		SET_BUSYCURSOR();
		EndDialog(IDRETRY);
	}
	else
	{
		GotoDlgCtrl(GetDlgItem(IDC_JOBLIST));
	}
}

void CJobListDlg::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_CLOSING_JOBS_USING_CHANGELISTS);
}

BOOL CJobListDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
