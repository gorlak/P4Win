//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// JobListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "JobListCtrl.h"
#include "JobsConfigure.h"
#include "MainFrm.h"
#include "TokenString.h"
#include "JobDescribe.h"
#include "SpecDescDlg.h"
#include "cmd_editspec.h"
#include "cmd_jobs.h"
#include "cmd_delete.h"
#include "catchalldlg.h"
#include "RegKeyEx.h"
#include "ImageList.h"

static LPCTSTR sRegKey = _T("Software\\Perforce\\P4Win\\Layout\\Job List");
static LPCTSTR sRegValue_ColumnWidths = _T("Column Widths");
static LPCTSTR sRegValue_SortColumns = _T("Sort Columns");

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IMG_INDEX(x) (x-IDB_PERFORCE)

/////////////////////////////////////////////////////////////////////////////
// CJobListCtrl

IMPLEMENT_DYNCREATE(CJobListCtrl, CP4ListCtrl)

BEGIN_MESSAGE_MAP(CJobListCtrl, CP4ListCtrl)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_VIEW_UPDATE_RIGHT, OnUpdateViewUpdate)
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_JOB_DELETE, OnUpdateJobDelete)
	ON_COMMAND(ID_JOB_DELETE, OnJobDelete)
	ON_UPDATE_COMMAND_UI(ID_JOB_EDITSPEC, OnUpdateJobEditspec)
	ON_COMMAND(ID_JOB_EDITSPEC, OnJobEditspec)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
	ON_UPDATE_COMMAND_UI(ID_JOB_DESCRIBE, OnUpdateJobDescribe)
	ON_WM_LBUTTONDBLCLK()

	ON_COMMAND(ID_SETFILTER_JOBS, OnJobSetFilter)
	ON_COMMAND(ID_CLEARFILTER_JOBS, OnJobRemovefilter)

	ON_UPDATE_COMMAND_UI(ID_JOB_SETFILTER, OnUpdateJobSetFilter)
	ON_COMMAND(ID_JOB_SETFILTER, OnJobSetFilter)
	ON_UPDATE_COMMAND_UI(ID_JOB_REMOVEFILTER, OnUpdateJobRemovefilter)
	ON_COMMAND(ID_JOB_REMOVEFILTER, OnJobRemovefilter)

	ON_UPDATE_COMMAND_UI(ID_JOB_SETFILEFILTER, OnUpdateJobSetFileFilter)
	ON_COMMAND(ID_JOB_SETFILEFILTER, OnJobSetFileFilter)
	ON_UPDATE_COMMAND_UI(ID_JOB_SETFILEFILTERINTEG, OnUpdateJobSetFileFilter)
	ON_COMMAND(ID_JOB_SETFILEFILTERINTEG, OnJobSetFileFilterInteg)
	ON_UPDATE_COMMAND_UI(ID_JOB_REMOVEFILEFILTER, OnUpdateJobRemoveFileFilter)
	ON_COMMAND(ID_JOB_REMOVEFILEFILTER, OnJobRemoveFileFilter)

	ON_UPDATE_COMMAND_UI(ID_JOB_NEW, OnUpdateJobNew)
	ON_COMMAND(ID_JOB_NEW, OnJobNew)
	ON_COMMAND(ID_JOB_DESCRIBE, OnDescribe)
	ON_COMMAND(ID_VIEW_UPDATE_RIGHT, OnViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_JOB_CONFIGURE, OnUpdateJobConfigure)
	ON_COMMAND(ID_JOB_CONFIGURE, OnJobConfigure)
	ON_COMMAND(ID_PERFORCE_OPTIONS, OnPerforceOptions)
	ON_MESSAGE(WM_P4JOBS, OnP4JobList )
	ON_MESSAGE(WM_P4EDITSPEC, OnP4JobSpec )
	ON_MESSAGE(WM_P4ENDSPECEDIT, OnP4EndSpecEdit )
	ON_MESSAGE(WM_P4DELETE, OnP4Delete )
	ON_MESSAGE(WM_P4JOBSPEC, OnP4JobSpecColumnNames )
	ON_MESSAGE(WM_P4DESCRIBE, OnP4Describe )
    ON_MESSAGE(WM_P4ENDDESCRIBE, OnP4EndDescribe )
    ON_MESSAGE(WM_FETCHJOBS, OnFetchJobs )
    ON_MESSAGE(WM_QUERYJOBS, OnQueryJobs )
    ON_MESSAGE(WM_QUERYJOBSPEC, OnQueryJobSpec )
    ON_MESSAGE(WM_QUERYJOBFIELDS, OnQueryJobFields )
    ON_MESSAGE(WM_QUERYJOBCOLS, OnQueryJobColumns )
    ON_MESSAGE(	WM_QUERYJOBSELECTION, OnQueryJobSelection )
	ON_MESSAGE(WM_JOB_FILTER, OnJobFilter2)
	ON_MESSAGE(WM_CLEARLIST, OnClear)
END_MESSAGE_MAP()

CJobListCtrl::CJobListCtrl()
{
	m_SortAscending = m_FilterIncIntegs = FALSE;
	m_LastSortCol=0;
	m_viewType = P4JOB_SPEC;
	m_bAlreadyGotColumns = m_Need2CallOnJobConfigure = FALSE;
    m_PostListToChangeNum= 0;
	m_PostListToChangeWnd= 0;
	m_Need2DoNew = FALSE;
	m_NewJob = FALSE;
    m_ColCodes.RemoveAll();
    m_captionplain = LoadStringResource(IDS_PERFORCE_JOBS);

	m_FastJobs = GetSavedColumnNames(m_DesiredCols, _T("Job List"));
	m_CF_JOB   = static_cast<CLIPFORMAT>(RegisterClipboardFormat(LoadStringResource(IDS_DRAGFROMJOB)));
	m_CF_DEPOT = static_cast<CLIPFORMAT>(RegisterClipboardFormat(LoadStringResource(IDS_DRAGFROMDEPOT)));
}

CJobListCtrl::~CJobListCtrl()
{

}

/////////////////////////////////////////////////////////////////////////////
// CJobListCtrl diagnostics

#ifdef _DEBUG
void CJobListCtrl::AssertValid() const
{
	CP4ListCtrl::AssertValid();
}

void CJobListCtrl::Dump(CDumpContext& dc) const
{
	CP4ListCtrl::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJobListCtrl message handlers

void CJobListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int index = GetHitItem ( point );
	if(index != -1)
	{
		SetItemState( index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
		OnDescribe();
	}
	else
		CP4ListCtrl::OnLButtonDblClk(nFlags, point);
}

LRESULT CJobListCtrl::OnClear( WPARAM wParam, LPARAM lParam )
{
	m_sFilter.Empty( );
	PersistentJobFilter( KEY_READ );
	Clear();
	return 0;
}

void CJobListCtrl::Clear()
{
    SetRedraw(FALSE);
	DeleteAllItems();
    SetRedraw(TRUE);

	CP4ListCtrl::Clear();
}

/////////////////////////////////
// Three functions to allow the changelist pane to fetch the job list
// and the current job codes list
/////////////////////////////////

LRESULT CJobListCtrl::OnFetchJobs( WPARAM wParam, LPARAM lParam )
{
    m_PostListToChangeNum= wParam;
	m_PostListToChangeWnd= (HWND)lParam;

    if( GetItemCount() > 0 )
    {
        ::SendMessage(m_PostListToChangeWnd, WM_P4JOBS, m_PostListToChangeNum, 0);
        m_PostListToChangeNum=0;
		m_PostListToChangeWnd=0;
    }
    else
        OnViewUpdate();

    return 0;
}

LRESULT CJobListCtrl::OnQueryJobs( WPARAM wParam, LPARAM lParam )
{
    CObList *m_pJobList= new CObList;

    // Make a copy of the joblist, so the consumer doesnt need
    // to worry about any refresh that happens here at a later
    // time.  (They just need to delete the list when done)
    for( int i = 0; i < GetItemCount(); i++ )
	{
        CP4Job *job= (CP4Job *) GetItemData(i);
		CP4Job *newJob= new CP4Job;
        newJob->Create(job);
        m_pJobList->AddHead(newJob);
	}  

    return (LRESULT) m_pJobList;
}

LRESULT CJobListCtrl::OnQueryJobSpec( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)&m_Spec;
}

LRESULT CJobListCtrl::OnQueryJobFields( WPARAM wParam, LPARAM lParam )
{
    // The caller has provided an array.  Just transpose our array
    // into that array, to give the caller a snapshot.
    CArray<int,int> *array= ( CArray<int,int> *) wParam;

    array->RemoveAll();
    for(int i=0; i < m_ColCodes.GetSize(); i++)
        array->Add( m_ColCodes[i] );

    return (LRESULT) array;
}

LRESULT CJobListCtrl::OnQueryJobColumns( WPARAM wParam, LPARAM lParam )
{
	return (LRESULT)&m_ColNames;
}

LRESULT CJobListCtrl::OnQueryJobSelection( WPARAM wParam, LPARAM lParam )
{
	m_Active = GetSelectedItemText();
	return (LRESULT)&m_Active;
}

void CJobListCtrl::OnUpdateJobNew(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText ( LoadStringResource( IDS_NEW ) );
	pCmdUI->Enable(!SERVER_BUSY() && !m_EditInProgress);		
	m_Need2DoNew = FALSE;
}

/*
	_________________________________________________________________

	since users can now define their own types in the job tracker, we
	have to get the names from the server. 
	_________________________________________________________________
*/

LRESULT CJobListCtrl::OnP4JobSpecColumnNames( WPARAM wParam, LPARAM lParam )
{
	CCmd_JobSpec *pCmd = ( CCmd_JobSpec *) wParam;
	ASSERT_KINDOF(CCmd_JobSpec, pCmd);

	if(!pCmd->GetError())
	{
        // Delete the old column names
		int i;
        for( i= m_ColCodes.GetUpperBound(); i>=0; i-- )
            DeleteColumn(i);

		pCmd->GetSpec( m_Spec );
		m_FieldNames.SetSize(0);
		GetFldNames( m_FieldNames, m_Spec );

		//	Default column widths based on all 5 default columns being present
		//
		int width[MAX_JOBS_COLUMNS] = { 90,80,60,90,200 };

		RestoreSavedWidths( width, max(MAX_JOBS_COLUMNS, m_ColCodes.GetSize()), _T("Job List") );
		for (i = -1; ++i < MAX_JOBS_COLUMNS; )
		{
			if (width[i] > 5000)
				width[i] = 30;
		}
		InsertColumnHeaders( m_ColNames, width );
		m_bAlreadyGotColumns = TRUE;

		//	okay, we got all the job column names.
		//	now get the job list or start the configure dialog
		//
		if (m_Need2CallOnJobConfigure)
			OnJobConfigure();
		else
			GetJobs( );
	}
    else if(m_PostListToChangeNum != 0)
    {
        ::SendMessage(m_PostListToChangeWnd, WM_P4JOBS, m_PostListToChangeNum, 0);
        m_PostListToChangeNum=0;
		m_PostListToChangeWnd=0;
    }
        
	
	delete pCmd;
	MainFrame()->ClearStatus();
	return 0;
}


/*
	_________________________________________________________________

	Simple parse code to check jobspec for spec codes 101-105, 
    which are reserved codes for default fields that may be present in the
    output of p4 jobs.  Record the column headings for these codes.
	_________________________________________________________________
*/

BOOL CJobListCtrl::GetFldNames( CStringArray &fldNames, const CString &spec )
{
	int i;
    CString errorText;
    CString fields;

	m_SpecNames.RemoveAll();
	m_ColNames.RemoveAll();
	m_ColNames.SetSize(0);
	m_ColNames.Copy(m_DesiredCols);

	//  Get the field names from the spec. they are between the word 
    //  "Fields:" and the word "Required" strip the separator.  Don't
    //  crash if the jobspec is not recognized.
    
	int start= spec.Find(_T("Fields:")) + lstrlen(_T("Fields:"));
    int end= spec.Find(_T("Required:"));
    if( start == -1 || end == -1 || end < start || end >= spec.GetLength())
        errorText=LoadStringResource(IDS_UNABLE_TO_FIND_FIELD_DELIMITERS);
    
    if( errorText.IsEmpty() )
    {
	    fields = spec.Mid(start, end - start);
	    start= fields.Find( 0x09 );
        if( start == -1 )
            errorText=LoadStringResource(IDS_UNABLE_TO_FIND_INITIAL_FIELD_DELIMITER);
    }

    if( errorText.IsEmpty() )
    {
        m_ColCodes.RemoveAll();        
	    fields = fields.Mid( start + 1 );

	    // Pick out the requested field names if present
	    
	    int code;
	    CString field;
		int max = m_ColNames.GetSize();
    	do  
	    {
		    //		get the field spec (e.g., "101 Job word 32")
		    //		and truncate the fields.
		    //
			int delim = fields.Find( 0x0a);
			if(delim < 9)	// need at least "n Job x n" or something's wrong
				break;
		    field = fields.Left ( delim + 1 );
		    fields = fields.Right ( fields.GetLength( ) - (delim + 1) );

		    //		get the "101" part of the spec in code
		    //
			delim = field.Find(_T(' '));
			if(delim < 1)
				break;
		    code = _ttoi ( field.Left ( delim ) ) ;
		    field = field.Right ( field.GetLength ( ) - (delim + 1) );
		    //		get the "Job" part in field.
			//
			delim = field.Find(_T(' '));
			if(delim < 1)
				break;
		    field = field.Left ( delim );

			m_SpecNames.Add(field);	// save all field names from the spec here.

			if (code == 101)	// if this is the jobname field, capture its name for 1st column
				m_ColNames.SetAt(0, field);
			else if (code == 103)
				m_ReportedByTitle = field;

			for (i = -1; ++i < max; )
            {
				CString colname = m_ColNames.GetAt(i);
				if (colname == field)
				{
					fldNames.Add( field );
	                m_ColCodes.Add( code );
					break;
				}
				else if ((colname.GetAt(0) == _T(':')) && (colname.GetAt(1) == _T('1')) && (colname.GetAt(2) == _T('0')))
				{
					TCHAR digit = colname.GetAt(3);
					int colcode = 100 + (digit & 0x0F);
					if (colcode == code)
					{
						m_ColNames.SetAt(i, field);
						fldNames.Add( field );
						m_ColCodes.Add( code );
						break;
					}
				}
            }
	    }
	    while ( !field.IsEmpty( ) );
    }

    if( !errorText.IsEmpty() )
    {
        // Very unlikely that we will ever report this error, but an error
        // message is always preferable to an inexplicable crash
		AddToStatus(CString(_T("JobView::GetFldNames()") + errorText), SV_ERROR);
        return FALSE;
    }
    else
	    return TRUE;
}


/*
	_________________________________________________________________
*/

void CJobListCtrl::InsertJob(CP4Job *job, int index)
{
	LV_ITEM lvItem;
	int iActualItem = -1;
	CString txt;
	CString colName;
	
	ASSERT(job != NULL);
	m_iImage = CP4ViewImageList::VI_JOB;

	int maxcols = m_ColNames.GetSize();
	for(int subItem=0; subItem < maxcols; subItem++)
	{
		lvItem.mask=LVIF_TEXT | 
					((subItem==0) ? LVIF_IMAGE : 0) |
					((subItem==0) ? LVIF_PARAM : 0);

		lvItem.iItem= (subItem==0) ? index : iActualItem;
        ASSERT(lvItem.iItem != -1);
		lvItem.iSubItem= subItem;
		lvItem.iImage = CP4ViewImageList::VI_JOB;
		lvItem.lParam=(LPARAM) job;

		txt=PadCRs(job->GetJobField(subItem));
		lvItem.pszText = const_cast<LPTSTR>( (LPCTSTR ) txt);

		if(subItem==0)
	    	iActualItem=InsertItem(&lvItem);
		else
		    SetItem(&lvItem);
        
	}
}


/*	_________________________________________________________________
	
	After a spec edit, update the appropriate tree item
	_________________________________________________________________
*/

void CJobListCtrl::UpdateJob(CP4Job *job, int index)
{
	// First, switch the user data
	CP4Job *oldJob= (CP4Job *) GetItemData(index);
	delete oldJob;
	SetItemData(index, (LPARAM) job);

    CString txt;
	CString colName;

    // Then update the text for any fields that are present
	int maxCols = m_ColNames.GetSize();
    for( int subItem=0; subItem < maxCols; subItem++ )
    {
		txt=PadCRs(job->GetJobField(subItem));
		SetItemText(index, subItem, const_cast<LPTSTR>((LPCTSTR)txt));
    }
}


/*
	_________________________________________________________________
*/

void CJobListCtrl::OnUpdateJobDescribe(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( OnUpdateShowMenuItem( pCmdUI, IDS_DESCRIBEIT_s ) );	
}


/*
	_________________________________________________________________
*/

LRESULT CJobListCtrl::OnP4JobList(WPARAM wParam, LPARAM lParam)
{
	POSITION pos;
	CP4Job *job;
	int index;
	CObList *jobs;

	CCmd_Jobs *pCmd= (CCmd_Jobs *) wParam;

	if(!pCmd->GetError())
	{
		SET_BUSYCURSOR();
		jobs= pCmd->GetList();

        SetRedraw(FALSE);
		CString bigjob = _T("");
		for(pos= jobs->GetHeadPosition(), index=0; pos != NULL; index++)
		{
			job=(CP4Job *) jobs->GetNext(pos);
			if (m_FilterView.GetCount() > 1)	// check for duplicates
			{									// if filtering on more than 1 file
				CString jobname = job->GetJobName();
				if (jobname <= bigjob)
				{
					if (jobname == bigjob || FindInList(jobname) != -1)
					{
						delete job;
						continue;
					}
				}
				else
					bigjob = jobname;
			}
			InsertJob(job, index);
		}
        SetRedraw(TRUE);

		CString msg;
		msg.FormatMessage(IDS_NUMBER_OF_JOBS_n, index );
		AddToStatus( msg, SV_COMPLETION );

		////jobs->RemoveAll();

		ReSort();
	
		if(jobs->GetCount() > 0)
		{
			int i = FindInList(m_Active);
			if (i < 0)	i=0;
			SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			EnsureVisible(i, FALSE);
		}

		CP4ListCtrl::SetUpdateDone();
		if (m_Need2DoNew)
			OnJobNew();
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	}
	else
	{
		CP4ListCtrl::SetUpdateFailed();
		m_Need2DoNew = FALSE;
	}

    if(m_PostListToChangeNum != 0)
    {
        ::SendMessage(m_PostListToChangeWnd, WM_P4JOBS, m_PostListToChangeNum, 0);
        m_PostListToChangeNum= 0;
		m_PostListToChangeWnd= 0;
    }
	
    delete pCmd;
	MainFrame()->ClearStatus();

	// Notify the mainframe that we have finished getting the jobs,
	// hence the entire set of async command have finished.
	MainFrame()->ExpandDepotIfNeedBe();

	return 0;
}

/*
	_________________________________________________________________
*/

void CJobListCtrl::OnUpdateViewUpdate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && !MainFrame()->IsModlessUp());
}

void CJobListCtrl::OnViewUpdate( ) 
{ 
	MainFrame()->SetJobUpdateTime(GetTickCount());
	m_Active = GetSelectedItemText();
	SetCaption( );
	if( m_ReadSavedWidths )
		SaveColumnWidths();


	if( GET_SERVERLEVEL() > 3 )
		GetJobSpec( ) ;		//which also calls GetJobs( )
    else
    {
        // Against 97.3 server, the column names are always the same
       	m_ColNames.RemoveAll();
        m_ColNames.Add(LoadStringResource(IDS_P4JOB));
        m_ColNames.Add(LoadStringResource(IDS_P4STATUS));
        m_ColNames.Add(LoadStringResource(IDS_REPORTEDBY));
        m_ColNames.Add(LoadStringResource(IDS_REPORTEDDATE));
        m_ColNames.Add(LoadStringResource(IDS_DESCRIPTION));

        m_ColCodes.RemoveAll();

		int i;
        for(i=101; i <= 105; i++)
            m_ColCodes.Add(i);
        

		//		there is no default for the jobs now that the fields 
		//		are user-defined.
		//
		int width[ MAX_JOBS_COLUMNS ] = { 90,80,60,90,200 };

		RestoreSavedWidths( width, m_ColCodes.GetSize(), _T("Job List") );
		for (i = -1; ++i < MAX_JOBS_COLUMNS; )
		{
			if (width[i] > 5000)
				width[i] = 30;
		}
		InsertColumnHeaders( m_ColNames, width );
		m_bAlreadyGotColumns = TRUE;
        GetJobs();
    }
}


void CJobListCtrl::GetJobSpec( )
{
    CCmd_JobSpec *pCmd = new CCmd_JobSpec;
	pCmd->Init( m_hWnd, RUN_ASYNC );
	
    if( !pCmd->Run( ) )
	   	delete pCmd;
}


void CJobListCtrl::GetJobs( )
{
	CCmd_Jobs *pCmd = new CCmd_Jobs;
	pCmd->Init( m_hWnd, RUN_ASYNC);
	
    if( GET_SERVERLEVEL() == 3)
    {
        m_sFilter.Empty();
       	pCmd->SetFilter( FALSE );
    }
    else
    	pCmd->SetFilter( !m_sFilter.IsEmpty( ) );

	ASSERT(m_bAlreadyGotColumns);
	int maxFlds = m_FieldNames.GetSize();
	int maxCols = m_ColNames.GetSize();
	int i;
	for (i = -1; ++i < maxCols; )	// add the fields desired in column order
	{
		CString colName = m_ColNames.GetAt(i);
		int j;
		for (j = -1; ++j < maxFlds; )
		{
			if (m_FieldNames.GetAt(j) == colName)
			{
				pCmd->GetFieldNames().Add(colName);
				pCmd->GetFieldCodes().Add(m_ColCodes.GetAt(j));
				break;
			}
		}
		if (j >= maxFlds)
		{
			pCmd->GetFieldNames().Add(_T(""));
			pCmd->GetFieldCodes().Add(0);
		}
	}

	// Make a copy of the filter view, because CCmd_Jobs will
	// destroy that copy
	POSITION pos=m_FilterView.GetHeadPosition();
	m_StrList.RemoveAll();
	while(pos != NULL)
		m_StrList.AddTail(m_FilterView.GetNext(pos));

	if( pCmd->Run( m_sFilter, m_FastJobs, &m_StrList, m_FilterIncIntegs ) )
	{
		Clear();
		CP4ListCtrl::OnViewUpdate();
		MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_JOBS_LISTING) );
	}
	else
		delete pCmd;
}


/*
	_________________________________________________________________
*/

CString CJobListCtrl::SetCaption()
{
	if( (m_sFilter.IsEmpty() && m_FilterView.IsEmpty()) || GET_SERVERLEVEL() < 4)
		m_caption = LoadStringResource(IDS_PERFORCE_JOBS);
	else
    {
        CString filter;
		if (!m_sFilter.IsEmpty() && !m_FilterView.IsEmpty())
			filter.FormatMessage(IDS_FILTERED_BOTH, m_sFilter);
		else if (!m_FilterView.IsEmpty())
		{
			CString filelist=m_FilterView.GetHead();
			if (m_FilterView.GetCount() > 1)
			{
				POSITION pos= m_FilterView.GetHeadPosition();
				m_FilterView.GetNext(pos);
				while( pos != NULL )
					filelist += _T(", ") + m_FilterView.GetNext(pos);
			}
			filter.FormatMessage(IDS_FILTERED_BY_FILES, filelist);
		}
		else
			filter.FormatMessage(IDS_FILTERED, m_sFilter);
		m_caption = LoadStringResource(IDS_PERFORCE_JOBS) + filter;
    }

	CP4PaneContent::GetView()->SetCaption();

	return m_caption;
}


/*
	_________________________________________________________________
*/

void CJobListCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// make sure window is active
	GetParentFrame()->ActivateFrame();

	///////////////////////////////
	// See ContextMenuRules.txt for order of menu commands!

	// create an empty context menu
	CP4Menu popMenu;
	popMenu.CreatePopupMenu();

	int	index;
    SetIndexAndPoint( index, point );

	// Can always create new job
	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_JOB_NEW );

	if(index != -1)
	{
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_JOB_EDITSPEC );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_JOB_DESCRIBE );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_JOB_DELETE );
	}
	popMenu.AppendMenu(MF_SEPARATOR);
	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_JOB_SETFILTER );
	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_JOB_REMOVEFILTER );
	popMenu.AppendMenu(MF_SEPARATOR);
	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_JOB_SETFILEFILTER, LoadStringResource(IDS_FILTER_JOBVIEW) );
	if (GET_P4REGPTR()->GetEnableSubChgIntegFilter())
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_JOB_SETFILEFILTERINTEG, LoadStringResource(IDS_FILTERINTEG_JOBVIEW) );
	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_JOB_REMOVEFILEFILTER, LoadStringResource(IDS_JOB_REMOVEFILEFILTER) );
	popMenu.AppendMenu(MF_SEPARATOR);
	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_JOB_CONFIGURE, LoadStringResource(IDS_JOB_CONFIGURE) );
	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_VIEW_UPDATE, LoadStringResource(IDS_REFRESH));

	MainFrame()->AddToolsToContextMenu(&popMenu);

	popMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	point.x, point.y, AfxGetMainWnd());
}


/*
	_________________________________________________________________
*/

void CJobListCtrl::OnUpdateJobDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( OnUpdateShowMenuItem( pCmdUI, IDS_DELETE_s ) );	
}

void CJobListCtrl::OnJobDelete() 
{
	OnDelete( P4JOB_DEL );
}


/*
	_________________________________________________________________
*/

void CJobListCtrl::OnUpdateJobEditspec( CCmdUI* pCmdUI ) 
{
	pCmdUI->Enable( OnUpdateShowMenuItem(pCmdUI, IDS_EDITSPEC_s) && !m_EditInProgress );	
}

void CJobListCtrl::EditTheSpec(CString *name)
{
	OnJobEditspec(name);
}

void CJobListCtrl::OnJobEditspec() 
{
	CString str = GetSelectedItemText();
	OnJobEditspec(&str);
}

void CJobListCtrl::OnJobEditspec(CString *jobname) 
{
	m_Active = *jobname;
	m_NewJob = FALSE;
	EditSpec( ) ;
}


void CJobListCtrl::OnJobNew() 
{
	MainFrame()->ViewJobs();
	if (SERVER_BUSY())
		m_Need2DoNew = TRUE;
	else
	{
		m_Need2DoNew = FALSE;
		m_Active.Empty();
		m_NewJob = TRUE;
		EditSpec( ) ;
	}
}


void CJobListCtrl::EditSpec()
{
	if (m_EditInProgress)
	{
		CantEditRightNow(IDS_JOB);
		return;
	}

	m_pNewSpec = new CP4Job;
	
	CCmd_EditSpec *pCmd = new CCmd_EditSpec;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
	if( pCmd->Run( P4JOB_SPEC, m_Active, m_pNewSpec ) )//fanny:is this okay for edit and new?
		MainFrame()->UpdateStatus( LoadStringResource(IDS_EDITING_JOB_SPEC) );
	else
	{
		delete pCmd;
		delete m_pNewSpec;
	}
}


LRESULT CJobListCtrl::OnP4JobSpec(WPARAM wParam, LPARAM lParam)
{
	CCmd_EditSpec *pCmd= (CCmd_EditSpec *) wParam;

	// let the dialogbox know whether this is a new job or an edit of an existing one
	pCmd->SetIsRequestingNew(m_NewJob);
	pCmd->SetCaller(DYNAMIC_DOWNCAST(CView, GetParent()));
	if(!pCmd->GetError() && !m_EditInProgress && pCmd->DoSpecDlg(this))
	{
		m_EditInProgress = TRUE;
		m_EditInProgressWnd = pCmd->GetSpecSheet();
	}
	else
	{
		delete m_pNewSpec;
		if (pCmd->HaveServerLock())
			pCmd->ReleaseServerLock();
		delete pCmd;
	}
	MainFrame()->ClearStatus();
	return 0;
}

LRESULT CJobListCtrl::OnP4EndSpecEdit( WPARAM wParam, LPARAM lParam )
{
	CCmd_EditSpec *pCmd= (CCmd_EditSpec *) wParam;
	int index;

	if (lParam != IDCANCEL && lParam != IDABORT && lParam != IDRETRY)
	{
		m_pNewSpec->SetJobName(pCmd->GetNewJobName());

		if(m_NewJob)
		{
			LPCTSTR statustext = m_pNewSpec->GetStatusText();
			if (!*statustext)
				m_pNewSpec->SetJobStatus(JOB_OPEN);
		}

		m_pNewSpec->ConvertToColumns(m_ColCodes, m_ColNames, m_FieldNames);
		if(m_NewJob & (FindInList(pCmd->GetNewJobName()) == -1) )
		{
			InsertJob(m_pNewSpec, GetItemCount());
			ReSort();
			index= FindInList(pCmd->GetNewJobName());
		}
		else
		{
			index= FindInList(pCmd->GetNewJobName());
			if (index != -1)
				UpdateJob(m_pNewSpec, index);
			else if (m_ColNames.GetSize())	// Has job pane been initialized?
			{
				InsertJob(m_pNewSpec, GetItemCount());
				ReSort();
				index= FindInList(pCmd->GetNewJobName());
			}
			else
				delete m_pNewSpec;
		}
		EnsureVisible( index, TRUE );
		SetItemState( index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
	}
	else
		delete m_pNewSpec;

	if (lParam != IDABORT)
	{
		MainFrame()->ClearStatus();
		if (pCmd->HaveServerLock())
			pCmd->ReleaseServerLock();
		CDialog *dlg = (CDialog *)pCmd->GetSpecSheet();
		dlg->DestroyWindow();
	}
	delete pCmd;
	m_EditInProgress = FALSE;
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Sort callback, not in class


int CJobListCtrl::OnCompareItems(LPARAM lParam1, LPARAM lParam2, int subItem)
{
    ASSERT(lParam1 && lParam2);
	CString txt1= ((CP4Job const *)lParam1)->GetJobField(subItem);
	CString txt2= ((CP4Job const *)lParam2)->GetJobField(subItem);

	int rc;

	if(m_SortAscending)
		rc = txt1.Compare(txt2);
	else
		rc = txt2.Compare(txt1);

	return rc;
}

/*
	_________________________________________________________________
*/

void CJobListCtrl::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	delete (CP4Job *) GetItemData(pNMListView->iItem);
			
	*pResult = 0;
}


/*
	_________________________________________________________________
*/

void CJobListCtrl::OnUpdateJobSetFilter(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText ( LoadStringResource( IDS_FILTER ) );
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, GET_SERVERLEVEL() > 3 && !SERVER_BUSY()));
}

LRESULT CJobListCtrl::OnJobFilter2(WPARAM, LPARAM) 
{
	OnJobSetFilter();
	return 0;
}

void CJobListCtrl::OnJobSetFilter() 
{
	RECT	rect;

	CJobFilter dlg;
	dlg.SetFilterString ( m_sFilter );
	GetWindowRect(&rect);										// we want to position filter dialog box
	dlg.m_top  = rect.top + GetSystemMetrics(SM_CYCAPTION);		// at the top left of this pane, so pass
	dlg.m_left = rect.left + 2;									// it our current screen location
	dlg.m_right= rect.right;
	MainFrame()->DoNotAutoPoll();
	dlg.DoModal( );
	MainFrame()->ResumeAutoPoll();
	if ( m_sFilter != dlg.GetFilterString ( ) )
	{
		m_sFilter = dlg.GetFilterString ( );
		PersistentJobFilter( KEY_WRITE );
		OnViewUpdate( );	
	}
}

void CJobListCtrl::OnUpdateJobRemovefilter(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText ( LoadStringResource( IDS_CLEARFILTER ) );
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, GET_SERVERLEVEL() > 3 
		&& !SERVER_BUSY() && !m_sFilter.IsEmpty()));
}

void CJobListCtrl::OnJobRemovefilter() 
{
	m_sFilter.Empty ( );
	PersistentJobFilter( KEY_WRITE );
	OnViewUpdate( );
}


void CJobListCtrl::PersistentJobFilter( REGSAM accessmask )
{
	HKEY hKey = NULL;
	CString sKey = _T("Software\\Perforce\\P4Win\\");
	CString sEntry = _T("JobFilter");
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
				m_sFilter = buf;
			}
		}

		RegCloseKey( hKey );
	}
}

void CJobListCtrl::OnUpdateJobConfigure(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY()));
}

int CJobListCtrl::GetFieldNbr( CString str, const CString &spec )
{
	//  Get the field names from the spec. they are between the word 
    //  "Fields:" and the word "Required" strip the separator.  Don't
    //  crash if the jobspec is not recognized.

    CString errorText;
    CString fields;
    
	int start= spec.Find(_T("Fields:")) + lstrlen(_T("Fields:"));
    int end= spec.Find(_T("Required:"));
    if( start == -1 || end == -1 || end < start || end >= spec.GetLength())
        errorText=LoadStringResource(IDS_UNABLE_TO_FIND_FIELD_DELIMITERS);
    
    if( errorText.IsEmpty() )
    {
	    fields = spec.Mid(start, end - start);
	    start= fields.Find( _T('\t') );
        if( start == -1 )
            errorText=LoadStringResource(IDS_UNABLE_TO_FIND_INITIAL_FIELD_DELIMITER);
    }

    if( errorText.IsEmpty() )
    {
	    fields = fields.Mid( start + 1 );

	    // Pick out the requested field names if present
	    
	    int code;
	    CString field;
    	do  
	    {
		    //		get the field spec (e.g., "101 Job word 32")
		    //		and truncate the fields.
		    //
		    field = fields.Left ( fields.Find( _T('\n') ) + 1 );
		    fields = fields.Right ( fields.GetLength( ) - field.Find ( _T('\n') ) - 1 );

		    //		get the "101" part of the spec in code
		    //		and the "Job" part in field.
		    //
			int i = field.Find( _T(' ') );
			if (i == -1)
				return 0x7FFF;
		    code = _ttoi ( field.Left ( i ) ) ;
		    field = field.Right ( field.GetLength ( ) - field.Find( _T(' ') ) - 1 );
		    field = field.Left ( field.Find ( _T(' ') ) );

			if (str == field)
                return code;
	    }
	    while ( !field.IsEmpty( ) );
    }

    if( !errorText.IsEmpty() )
    {
		// Very unlikely that we will ever report this error, but an error
		// message is always preferable to an inexplicable crash
		AddToStatus(CString(_T("JobView::GetFldNbr()") + errorText), SV_ERROR);
	}
    return 0x7FFF;
}

void CJobListCtrl::OnJobConfigure() 
{
	int rc;
	CJobsConfigure dlg;
	int maxcols = m_ColNames.GetSize();
	if (!maxcols && !m_Need2CallOnJobConfigure)
	{
		m_Need2CallOnJobConfigure = TRUE;
		GetJobSpec( );
		return;
	}
	m_Need2CallOnJobConfigure = FALSE;

	CStringArray colNames;
	colNames.SetSize(maxcols);
    CArray<int, int> colWidths;
	colWidths.SetSize(maxcols);
	int subItem;
	for(subItem=0; subItem < maxcols; subItem++)
	{
		dlg.m_ColNames += m_ColNames.GetAt(subItem) + _T(' ');
		colWidths.SetAt(subItem, GetColumnWidth(subItem));
		colNames.SetAt(subItem, m_ColNames.GetAt(subItem));
	}
	int maxflds = m_SpecNames.GetSize();
	for(subItem=0; subItem < maxflds; subItem++)
		dlg.m_SpecNames += m_SpecNames.GetAt(subItem) + _T(' ');
	if ((rc = dlg.DoModal( )) != IDCANCEL)
	{
		BOOL need2save = FALSE;
		CString newWidths;
		CString newSorts;
		if (rc == IDOK)
		{
			int i, j;
			CString num;
			CString newNames;
			int newSortCols[MAX_SORT_COLUMNS] = {0,0,0,0};
			int tokenctr = 1;
			CTokenString tokstr;
			CString token;
			tokstr.Create(dlg.m_ColNames);
			tokstr.PrepareParse( );
			token=tokstr.GetToken();  
			while(!token.IsEmpty())
			{
				num = _T("50");
				for (i = -1; ++i < maxcols; )
				{
					if (m_ColNames.GetAt(i) == token)
					{
						int k;
						for (k=-1; ++k < maxcols; )
						{
							if (colNames.GetAt(k) == token)
								break;
						}
						if (k < maxcols)
							num.Format(_T("%d"), colWidths.GetAt(k));
						for (j=-1, ++i; ++j < MAX_SORT_COLUMNS; )
						{
							if (abs(m_SortColumns[j]) == i)
							{
								newSortCols[j] = m_SortColumns[j] < 0 ? -tokenctr : tokenctr;
								break;
							}
						}
						break;
					}
				}
			    if(!newWidths.IsEmpty())
				    newWidths += _T(",");
				newWidths += num;
				int n = GetFieldNbr(token, m_Spec);
				if (n < 106)
					token.Format(_T(":%d"), n);
				newNames += token + _T(' ');
				token=tokstr.GetToken();
				tokenctr++;
			}
			SaveColumnNames(newNames, _T("Job List"));
			for (i = -1; ++i < MAX_SORT_COLUMNS; )
			{
				num.Format(_T("%d"), newSortCols[i]);
				if(i)
					newSorts+=_T(",");
				newSorts+=num;
			}
			need2save = TRUE;
		}
		else if (rc == IDIGNORE)
		{
			DeleteColumnNames(_T("Job List"));
			newWidths = _T("90,80,60,90,200");
			newSorts  = _T("-1,0,0,0");
			need2save = TRUE;
		}
		if (need2save)
		{
			// Save the column widths
			CString theKey = sRegKey;
			CRegKeyEx key;
			if(ERROR_SUCCESS == key.Create(HKEY_CURRENT_USER, theKey))
			{
				key.SetValueString(newWidths, sRegValue_ColumnWidths);
	            key.SetValueString(newSorts, sRegValue_SortColumns);
				m_ReadSavedWidths = FALSE;	
			}
		}
		m_DesiredCols.RemoveAll();
		m_DesiredCols.SetSize(0);
		m_FastJobs = GetSavedColumnNames(m_DesiredCols, _T("Job List"));
		m_FieldNames.SetSize(0);
		GetFldNames( m_FieldNames, m_Spec );
		OnViewUpdate( );
	}
}

void CJobListCtrl::OnDescribeJob() 
{
	//		let user type in the job name. if it's blank the user bailed.
	//
	CJobDescribe dlg;
	dlg.m_JobStr = m_Describing;
	if( dlg.DoModal( ) == IDCANCEL )
		return;

	m_Describing = dlg.GetJobStr( ) ;
	if ( m_Describing.IsEmpty( ) )
		return;

	m_Active = GetSelectedItemText();	// so we can see whether to show the Edit button or not

	CCmd_Describe *pCmd= new CCmd_Describe;
	pCmd->Init( m_hWnd, RUN_ASYNC);
	if( pCmd->Run( P4JOB_SPEC, m_Describing) )
		MainFrame()->UpdateStatus(LoadStringResource(IDS_FETCHING_JOB_SPEC));
	else
		delete pCmd;
}

BOOL CJobListCtrl::TryDragDrop( )
{
	// Store the job this is from
	m_DragFromItemName = GetSelectedItemText();

	m_OLESource.DelayRenderData( (unsigned short) m_CF_JOB);

	return m_OLESource.DoDragDrop(DROPEFFECT_COPY, &m_DragSourceRect, NULL)
			== DROPEFFECT_NONE ? FALSE : TRUE;
}

/////////////////////////////////////////////////////////////////////
// OLE drag-drop support, to accept depot files or folders
// or accept user or client names which will
// define a view to be used to filter the submitted
// changes that this window displays.
// Also can drop Jobs to be Fixed.
/////////////////////////////////////////////////////////////////////

DROPEFFECT CJobListCtrl::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	m_DropEffect=DROPEFFECT_NONE;
	m_DragDataFormat=0;

	// Dont allow a drop if the server is busy, since a drop immediately attempts to
	// invoke a server command
	if(SERVER_BUSY())
		return DROPEFFECT_NONE;
		
	if(pDataObject->IsDataAvailable( (unsigned short) m_CF_DEPOT))
	{
		m_DropEffect=DROPEFFECT_COPY;
		m_DragDataFormat=m_CF_DEPOT;
	}
#ifdef UNICODE
	else if(pDataObject->IsDataAvailable( (unsigned short) CF_UNICODETEXT))
	{
		m_DropEffect=DROPEFFECT_COPY;
		m_DragDataFormat=CF_UNICODETEXT;
	}
#else
	else if(pDataObject->IsDataAvailable( (unsigned short) CF_TEXT))
	{
		m_DropEffect=DROPEFFECT_COPY;
		m_DragDataFormat=CF_TEXT;
	}
#endif
	return m_DropEffect;
}

DROPEFFECT CJobListCtrl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	// Dont allow a drop if the server is busy, since a drop immediately attempts to
	// invoke a server command
	if(SERVER_BUSY())
		m_DropEffect= DROPEFFECT_NONE;
		
	return m_DropEffect;
}

BOOL CJobListCtrl::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	CString fname;
	
	if(SERVER_BUSY())
	{
		// OnDragEnter() and OnDragOver() should avoid a drop at 
		// the wrong time!
		ASSERT(0);
		return FALSE;
	}
	
	if(m_DragDataFormat == m_CF_DEPOT)
	{
		ClientToScreen(&point);
		::SendMessage(m_depotWnd, WM_DROPTARGET, JOBVIEW, MAKELPARAM(point.x,point.y));
		return TRUE;
	}
#ifdef UNICODE
	if(m_DragDataFormat == CF_UNICODETEXT)
	{
		HGLOBAL hGlob = pDataObject->GetGlobalData(CF_UNICODETEXT);
#else
	if(m_DragDataFormat == CF_TEXT)
	{
		HGLOBAL hGlob = pDataObject->GetGlobalData(CF_TEXT);
#endif
		LPCTSTR p;

		if ((hGlob != NULL)	&& ((p = (LPCTSTR)::GlobalLock(hGlob)) != NULL))
		{
			CString itemStr = p;
			::GlobalUnlock(hGlob);
			OnEditPaste( itemStr );
		}
		return TRUE;
	}
	// Return false, so depot window doesnt start a file-open operation
	return FALSE;
}

int CJobListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CP4ListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_sFilter.Empty( );
	PersistentJobFilter( KEY_READ );

	return 0;
}


void CJobListCtrl::OnJobSetFileFilter() 
{
    if( !SERVER_BUSY() )
    {
	    ::SendMessage(m_depotWnd, WM_GETSELLIST, (WPARAM) &m_FilterView, 0);
		m_FilterIncIntegs = FALSE;
	    OnViewUpdate();
    }
}

void CJobListCtrl::OnJobSetFileFilterInteg() 
{
    if( !SERVER_BUSY() )
    {
	    ::SendMessage(m_depotWnd, WM_GETSELLIST, (WPARAM) &m_FilterView, 0);
		m_FilterIncIntegs = GET_P4REGPTR()->GetEnableSubChgIntegFilter();
	    OnViewUpdate();
    }
}

void CJobListCtrl::OnUpdateJobSetFileFilter(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SERVER_BUSY() 
			&& (pCmdUI->m_nID != ID_JOB_SETFILEFILTERINTEG 
			 || GET_P4REGPTR()->GetEnableSubChgIntegFilter())
			&& ::SendMessage(m_depotWnd, WM_GETSELCOUNT, 0, 0) > 0 );
}

void CJobListCtrl::OnUpdateJobRemoveFileFilter(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && !m_FilterView.IsEmpty());
}

void CJobListCtrl::OnJobRemoveFileFilter() 
{
	m_FilterView.RemoveAll(); 
	m_FilterIncIntegs = FALSE;
	OnViewUpdate( );
}

void CJobListCtrl::OnUpdateSetFilterJobs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( !SERVER_BUSY() && 
		// can do expression filtering
		(GET_SERVERLEVEL() > 3 ||	
		// or files selected for filter by files
		 ::SendMessage(m_depotWnd, WM_GETSELCOUNT, 0, 0) > 0 ));
}

void CJobListCtrl::OnUpdateClearFilterJobs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!SERVER_BUSY() && 
		(!m_FilterView.IsEmpty() || !m_sFilter.IsEmpty()));
}

void CJobListCtrl::OnPerforceOptions()
{
	MainFrame()->OnPerforceOptions(TRUE, FALSE, IDS_PAGE_JOB);
}
