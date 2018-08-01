//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// DeltaTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
//#define TRACE_HERE
#include "p4win.h"
#include "DeltaTreeCtrl.h"
#include "resource.h"
#include "MainFrm.h"
#ifdef _ALPHA
#include <winnetwk.h>
#endif // _ALPHA
#include "AddListDlg.h"
#include "RevertListDlg.h"
#include <shlobj.h>
#include "P4change.h"
#include "P4Fix.h"
#include "JobListDlg.h"
#include "AutoResolveDlg.h"
#include "TokenString.h"
#include "merge\GuiClientMerge.h"
#include "merge\Merge2Dlg.h"
#include "merge\Merge3Dlg.h"
#include "SpecDescDlg.h"
#include "FileType.h"
#include "MoveFiles.h"
#include "RemoveViewer.h"
#include "ResolveFlagsDlg.h"
#include "FileInfoDlg.h"
#include "P4SpecDlg.h"
#include "MsgBox.h"
#include "ImageList.h"
#include "OldChgFilterDlg.h"

#include "cmd_add.h"
#include "cmd_autoresolve.h"
#include "cmd_changes.h"
#include "cmd_delete.h"
#include "cmd_editspec.h"
#include "cmd_fstat.h"
#include "cmd_diff.h"
#include "cmd_get.h"
#include "cmd_jobs.h"
#include "cmd_fix.h"
#include "cmd_fixes.h"
#include "cmd_history.h"
#include "cmd_listopstat.h"
#include "cmd_ostat.h"
#include "cmd_opened.h"
#include "cmd_prepbrowse.h"
#include "cmd_resolve.h"
#include "cmd_resolved.h"
#include "cmd_revert.h"
#include "cmd_unresolved.h"
#include "cmd_where.h"
#include "strops.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_GOTDROPLIST (WM_USER+500)
#define WM_GOTMOVELISTS (WM_USER+501)
#define WM_OLEADDFILES (WM_USER+502)

#define CHANGELISTS_MINE 0
#define CHANGELISTS_OTHERS 1

///////////////////////
//  Note on use of lParam for tree nodes:
//	If its a file, lParam is a CP4FileStats ptr
//	Otherwise, lParam is NULL
//	DeleteItem() takes care of deleting CP4FileStats as reqd
//////////////////////


/////////////////////////////////////////////////////////////////////////////
// CDeltaTreeCtrl

IMPLEMENT_DYNCREATE(CDeltaTreeCtrl, CMultiSelTreeCtrl)

BEGIN_MESSAGE_MAP(CDeltaTreeCtrl, CMultiSelTreeCtrl)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_VSCROLL()
	ON_WM_SHOWWINDOW()
	ON_UPDATE_COMMAND_UI(ID_CHANGE_EDSPEC, OnUpdateChgEdspec)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_DEL, OnUpdateChgDel)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_NEW, OnUpdateChgNew)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_REVORIG, OnUpdateChgRevorig)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_SUBMIT, OnUpdateChgSubmit)
	ON_UPDATE_COMMAND_UI(ID_FILE_DIFFHEAD, OnUpdateFileDiffhead)
	ON_UPDATE_COMMAND_UI(ID_FILE_REVERT, OnUpdateFileRevert)
	ON_COMMAND(ID_CHANGE_DEL, OnChangeDel)
	ON_COMMAND(ID_CHANGE_EDSPEC, OnChangeEdspec)
	ON_COMMAND(ID_CHANGE_NEW, OnChangeNew)
	ON_COMMAND(ID_CHANGE_REVORIG, OnChangeRevorig)
	ON_COMMAND(ID_CHANGE_SUBMIT, OnChangeSubmit)
	ON_COMMAND(ID_FILE_DIFFHEAD, OnFileDiff)
	ON_COMMAND(ID_FILE_REVERT, OnFileRevert)
	ON_UPDATE_COMMAND_UI(ID_FILE_AUTORESOLVE, OnUpdateFileAutoresolve)
	ON_COMMAND(ID_FILE_AUTORESOLVE, OnFileAutoresolve)
	ON_UPDATE_COMMAND_UI(ID_FILE_RESOLVE, OnUpdateFileResolve)
	ON_COMMAND(ID_FILE_RESOLVE, OnFileResolve)
	ON_UPDATE_COMMAND_UI(ID_FILE_RUNMERGETOOL, OnUpdateFileResolve)
	ON_COMMAND(ID_FILE_RUNMERGETOOL, OnFileMerge)
	ON_UPDATE_COMMAND_UI(ID_THEIRFILE_FINDINDEPOT, OnUpdateTheirFile)
	ON_COMMAND(ID_THEIRFILE_FINDINDEPOT, OnTheirFindInDepot)
	ON_UPDATE_COMMAND_UI(ID_THEIRFILE_REVISIONHISTORY, OnUpdateTheirFile)
	ON_COMMAND(ID_THEIRFILE_REVISIONHISTORY, OnTheirHistory)
	ON_UPDATE_COMMAND_UI(ID_THEIRFILE_PROPERTIES, OnUpdateTheirFile)
	ON_COMMAND(ID_THEIRFILE_PROPERTIES, OnTheirProperties)
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_JOB_DESCRIBE, OnUpdateJobDescribe)
	ON_COMMAND(ID_JOB_DESCRIBE, OnJobDescribe)
	ON_UPDATE_COMMAND_UI(ID_JOB_EDITSPEC, OnUpdateJobEditspec)
	ON_COMMAND(ID_JOB_EDITSPEC, OnJobEditspec)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_REMOVEFIX, OnUpdateRemovefix)
	ON_COMMAND(ID_CHANGE_REMOVEFIX, OnRemovefix)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_ADDJOBFIX, OnUpdateAddjobfix)
	ON_COMMAND(ID_CHANGE_ADDJOBFIX, OnAddjobfix)
	ON_WM_DROPFILES()
	ON_UPDATE_COMMAND_UI(ID_FILE_AUTOEDIT, OnUpdateFileAutoedit)
	ON_UPDATE_COMMAND_UI(ID_FILE_QUICKBROWSE, OnUpdateFileAutobrowse)
	ON_COMMAND(ID_FILE_AUTOEDIT, OnFileAutoedit)
	ON_COMMAND(ID_FILE_QUICKEDIT, OnFileQuickedit)
	ON_COMMAND(ID_FILE_QUICKBROWSE, OnFileQuickbrowse)
	ON_WM_LBUTTONDBLCLK()
	ON_UPDATE_COMMAND_UI(ID_FILE_LOCK, OnUpdateFileLock)
	ON_COMMAND(ID_FILE_LOCK, OnFileLock)
	ON_UPDATE_COMMAND_UI(ID_FILE_UNLOCK, OnUpdateFileUnlock)
	ON_COMMAND(ID_FILE_UNLOCK, OnFileUnlock)
	ON_UPDATE_COMMAND_UI(ID_FILE_GETWHATIF, OnUpdateFileGet)
	ON_COMMAND(ID_FILE_GETWHATIF, OnFileGetWhatIf)
	ON_UPDATE_COMMAND_UI(ID_FILE_GET, OnUpdateFileGet)
	ON_COMMAND(ID_FILE_GET, OnFileGet)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYCLIENTPATH, OnUpdateEditCopyclientpath)
	ON_COMMAND(ID_EDIT_COPYCLIENTPATH, OnEditCopyclientpath)
	ON_UPDATE_COMMAND_UI(ID_FILE_REVISIONHISTORY, OnUpdateFileRevisionhistory)
	ON_COMMAND(ID_FILE_REVISIONHISTORY, OnFileRevisionhistory)
	ON_UPDATE_COMMAND_UI(ID_FILE_REVISIONTREE, OnUpdateFileRevisionhistory)
	ON_COMMAND(ID_FILE_REVISIONTREE, OnFileRevisionTree)
	ON_UPDATE_COMMAND_UI(ID_FILE_ANNOTATIONS, OnUpdateFileAnnotate)
	ON_COMMAND(ID_FILE_ANNOTATIONS, OnFileTimeLapseView)
	ON_UPDATE_COMMAND_UI(ID_FILE_PROPERTIES, OnUpdateFileInformation)
	ON_COMMAND(ID_FILE_PROPERTIES, OnFileInformation)
	ON_UPDATE_COMMAND_UI(ID_WINEXPLORE, OnUpdateWinExplore)
	ON_COMMAND(ID_WINEXPLORE, OnWinExplore)
	ON_UPDATE_COMMAND_UI(ID_CMDPROMPT, OnUpdateCmdPrompt)
	ON_COMMAND(ID_CMDPROMPT, OnCmdPrompt)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_DESCRIBE, OnUpdateChangeDescribe)
	ON_COMMAND(ID_CHANGE_DESCRIBE, OnChangeDescribe)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPENEDIT, OnUpdateFileOpenedit)
	ON_COMMAND(ID_FILE_OPENEDIT, OnFileOpenedit)
	ON_UPDATE_COMMAND_UI(ID_FILETYPE, OnUpdateFiletype)
	ON_COMMAND(ID_FILETYPE, OnFiletype)
	ON_UPDATE_COMMAND_UI(ID_FILE_MV2OTHERCHGLIST, OnUpdateMoveFiles)
	ON_COMMAND(ID_FILE_MV2OTHERCHGLIST, OnMoveFiles)
	ON_UPDATE_COMMAND_UI(ID_POSITIONDEPOT, OnUpdatePositionDepot)
	ON_COMMAND(ID_POSITIONDEPOT, OnPositionDepot)
	ON_UPDATE_COMMAND_UI(ID_POSITIONCHGS, OnUpdatePositionOtherChgs)
	ON_COMMAND(ID_POSITIONCHGS, OnPositionOtherChgs)
	ON_UPDATE_COMMAND_UI(ID_POSITIONTOPATTERN, OnUpdatePositionToPattern)
	ON_COMMAND(ID_POSITIONTOPATTERN, OnPositionToPattern)
	ON_UPDATE_COMMAND_UI(ID_FILE_SCHEDULE, OnUpdateFileSchedule)
	ON_COMMAND(ID_SORTCHGFILESBYNAME, OnSortChgFilesByName)
	ON_UPDATE_COMMAND_UI(ID_SORTCHGFILESBYNAME, OnUpdateSortChgFilesByName)
	ON_COMMAND(ID_SORTCHGFILESBYEXT, OnSortChgFilesByExt)
	ON_UPDATE_COMMAND_UI(ID_SORTCHGFILESBYEXT, OnUpdateSortChgFilesByExt)
	ON_COMMAND(ID_SORTCHGFILESBYACTION, OnSortChgFilesByAction)
	ON_UPDATE_COMMAND_UI(ID_SORTCHGFILESBYACTION, OnUpdateSortChgFilesByAction)
	ON_COMMAND(ID_SORTCHGFILESBYRESOLVE, OnSortChgFilesByResolve)
	ON_UPDATE_COMMAND_UI(ID_SORTCHGFILESBYRESOLVE, OnUpdateSortChgFilesByResolve)
	ON_COMMAND(ID_SORTCHGSBYUSER, OnSortChgsByUser)
	ON_UPDATE_COMMAND_UI(ID_SORTCHGSBYUSER, OnUpdateSortChgsByUser)
	ON_COMMAND(ID_FILE_SCHEDULE, OnFileGet)
	ON_UPDATE_COMMAND_UI(ID_FILE_QUICKEDIT, OnUpdateFileAutoedit)
	ON_UPDATE_COMMAND_UI(ID_USER_SWITCHTOUSER, OnUpdateUserSwitchtouser)
	ON_COMMAND(ID_USER_SWITCHTOUSER, OnUserSwitchtouser)
	ON_UPDATE_COMMAND_UI(ID_CLIENTSPEC_SWITCH, OnUpdateClientspecSwitch)
	ON_COMMAND(ID_CLIENTSPEC_SWITCH, OnClientspecSwitch)
	ON_UPDATE_COMMAND_UI(ID_FILE_RMVEDITOR, OnUpdateRemoveViewer)
	ON_COMMAND(ID_FILE_RMVEDITOR, OnRemoveViewer)
	ON_COMMAND_RANGE(ID_FILE_EDITOR_1, ID_FILE_EDITOR_1+MAX_MRU_VIEWERS-1, OnFileMRUEditor)
	ON_COMMAND_RANGE(ID_FILE_BROWSER_1, ID_FILE_BROWSER_1+MAX_MRU_VIEWERS-1, OnFileMRUBrowser)
	ON_COMMAND(ID_FILE_NEWEDITOR, OnFileNewEditor)
	ON_COMMAND(ID_FILE_NEWBROWSER, OnFileNewBrowser)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UPDATE_RIGHT, OnUpdateViewUpdate)
	ON_COMMAND(ID_VIEW_UPDATE_RIGHT, OnViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_SELECTFILES_CHANGED, OnUpdateSelectChanged)
	ON_COMMAND(ID_SELECTFILES_CHANGED, OnSelectChanged)
	ON_UPDATE_COMMAND_UI(ID_SELECTFILES_UNCHANGED, OnUpdateSelectChanged)
	ON_COMMAND(ID_SELECTFILES_UNCHANGED, OnSelectUnchanged)
	ON_COMMAND(ID_PERFORCE_OPTIONS, OnPerforceOptions)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SETVIEW, OnUpdateFilterSetview)
	ON_COMMAND(ID_FILTER_SETVIEW, OnFilterSetview)
	ON_UPDATE_COMMAND_UI(ID_FILTER_CLEARVIEW, OnUpdateFilterClearview)
	ON_COMMAND(ID_FILTER_CLEARVIEW, OnFilterClearview)
	ON_UPDATE_COMMAND_UI(ID_ADD_BOOKMARK, OnUpdateAddBookmark)
	ON_COMMAND(ID_ADD_BOOKMARK, OnAddBookmark)
	ON_UPDATE_COMMAND_UI(ID_FILE_ANNOTATE, OnUpdateFileAnnotate)
	ON_UPDATE_COMMAND_UI(ID_FILE_ANNOTATEALL, OnUpdateFileAnnotate)
	ON_UPDATE_COMMAND_UI(ID_FILE_ANNOTATECHG, OnUpdateFileAnnotate)
	ON_UPDATE_COMMAND_UI(ID_FILE_ANNOTATECHGALL, OnUpdateFileAnnotate)
	ON_COMMAND(ID_FILE_ANNOTATE, OnFileAnnotate)
	ON_COMMAND(ID_FILE_ANNOTATEALL, OnFileAnnotateAll)
	ON_COMMAND(ID_FILE_ANNOTATECHG, OnFileAnnotateChg)
	ON_COMMAND(ID_FILE_ANNOTATECHGALL, OnFileAnnotateChgAll)
	ON_MESSAGE(WM_P4ADD, OnP4Add )
	ON_MESSAGE(WM_P4AUTORESOLVE, OnP4AutoResolve )
	ON_MESSAGE(WM_P4MERGE2, OnP4Merge2 )
	ON_MESSAGE(WM_P4MERGE3, OnP4Merge3 )
	ON_MESSAGE(WM_P4RESOLVE, OnP4Resolve )
	ON_MESSAGE(WM_P4CHANGES, OnP4Change )
	ON_MESSAGE(WM_P4OSTAT, OnP4Ostat )
	ON_MESSAGE(WM_P4EDITSPEC, OnP4ChangeSpec )
	ON_MESSAGE(WM_P4ENDSPECEDIT, OnP4EndSpecEdit )
	ON_MESSAGE(WM_P4DELETE, OnP4ChangeDel )
	ON_MESSAGE(WM_P4DIFF, OnP4Diff )
	ON_MESSAGE(WM_P4LISTOPSTAT, OnP4ListOp )
	ON_MESSAGE(WM_P4GET, OnP4SyncAndEdit )
	ON_MESSAGE(WM_P4FIXES, OnP4Fixes )
	ON_MESSAGE(WM_P4FIX, OnP4Fix )
	ON_MESSAGE(WM_P4DESCRIBE, OnP4Describe )
	ON_MESSAGE(WM_P4ENDDESCRIBE, OnP4EndDescribe )
	ON_MESSAGE(WM_P4UNRESOLVED, OnP4UnResolved )
	ON_MESSAGE(WM_P4RESOLVED, OnP4Resolved )
    ON_MESSAGE(WM_P4JOBS, OnP4JobList )
    ON_MESSAGE(WM_JOBDELETED, OnP4JobDel )
	ON_MESSAGE(WM_UPDATEOPEN, OnP4UpdateOpen )
	ON_MESSAGE(WM_REVERTLIST, OnP4UpdateRevert )
	ON_MESSAGE(WM_SETUNRESOLVED, OnP4SetUnresolved )
	ON_MESSAGE(WM_GOTMOVELISTS, OnGotMoveLists )
	ON_MESSAGE(WM_GETDRAGTOCHANGENUM, OnGetDragToChangeNum )
	ON_MESSAGE(WM_INITTREE, OnInitTree )
	ON_MESSAGE(WM_OLEADDFILES, OnOLEAddFiles )
	ON_MESSAGE(WM_GETMYCHANGESLIST, OnGetMyChangesList )
	ON_MESSAGE(WM_P4FILEREVERT, OnP4FileRevert )
	ON_MESSAGE(WM_P4REVERT, OnP4Revert )
	ON_MESSAGE(WM_P4FILEINFORMATION, OnP4FileInformation )
	ON_MESSAGE(WM_P4ENDFILEINFORMATION, OnP4EndFileInformation )
	ON_MESSAGE(WM_P4DIFFCHANGEEDIT, OnP4DiffChangeEdit )
	ON_MESSAGE(WM_THEIRFINDINDEPOT, OnP4TheirFindInDepot )
	ON_MESSAGE(WM_THEIRHISTORY, OnP4TheirHistory )
	ON_MESSAGE(WM_THEIRPROPERTIES, OnP4TheirProperties )
	ON_MESSAGE(WM_SUBCHGOUFC, CallOnUpdateFilterClearview )
	ON_MESSAGE( WM_ACTIVATEMODELESS, OnActivateModeless )
END_MESSAGE_MAP()

CDeltaTreeCtrl::CDeltaTreeCtrl()
{
	m_OLESource.SetTreeCtrl(this);
	m_ItemCount=0;

	m_CF_DELTA = RegisterClipboardFormat(LoadStringResource(IDS_DRAGFROMDELTA));
	m_CF_DEPOT = RegisterClipboardFormat(LoadStringResource(IDS_DRAGFROMDEPOT));
	m_CF_JOB   = RegisterClipboardFormat(LoadStringResource(IDS_DRAGFROMJOB));
	m_MyRootExpanded= m_OthersRootExpanded= FALSE;
	m_SortByFilename  = GET_P4REGPTR()->SortChgFilesByName();
	m_SortByExtension = GET_P4REGPTR()->SortChgFilesByExt();
	m_SortByAction    = GET_P4REGPTR()->SortChgFilesByAction();
	m_SortByResolveStat = GET_P4REGPTR()->SortChgFilesByResolve();

    // Initialize tree state info
    m_FirstVisibleNodeGroup= CHANGELISTS_MINE;

	m_MyRoot= m_OthersRoot=NULL;

	m_ContextPoint.x = m_ContextPoint.y = -1;
	m_InContextMenu = FALSE;
	m_Need2Edit = m_Need2Refresh = m_EditInProgress = FALSE;
	m_DeltaIsDropTarget = FALSE;
	m_LastDragDropTime  = 0;
	m_DragDropCtr = 0;
	m_NewChgNbr = m_DragToChangeNum = 0;
	m_DragToChange = 0;
	m_PositionTo = _T("");
	m_caption = LoadStringResource(IDS_PENDING_PERFORCE_CHANGELISTS);
	m_RedoExpansion = FALSE;
	if (GET_P4REGPTR()->ExpandChgLists())
	{
		m_PrevExpansion = GET_P4REGPTR()->GetPendChgExpansion();
		if (!m_PrevExpansion.IsEmpty() && m_PrevExpansion.GetAt(0) == _T('1'))
			m_RedoExpansion = TRUE;
	}
}

CDeltaTreeCtrl::~CDeltaTreeCtrl()
{
}

void CDeltaTreeCtrl::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (m_EditInProgress && (bShow || GET_P4REGPTR()->AutoMinEditDlg()))
		m_EditInProgressWnd->ShowWindow(bShow ? SW_RESTORE : SW_SHOWMINNOACTIVE);
}

LRESULT CDeltaTreeCtrl::OnActivateModeless(WPARAM wParam, LPARAM lParam)
{
	if (m_EditInProgress && wParam == WA_ACTIVE)
		::SetFocus(m_EditInProgressWnd->m_hWnd);
	return 0;
}

void CDeltaTreeCtrl::SaveExpansion()
{
	if (GET_P4REGPTR()->ExpandChgLists())
	{
		CString	exp;
		TV_ITEM tvItem;
		tvItem.hItem = m_MyRoot;
		tvItem.stateMask = TVIS_EXPANDED;
		tvItem.mask = TVIF_STATE;
		TreeView_GetItem(m_hWnd, &tvItem);
		if(tvItem.state & TVIS_EXPANDED)
		{
			exp = _T("1");
			HTREEITEM item = GetChildItem(m_MyRoot);
			CString chglit = LoadStringResource(IDS_CHANGE);
			int chglgth = chglit.GetLength();
			while(item != NULL)
			{
				tvItem.hItem = item;
				tvItem.stateMask = TVIS_EXPANDED;
				tvItem.mask = TVIF_STATE;
				TreeView_GetItem(m_hWnd, &tvItem);
				if(tvItem.state & TVIS_EXPANDED)
				{
					int	i;
					CString txt = GetItemText(item);
					txt.TrimLeft();
					if ((i = txt.Find(chglit)) != -1)
					{
						txt = txt.Mid(i + chglgth);
						txt.TrimLeft();
						if ((i = txt.Find(_T(' '), 1)) != -1)
							txt = txt.Left(i);
					}
					exp += _T(",") + txt;
				}
				item = GetNextSiblingItem(item);
			}
		}
		else
			exp = _T("0");
		GET_P4REGPTR()->SetPendChgExpansion(exp);
	}
}


BOOL CDeltaTreeCtrl::IsAFile(HTREEITEM curr_item)
{
#ifdef _DEBUG
	// Caller should already have checked that tree level is correct
	BOOL underMyRoot;
	ASSERT(GetItemLevel(curr_item, &underMyRoot) == 2);
#endif

	if(GetLParam(curr_item) == NULL)
		return(FALSE);
	else
		return(TRUE);
}

UINT CDeltaTreeCtrl::GetItemState(HTREEITEM curr_item)
{
	TV_ITEM item;
	item.hItem=curr_item;
	item.mask=TVIF_STATE | TVIF_HANDLE;
	GetItem(&item);	
	return(item.state);
}

void CDeltaTreeCtrl::SetUnexpanded(HTREEITEM curr_item)
{
	TV_ITEM tvItem;
	tvItem.hItem=curr_item;
    tvItem.stateMask= TVIS_EXPANDED ;
	tvItem.state=0;
	tvItem.mask=TVIF_STATE | TVIF_HANDLE;
	SetItem(&tvItem);	
}

BOOL CDeltaTreeCtrl::HasChildren(HTREEITEM curr_item)
{
	TV_ITEM item;
	item.hItem=curr_item;
	item.mask=TVIF_CHILDREN | TVIF_HANDLE;
	GetItem(&item);	
	if(item.cChildren > 0 )
		return TRUE;
	else
		return FALSE;
}



/////////////////////////////////////////////////////////////////////////////
// CDeltaTreeCtrl diagnostics

#ifdef _DEBUG
void CDeltaTreeCtrl::AssertValid() const
{
	CMultiSelTreeCtrl::AssertValid();
}

void CDeltaTreeCtrl::Dump(CDumpContext& dc) const
{
	CMultiSelTreeCtrl::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDeltaTreeCtrl message handlers

void CDeltaTreeCtrl::Clear()
{
	InitList();
}

LRESULT CDeltaTreeCtrl::OnInitTree(WPARAM wParam, LPARAM lParam)
{
	InitList();
	return 0;
}

void CDeltaTreeCtrl::DeleteItem(HTREEITEM item)
{
	ASSERT(item != NULL);

	LPARAM lParam=GetLParam(item);
	if(lParam > 0)
		delete (CP4FileStats *) lParam;
	
	CMultiSelTreeCtrl::DeleteItem(item);
}

void CDeltaTreeCtrl::DeleteLParams(HTREEITEM root)
{
	if(root==NULL)
		return;

	HTREEITEM change= GetChildItem(root);
	HTREEITEM file;
	LPARAM lParam;

	while(change != NULL)
	{
		file= GetChildItem(change);	
		while(file != NULL)
		{
			lParam=GetLParam(file);
			if(lParam > 0)
            {
                ASSERT_KINDOF(CP4FileStats, (CP4FileStats *) lParam);
                XTRACE(_T("CDeltaTreeCtrl::DeleteLParams: %s\n"), ((CP4FileStats *) lParam)->GetDepotFilename());
				delete (CP4FileStats *) lParam;
            }

			file=GetNextSiblingItem(file);
		}
		change=GetNextSiblingItem(change);
	}
}

void CDeltaTreeCtrl::InitList()
{
	UpdateTreeState(TRUE);	// saves the current expansion

	// Traverse tree, deleting each item's lParam
	DeleteLParams(m_MyRoot);
	if (m_OthersRoot)
	{
		DeleteLParams(m_OthersRoot);
		m_OthersRoot = NULL;
	}

    SetRedraw(FALSE);

    // Then delete all tree items and replace root level entries
	DeleteAllItems();
	CString rootName;
	rootName.FormatMessage(IDS_PENDING_CHANGELISTS_MY_CLIENT_s, GET_P4REGPTR()->GetP4Client());
	m_MyRoot=Insert(rootName, CP4ViewImageList::VI_YOURPENDING, EXPAND_FOLDER, TVI_ROOT, TRUE);
	m_MyDefault=Insert(LoadStringResource(IDS_DEFAULTCHANGELISTNAME), CP4ViewImageList::VI_YOURCHANGE, 0, m_MyRoot, TRUE);
	if (!m_DragToChangeNum)
		m_DragToChange=m_MyDefault;

	if ( GET_P4REGPTR()->GetEnablePendingChgsOtherClients( ) )
	{
		CString txt;
		int i = GET_P4REGPTR()->FilterPendChgsByMyClient();
		if (GET_SERVERLEVEL() < 21 && i > 1)
		{
			OnFilterClearview();
			i = 0;
		}
		switch(i)
		{
		case 1:
			txt.FormatMessage(IDS_PENDING_CHANGELISTS_OTHER_CLIENTS_FILTERED, 
						CString(_T("//")) + GET_P4REGPTR()->GetP4Client() + CString(_T("/...")));
			break;
		case 2:
			txt.FormatMessage(IDS_PENDING_CHANGELISTS_OTHER_CLIENTS_FILTERED, 
						GET_P4REGPTR()->FilterPendChgsByPath());
			break;
		default:
			txt = LoadStringResource(IDS_PENDING_CHANGELISTS_OTHER_CLIENTS);
			break;
		}
		// Initialize others root as already expanded so expand attempts during refresh are ignored
		// in cases where that refresh will already be fetching the needed ifo (running opened -a)
		// The final lParam and child count values are handled after the refresh in UpdateTreeState
		if( m_OthersRootExpanded )
			m_OthersRoot=Insert(txt, CP4ViewImageList::VI_THEIRPENDING, FOLDER_ALREADY_EXPANDED, TVI_ROOT, TRUE);
		else
			m_OthersRoot=Insert(txt, CP4ViewImageList::VI_THEIRPENDING, EXPAND_FOLDER, TVI_ROOT, TRUE);
	}
    SetRedraw(TRUE);

	// Select nothing, so there is no focus rect
	SelectItem(NULL);	

	// Clear flag for expanding others pending changelist root
	m_ExpandingOthersRoot= FALSE;
}


// During a tree refresh, the expanded states of items are lost, so record
// this info prior to the refresh and apply it to the view after the refresh
void CDeltaTreeCtrl::UpdateTreeState(BOOL saveTreeState)
{
	HTREEITEM item;

	if(saveTreeState)
	{
		m_MyRootExpanded= m_OthersRootExpanded= FALSE;
		m_ExpandedItems.RemoveAll();
		m_SelectedItems.RemoveAll();

		// First save selected items
		for(int i=-1; ++i < GetSelectedCount(); )
		{
			int j;
			HTREEITEM item= GetSelectedItem(i);
			CString txt = GetItemText(item);
			if (txt.GetAt(0) == _T('/') && ((j = txt.ReverseFind(_T('#'))) != -1))
				txt = txt.Left(j);
			m_SelectedItems.AddTail(txt);
		}

		// Now save the tree expansion
		if(GetCount() > 0)
		{
			// First, save expanded state of root items
			if(HasExpandedChildren(m_MyRoot))
				m_MyRootExpanded= TRUE;

			if (m_OthersRoot)
			{
				if(HasExpandedChildren(m_OthersRoot))
					m_OthersRootExpanded= TRUE;
			}

			// Then save expaned state of my changes
			item= GetChildItem(m_MyRoot);
			while(item != NULL)
			{
				if(HasExpandedChildren(item))
					m_ExpandedItems.AddHead(GetItemText(item));
				item= GetNextSiblingItem(item);
			}

			if (m_OthersRoot)
			{
				// Then save expanded state of other's changes
				item= GetChildItem(m_OthersRoot);
				while(item != NULL)
				{
					if(HasExpandedChildren(item))
						m_ExpandedItems.AddHead(GetItemText(item));
					item= GetNextSiblingItem(item);
				}
			}
		}

        // Record the first visible item
        m_FirstVisibleNodeGroup= CHANGELISTS_MINE;
        m_FirstVisibleNodeChange.Empty();
        m_FirstVisibleNodeFile.Empty();

        CString nodeText;
        HTREEITEM item= GetFirstVisibleItem();
   
        if( item != NULL )
        {
            BOOL underMyRoot=FALSE;
	        int level=GetItemLevel(item, &underMyRoot);
	        
            // If its a file, record filename minus rev and then set the item to parent
            if( level == 2 )
            {
                if( IsAFile( item ) )
                {
                    nodeText= GetItemText( item ); 

                    int pound= nodeText.ReverseFind(_T('#'));
                    if( pound != -1 )
                        m_FirstVisibleNodeFile= nodeText.Left(pound);

                }
                item=GetParentItem(item);
                level--;
            }

            // If item is a change, record change name and then set item to parent
            ASSERT( item != NULL && item != TVI_ROOT );
            if( level == 1 && item != NULL )
            {
                nodeText= GetItemText( item ); 
                if( nodeText.Find(LoadStringResource(IDS_DEFAULTCHANGELISTNAME)) == 0 )
                    m_FirstVisibleNodeChange= nodeText;
                else if( underMyRoot )
                {
                    m_FirstVisibleNodeChange= nodeText;
                    int comment= m_FirstVisibleNodeChange.Find(_T("{"));
                    if( comment != -1 )
                        m_FirstVisibleNodeChange= m_FirstVisibleNodeChange.Left(comment);
                }
                else
                {
                    int separator= nodeText.Find(_T(" - "));
                    if( separator != -1 )
                        m_FirstVisibleNodeChange= nodeText.Left(separator);
                }

                item=GetParentItem(item);
                level--;
            }

            // If item not null, record the node group
            ASSERT( item != NULL && item != TVI_ROOT );
            if( item != m_MyRoot )
                m_FirstVisibleNodeGroup= CHANGELISTS_OTHERS;
        }
    }
	else
	{
		if(m_MyRootExpanded)
			Expand(m_MyRoot, TVE_EXPAND);

		if (m_OthersRoot)
		{
			if(m_OthersRootExpanded && GetChildCount( m_OthersRoot ) )
				Expand(m_OthersRoot, TVE_EXPAND);
			else
			{
				// Make it ready for an attempted expand operation
				m_OthersRootExpanded= FALSE;
				SetLParam( m_OthersRoot, EXPAND_FOLDER );
				SetChildCount( m_OthersRoot, 1 );
			}
		}

		POSITION pos= m_ExpandedItems.GetHeadPosition();
		
		for( int i=0; i< m_ExpandedItems.GetCount(); i++)
		{
			item= FindItemByText(m_ExpandedItems.GetNext(pos));
			if(item != NULL)
				Expand(item, TVE_EXPAND);
		}

        // Attempt to scroll previous first item back into view, or at least
        // scroll the parent of that item back to the top of screen
        //
        // First set item to recorded node group
        if( m_FirstVisibleNodeGroup == CHANGELISTS_MINE )
            item= m_MyRoot;
        else
            item= m_OthersRoot;

        HTREEITEM changeItem= NULL;
        CString testtext;

        // Then try to set item to recorded change, if any
        if(!m_FirstVisibleNodeChange.IsEmpty() )
        {
            int testlen=m_FirstVisibleNodeChange.GetLength();
            changeItem=GetChildItem(item);
            
	        while(changeItem !=NULL)
	        {
		        testtext=GetItemText(changeItem);
                if(m_FirstVisibleNodeChange.Compare(testtext.Left(testlen)) == 0)
			        break;
			    
		        changeItem=GetNextSiblingItem(changeItem);
	        }

            if( changeItem != NULL )
                item= changeItem;
        }

        // Finally try to set item to recorded file, if any
        if(changeItem != NULL && !m_FirstVisibleNodeFile.IsEmpty() )
        {
            int testlen=m_FirstVisibleNodeFile.GetLength();
            HTREEITEM fileItem=GetChildItem(changeItem);

            while(fileItem !=NULL)
	        {
		        testtext=GetItemText(fileItem);
                if(m_FirstVisibleNodeFile.Compare(testtext.Left(testlen)) == 0 && testtext[testlen]==_T('#') )
			        break;
			    
		        fileItem=GetNextSiblingItem(fileItem);
	        }
            
            if( fileItem != NULL )
                item= fileItem;
        }

        // Then scroll the tree into position
        if( item != NULL )
            ScrollToFirstItem( item );

		// Now reselect any previously selected items that are still in my changelists
		if (!m_SelectedItems.IsEmpty())
		{
			pos= m_SelectedItems.GetHeadPosition();
			
			item = NULL;
			for( int i=0; i< m_SelectedItems.GetCount(); i++)
			{
				CString txt = m_SelectedItems.GetNext(pos);
				if (txt.GetAt(0) == _T('/'))
					item= FindMyOpenFile(txt, item);
				else
					item= FindItemByText(txt);
				if(item != NULL)
					SetSelectState(item, TRUE);
			}
			m_SelectedItems.RemoveAll();
			ApplySelectAtts(GetSelectAtts());
		}
	}
}

// A message handler to get target change number without having to
// include CDeltaTreeCtrl.h
LRESULT CDeltaTreeCtrl::OnGetDragToChangeNum(WPARAM wParam, LPARAM lParam)
{
	CPoint *point= (CPoint *) wParam;

	point->x= m_DragToPoint.x;
	point->y= m_DragToPoint.y;

	XTRACE(_T("OnGetDragToChangeNum change=%d\n"), m_DragToChangeNum);
	return (LRESULT) m_DragToChangeNum;
}

// Note: this handler is a stripped down version of OnP4Ostat
//       - doesnt need to run 'P4 fixes'
//       - doesnt need to forward info to Depot window
//       - doesnt need to consider unresolved files
LRESULT CDeltaTreeCtrl::OnP4Add(WPARAM wParam, LPARAM lParam)
{
	BOOL bSorted=FALSE;
	BOOL chainedCommands=FALSE;
	CP4FileStats *stats;
	CString text;
	POSITION pos;
	
	CCmd_Add *pCmd= (CCmd_Add *) wParam;
	ASSERT_KINDOF(CCmd_Add, pCmd);

	if( !pCmd->GetError() )
	{
		text.FormatMessage(IDS_ADDED_n_FILES, pCmd->GetAddedFileCount());
		AddToStatus(text, SV_COMPLETION);
	}
	if ( !pCmd->GetError() && pCmd->GetOpenAction() && !pCmd->GetStr2Edit().IsEmpty() )
	{
		int key= pCmd->GetServerKey();
		chainedCommands= TRUE;

		m_StringList.RemoveAll();

		pos= pCmd->GetStr2Edit().GetHeadPosition();
		while( pos != NULL )
		{
			// Get the filenames to open for edit
			CString txt = pCmd->GetStr2Edit().GetNext(pos);
			m_StringList.AddTail(txt);
		}

		CCmd_ListOpStat *pCmd2= new CCmd_ListOpStat;
		pCmd2->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key );
		pCmd2->SetHitMaxFileSeeks(TRUE);	// we will need to do a full refresh at the end
		pCmd2->SetChkForSyncs(TRUE);
		CObList const * pList = pCmd->GetList();
		if (pList->GetCount())
		{
			for( pos= pList->GetHeadPosition(); pos!= NULL; )
				pCmd2->Add2RevertList( pList->GetNext(pos), pCmd->GetOpenAction() );
		}
		if( pCmd2->Run( &m_StringList, P4EDIT, pCmd->GetChangeNum() ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_OPENING_FILES_FOR_EDIT) );
		else
			delete pCmd2;
		// delete the stats in the pCmd's main list
		// because we are now done with them
		CObList const *list= pCmd->GetList();
		if (!list->IsEmpty())
		{
			for( pos= list->GetHeadPosition(); pos!= NULL; )
				delete (CP4FileStats *) list->GetNext(pos);
		}
	}
	else if ( !pCmd->GetError() && (pCmd->GetOpenAction() == 1) && !pCmd->GetList()->IsEmpty() )
	{	// user requested Edit only, but nothing to edit - must reverts any adds
		int key= pCmd->GetServerKey();
		chainedCommands= TRUE;

		m_StringList.RemoveAll();

		CObList const * pList = pCmd->GetList();
		for( pos= pList->GetHeadPosition(); pos!= NULL; )
		{
			CP4FileStats *stats= (CP4FileStats *)(pList->GetNext(pos));
			CString name=stats->GetFullDepotPath();
			m_StringList.AddTail(name);
		}

		CCmd_ListOpStat *pCmd2= new CCmd_ListOpStat;
		pCmd2->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key );
		pCmd2->SetHitMaxFileSeeks(TRUE);	// we will need to do a full refresh at the end
		if( pCmd2->Run( &m_StringList, P4REVERT ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_REVERT) );
		else
			delete pCmd2;
	}
	else if (!pCmd->GetError() 
		  && (pCmd->HitMaxFileSeeks() 
		   || (GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALTREE)))
	{
		// Too much was added for seeking out each file for an att update
		// to be efficient (or we need to refresh the local view).
		// Just start a full update.
		int key= pCmd->GetServerKey();
		chainedCommands= TRUE;
		MainFrame()->UpdateDepotandChangeViews(REDRILL, key);
		// Clean up any 'stats' in the pCmd's list because this pCmd is soon deleted
		CObList const * pList = pCmd->GetList();
		for( pos= pList->GetHeadPosition(); pos!= NULL; )
		{
			CP4FileStats *stats= (CP4FileStats *)(pList->GetNext(pos));
			delete stats;
		}
	}
	else if( !pCmd->GetError() )
	{
		// Get the filelist
		CObList const *list= pCmd->GetList();
		HTREEITEM currentItem = NULL;

        SetRedraw(FALSE);
		if(list->GetCount() > 0)
		{
			// then get the list contents into the tree
			POSITION pos= list->GetHeadPosition();
			while( pos != NULL )
			{
				// Get the cursed filename
				stats= (CP4FileStats *) list->GetNext(pos);

				// Find the change this file is under.  Create change if reqd.
				currentItem=InsertChange(stats, TRUE);
				if(currentItem!=NULL)
				{
					Insert(stats->GetFormattedChangeFile(GET_P4REGPTR()->ShowFileType(), GET_P4REGPTR()->ShowOpenAction()),
								TheApp()->GetFileImageIndex(stats,TRUE), (LPARAM) stats, currentItem, FALSE);
				}
			} //while
		} // if
		if( currentItem != NULL && GetChildItem(currentItem) != NULL )
		{
			SetChildCount(currentItem, 1);
			if (GET_P4REGPTR()->ExpandChgLists( ))
				Expand(currentItem, TVE_EXPAND);
		}
        SetRedraw(TRUE);

		SortTree();
		bSorted = TRUE;
		RedrawWindow();
		MainFrame()->SetLastUpdateTime(UPDATE_SUCCESS);
		MainFrame()->ClearStatus();
	}
	else
		MainFrame()->ClearStatus();

	if( !chainedCommands || MainFrame()->IsQuitting() )
	{
		pCmd->ReleaseServerLock();
		if (!bSorted && (m_SortByExtension || m_SortByResolveStat 
			          || m_SortByAction    || m_SortByFilename))
			SortTree();
	}
    delete pCmd;

	return 0;
}


LRESULT CDeltaTreeCtrl::OnP4AutoResolve(WPARAM wParam, LPARAM lParam)
{
	CCmd_AutoResolve *pCmd= (CCmd_AutoResolve *) wParam;

	if( !pCmd->GetError() && !MainFrame()->IsQuitting() )
	{
		CStringList *list= pCmd->GetList();
	
		CString temp;
		if(pCmd->IsPreview())
			temp.FormatMessage(IDS_n_FILES_WOULD_BE_RESOLVED, list->GetCount());
		else
			temp.FormatMessage(IDS_n_FILES_RESOLVED, list->GetCount());
		
		AddToStatus(temp, SV_COMPLETION);

		if(!pCmd->IsPreview())
		{
			// 1) Hold onto the key 
			// 2) Clear all unresolved attributes
			// 3) Run resolved -n with the key
			int key= pCmd->GetServerKey();
			ClearUnresolvedFlags();
			CCmd_Unresolved *pCmd2= new CCmd_Unresolved;
			pCmd2->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key );

			if( !pCmd2->Run() )
			{
				ASSERT(0);
				delete pCmd2;
			}
		}
		else
			pCmd->ReleaseServerLock();
	}
	else
		pCmd->ReleaseServerLock();
		
	MainFrame()->ClearStatus();
	delete pCmd;
	return 0;
}


void CDeltaTreeCtrl::SetCorrectChglistImage(HTREEITEM change)
{
	HTREEITEM file= GetChildItem(change);	
	while(file != NULL)
	{
		LPARAM lParam=GetLParam(file);
		if(lParam > 0)
        {
			CP4FileStats *stats= (CP4FileStats *) lParam;
            ASSERT_KINDOF(CP4FileStats, stats);
            if (stats->IsUnresolved())
			{
				int ix = stats->IsOtherUserMyClient() ? CP4ViewImageList::VI_YOUROTHERCHGUNRES 
													  : CP4ViewImageList::VI_YOURCHGUNRES;
				SetImage(change, ix, ix);
				return;
			}
		}
		file=GetNextSiblingItem(file);
	}
	SetImage(change, CP4ViewImageList::VI_YOURCHANGE, CP4ViewImageList::VI_YOURCHANGE);
}

void CDeltaTreeCtrl::ClearUnresolvedFlags( )
{
	HTREEITEM change= GetChildItem(m_MyRoot);
	HTREEITEM file;
	LPARAM lParam;

	while(change != NULL)
	{
		SetImage(change, CP4ViewImageList::VI_YOURCHANGE, CP4ViewImageList::VI_YOURCHANGE);
		file= GetChildItem(change);	
		while(file != NULL)
		{
			lParam=GetLParam(file);
			if(lParam > 0)
            {
				CP4FileStats *stats= (CP4FileStats *) lParam;
                ASSERT_KINDOF(CP4FileStats, stats);
                stats->SetUnresolved(FALSE);
				int img=TheApp()->GetFileImageIndex(stats,TRUE);
				SetImage(file, img, img);
            }

			file=GetNextSiblingItem(file);
		}
		change=GetNextSiblingItem(change);
	}
}


LRESULT CDeltaTreeCtrl::OnP4Merge2(WPARAM wParam, LPARAM lParam)
{
	MainFrame()->ClearStatus();

	if(wParam !=0)
	{
		MainFrame()->DoNotAutoPoll();
		CGuiClientMerge *merge= (CGuiClientMerge *) wParam;
		CMerge2Dlg dlg;
		dlg.SetKey(lParam);
		dlg.SetMergeInfo(merge);
		if (dlg.DoModal() == IDC_CANCEL_ALL)
			m_ResolveList.RemoveAll();
		merge->Signal();
		MainFrame()->ResumeAutoPoll();
	}
	
	return 0;
}

LRESULT CDeltaTreeCtrl::OnP4Merge3(WPARAM wParam, LPARAM lParam)
{
	MainFrame()->ClearStatus();

	if(wParam !=0)
	{
		MainFrame()->DoNotAutoPoll();
		CGuiClientMerge *merge= (CGuiClientMerge *) wParam;
		CMerge3Dlg dlg;
		dlg.SetMergeInfo(merge);
		dlg.SetForceFlag(m_ForcedResolve);
		dlg.SetTextualFlag(m_TextualMerge);
		dlg.SetRunMerge(m_bRunMerge);
		dlg.SetKey(lParam);
		if (dlg.DoModal() == IDC_CANCEL_ALL)
			m_ResolveList.RemoveAll();
		merge->Signal();
		// can't use MainFrame()-> construct
		// because mainfram might have closed.
		CMainFrame * mainWnd = MainFrame();
		if (mainWnd)
			mainWnd->ResumeAutoPoll();
	}
	
	return 0;
}


LRESULT CDeltaTreeCtrl::OnP4Resolve(WPARAM wParam, LPARAM lParam)
{
	CCmd_Resolve *pCmd= (CCmd_Resolve *) wParam;

	if(!pCmd->GetError())
	{
		if(pCmd->GetResolved())
		{
			CP4FileStats *stats= (CP4FileStats *) GetLParam(m_ActiveItem);
			ASSERT_KINDOF(CP4FileStats, stats);

			stats->SetUnresolved(FALSE);
			stats->SetResolved(TRUE);
			int img=TheApp()->GetFileImageIndex(stats,TRUE);
			SetImage(m_ActiveItem, img, img);
			SetCorrectChglistImage(TreeView_GetParent(m_hWnd, m_ActiveItem));
		}
	}
	
	delete pCmd;
	MainFrame()->ClearStatus();

	// if there are more items to resolve, fire up a resolve on the next one
	if (!m_ResolveList.IsEmpty())
	{
		HTREEITEM item = (HTREEITEM)(m_ResolveList.RemoveHead());
		ResolveItem(item);
	}
	else if (m_SortByExtension || m_SortByResolveStat || m_SortByAction || m_SortByFilename)
		SortTree();
	return 0;
}

LRESULT CDeltaTreeCtrl::OnP4UnResolved(WPARAM wParam, LPARAM lParam)
{
	CCmd_Unresolved *pCmd= (CCmd_Unresolved *) wParam;

	if(!pCmd->GetError() && !MainFrame()->IsQuitting())
	{
		SET_BUSYCURSOR();
		HTREEITEM item = NULL;
		CObArray const *pArray= pCmd->GetArray();
		for( int i=0; i < pArray->GetSize(); i++ )
		{
			CP4FileStats *stats= (CP4FileStats *) pArray->GetAt(i);

			item= FindMyOpenFile( stats->GetFullDepotPath(), item );

			if( item != NULL )
			{
				int lParam= GetLParam(item);
				if( lParam > 0 )
				{
					CP4FileStats *fs= (CP4FileStats *) lParam;
					fs->SetUnresolved(TRUE);
					int img=TheApp()->GetFileImageIndex(fs,TRUE);
					SetImage(item, img, img);
					int ix = fs->IsOtherUserMyClient() ? CP4ViewImageList::VI_YOUROTHERCHGUNRES 
													   : CP4ViewImageList::VI_YOURCHGUNRES;
					SetImage(TreeView_GetParent(m_hWnd, item), ix, ix);
				}
				else
					ASSERT(0);
			}

			delete stats;
		}
		int key= pCmd->GetServerKey();
		CCmd_Resolved *pCmd2= new CCmd_Resolved;
		pCmd2->Init( m_hWnd, RUN_ASYNC, LOSE_LOCK, key );
		if( !pCmd2->Run() )
		{
			ASSERT(0);
			delete pCmd2;
			::SetCursor(::LoadCursor(NULL, IDC_ARROW));
		}
	}
	else pCmd->ReleaseServerLock();
	
	delete pCmd;
	MainFrame()->ClearStatus();
	return 0;
}

LRESULT CDeltaTreeCtrl::OnP4Resolved(WPARAM wParam, LPARAM lParam)
{
	CCmd_Resolved *pCmd= (CCmd_Resolved *) wParam;

	if(!pCmd->GetError())
	{
		SET_BUSYCURSOR();
		HTREEITEM item = NULL;
		CObArray const *pArray= pCmd->GetArray();
		for( int i=0; i < pArray->GetSize(); i++ )
		{
			CP4FileStats *stats= (CP4FileStats *) pArray->GetAt(i);

			item= FindMyOpenFile( stats->GetFullDepotPath(), item );

			if( item != NULL )
			{
				int lParam= GetLParam(item);
				if( lParam > 0 )
				{
					CP4FileStats *fs= (CP4FileStats *) lParam;
					fs->SetResolved(TRUE);
				}
				else
					ASSERT(0);
			}

			delete stats;
		}
		if (m_SortByExtension || m_SortByResolveStat || m_SortByAction || m_SortByFilename)
			SortTree();
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	}
	
	delete pCmd;
	MainFrame()->ClearStatus();
	return 0;
}

LRESULT CDeltaTreeCtrl::OnP4Diff(WPARAM wParam, LPARAM lParam)
{
	CCmd_Diff *pCmd= (CCmd_Diff *) wParam;
    BOOL chainedCommands= FALSE;
    
	if(!pCmd->GetError())
	{
		CStringList *list= pCmd->GetList();
	
		if(m_DoRevert)
		{
			chainedCommands = DoRevert(list, pCmd->GetServerKey());
        }
		else // not m_DoRevert
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
	}
    
    if( !chainedCommands || MainFrame()->IsQuitting() )
    {
        // Error, so make sure server lock is released
        if(pCmd->HaveServerLock())
            pCmd->ReleaseServerLock();
    	MainFrame()->ClearStatus();
    }
	delete pCmd;
	return 0;
}

BOOL CDeltaTreeCtrl::DoRevert(CStringList *list, int key/*=0*/, BOOL bUnChg/*=FALSE*/)
{
    BOOL chainedCommands= FALSE;
	POSITION pos;

	if(!list->IsEmpty())  // Some filenames in the list
	{
		HTREEITEM currItem=GetLastSelection();  
		CRevertListDlg listDlg;
		listDlg.Init(list);
		if(listDlg.DoModal() == IDOK)
		{
			// first check to see if they want to delete all unchanged files in this chg
			// if so, we can do it quickly using p4 revert -a -c [chg#]
			// however if there are only 1 or 2 files, we do them individually
			if (!listDlg.AnyRowsDeleted() && GET_SERVERLEVEL() >= 14 
			  && currItem && list->GetCount() > 2)
			{
				CCmd_Revert *pCmd= new CCmd_Revert;
				pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK);
				CString chgnbr;
				m_EditChangeNum=GetChangeNumber(currItem);
				if (m_EditChangeNum)
					chgnbr.Format(_T("%ld"), m_EditChangeNum);
				else
					chgnbr = _T("default");
				pCmd->SetChgName(chgnbr);
				if( pCmd->Run( chgnbr, TRUE, TRUE, FALSE ) )
					m_DoRevert=FALSE;
				else
				{
					MainFrame()->ClearStatus();
					delete pCmd;
				}
			}
			else if(!list->IsEmpty())
			{
				// Copy the stringlist to our list and then delete, so we dont have
				// to keep track of the list pointer and when to delete it
				for(pos=list->GetHeadPosition(); pos!=NULL; )
				{
					CString str = list->GetNext(pos);
					if (GET_SERVERLEVEL() < 14)	// pre 2002.2?
					{
						if (str.FindOneOf(_T("@#%*")) != -1)
						{
							StrBuf b;
							StrBuf f;
							f << CharFromCString(str);
							StrPtr *p = &f;
							StrOps::WildToStr(*p, b);
							str = CharToCString(b.Value());
						}
					}
					else
					{
						int i;
						if ((i = str.ReverseFind(_T('#'))) != -1)
							str = str.Left(i);
					}
					m_StringList.AddHead(str);
				}
			
				// Then start the revert
				CCmd_ListOpStat *pCmdRevert= new CCmd_ListOpStat;
				pCmdRevert->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key);
				if( pCmdRevert->Run( &m_StringList, bUnChg ? P4REVERTUNCHG : P4REVERT ) )
				{
					MainFrame()->UpdateStatus( LoadStringResource(IDS_REVERTING_FILES) );
					chainedCommands= TRUE;
				}
				else
					delete pCmdRevert;
			}
		}
	}
	else
		AddToStatus(LoadStringResource(IDS_NO_FILES_WILL_BE_REVERTED_BECAUSE), SV_WARNING);
	return chainedCommands;
}

void CDeltaTreeCtrl::OnP4Reopen(CStringList *list)
{
	POSITION pos;
	CString fileName;
	CString info;
	CString temp;
	BOOL isReopenByType= FALSE;
	
	HTREEITEM item = NULL;
	int imageIndex = -1;
	int ix = 0;
	LPARAM param;
	CP4FileStats *stats;
				
    SetRedraw(FALSE);
	for(pos=list->GetHeadPosition(); pos!=NULL;)
	{
		// Get the 'filename#n - reopened; change x' text
		//  or the 'filename#n - reopened; type x' text
		fileName=list->GetNext(pos);
		// Find the space after filename
		int separator= fileName.Find(_T(" - reopened; "));
		info=fileName.Mid(separator+13);
        		
		if(info.Find(_T("type")) != -1)
		{
			isReopenByType= TRUE;

            // Then find the file and update the file type info
            int pound=fileName.ReverseFind(_T('#'));
            if( pound != -1 )
                fileName=fileName.Left(pound);  
			
			item=FindMyOpenFile(fileName, item);
			ASSERT(item != NULL);
			if(item != NULL)
			{
				stats= (CP4FileStats *) GetLParam(item);
				stats->SetType(info.Mid(lstrlen(_T("type "))));
				SetItemText(item, stats->GetFormattedChangeFile(GET_P4REGPTR()->ShowFileType(), GET_P4REGPTR()->ShowOpenAction()));
				int img=TheApp()->GetFileImageIndex(stats,TRUE);
				SetImage(item, img, img);
			}
		}
		else
		{
            // Then find and remove the file from beneath m_DragFromChange

			int i;
            fileName=fileName.Left(separator);
			if ((i = fileName.ReverseFind(_T('#'))) != -1)
				fileName = fileName.Left(i);
			
			item=GetChildItem(m_DragFromChange);
			param=NULL;
			while(item != NULL)
			{
				if(IsAFile(item))
				{
					param= GetLParam(item);
					ASSERT(param != 0);
					stats= (CP4FileStats *) param;
					ASSERT_KINDOF(CP4FileStats, stats);
					temp= stats->GetFullDepotPath();
					if(temp == fileName)
					{
						imageIndex=GetImage(item);
						stats->SetOpenChangeNum( m_DragToChangeNum );
						ix = stats->IsOtherUserMyClient() ? CP4ViewImageList::VI_YOUROTHERCHANGE 
														  : CP4ViewImageList::VI_YOURCHANGE;

						// reopen and update open file info
						//
						if(stats->GetOtherOpens())
						{
							if (GET_SERVERLEVEL() < 19)	// earlier than 2005.1?
							{
								if(stats->IsOtherLock())
									stats->SetLocked(TRUE, FALSE);
								stats->SetOpenAction( stats->GetOtherOpenAction(), FALSE);
								stats->SetOpenAction( 0, TRUE );
								stats->SetOtherUsers(_T(""));
							}
							imageIndex= TheApp()->GetFileImageIndex(stats, TRUE);
						}

						temp= stats->GetFormattedChangeFile(
							GET_P4REGPTR()->ShowFileType(), 
							GET_P4REGPTR()->ShowOpenAction());
						// Do NOT call DeltaTreeCtrl::DeleteItem() because 
						// that will nuke the lParam.  All we want to do here
						// is remove the item from the tree control.
						CMultiSelTreeCtrl::DeleteItem(item);
						break;
					}
				}
				item=GetNextSiblingItem(item);
			}

			ASSERT(item != NULL);  // just reopened an item that wasnt in tree!
				
			// And add under m_DragToChange
			if(item != NULL && imageIndex >= 0)
			{
				item=Insert(temp, imageIndex, param, m_DragToChange, TRUE);
				// we may have to reset the icon on the dragtochange because
				// the Insert() is looking at data for the drag from change
				// which might have been our client/other user. But we know
				// the dragtochange is ours - so force it for unresolved files.
				stats= (CP4FileStats *) GetLParam(item);
				if (stats->IsUnresolved())
					SetImage(m_DragToChange, CP4ViewImageList::VI_YOURCHGUNRES, 
											 CP4ViewImageList::VI_YOURCHGUNRES);
			}
			ASSERT(item != NULL);
		}// if reopen by type or change
	}

	if( !isReopenByType && list->GetCount() )
	{
		int img;
		// Make sure the source change is closed if it was just emptied
		if( GetChildItem(m_DragFromChange) == NULL )
		{
			SetUnexpanded(m_DragFromChange);
			SetChildCount(m_DragFromChange, 0);
			SetLParam(m_DragFromChange, EXPAND_FOLDER);
			SetImage(m_DragFromChange, ix, ix);
		}
		else if ((img = GetImage(m_DragFromChange)) == CP4ViewImageList::VI_YOURCHGUNRES
			   || img == CP4ViewImageList::VI_YOUROTHERCHGUNRES)
		{
			SetCorrectChglistImage(m_DragFromChange);
		}

		// Make sure the target change is openable if it contains children
		if( GetChildItem(m_DragToChange) != NULL )
		{
			SetChildCount(m_DragToChange, 1);
			if (GET_P4REGPTR()->ExpandChgLists( ))
				Expand(m_DragToChange, TVE_EXPAND);
		}
		if (m_SortByExtension || m_SortByResolveStat || m_SortByAction || m_SortByFilename)
			SortTree();
	}

    SetRedraw(TRUE);
	
	// Make sure all selected items got cleared and window repainted correctly
	UnselectAll();
	RedrawWindow();

   	MainFrame()->ClearStatus();
}


LRESULT CDeltaTreeCtrl::OnP4ListOp(WPARAM wParam, LPARAM lParam)
{
	int key = -1;
	POSITION pos;
	BOOL chainedCommands=FALSE;
	BOOL bNeed2Sync = FALSE;
	BOOL bNeed2Revert = FALSE;
	BOOL bRevertAdds = FALSE;
	int  iRedoOpenedFilter = 0;
	CCmd_ListOpStat *pCmd= (CCmd_ListOpStat *) wParam;
	
	if(!pCmd->GetError())
	{
		iRedoOpenedFilter = pCmd->GetRedoOpenedFilter();
		if( pCmd->GetChkForSyncs() )
		{
			int rc;

			key = pCmd->GetServerKey();
			chainedCommands = TRUE;
			if((rc = MsgBox(IDS_ONE_OR_MORE_FILES_IS_NOT_THE_HEAD_REVISION, 
						MB_ICONEXCLAMATION | MB_DEFBUTTON3)) == IDC_BUTTON1)
			{
				if(pCmd->GetRevertAdds()->GetCount())
					bRevertAdds = TRUE;
				else
					MainFrame()->UpdateDepotandChangeViews(REDRILL, key);
			}
			else if (rc == IDC_BUTTON2)	// Sync then edit
			{
				bNeed2Sync   = TRUE;
				bNeed2Revert = TRUE;
				if(pCmd->GetRevertAdds()->GetCount())
					bRevertAdds = TRUE;
			}
			else // (rc == IDC_BUTTON3)	// Cancel
				bNeed2Revert = TRUE;
		}
		else if( pCmd->GetSync2Head() )
		{
			key = pCmd->GetServerKey();
			chainedCommands = TRUE;
			CStringList *pSyncList = pCmd->GetUnsynced();	// the files to sync to head
			SyncList2Head(pSyncList, key);
		}
		else if( pCmd->GetRevertAdds()->GetCount() )
		{
			key = pCmd->GetServerKey();
			chainedCommands = TRUE;
			bRevertAdds = TRUE;
		}
		else if( pCmd->HitMaxFileSeeks() )
		{
			// Too much was added for seeking out each file for an att update
			// to be efficient.  Just start a full update.
			key = pCmd->GetServerKey();
			chainedCommands = TRUE;
			if (pCmd->GetRevertUnchgAfter())
			{
				CCmd_ListOpStat *pCmd2= new CCmd_ListOpStat;
				pCmd2->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key );
				pCmd2->SetRedoOpenedFilter(pCmd->GetRedoOpenedFilter());
				pCmd2->SetHitMaxFileSeeks(TRUE);
				if( pCmd2->Run( &m_StringList2, P4REVERTUNCHG ) )
				{
					MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_REVERT) );
					iRedoOpenedFilter = 0;
				}
				else
				{
					delete pCmd2;
					MainFrame()->UpdateDepotandChangeViews(REDRILL, key);
				}
			}
			else
				MainFrame()->UpdateDepotandChangeViews(REDRILL, key);
			m_Need2Refresh = FALSE;
		}
		else if(pCmd->GetCommand() == P4REVERT
			 || pCmd->GetCommand() == P4REVERTUNCHG
			 || pCmd->GetCommand() == P4VIRTREVERT)
		{
			chainedCommands = OnP4RevertFile(pCmd->GetList(), TRUE, 
									pCmd->GetOutputErrFlag(), pCmd->GetRevertUnchgAfter(), 
									pCmd->GetServerKey(), pCmd->GetRedoOpenedFilter());
			if (chainedCommands)
				iRedoOpenedFilter = 0;
		}
		else if(pCmd->GetCommand() == P4REOPEN)
		{
			OnP4Reopen(pCmd->GetList());
		}
		else if(pCmd->GetCommand() == P4EDIT)
		{
			CStringList *pList = pCmd->GetList();
			// An empty output list coming back from the server
			// can occur if a file has been branched and is then
			// opened for edit (which opens it for add, of course).
			// So if we arrive here with an empty list, just do a
			// full refresh since we have no data to work with.
			if (pList->IsEmpty())
			{
				key = pCmd->GetServerKey();
				chainedCommands = TRUE;
				MainFrame()->UpdateDepotandChangeViews(REDRILL, key);
			}
			else OnP4EditFile(pList);
		}
		else
			ASSERT(0);
	}

	// For the above commands, we do not need any ostat type
	// info that CCmd_ListOpStat may have collected, so just
	// delete it to avoid leakage
	pCmd->DeleteStatList();

	if( !chainedCommands || MainFrame()->IsQuitting() )
		pCmd->ReleaseServerLock();

	BOOL bOutputError = pCmd->GetOutputErrFlag();

	if (bNeed2Revert || bRevertAdds)
	{
		m_StringList.RemoveAll();

		CStringList * pNewList = 0;
		CStringList * pRevertList;
		CStringList * pSyncList = pCmd->GetUnsynced();	// the files to revert, then sync
		CCmd_ListOpStat *pCmd2= new CCmd_ListOpStat;
		if (bNeed2Sync)
			pNewList = pCmd2->GetUnsynced();	// a place to save the files to sync to head
		else if (bNeed2Revert)
		{
			pRevertList = pCmd->GetRevertIfCancel();
			for( pos= pRevertList->GetHeadPosition(); pos!= NULL; )
				m_StringList.AddTail( pRevertList->GetNext(pos) );
		}
		if (bRevertAdds)
		{
			pRevertList = pCmd->GetRevertAdds();
			for( pos= pRevertList->GetHeadPosition(); pos!= NULL; )
				m_StringList.AddTail( pRevertList->GetNext(pos) );
		}
		if (bNeed2Sync)
		{
			for( pos= pSyncList->GetHeadPosition(); pos!= NULL; )
			{
				CString txt = pSyncList->GetNext(pos);
				m_StringList.AddTail( txt );
                ASSERT(pNewList);
				pNewList->AddTail( txt );
			}
		}

        ASSERT(key != -1);
		pCmd2->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key );
		if (bNeed2Sync)
			 pCmd2->SetSync2Head(TRUE);			// they want to sync to head afterwards
		else pCmd2->SetHitMaxFileSeeks(TRUE);	// we will need to do a full refresh at the end
		if( pCmd2->Run( &m_StringList, P4REVERT ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_REVERT) );
		else
			delete pCmd2;
	}

	delete pCmd;

	if (m_Need2Edit)
	{
		m_Need2Edit = FALSE;
		::SendMessage(m_depotWnd, m_Msg2Send, (WPARAM) &m_ClientPath, m_SavelParam);
	}
	if (iRedoOpenedFilter)
		::SendMessage(m_depotWnd, WM_REDOOPENEDFILTER, (WPARAM)iRedoOpenedFilter, 0);
	else if (bOutputError)
		::PostMessage(m_depotWnd, WM_COMMAND, ID_VIEW_UPDATE_LEFT, 0);
	return 0;
}

void CDeltaTreeCtrl::SyncList2Head(CStringList *pSyncList, int key)
{
	POSITION pos;

	m_StringList.RemoveAll();

	for( pos= pSyncList->GetHeadPosition(); pos!= NULL; )
		m_StringList.AddTail( pSyncList->GetNext(pos) + _T("#head") );

	CCmd_Get *pCmd= new CCmd_Get;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key);
	if( pCmd->Run( &m_StringList, FALSE, TRUE ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_FILE_SYNC) );
	else
		delete pCmd;
}

LRESULT CDeltaTreeCtrl::OnP4SyncAndEdit(WPARAM wParam, LPARAM lParam)
{
	CCmd_Get *pCmd= (CCmd_Get *) wParam;

	int i;
	int key= pCmd->GetServerKey();
	POSITION pos;

	m_StringList.RemoveAll();

	CStringList *pSyncList = pCmd->GetGetList();
	for( pos= pSyncList->GetHeadPosition(); pos!= NULL; )
	{
		// Get the filenames to open for edit
		CString txt = pSyncList->GetNext(pos);
		if ((i = txt.Find(_T('#'))) != -1)
			txt = txt.Left(i);
		m_StringList.AddTail(txt);
	}

	CCmd_ListOpStat *pCmd2= new CCmd_ListOpStat;
	pCmd2->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key );
	pCmd2->SetHitMaxFileSeeks(TRUE);	// we will need to do a full refresh at the end
	if( pCmd2->Run( &m_StringList, P4EDIT, 0 ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_OPENING_FILES_FOR_EDIT) );
	else
		delete pCmd2;

	delete pCmd;

	return 0;
}

// Message handler to allow the depot window to pass a result
// set from 'p4 revert'
LRESULT CDeltaTreeCtrl::OnP4UpdateRevert(WPARAM wParam, LPARAM lParam)
{
	CStringList *list= (CStringList *) wParam;
	OnP4RevertFile(list, FALSE);
	return 0;
}

BOOL CDeltaTreeCtrl::OnP4RevertFile(CStringList *list, BOOL notifyDepotWnd/*=TRUE*/, 
									BOOL errs/*=FALSE*/, BOOL revertUnchgAfter/*=FALSE*/,
									int key/*=0*/, BOOL redoOpenedFilter/*=FALSE*/)
{
	POSITION pos;
	CString fileName;
	int pound;

    // Temporarily disable redraws for both windows
    SetRedraw(FALSE);
    ::SendMessage(m_depotWnd, WM_SETREDRAW, FALSE, 0);

	for(pos=list->GetHeadPosition(); pos!=NULL;)
	{
		fileName=list->GetNext(pos);

		// Strip revision number if present
		pound=fileName.ReverseFind(_T('#'));
		if(pound != -1)
			fileName=fileName.Left(pound);  
	
		// Let the depot view know what happened
		if(notifyDepotWnd)
			::SendMessage(m_depotWnd, WM_UPDATEOPEN, (WPARAM) &fileName, 0);

		// And finally, delete it from this tree
		HTREEITEM item=FindMyOpenFile(fileName);
		if(item != NULL)
		{
			HTREEITEM change= GetParentItem(item);
			DeleteItem(item);
			// Make sure the source change is closed if it was just emptied
			if( change != NULL && GetChildItem(change) == NULL && !errs)
			{
				SetUnexpanded(change);
				SetChildCount(change, 0);
				SetLParam(change, EXPAND_FOLDER);
			}
			SetCorrectChglistImage(change);
		}
		else
			ASSERT(0);
	}	

    SetRedraw(TRUE);
    ::SendMessage(m_depotWnd, WM_SETREDRAW, TRUE, 0);
    RedrawWindow();
    ::RedrawWindow( m_depotWnd, NULL, NULL, RDW_INVALIDATE );
	
	if (revertUnchgAfter)
	{
		CCmd_ListOpStat *pCmd2= new CCmd_ListOpStat;
		pCmd2->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key );
		pCmd2->SetNbrChgedFilesReverted(list->GetCount());
		pCmd2->SetRedoOpenedFilter(redoOpenedFilter);
		if( pCmd2->Run( &m_StringList2, P4REVERTUNCHG ) )
		{
			MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_REVERT) );
			return TRUE;
		}
		else
		{
			delete pCmd2;
			MainFrame()->UpdateDepotandChangeViews(REDRILL, key);
		}
	}
	else
		MainFrame()->ClearStatus();

	return FALSE;
}

void CDeltaTreeCtrl::OnP4EditFile(CStringList *list)
{
	POSITION pos;
	CString fileName;
	int pound;

    // Temporarily disable redraws for windows
    SetRedraw(FALSE);

	for(pos=list->GetHeadPosition(); pos!=NULL;)
	{
		fileName=list->GetNext(pos);

		// Strip revision number if present
		pound=fileName.ReverseFind(_T('#'));
		if(pound != -1)
			fileName=fileName.Left(pound);  
	
		// And finally, update the item's status
		HTREEITEM item=FindMyOpenFile(fileName);
		if(item != NULL)
		{
			CP4FileStats *newStats;
			CP4FileStats *stats= (CP4FileStats *) GetLParam(item);
			HTREEITEM change= GetParentItem(item);

			newStats= new CP4FileStats;
			newStats->Create(stats);
			newStats->SetOpenAction(F_EDIT, FALSE);
			DeleteItem(item);
			Insert(newStats->GetFormattedChangeFile(GET_P4REGPTR()->ShowFileType(), 
				GET_P4REGPTR()->ShowOpenAction()), 
				TheApp()->GetFileImageIndex(newStats, TRUE), (LPARAM) newStats, change, TRUE);
		}
		else
			ASSERT(0);
	}	

    SetRedraw(TRUE);
    RedrawWindow();
	
   	MainFrame()->ClearStatus();
}

// A change description was sent to the server, 'P4 change -i".
// This should be a confirming message. 
LRESULT CDeltaTreeCtrl::OnP4ChangeSpec(WPARAM wParam, LPARAM lParam)
{
	CCmd_EditSpec *pCmd= (CCmd_EditSpec *) wParam;
   	MainFrame()->ClearStatus();

	if(!pCmd->GetError() && !m_EditInProgress 
	 && pCmd->PreprocessChgSpec() && pCmd->DoSpecDlg(this))
	{
		m_EditInProgress = TRUE;
		m_EditInProgressWnd = pCmd->GetSpecSheet();
		DragAcceptFiles(FALSE);
	}
	else
	{
		if (pCmd->HaveServerLock())
			pCmd->ReleaseServerLock();
		if( !TheApp()->m_SubmitPath.IsEmpty() )
			MainFrame()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
		delete pCmd;
	}
	return 0;
}

LRESULT CDeltaTreeCtrl::OnP4EndSpecEdit( WPARAM wParam, LPARAM lParam )
{
	CCmd_EditSpec *pCmd= (CCmd_EditSpec *) wParam;
    BOOL chainedCommands=FALSE;

	if (lParam != IDCANCEL && lParam != IDABORT)
    {
		if(pCmd->GetSpecDlgExit() == IDALTERNATE || m_EditChangeNum == 0 || pCmd->EditedLists())
		{
			if (TheApp()->m_SubmitPath.IsEmpty())
			{
				// Get the lock and start the update process w/out dropping the lock
				int key= pCmd->HaveServerLock() ? pCmd->GetServerKey( ) : 0;
           		MainFrame()->UpdateDepotandChangeViews(REDRILL, key);
				chainedCommands=TRUE;
			}

			CString txt;
			if(pCmd->GetSpecDlgExit() == IDALTERNATE)
			{
				txt.FormatMessage(IDS_CHANGE_n_SUBMITTED, pCmd->GetNewChangeNum());
				AddToStatus(txt, SV_COMPLETION);
				MainFrame()->SetOldChgUpdateTime(0);
				MainFrame()->SetJobUpdateTime(0);
			}
			UnselectAll();
		}
		else
		{
			if(pCmd->GetSpecDlgExit() == IDNEEDTOREFRESH)
			{
				// We need to do a resolve so update our internal stats
				// Get the lock and start the update process w/out dropping the lock
				int key= pCmd->HaveServerLock() ? pCmd->GetServerKey( ) : 0;
           		MainFrame()->UpdateDepotandChangeViews(REDRILL, key);
				chainedCommands=TRUE;
				// If we got here via a cmd line -S flag,
				// restore the main window and clear the submit path
				if (!TheApp()->m_SubmitPath.IsEmpty())
				{
					MainFrame()->ShowWindow(SW_RESTORE);
					TheApp()->m_SubmitPath.Empty();
				}
			}
			else	// we need to update the screen to reflect the new description
			{
				// The change description is always changed, so put together the
				// new item text for the change node.  Note that the change number
				// must be correctly formatted to avoid bungling the sort order
									
				CString txt;
				CString desctxt=PadCRs(pCmd->GetChangeDesc());

				if(GET_P4REGPTR()->ShowChangeDesc())
				{
					int trunc = (GET_SERVERLEVEL() >= 19) 
							  ? GET_P4REGPTR()->GetUseLongChglistDesc() : 31;
					CString shorttxt = TruncateString(desctxt, trunc);
					txt.FormatMessage(
                        shorttxt == desctxt ? IDS_CHANGE_n_s
                                            : IDS_CHANGE_n_s_TRUNC, 
                        pCmd->GetNewChangeNum(), 
						shorttxt);
				}
				else
					txt.FormatMessage(IDS_CHANGE_n, pCmd->GetNewChangeNum());
				
				if(m_EditChangeNum == 0)
				{
					// The default change was edited, so rename the change, and
					// insert a new default change in the tree
					
					SetItemText(m_MyDefault, txt);
					m_MyDefault=Insert(LoadStringResource(IDS_DEFAULTCHANGELISTNAME), CP4ViewImageList::VI_YOURCHANGE, NULL, m_MyRoot, TRUE);
					if (!m_DragToChangeNum)
						m_DragToChange=m_MyDefault;
				}
				else
					// We just edited an existing change, so just update the 
					// description text
					SetItemText(m_EditChange, txt);
		
				txt.FormatMessage(IDS_CHANGE_n_UPDATED, (long) pCmd->GetNewChangeNum());
				AddToStatus(txt);
			}
		}
	}
	if (lParam != IDABORT)
	{
		MainFrame()->ClearStatus();
		if( !chainedCommands || MainFrame()->IsQuitting() )
		{
			if (pCmd->HaveServerLock())
				pCmd->ReleaseServerLock();
			if( !chainedCommands && !TheApp()->m_SubmitPath.IsEmpty() )
				MainFrame()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
		}
		CDialog *dlg = (CDialog *)pCmd->GetSpecSheet();
		dlg->DestroyWindow();
	}
	delete pCmd;
	m_EditInProgress = FALSE;
	DragAcceptFiles(TRUE);
	return 0;
}

LRESULT CDeltaTreeCtrl::OnP4ChangeDel(WPARAM wParam, LPARAM lParam)
{
	CCmd_Delete *pCmd= (CCmd_Delete *) wParam;

	if(!pCmd->GetError())
	{
		CString text=pCmd->GetCompletionData();
		int offset= text.Find(_T("deleted"));
		
		if(offset >= 7 && offset < 18)
		{
			// Completion looks like "Change 5000 deleted."
			AddToStatus( pCmd->GetCompletionMessage(), SV_COMPLETION);
			// No need to delete children since its an empty change
			if (!pCmd->IgnoreActiveItem())
				DeleteItem(m_ActiveItem);
			// Make sure the updated window draws correctly
			UnselectAll();
			RedrawWindow();	
		}
		else
		{
			// Completion looks like "Change 5000 has 4 open files and can't be deleted."
			AddToStatus( pCmd->GetCompletionMessage(), SV_WARNING);
			ASSERT(0);
		}
	}
	
	delete pCmd;
   	MainFrame()->ClearStatus();
	return 0;
}

LRESULT CDeltaTreeCtrl::OnP4Ostat(WPARAM wParam, LPARAM lParam)
{
	CP4FileStats *stats;
	
	CCmd_Ostat *pCmd= (CCmd_Ostat *) wParam;
	// Get the filelist
	CObArray const *array= pCmd->GetArray();
	ASSERT_KINDOF(CObArray,array);

	if(pCmd->GetError() || MainFrame()->IsQuitting())
	{
		// Something went wrong, so delete the command and the result list
		if(array->GetSize() > 0)
		{
			for( int i=0; i < array->GetSize(); i++)
			   delete (CP4FileStats *) array->GetAt(i);
		}
        pCmd->ReleaseServerLock();
		delete pCmd;

		RedrawWindow();
		MainFrame()->SetLastUpdateTime(UPDATE_FAILED);
		
       	MainFrame()->ClearStatus();
		return 0;
	}
	
	// Change-verification parameters used to avoid searching for
	// the same change repeatedly
	HTREEITEM changeItem=NULL;
	int lastChangeNum= -1;
	CString lastOtherUser;
		
	if(array->GetSize() > 0)
	{
		// Temporarily disable redraws
        SetRedraw(FALSE);
		BOOL needExpand=FALSE;

		// Array of filestats open for Add - may need to pass this list to the Depot pane
		CObList lAdds;
        
        // then get the list contents into the tree
		for( int i=0; i<array->GetSize(); i++ )
		{
			// Get the cursed filename
			stats= (CP4FileStats *) array->GetAt(i);

			if( m_ExpandingOthersRoot && ( stats->IsMyOpen() || stats->IsOtherUserMyClient() ) )
			{
				delete stats;
				continue;
			}
			else if( m_ExpandingOthersRoot )
				needExpand=TRUE;

            // Find/create the change this file is under if the change number or user@client has changed
            // Otherwise, use the cached changeItem, because verifying that a change exists is slow as
            // a MUNI light rail car
			if( lastChangeNum != stats->GetOpenChangeNum() || Compare(lastOtherUser, stats->GetOtherUsers()) != 0 )
			{
                if( !stats->IsMyOpen() && stats->GetOpenChangeNum() == 0 
									   && stats->GetOtherOpens() != 0)
                {
                    // Someone else's default change; insert it without checking, because it can't be
                    // in the tree yet.  Note this only works because CCmd_Ostat has sorted the
                    // results by ismyopen+changenum+user@client
                    changeItem=InsertChange(stats, FALSE);
                }
                else
				    changeItem=InsertChange(stats, TRUE);

				lastChangeNum = stats->GetOpenChangeNum();
				lastOtherUser = !lastChangeNum && stats->IsMyOpen() 
							  ? _T("") : stats->GetOtherUsers();
			}

			if(changeItem!=NULL)
			{
				// Insert the file under the change
				Insert(stats->GetFormattedChangeFile(GET_P4REGPTR()->ShowFileType(), GET_P4REGPTR()->ShowOpenAction()),
							TheApp()->GetFileImageIndex(stats, TRUE), (LPARAM) stats, changeItem, FALSE);
			}
			else
				ASSERT(0);

			// if we are showing local files in client tree,
			// save the stats of any adds for passing to the depot pane
			if ((GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALTREE)
			 && (stats->GetMyOpenAction() == F_ADD || stats->GetOtherOpenAction() == F_ADD 
			  || stats->GetMyOpenAction() == F_BRANCH || stats->GetOtherOpenAction() == F_BRANCH))
				lAdds.AddTail(stats);

			if ( !PumpMessages( ) || MainFrame()->IsQuitting() )
			{
                pCmd->ReleaseServerLock();
				delete pCmd;
                SetRedraw(TRUE);
                ::SendMessage(m_depotWnd, WM_SETREDRAW, TRUE, 0);
				return 0;
			}
		} // for entire array
#if 0	// the if() is not quite right yet
		if( m_OthersRoot && needExpand && GET_P4REGPTR()->FilterPendChgsByMyClient() )
		{
			// walk all other client changes and remove empty ones
			HTREEITEM item= m_OthersRoot;

			item=GetChildItem(item);	// Search for the second level node
			while(item != NULL)
			{
				HTREEITEM nextitem = GetNextSiblingItem(item);
				HTREEITEM firstchild = GetChildItem(item);
				if (!firstchild)
					DeleteItem(item);
//				else
//				{
//					CString childStr=GetItemText(firstchild);
//					int ii = 0;
//				}
				item = nextitem;
			}
		}
#endif
		if( m_OthersRoot && needExpand )
		{
			SetLParam( m_OthersRoot, FOLDER_ALREADY_EXPANDED);
			Expand( m_OthersRoot, TVE_EXPAND );
		}

		// if there were any files opend for add
		// (and we save their fstats because we are showing all local files in client tree)
		// pass those fstat info to the depot pane
		if (lAdds.GetCount())
			::SendMessage(m_depotWnd, WM_SETADDFSTATS, (BOOL)m_ExpandingOthersRoot, (LPARAM)&lAdds);
    } // if

    
	// Expand the tree again
    SetRedraw(FALSE);
	SortTree();
	if( !m_ExpandingOthersRoot )
	{
		// First time only - should we re-expand the tree to the previous execution's state?
		if (m_RedoExpansion)
		{
			int i = 2;
			m_MyRootExpanded = TRUE;
			m_ExpandedItems.RemoveAll();
			if ((i = m_PrevExpansion.Find(_T(','))) != -1)
			{
				CString txt;
				CString testtext;
				HTREEITEM item;
				m_PrevExpansion = m_PrevExpansion.Mid(i+1);
				while ((i = m_PrevExpansion.Find(_T(','))) != -1)
				{
					txt = _T(' ') + m_PrevExpansion.Left(i);
					item=GetChildItem(m_MyRoot);
					while (item)
					{
						testtext=GetItemText(item);
						if(testtext.Find(txt) != -1)
						{
							m_ExpandedItems.AddTail(testtext);
							break;
						}
						item=GetNextSiblingItem(item);
					}
					m_PrevExpansion = m_PrevExpansion.Mid(i+1);
				}
				if (!m_PrevExpansion.IsEmpty())
				{
					txt = _T(' ') + m_PrevExpansion;
					item=GetChildItem(m_MyRoot);
					while (item)
					{
						testtext=GetItemText(item);
						if(testtext.Find(txt) != -1)
						{
							m_ExpandedItems.AddTail(testtext);
							break;
						}
						item=GetNextSiblingItem(item);
					}
				}
			}
			m_RedoExpansion = FALSE;
		}
		UpdateTreeState(FALSE);
	}
        
    // And finally, make sure both windows redraw.  This is the last step in a multi-step
    // process, so it is a good place to verify that everything displays properly.  These
    // two lines are not inside the above if{} because we want to make sure that redraw
    // is turned on, even if there are no open files
    SetRedraw(TRUE);
    ::SendMessage(m_depotWnd, WM_SETREDRAW, TRUE, 0);

	RedrawWindow();
	if( !m_ExpandingOthersRoot )
	{
		::RedrawWindow( m_depotWnd, NULL, NULL, RDW_INVALIDATE );
		MainFrame()->SetFullRefresh(FALSE);
	}
	
	MainFrame()->SetLastUpdateTime(UPDATE_SUCCESS);
    pCmd->ReleaseServerLock();
	MainFrame()->UpdateStatus(_T(" "));

	if( m_OthersRoot && m_ExpandingOthersRoot )
	{
		m_ExpandingOthersRoot= FALSE;
		if (!m_PositionTo.IsEmpty())
		{
			PositionToFileInChg(m_PositionTo, m_OthersRoot, m_OthersRoot, TRUE);
			m_PositionTo.Empty();
		}
	}
	else
	{
		// Notify the mainframe that we have finished dealing with changlists,
		// hence the entire set of port connection async command have finished.
		MainFrame()->FinishedGettingChgs(TRUE);
	}
	delete pCmd;
	return 0;
}


// Support for fetching fixes as numbered changes are opened:
//
// 1) When changes are added via OnP4Changes() or VerifyChange(),
//    they are added with LParam==0
// 2) If we have fetched the fixes info for a given numbered change,
//    they have LParam==FOLDER_ALREADY_EXPANDED

BOOL CDeltaTreeCtrl::ExpandTree( const HTREEITEM item )
{
    if( APP_HALTED() || !item )
        return FALSE;

	int lParam= GetLParam( item );

	//  A) See if OtherPendingChanges just got expanded for first time, and
	//     if so, go handle that special case
	if( item == m_OthersRoot && lParam == EXPAND_FOLDER && !m_OthersRootExpanded )
		return ExpandOthersRoot( );

    //	B) Test to see if it's a numbered change that we havent already expanded
	//	   If it's my root and it has not been previously expanded,
	//	   expand each of my changelists with no files to remove the
	//	   plus sign, or show that it has only jobs

	if ( item == m_MyRoot && lParam != FOLDER_ALREADY_EXPANDED )
	{
		ExpandEmptyChglists();
		SetLParam(m_MyRoot, FOLDER_ALREADY_EXPANDED);
	}
	
	if ( item == m_MyRoot || item == m_OthersRoot || lParam == FOLDER_ALREADY_EXPANDED )
		return TRUE;

	//	C) Get the change number, and bail if it's somebody's
	//     default change, since there wont be any fixes
	int changeNum= GetChangeNumber(item);
	if ( changeNum <= 0 )
		return TRUE;

	//  D) And finally fire up fixes
	CCmd_Fixes *pCmdFixes= new CCmd_Fixes;
	pCmdFixes->Init( m_hWnd, RUN_ASYNC);
	if( pCmdFixes->Run(changeNum, item) )
	{
		MainFrame()->UpdateStatus( LoadStringResource(IDS_UPDATING_JOB_FIXES) );
	}
	else
	{
		delete pCmdFixes;
		RedrawWindow();
       	MainFrame()->ClearStatus();
	}

	return TRUE;
}

BOOL CDeltaTreeCtrl::ExpandOthersRoot()
{
	// If server is busy, ignore the request
	if( SERVER_BUSY() )
	{
		Sleep(0);
		SET_BUSYCURSOR();
		// wait a bit in 1/10 sec intervals to see if the prev request finishes
		int t=GET_P4REGPTR()->BusyWaitTime();
		do
		{
			Sleep(50);
			t -= 50;
		} while (SERVER_BUSY() && t > 0);
		if( SERVER_BUSY() )
		{
			MessageBeep(0);
			return FALSE;
		}
	}

	CCmd_Ostat *pCmdOstat= new CCmd_Ostat;
	pCmdOstat->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
	
	if( pCmdOstat->Run( TRUE ) )  
	{
		MainFrame()->UpdateStatus( LoadStringResource(IDS_CHECKING_OPEN_FILES) );
		m_ExpandingOthersRoot=TRUE;
		return TRUE;
	}
	else
	{
		delete pCmdOstat;
		return FALSE;
	}
}

LRESULT CDeltaTreeCtrl::OnP4Fixes(WPARAM wParam, LPARAM lParam)
{
	CCmd_Fixes *pCmd= (CCmd_Fixes *) wParam;

	if(!pCmd->GetError())
	{
        // The fixes command has the HTREEITEM that it was launched with,
        // and that item is 99.9% likely to still be valid and still point
        // to the right changelist.  But look up the change number anyway,
        // since that 0.1% of cases may happen.
        
        HTREEITEM item= FindChange(pCmd->GetFixedChangeNumber());
        if( item != NULL )
        {
			SetLParam( item, FOLDER_ALREADY_EXPANDED);
			if( pCmd->GetList()->GetCount() == 0 )
			{
				if( GetChildItem( item ) == NULL )
					SetChildCount( item, 0 );
			}
			else
			{
				OnFixes( item, pCmd->GetList());
				Expand( item, TVE_EXPAND );
			}
		}
	}
	
   	MainFrame()->ClearStatus();
		
	delete pCmd;
	return 0;
}

void CDeltaTreeCtrl::OnFixes(HTREEITEM activeItem, CObList *fixes)
{
	ASSERT_KINDOF(CObList,fixes);
	CP4Fix *fix;
	CString text;

	HTREEITEM changeItem = NULL;

    if( !fixes->GetCount() )
        return;

    // Turn off redraw and record the top item before we start, so
    // we can undo any spurious scrolling before user sees it
    SetRedraw(FALSE);
    HTREEITEM oldTop= GetFirstVisibleItem();

	POSITION pos;
	for(pos= fixes->GetHeadPosition(); pos != NULL; )
	{
		fix=(CP4Fix *) fixes->GetNext(pos);
		ASSERT_KINDOF(CP4Fix,fix);
		
        changeItem = FindChange(fix->GetChangeNum());
		if( changeItem != NULL )
        {
			// Insert the fix under the change
		    text= fix->GetJobName();
		    if(FindFix(fix->GetChangeNum(), fix->GetJobName()) == NULL)
				Insert(text, CP4ViewImageList::VI_JOB, NULL, changeItem, TRUE);
        }
		delete fix;
	} //for

	// Make sure the target change is openable if it contains children
	if( changeItem != NULL && GetChildItem(changeItem) != NULL )
	{
		SetChildCount(changeItem, 1);
		if (GET_P4REGPTR()->ExpandChgLists( ))
			Expand(changeItem, TVE_EXPAND);
	}

    // Scroll back to old top row, and then redraw
    SetRedraw(TRUE);
    if( oldTop != NULL )
        ScrollToFirstItem( oldTop );
    
}


LRESULT CDeltaTreeCtrl::OnP4JobDel(WPARAM wParam, LPARAM lParam)
{
	// A  job was just deleted, so we need to verify that the
	// job does not exist in this view as a fix or fixes
	// wParam is an LPCTSTR that contains the job name
	
	ASSERT((LPCTSTR) wParam != NULL && lstrlen((LPCTSTR) wParam) <256);
	
	CString jobName= (LPCTSTR) wParam;
	int compareLen=jobName.GetLength();

	HTREEITEM subItem;
	HTREEITEM delItem;
	HTREEITEM item=GetChildItem(m_MyRoot);
	while(item !=NULL)
	{
		subItem=GetChildItem(item);
		while(subItem != NULL)
		{
			delItem=subItem;
			subItem=GetNextSiblingItem(subItem);
	
			if(jobName==(GetItemText(delItem)).Left(compareLen))  
				DeleteItem(delItem);
		}
		item=GetNextSiblingItem(item);
	}

	if (m_OthersRoot)
	{
		item=GetChildItem(m_OthersRoot);
		while(item !=NULL)
		{
			subItem=GetChildItem(item);
			while(subItem != NULL)
			{
				delItem=subItem;
				subItem=GetNextSiblingItem(subItem);
		
				if(jobName==(GetItemText(delItem)).Left(compareLen))  
					DeleteItem(delItem);
			}
			item=GetNextSiblingItem(item);
		}
	}

	RedrawWindow();
	return 0;
}


LRESULT CDeltaTreeCtrl::OnP4Fix(WPARAM wParam, LPARAM lParam)
{
	CP4Fix *fix;
	CString text;
	HTREEITEM currentItem;
	int change=0;
	
	CCmd_Fix *pCmd= (CCmd_Fix *) wParam;
	if(pCmd->IsUnfixing() && !pCmd->GetError())
	{
		CObList *list= pCmd->GetList();
	
		if(list->GetCount() > 0)
		{
			POSITION pos= list->GetHeadPosition();
			while( pos != NULL )
			{
				fix= (CP4Fix *) list->GetNext(pos);
				
				// Find the fix and delete it
				currentItem=FindFix(fix->GetChangeNum(), fix->GetJobName());
				change= fix->GetChangeNum();
				if(currentItem!=NULL)
					DeleteItem(currentItem);	
				else
					ASSERT(0);  // Should have been in tree!

				delete fix;
			} //while
		} // if

		// Make sure the source change looks empty if it was just emptied
		if( change > 0 )
		{
			HTREEITEM parentItem= FindChange( change );
			if( parentItem != NULL && GetChildItem(parentItem) == NULL )
			{
				SetUnexpanded( parentItem );
				SetChildCount( parentItem, 0);
				SetLParam( parentItem, EXPAND_FOLDER );
			}
		}

		RedrawWindow();
	}
	else
	{
		// Add new fixes to display
		OnFixes(m_ActiveItem, pCmd->GetList());
        RedrawWindow();
	}
	
   	MainFrame()->ClearStatus();
	delete pCmd;
	return 0;
}


LRESULT CDeltaTreeCtrl::OnP4JobDescribe(WPARAM wParam, LPARAM lParam)
{
	CCmd_Describe *pCmd= (CCmd_Describe *) wParam;

	if(!pCmd->GetError())
	{
		CString desc= MakeCRs(pCmd->GetDescription());
		
		int key;
		CSpecDescDlg *dlg = new CSpecDescDlg(this);
		dlg->SetIsModeless(TRUE);
		dlg->SetKey(key = pCmd->HaveServerLock()? pCmd->GetServerKey() : 0);
		dlg->SetItemName(pCmd->GetReference());
		dlg->SetDescription(desc);
		dlg->SetCaption( LoadStringResource(IDS_PERFORCE_FIXED_JOB_DESCRIPTION) );
		dlg->SetViewType(P4JOB_SPEC);
		dlg->SetShowEditBtn(!key ? TRUE : FALSE);
		if (!dlg->Create(IDD_SPECDESC, this))	// display the description dialog box
		{
			dlg->DestroyWindow();	// some error! clean up
			delete dlg;
		}
	}

	delete pCmd;
	MainFrame()->ClearStatus();
	return 0;
}

LRESULT CDeltaTreeCtrl::OnP4EndJobDescribe( WPARAM wParam, LPARAM lParam )
{
	CSpecDescDlg *dlg = (CSpecDescDlg *)lParam;

	if (wParam == IDC_EDITIT)				// which button did they click to close the box?
	{
		CString jobname = dlg->GetItemName();
		ASSERT(!jobname.IsEmpty());
		MainFrame()->EditJobSpec(&jobname);
	}
	dlg->DestroyWindow();
	return TRUE;
}
		
LRESULT CDeltaTreeCtrl::OnP4UpdateOpen(WPARAM wParam, LPARAM lParam)
{
	// This message is SENDMESSAGE'd by DepotView when a file's open status
	// is changed.  Only three types of status updates are possible:
	// 1) file was locked
	// 2) file was unlocked
	// 3) file was opened for edit, delete, integ or branch

	// The command is in lParam, and a CP4FileStats obj is in wParam
	
	CP4FileStats *stats= (CP4FileStats *) wParam;
	CP4FileStats *newStats, *oldStats;

	// First step is to try finding the item
	HTREEITEM item = FindMyOpenFile(stats->GetFullDepotPath());
	
	switch(lParam)
	{
	case P4LOCK:
	case P4UNLOCK:
		if(item != NULL)
		{
			oldStats= (CP4FileStats *) GetLParam(item);

			// update image and stats values
			oldStats->SetLocked(stats->IsMyLock(), FALSE);
			oldStats->SetLocked(FALSE, TRUE);
			int img=TheApp()->GetFileImageIndex(oldStats, TRUE);
			SetImage(item, img, img);
		}
		else
			// How did we just lock a file that we previously did not
			// have open?
			ASSERT(0);

		break;

	case P4ADD:
	case P4EDIT:
	case P4DELETE:
	case P4INTEG:
		if(item != NULL)
		{
			oldStats= (CP4FileStats *) GetLParam(item);
			if(stats->GetMyOpenAction() > 0) // Its in the tree and still open
			{
				// update image and stats values
				oldStats->SetLocked(stats->IsMyLock(), FALSE);
				if(stats->IsMyLock())
					// If I have a lock, no other user can
					oldStats->SetLocked(FALSE, TRUE);
				int img=TheApp()->GetFileImageIndex(oldStats, TRUE);
				if ((oldStats->GetMyOpenAction() == F_INTEGRATE) 
					&& (stats->GetMyOpenAction() == F_EDIT))
				{
					HTREEITEM change;

					if(stats->GetOpenChangeNum() == m_DragToChangeNum)
						change=m_DragToChange;
					else
					{
						change= InsertChange(stats,TRUE);
						m_DragToChange= change;
						m_DragToChangeNum= stats->GetOpenChangeNum();
					}
					DeleteItem(item);
					newStats= new CP4FileStats;
					newStats->Create(stats);
					Insert(newStats->GetFormattedChangeFile(GET_P4REGPTR()->ShowFileType(), 
						GET_P4REGPTR()->ShowOpenAction()), 
						TheApp()->GetFileImageIndex(newStats, TRUE), (LPARAM) newStats, change, TRUE);
				}
				else if ((lParam == P4INTEG) && stats->IsUnresolved()
					  && (oldStats->GetMyOpenAction() == F_EDIT) 
					  && (stats->GetMyOpenAction() == F_EDIT))
				{
					oldStats->SetUnresolved(TRUE);
					img=TheApp()->GetFileImageIndex(oldStats, TRUE);
					SetImage(item, img, img);
					int ix = stats->IsOtherUserMyClient() ? CP4ViewImageList::VI_YOUROTHERCHGUNRES 
														  : CP4ViewImageList::VI_YOURCHGUNRES;
					SetImage(TreeView_GetParent(m_hWnd, item), ix, ix);
				}
				else
					SetImage(item, img, img);
            }
			else
			{
				// There is probably something wrong if we are here, because we should
				// not be processing reverted files here.  But if the file has no open
				// action and we found it here, deleted it from the tree
				if(stats->GetOtherOpenAction() == 0) // Do the delete if no one else has it open
				{
					ASSERT(0);
					DeleteItem(item);
				}
			}
		}
		else  // item not found in tree
		{
			// File was just opened for edit
			if(stats->GetMyOpenAction() > 0)  
			{
				HTREEITEM change;
				if(stats->GetOpenChangeNum() > 0)
				{
					// Two ways to get here:
					if(stats->GetOpenChangeNum() == m_DragToChangeNum)
						// 1) we opened file as a result of drag-drop
						change=m_DragToChange;
					else
					{
						// 2) we specified the target change during an integrate command
						//    so we need to find the change
						change= InsertChange(stats,TRUE);
					
						// Even though this isnt a drag-drop operation, cache the change info
						// into the drag-drop variables, so we dont need to do this lookup for 
						// every file that was just integrated
						m_DragToChange= change;
						m_DragToChangeNum= stats->GetOpenChangeNum();
					}
				}
				else
				{
					// Two ways to get here:  1) right click on a file in depot view, or 2) drag a
					// file from depot view to default change in this view
					change=m_MyDefault;
				}

				newStats= new CP4FileStats;
				newStats->Create(stats);
				if (GET_SERVERLEVEL() < 19)	// earlier than 2005.1?
					newStats->SetOtherOpens(FALSE);  // be consistent w/ data returned by ostat
				Insert(newStats->GetFormattedChangeFile(GET_P4REGPTR()->ShowFileType(), 
					GET_P4REGPTR()->ShowOpenAction()), 
					TheApp()->GetFileImageIndex(newStats, TRUE), (LPARAM) newStats, change, TRUE);
				if( GetChildItem(change) != NULL )
				{
					SetChildCount(change, 1);
					if (GET_P4REGPTR()->ExpandChgLists( ))
						Expand(change, TVE_EXPAND);
				}
			}
			//else (don't assert!  the likely reason we didnt find an open file under
            //      our changes is that the file is open by another user on my client
		} // if item found 
		break;

	default:
		ASSERT(0);

	} // switch(command)

	if (m_SortByExtension || m_SortByResolveStat || m_SortByAction || m_SortByFilename)
		m_Timer = ::SetTimer( m_hWnd, SORT_TIMER, 100, NULL );
	return 0;
}

LRESULT CDeltaTreeCtrl::OnP4SetUnresolved(WPARAM wParam, LPARAM lParam)
{
	// This message is SENDMESSAGE'd by DepotView when an open file is gotten
	// at a different revision.

	// Find the file
	CP4FileStats *stats= (CP4FileStats *) wParam;

	HTREEITEM item=FindMyOpenFile(stats->GetFullDepotPath());
		
	if(item != NULL)
	{
		// Found it, so see if unresolved
		CP4FileStats *oldStats= (CP4FileStats *) GetLParam(item);
        if(oldStats->GetHaveRev() < stats->GetHaveRev())
		    oldStats->SetUnresolved(TRUE);
		oldStats->SetHaveRev(stats->GetHaveRev());
		int img=TheApp()->GetFileImageIndex(oldStats, TRUE);

		// And update image and text
		SetImage(item, img, img);
		SetItemText(item, oldStats->GetFormattedChangeFile(GET_P4REGPTR()->ShowFileType(), GET_P4REGPTR()->ShowOpenAction()));
		SetCorrectChglistImage(TreeView_GetParent(m_hWnd, item));
	}
	// else
		// Not a fatal error, could just be that our instance of
		// the gui doesnt yet have this item in the changes window
		//ASSERT(0);
		
	return 0;
}

// Get a list of all of my open changes
LRESULT CDeltaTreeCtrl::OnGetMyChangesList(WPARAM wParam, LPARAM lParam)
{
	CStringList *list= (CStringList *) wParam;
	ASSERT_KINDOF(CStringList,list);

	GetMyChangesList(list);
	return 0;
}


HTREEITEM CDeltaTreeCtrl::FindChange(long changeNum)
{
	HTREEITEM item, currentItem;

	// Directly assign the root node
	currentItem= m_MyRoot;
		
	// Search for the second level node
	item=GetChildItem(currentItem);
	if(item != NULL)  // there is at least one child
	{
		while(GetChangeNumber(item) != changeNum)
		{
			item=GetNextSiblingItem(item);
			if(item==NULL)
				break;
		}
	}

	if(item != NULL)
		return item;  // node exists - return the HTREEITEM

	if (m_OthersRoot)
	{
		// Directly assign the root node
		currentItem= m_OthersRoot;

		// Search for the second level node
		item=GetChildItem(currentItem);
		if(item != NULL)  // there is at least one child
		{
			while(GetChangeNumber(item) != changeNum)
			{
				item=GetNextSiblingItem(item);
				if(item==NULL)
					break;
			}
		}
	}
	
	// return the item if found, otherwise will return NULL
	return item;  
}

HTREEITEM CDeltaTreeCtrl::FindFix(long changeNum, LPCTSTR jobName)
{
	HTREEITEM change= FindChange(changeNum);
	if(change == NULL)
		return NULL;

	HTREEITEM item= GetChildItem(change);
	CString temp;
	
	while(item != NULL)
	{
		if(!IsAFile(item))
		{
			temp= GetItemText(item);
			temp.TrimLeft();  // lose the leading space
			if(temp.Compare(jobName)==0)
				break;
		}
		item=GetNextSiblingItem(item);
	}

	return item;
}

HTREEITEM CDeltaTreeCtrl::FindMyOpenFile(LPCTSTR fileName, HTREEITEM lastfound/*=NULL*/)
{
	// Search for a filename (no rev#)
	
	int fileNameLen = lstrlen(fileName);

	if (lastfound)	// we have a guess as to where to start looking - check the next item
	{
		HTREEITEM subItem=GetNextSiblingItem(lastfound);
		if (subItem)
		{
			CString subItemText= GetItemText(subItem);
			int subItemTextLen= subItemText.ReverseFind(_T('#'));

			if( fileNameLen == subItemTextLen &&
			   !lstrcmp(fileName, GetItemText(subItem).Left(fileNameLen)) ) 
				return subItem;
		}
	}

	BOOL found=FALSE;
	HTREEITEM subItem = NULL;
	HTREEITEM item=GetChildItem(m_MyRoot);
	while(item !=NULL && !found)
	{
		subItem=GetChildItem(item);
		while(subItem != NULL)
		{
			// Compare the fileName with the portion of the
			// item text that precedes the revision '#'
			CString subItemText= GetItemText(subItem);
			int subItemTextLen= subItemText.ReverseFind(_T('#'));

			if( fileNameLen == subItemTextLen &&
				!lstrcmp(fileName, GetItemText(subItem).Left(fileNameLen)) ) 
			{
				found=TRUE;
				break;
			}
			subItem=GetNextSiblingItem(subItem);
		}
		item=GetNextSiblingItem(item);
	}
	
	if(found)
		return subItem;
	else
		return NULL;
}

HTREEITEM CDeltaTreeCtrl::FindItemByText(LPCTSTR text)
{
	// Search for text
	BOOL found=FALSE;
	CString testtext;

	HTREEITEM item=GetChildItem(m_MyRoot);
	while(item !=NULL && !found)
	{
		testtext=GetItemText(item);
		if(lstrcmp(text, testtext) == 0)
			{
				found=TRUE;
				break;
			}
		item=GetNextSiblingItem(item);
	}
	
	if(!found && m_OthersRoot)
	{
		item=GetChildItem(m_OthersRoot);
		while(item !=NULL && !found)
		{
			testtext=GetItemText(item);
			if(lstrcmp(text, testtext) == 0)
			{
				found=TRUE;
				break;
			}
			item=GetNextSiblingItem(item);
		}
	}

	if(found)
		return item;
	else
		return NULL;
}


// InsertChange()
// Insert the change associated with the given file.  Optionally search for that
// change and skip the insert if it is found.  Return tree node for the change.

HTREEITEM CDeltaTreeCtrl::InsertChange(CP4FileStats *stats, BOOL searchFirst /*=TRUE*/)
{
	HTREEITEM item, currentItem;
	int  imageIndex;
	BOOL sort = FALSE;

	// Directly assign the root node, based on change category
	if( stats->IsOtherUserMyClient() )	
	{
		currentItem= m_MyRoot;
		imageIndex = CP4ViewImageList::VI_YOUROTHERCHANGE;
	}
	else if(stats->IsMyOpen())
	{
		currentItem= m_MyRoot;
		imageIndex = CP4ViewImageList::VI_YOURCHANGE;
	}
	else if( m_OthersRoot )
	{
		currentItem= m_OthersRoot;
		imageIndex = CP4ViewImageList::VI_THEIRCHANGE;
	}
	else return NULL;

	// Format the text for the second level node
	CString changeName;
	int changeNumber;
	if((changeNumber = stats->GetOpenChangeNum())==0)
	{
		if(stats->IsMyOpen() && !stats->IsOtherUserMyClient())
			changeName.FormatMessage(IDS_CHANGE_DEFAULT);
		else if(GET_P4REGPTR()->SortChgsByUser())
			changeName.FormatMessage(IDS_CHANGE_USER_s_DEFAULT, stats->GetOtherUsers());
		else
			changeName.FormatMessage(IDS_CHANGE_DEFAULT_USER_s, stats->GetOtherUsers());
	}
	else 
	{
		if(stats->IsMyOpen() && !stats->IsOtherUserMyClient())
			changeName.FormatMessage(IDS_CHANGE_n, changeNumber);
		else
		{
			if (GET_P4REGPTR()->SortChgsByUser())
				changeName.FormatMessage(IDS_CHANGE_USER_s_n, stats->GetOtherUsers(), changeNumber);
			else
				changeName.FormatMessage(IDS_CHANGE_n_USER_s, changeNumber, stats->GetOtherUsers());
		}
	}

    if( searchFirst )
    {
	    // Search for the second level node
	    item=GetChildItem(currentItem);
	    if(item != NULL)  // there is at least one child
    	{
	    	while(_tcsncmp(GetItemText(item), changeName, changeName.GetLength()) != 0)
		    {
			    item=GetNextSiblingItem(item);
			    if(item==NULL)
		    		break;
		    }
	    }

	    if(item != NULL)
		{
			if (m_DragToChangeNum == changeNumber)
				m_DragToChange = item;
		    return item;  // node exists - return the HTREEITEM
		}

		sort = TRUE;  // node doesn't exists - fall thru to insert it and sort the tree
		if (changeNumber && (changeNumber==m_NewChgNbr) 
		 && !m_NewDesc.IsEmpty() && GET_P4REGPTR()->ShowChangeDesc())
		{
			CString desctxt=PadCRs(m_NewDesc);
			int trunc = (GET_SERVERLEVEL() >= 19) ? GET_P4REGPTR()->GetUseLongChglistDesc() : 31;
			CString shorttxt = TruncateString(desctxt, trunc);
			changeName.FormatMessage(shorttxt == desctxt ? IDS_CHANGE_n_s
													     : IDS_CHANGE_n_s_TRUNC, 
														changeNumber, shorttxt);
		}
    }

	item = Insert( changeName, imageIndex, 0, currentItem, sort);
	if (m_DragToChangeNum == changeNumber)
		m_DragToChange = item;
    return item;  // return the HTREEITEM
}

HTREEITEM CDeltaTreeCtrl::Insert(LPCTSTR text, int imageIndex, LPARAM lParam, 
							 HTREEITEM hParent, BOOL sort)
{
	XTRACE(_T("CDeltaTreeCtrl::Insert txt=%s\n"), text);

	if( lParam > 0 )
	{
		CP4FileStats *fs= (CP4FileStats *) lParam;
		if (fs->IsUnresolved())
		{
			int ix = fs->IsOtherUserMyClient() ? CP4ViewImageList::VI_YOUROTHERCHGUNRES 
											   : CP4ViewImageList::VI_YOURCHGUNRES;
			SetImage(hParent, ix, ix);
		}
	}

	// Add an entry to the tree
	TV_INSERTSTRUCT tree_insert;
	tree_insert.hInsertAfter= sort ? TVI_SORT : TVI_FIRST;
	tree_insert.hParent=hParent;
	tree_insert.item.mask= TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tree_insert.item.iImage=imageIndex;
	tree_insert.item.lParam=lParam;
	tree_insert.item.iSelectedImage=imageIndex;
	tree_insert.item.pszText=const_cast<LPTSTR>(text);
	tree_insert.item.cchTextMax=lstrlen(text);
	return(InsertItem(&tree_insert));
}

void CDeltaTreeCtrl::OnUpdateSortChgFilesByName(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY());
	pCmdUI->SetCheck(GET_P4REGPTR()->SortChgFilesByName());
}

void CDeltaTreeCtrl::OnUpdateSortChgFilesByExt(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY());
	pCmdUI->SetCheck(GET_P4REGPTR()->SortChgFilesByExt());
}

void CDeltaTreeCtrl::OnUpdateSortChgFilesByAction(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY());
	pCmdUI->SetCheck(GET_P4REGPTR()->SortChgFilesByAction());
}

void CDeltaTreeCtrl::OnSortChgFilesByName() 
{
	m_SortByFilename = !GET_P4REGPTR()->SortChgFilesByName();
	GET_P4REGPTR()->SetSortChgFilesByName( m_SortByFilename );
	::SendMessage(m_depotWnd, WM_COMMAND, ID_VIEW_UPDATE_LEFT, 0);
}

void CDeltaTreeCtrl::OnSortChgFilesByExt() 
{
	m_SortByExtension = !GET_P4REGPTR()->SortChgFilesByExt();
	GET_P4REGPTR()->SetSortChgFilesByExt( m_SortByExtension );
	::SendMessage(m_depotWnd, WM_COMMAND, ID_VIEW_UPDATE_LEFT, 0);
}

void CDeltaTreeCtrl::OnSortChgFilesByAction() 
{
	m_SortByAction = !GET_P4REGPTR()->SortChgFilesByAction();
	GET_P4REGPTR()->SetSortChgFilesByAction( m_SortByAction );
	::SendMessage(m_depotWnd, WM_COMMAND, ID_VIEW_UPDATE_LEFT, 0);
}

void CDeltaTreeCtrl::OnUpdateSortChgFilesByResolve(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY());
	pCmdUI->SetCheck(GET_P4REGPTR()->SortChgFilesByResolve());
}

void CDeltaTreeCtrl::OnSortChgFilesByResolve() 
{
	m_SortByResolveStat = !GET_P4REGPTR()->SortChgFilesByResolve();
	GET_P4REGPTR()->SetSortChgFilesByResolve( m_SortByResolveStat );
	SortTree();
}

void CDeltaTreeCtrl::OnUpdateSortChgsByUser(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && GET_P4REGPTR()->GetEnablePendingChgsOtherClients());
	pCmdUI->SetCheck(GET_P4REGPTR()->SortChgsByUser());
}

void CDeltaTreeCtrl::OnSortChgsByUser() 
{
	BOOL sortChgsByUser = !GET_P4REGPTR()->SortChgsByUser();
	GET_P4REGPTR()->SetSortChgsByUser( sortChgsByUser );
	::SendMessage(m_depotWnd, WM_COMMAND, ID_VIEW_UPDATE_LEFT, 0);
}

LRESULT CDeltaTreeCtrl::OnP4Change(WPARAM wParam, LPARAM lParam)
{
	XTRACE(_T("OnP4Change() wParam=%ld lParam=%ld\n"), wParam, lParam);
	if( lParam )
	{
		// Just got a list of changes
		CObList *list= (CObList *) wParam;
		ASSERT_KINDOF(CObList, list);

		POSITION pos= list->GetHeadPosition();

        SetRedraw(FALSE);
		while(pos != NULL)
		{
			CP4Change *change= (CP4Change *) list->GetNext(pos);
			ASSERT_KINDOF(CP4Change, change);
	
			if(change->IsPending())
			{
				HTREEITEM item;
				if(change->IsMyChange())
				{
					// Its my client, but maybe not my user
					if( Compare( change->GetUser(), GET_P4REGPTR()->GetMyID()) == 0 )
						item=Insert(change->GetFormattedChange(GET_P4REGPTR()->ShowChangeDesc(),
																		GET_P4REGPTR()->SortChgsByUser()), 
									CP4ViewImageList::VI_YOURCHANGE, NULL, m_MyRoot, TRUE);
					else
						item=Insert(change->GetFormattedChange(GET_P4REGPTR()->ShowChangeDesc(),
																		GET_P4REGPTR()->SortChgsByUser()), 
																		CP4ViewImageList::VI_YOUROTHERCHANGE, NULL, m_MyRoot, TRUE);
				}
				else if (m_OthersRoot)
				{
					item=Insert(change->GetFormattedChange(GET_P4REGPTR()->ShowChangeDesc(),
																	GET_P4REGPTR()->SortChgsByUser()), 
									CP4ViewImageList::VI_THEIRCHANGE, NULL, m_OthersRoot, TRUE);
				}
				else item = NULL;

				if( change->GetChangeNumber() > 0 && item )
					SetChildCount( item, 1 );
			}
			delete change;
		}
        SetRedraw(TRUE);
		delete list;
	}
	else
	{
		CCmd_Changes *pCmd= (CCmd_Changes *) wParam;
		ASSERT_KINDOF(CCmd_Changes,pCmd);

		if(pCmd->GetError() || MainFrame()->IsQuitting())
		{
           	MainFrame()->ClearStatus();
			MainFrame()->SetLastUpdateTime(UPDATE_FAILED);
			pCmd->ReleaseServerLock();
		}
		else
		{
            int key= pCmd->GetServerKey( );

			CCmd_Ostat *pCmdOstat= new CCmd_Ostat;
			pCmdOstat->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key);
			
			// Only run 'p4 opened -a' if other pending changes root was expanded at
			// the time the refresh was initiated
			if( pCmdOstat->Run( m_OthersRoot && m_OthersRootExpanded ) )
			{
				MainFrame()->UpdateStatus( LoadStringResource(IDS_CHECKING_OPEN_FILES) );
				MainFrame()->SetDeltaUpdateTime(GetTickCount());
			}
			else
			{
				delete pCmdOstat;
               	MainFrame()->ClearStatus();
				pCmd->ReleaseServerLock();
				MainFrame()->SetLastUpdateTime(UPDATE_FAILED);
			}
		}
		delete pCmd;
	}
	return 0;
}

DROPEFFECT CDeltaTreeCtrl::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	STGMEDIUM stg;
	m_DropEffect=DROPEFFECT_NONE;
	m_DragDataFormat=0;
	m_DragLastOver=NULL;
	m_DragLastHighlite=NULL;
	CString fname;

	// Dont allow a drop if the server is busy, since a drop immediately attempts to
	// invoke a server command
	if(SERVER_BUSY() || m_EditInProgress)
		return DROPEFFECT_NONE;

	m_DeltaIsDropTarget = TRUE;
	if(pDataObject->IsDataAvailable( (unsigned short) m_CF_DELTA))
	{
		// Set the display of the drag-from items
		m_DropEffect=DROPEFFECT_MOVE;
		m_DragDataFormat=m_CF_DELTA;
	}
	else if(pDataObject->IsDataAvailable( (unsigned short) m_CF_DEPOT))
	{
		m_DragFromChange=NULL;
		m_DropEffect=DROPEFFECT_COPY;
		m_DragDataFormat=m_CF_DEPOT;
	}
	else if(pDataObject->IsDataAvailable( (unsigned short) m_CF_JOB))
	{
		m_DragFromChange=NULL;
		m_DropEffect=DROPEFFECT_COPY;
		m_DragDataFormat=m_CF_JOB;
	}
	else if(pDataObject->GetData(CF_HDROP, &stg, NULL))
	{
		if(stg.tymed==TYMED_HGLOBAL)
		{
			// Note: df.pFiles is offset in bytes to LPCWSTR filename list, a sequence of
			//       wide char null terminated strings followed by an additional null char
			void *buf=GlobalLock(stg.hGlobal);
			_DROPFILES *df=(DROPFILES *) buf;

			if(df->fWide)
			{
				// NT uses wide char set for file drag-drop
				fname=(LPCWSTR) ((char *)buf+df->pFiles);  // points to first filename
			}
			else
			{
				// NT-lite uses single byte chars for file drag-drop
				fname=(LPCSTR) ((char *)buf+df->pFiles);  // points to first filename
			}
			
			// Perform a crude check on string to see if it looks like a filename 
			
			if(fname[1] == ':')
			{
				m_DropEffect=DROPEFFECT_COPY;
				m_DragDataFormat=CF_HDROP;
			}
			else if ((fname[0] == '\\') && (fname[1] == '\\'))
			{
				m_DropEffect = DROPEFFECT_NONE;	// we don't handle UNC filename yet, so ignore it
			}
			else
			{
				AddToStatus(LoadStringResource(IDS_DATA_WAS_NOT_CF_HDROP_FILES), SV_ERROR);
				AddToStatus(fname, SV_ERROR);
			}
			GlobalUnlock(buf);
		}
		else
				AddToStatus(LoadStringResource(IDS_NO_GLOBAL_CF_HDROP_DATA_RECEIVED), SV_ERROR);

		ReleaseStgMedium(&stg);
		m_DragFromChange=NULL;
	}
	else
		AddToStatus(LoadStringResource(IDS_UNEXPECTED_CLIPBOARD_FORMAT));

	m_DeltaIsDropTarget = FALSE;
	return m_DropEffect;
}


void CDeltaTreeCtrl::OnDragLeave() 
{
	m_PendingDeselect=FALSE;
	
	// Undo drop target highlite, if any
	SelectDropTarget(NULL);
}

DROPEFFECT CDeltaTreeCtrl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	BOOL isDeltaWndDrag= pDataObject->IsDataAvailable( (unsigned short) m_CF_DELTA);

	if( isDeltaWndDrag )
	{
		// Left-drag support.  Dont clear pending deselect until the cursor
		// actually moves!
		CPoint pt= point;
		ClientToScreen( &pt );
		if( !m_DragSourceRect.PtInRect( pt ) )
			m_PendingDeselect=FALSE;
	}

	// If there are valid files to drop, drop effect is DROPEFFECT_COPY
	// Where is the drag?
	TV_HITTESTINFO hitTest;
	hitTest.pt=point;
	hitTest.flags=TVHT_ONITEM|TVHT_ONITEMRIGHT;
	HitTest(&hitTest);

	// Dont allow a drop if the server is busy, since a drop immediately attempts to
	// invoke a server command
	if(SERVER_BUSY())
	{
		m_DragLastOver=NULL;
		SelectDropTarget(NULL);
		return DROPEFFECT_NONE;
	}

	// Same as last time?
	if(hitTest.hItem == m_DragLastOver && hitTest.hItem != NULL )
		return m_DropEffect;

	// Find which item gets the drop highlite
	HTREEITEM dropItem;
	
	
	if( hitTest.hItem == m_MyRoot )
	{
		// Crack open the root to show default change if reqd
		EnsureVisible(m_MyDefault);
		dropItem=m_MyDefault;
	}
	else if( hitTest.hItem == NULL )
		dropItem=NULL;
	else
	{
		if( IsMyPendingChange(hitTest.hItem ) )
			// Its my change, so its drop-able
			dropItem=hitTest.hItem;
		else if( IsMyPendingChangeFile( hitTest.hItem ) )  
			// Its within my change - drop-able on that change
			dropItem=GetParentItem(hitTest.hItem);
		else 
			dropItem=NULL;     
	}

	// Finally, make sure we dont show a drop highlite if its a file being dragged
	// fromt the current change to the current change
	if(m_DragFromChange == dropItem)
		dropItem=NULL;
	
	// Update the highlited item.  If highlite item is NULL, any highlite is cleared
	if(m_DragLastHighlite != dropItem)
	{
		m_DragLastHighlite=dropItem;
		SelectDropTarget(m_DragLastHighlite);
	}
		
	return m_DropEffect;

}


int CDeltaTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMultiSelTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetIndent(15);
	SetScrollTime(10);
	SetImageList( TheApp()->GetImageList(), TVSIL_NORMAL );

	// Register that we accept file mangler files
	DragAcceptFiles(TRUE);

	return 0;
}

// Files can be added three ways:
// Method 1) Drop from MS Exploder - handled in this::OnDrop()
// Method 2) File-AddToSourceControl on menu
// Method 3) Drop from File Mangler - see this::OnDropFiles()
//
// Provide a public member to take the added files from methods 2 and 3
void CDeltaTreeCtrl::AddFileList(int changeNum, CStringList *list, BOOL bDropped/*=FALSE*/)
{
    int key=0;
	
	if(SERVER_BUSY() || m_EditInProgress || !GET_SERVER_LOCK(key))
	{
		if (m_EditInProgress)
			CantDoItRightNow(IDS_ADDEDIT_FILE);
		else
			ASSERT(0);
		return;
	}

	ASSERT_KINDOF(CStringList, list);

	ASSERT(changeNum >= 0);
   	MainFrame()->ClearStatus();

	CStringList demangledList;
	// Demangle the file list
	POSITION pos= list->GetHeadPosition();
	while(pos != NULL)
		demangledList.AddHead(DemanglePath(list->GetNext(pos)));

	m_DragToChangeNum= changeNum;
	m_DragToChange= FindChange(changeNum);
	CString changeTxt;
	if(changeNum)
		changeTxt.Format(_T("%ld"), changeNum);
    else
        changeTxt = LoadStringResource(IDS_DEFAULTCHANGELISTNAME);

	// Get a list of my changes
	CStringList changeList;
	GetMyChangesList(&changeList);
	
	CAddListDlg dlg;
	dlg.Init(&demangledList, &changeList, changeTxt, bDropped, key);

	if(dlg.DoModal() != IDCANCEL)
	{
		int action = dlg.GetAction();
		// Get a copy of the file list, since the list that the
		// dialog has will go out of scope while Cmd_Add runs
		CStringList *enumList= dlg.GetEnumeratedList();
		m_StringList.RemoveAll();
		POSITION pos= enumList->GetHeadPosition();
		while (pos != NULL)
		{
			CString filename = enumList->GetNext(pos);
			if ((action == 3) && (filename.FindOneOf(_T("@#%")) != -1))
			{
				StrBuf b;
				StrBuf f;
				f << CharFromCString(filename);
				StrPtr *p = &f;
				StrOps::WildToStr(*p, b);
				filename = CharToCString(b.Value());
			}
			m_StringList.AddHead(filename);
		}

		m_DragToChangeNum = dlg.GetSelectedChange();
		m_DragToChange = m_DragToChangeNum ? FindChange(m_DragToChangeNum) : m_MyDefault;

		if (action == 3)
		{
			// Start the delete operation
			CCmd_ListOpStat *pCmd= new CCmd_ListOpStat;
			pCmd->Init( m_depotWnd, RUN_ASYNC, HOLD_LOCK, key );
			if( pCmd->Run( &m_StringList, P4DELETE, m_DragToChangeNum ) )
				MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_DELETE) );
			else
				delete pCmd;
		}
		else
		{
			// Start the add and/or edit operation(s)
			CCmd_Add *pCmd= new CCmd_Add;
			pCmd->Init(m_hWnd, RUN_ASYNC, HOLD_LOCK, key);
			pCmd->SetOpenAction(action);
			pCmd->SetHitMaxFileSeeks(m_DragToChange ? dlg.GetNeed2Refresh() : TRUE);
			if( pCmd->Run( m_DragToChangeNum, &m_StringList ) )
				MainFrame()->UpdateStatus( LoadStringResource(IDS_ADDING_FILES) );
			else
				delete pCmd;
		}
	}
	else
		RELEASE_SERVER_LOCK(key);
}

// File mangler file drop
void CDeltaTreeCtrl::OnDropFiles(HDROP hDropInfo) 
{
	TCHAR buf[1024];
	UINT bufsize=1024;
	UINT index=0xFFFFFFFF;
	CStringList list;
	
	// m_LastDragDropTime contains the time the last drag from this window was dropped.
	// If the current time is REALLY CLOSE (1/2 second) to the same time as when that
	// drop was done, then OnDropFiles() is being called as a result of that drop and
	// we don't have anything to do because this is NOT a drop from File Mangler,
	// so just return and don't try to add files.
	// However, if m_LastDragDropTime is not close to the current time, this is a drop
	// from an external window (probably File Mangler) and we need to add the files that
	// were dropped.
	if ((m_LastDragDropTime + 500) > GetTickCount())
		return;

	POINT pt;
	if(!DragQueryPoint(hDropInfo, &pt))
		return;  // got dropped outside client area

	// Use pt to mark where the drop was
	m_DragToPoint.x = pt.x;
	m_DragToPoint.y = pt.y;
	ClientToScreen(&m_DragToPoint);
	// Get the item from the tree
	m_DragToChange = GetDropHilightItem();
	if (m_DragToChange != NULL)
		m_DragToChangeNum = GetChangeNumber(m_DragToChange);
	else
	{
		m_DragToChangeNum = 0;
		m_DragToChange = m_MyDefault;
	}
	XTRACE(_T("OnDropFiles change=%d\n"), m_DragToChangeNum);
	
	// Get the actual file list
	UINT numFiles=DragQueryFile(hDropInfo, index, buf, bufsize);
	for(index=0; index<numFiles; index++)
	{
		DragQueryFile(hDropInfo, index, buf, bufsize);
		list.AddHead(buf);
	}

	// Release the memory allocated for the drag-drop operation
	DragFinish(hDropInfo);

	// And perform the add operation
	if(numFiles > 0)
		AddFileList(m_DragToChangeNum, &list, TRUE);
}


BOOL CDeltaTreeCtrl::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	STGMEDIUM stg;
	BOOL success=FALSE;
	CString fname;
	HTREEITEM item;

	if(SERVER_BUSY() || m_EditInProgress)
	{
		// OnDragEnter() and OnDragOver() should avoid a drop at 
		// the wrong time!
		ASSERT(0);
		return FALSE;
	}
	
	// OnDragOver has already updated current selection
	m_DragToPoint.x=point.x;
	m_DragToPoint.y=point.y;
	ClientToScreen(&m_DragToPoint);
	m_DragToChange=GetDropHilightItem();

	if(m_DragToChange !=NULL)
		m_DragToChangeNum=GetChangeNumber(m_DragToChange);
	else
	{
		m_DragToChangeNum=0;
		m_DragToChange=m_MyDefault;
	}

	XTRACE(_T("OnDrop change=%d\n"), m_DragToChangeNum);

	m_DeltaIsDropTarget = TRUE;
	if(m_DragDataFormat == m_CF_DELTA)
	{
		// File(s) and or job(s) being being moved from one change to here
		if(m_DragLastHighlite == NULL)
			success=TRUE;  // No work to do since dropped into originating change (but must say we did work to prevent unnecessary fstat!)
		else
		{
			// Build a list of changes and a list of jobs
			m_DroppedFileList.RemoveAll();
			m_DroppedJobList.RemoveAll();
			for(int i=GetSelectedCount()-1; i>=0; i--)
			{
				item=GetSelectedItem(i);
				if(!IsAFile(item))
					m_DroppedJobList.AddHead(GetItemText(item));
				else
				{
					fname=GetItemText(item);
					fname=fname.Left(fname.ReverseFind(_T('#')));
					m_DroppedFileList.AddHead(fname);
				}
			}
			
			PostMessage(WM_GOTMOVELISTS, 0, 0);
			success=TRUE;
		}
	}
	else if (m_DragDataFormat == m_CF_DEPOT)
	{
		// File(s) being added from depot to this window - work will be done by
		// CDepotView. But do let CDepotView know it was dropped in CDeltaTreeCtrl.
		::SendMessage(m_depotWnd, WM_DROPTARGET, PENDINGCHG, 
						MAKELPARAM(m_DragToPoint.x, m_DragToPoint.y));
		success=TRUE;
	}
	else if (m_DragDataFormat == m_CF_JOB)
	{
		m_EditChangeNum = m_DragToChangeNum;
		if (m_EditChangeNum)
		{
			CStringList jobnames;
			jobnames.AddHead(MainFrame()->GetDragFromJob());
			AddJobFixes(&jobnames, NULL);
			success=TRUE;
		}
		else
		{
			AddToStatus(LoadStringResource(IDS_CANT_ADD_JOB_TO_DEFAULT_CHG), SV_WARNING);
			success=FALSE;
		}
	}
	else if(m_DragDataFormat == CF_HDROP)
	{
		m_DroppedFileList.RemoveAll();

		// Its a list of files from MS Exploder
		if(pDataObject->GetData(CF_HDROP, &stg, NULL))
		{
			if(stg.tymed==TYMED_HGLOBAL)
			{
				// Notes: 
				// 1) df.pFiles is offset in bytes to LPCWSTR filename list, a sequence of
				//    wide char null terminated strings followed by an additional null char
				// 2) fortunately, CString assignment operator will eat unicode and conver to chars
				// 3) just grab the info and then release the drag operation so exploder isnt frozen
				//    a WM_GOTDROPLIST posted back to ourselves handles the list processing
						
				void *buf=GlobalLock(stg.hGlobal);
				_DROPFILES *df=(DROPFILES *) buf;
							
				MainFrame()->UpdateStatus( LoadStringResource(IDS_ENUMERATING_FILES), TRUE );

				// NT and Win95 will provide unicode and bytechars, respectively, for drop list
				if(df->fWide)  
				{
					LPCWSTR tPtr=(LPCWSTR) ((char *)buf+df->pFiles);  // points to first filename
					while(*tPtr != 0)  // Not at double null terminator
					{
						// Add to list, do NOT recurse directories here so gui doesnt
						// get frozen w/ large recurse operations
#ifdef UNICODE
						fname=tPtr;
                        tPtr += lstrlen(tPtr)+1;
#else
                        int bufSize = WideCharToMultiByte(CP_ACP, 0, tPtr, -1, 0, 0, 0, 0);
                        LPTSTR buf = fname.GetBufferSetLength(bufSize);
                        WideCharToMultiByte(CP_ACP, 0, tPtr, -1, buf, bufSize, 0, 0);
                        fname.ReleaseBuffer();
                        tPtr += wcslen(tPtr)+1;
#endif
						m_DroppedFileList.AddHead(fname);
					}
				}
				else
				{
					LPCSTR tPtr=(LPCSTR) ((char *)buf+df->pFiles);  // points to first filename
					while(*tPtr != 0)  // Not at double null terminator
					{
						// Add to list, do NOT recurse directories here so gui doesnt
						// get frozen w/ large recurse operations
						fname=tPtr;
						m_DroppedFileList.AddHead(fname);
						tPtr+=(fname.GetLength()+1);  // Advance a number of wide chars
					}
				}
							
				success=TRUE;
			}
			ReleaseStgMedium(&stg);
			// Post a message so we can return from this OLE nightmare
			// before processing the file list
			if(m_DroppedFileList.GetCount())
				PostMessage(WM_OLEADDFILES, 0, 0);
			else
               	MainFrame()->ClearStatus();
		}
	}
	else
		ASSERT(0);  // Dropped unknown data somehow
	
	m_DeltaIsDropTarget = FALSE;
	// Dont leave drop target selected
	SelectDropTarget(NULL);
	return success;
}

// A message handler to queue file enumeration work after an OLE drag-drop
// operation, allowing the OLE operation to complete before we start this
LRESULT CDeltaTreeCtrl::OnOLEAddFiles(WPARAM wParam, LPARAM lParam)
{
	AddFileList( m_DragToChangeNum, &m_DroppedFileList, TRUE);
	return 0;
}

LRESULT CDeltaTreeCtrl::OnGotMoveLists(WPARAM wParam, LPARAM lParam)
{
	
	if(m_DroppedJobList.GetCount() > 0)
	{
		AfxMessageBox(IDS_UNABLE_TO_DRAG_JOBS_BETWEEN_CHANGES, MB_ICONINFORMATION);
	}
	
	if(m_DroppedFileList.GetCount() > 0)
	{
		CCmd_ListOpStat *pCmd= new CCmd_ListOpStat;
		pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
		if (m_Need2Refresh)
			pCmd->SetHitMaxFileSeeks(TRUE);	// we will need to do a full refresh at the end
		if( pCmd->Run( &m_DroppedFileList, P4REOPEN, m_DragToChangeNum ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_REOPENING_FILES) );
		else
			delete pCmd;
	}
	
	return 0;
}
			
BOOL CDeltaTreeCtrl::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll, BOOL *bScrolled) 
{
	BYTE vScroll = HIBYTE(nScrollCode);
	
	if(m_DropEffect==DROPEFFECT_COPY || m_DropEffect==DROPEFFECT_MOVE)
	{
		if(vScroll==0) // LINEUP
		{
			*bScrolled = ScrollTree(1);
			return TRUE;
		}
		else if(vScroll==1)  //LINEDOWN
		{
			*bScrolled = ScrollTree(-1);
			return TRUE;
		}
	}
	return FALSE;
}

// Utility function to fill a stringlist with the names
// of all of My Changes, for use in file add and integrate dialogs
void CDeltaTreeCtrl::GetMyChangesList(CStringList *changeList)
{
	if (m_EditInProgress)
	{
		changeList->AddHead(LoadStringResource(IDS_DEFAULTCHANGELISTNAME));
		return;
	}

    CString changeLabel = LoadStringResource(IDS_CHANGE);
	int lgthChgLabel = changeLabel.GetLength();
	HTREEITEM item=GetChildItem(m_MyRoot);
	while(item !=NULL)
	{
		CString changeName= GetItemText(item);
		if(changeName.Find(changeLabel)!= -1)
		{
			changeName=changeName.Mid(lgthChgLabel);
			changeName.TrimLeft();
			int blank=changeName.Find(_T(' '));
			if(blank != -1)
			{
				// check for different user: "NNNN - otheruser@myclient {Description ...}"
				if ((changeName.Find(_T(" - ")) == blank) && (changeName.Find(_T('@')) != -1))
				{
					item=GetNextSiblingItem(item);	// if other user, skip it - can't use it
					continue;
				}
				changeName.Replace(_T('{'), _T(' '));
				changeName.TrimRight(_T(" }"));
			}
		}
		else if (changeName == LoadStringResource(IDS_DEFAULTCHANGELISTNAME))
		{
			changeList->AddHead(LoadStringResource(IDS_DEFAULTCHANGELISTNAME));
			changeName = LoadStringResource(IDS_NEWCHANGELISTNAME);
		}
		else // if not a numbered change and not my Default - must be another user's Default
		{
			item=GetNextSiblingItem(item);	// skip it - can't use it
			continue;
		}
		changeList->AddHead(changeName);
		item=GetNextSiblingItem(item);
	}
}


// Utility function will all of my changelists that have no files
// This will result in the + going away for empty changelists
void CDeltaTreeCtrl::ExpandEmptyChglists()
{
    CString changeLabel = LoadStringResource(IDS_CHANGE);
	HTREEITEM item=GetChildItem(m_MyRoot);
	while(item !=NULL)
	{
		CString changeName= GetItemText(item);
		if(changeName.Find(changeLabel)!= -1)
		{
			changeName=changeName.Mid(7);
			changeName.TrimLeft();
			int blank=changeName.Find(_T(' '));
			if(blank != -1)
			{
				// check for different user: "NNNN - otheruser@myclient {Description ...}"
				if ((changeName.Find(_T(" - ")) == blank) && (changeName.Find(_T('@')) != -1))
				{
					item=GetNextSiblingItem(item);	// if other user, skip it - only expand ours
					continue;
				}
			}
		}
		else	// not a numbered change
		{
			item=GetNextSiblingItem(item);
			continue;
		}
		if (!GetChildItem(item))
			ExpandTree(item);
		item=GetNextSiblingItem(item);
	}
}


long CDeltaTreeCtrl::GetChangeNumber(HTREEITEM item)
{
	ASSERT(item != NULL);

 	BOOL underMyRoot;
	if ((GetItemLevel(item, &underMyRoot) == 2) && IsAFile(item))
		item = GetParentItem(item);

	long changeNo= -1L;
	
	CString itemStr= GetItemText(item);
	if(itemStr.Find(LoadStringResource(IDS_DEFAULTCHANGELISTNAME)) == 0)
		changeNo= 0;
	else if (GET_P4REGPTR()->SortChgsByUser() 
	      && ((itemStr.Find(_T(" -  Default")) + (int)(sizeof(_T(" -  Default"))/sizeof(TCHAR) - 1)) == itemStr.GetLength()))
		changeNo= 0;
	else
	{
		int i;
		CString ChgLit = LoadStringResource(IDS_CHANGE_n);
		i = ChgLit.Find(_T('%'));
		ASSERT(i != -1);
		ChgLit = ChgLit.Left(i);
		ChgLit.TrimRight();
		i = itemStr.Find(ChgLit);
		i += (i == -1) ? 1 : ChgLit.GetLength();
		for(; i< itemStr.GetLength(); i++)
		{
			if(_istdigit(itemStr[i]))
				 break;
		}
		if(i<itemStr.GetLength())
		{	
			itemStr=itemStr.Mid(i);
			changeNo=_ttol(itemStr);
		}
	}
	// Shouldnt be calling this function unless user was on a change!
	ASSERT(changeNo >= 0);
	return changeNo;
}

long CDeltaTreeCtrl::GetSelectedChangeNumber()
{
	HTREEITEM item=GetLastSelection();
	return GetChangeNumber(item);
}

void CDeltaTreeCtrl::OnChangeDel() 
{
	HTREEITEM item=GetLastSelection();  
			
	if (item == NULL)
		{ ASSERT(0); return; }

	long changeNo=GetSelectedChangeNumber();

	if(changeNo == 0)
	{
		AfxMessageBox(IDS_DEFAULT_CHANGELIST_MAY_NOT_BE_DELETED, MB_ICONEXCLAMATION);
		return;
	}
	
	CString txt;
	txt.FormatMessage(IDS_ARE_YOU_SURE_YOU_WANT_TO_DELETE_CHANGELIST_n, changeNo);
	if(AfxMessageBox(txt, MB_ICONQUESTION|MB_YESNO) != IDYES)
		return;

	CCmd_Delete *pCmd= new CCmd_Delete;
	txt.Format(_T("%ld"), changeNo);
	pCmd->Init( m_hWnd, RUN_ASYNC);
	if( pCmd->Run( P4CHANGE_DEL, txt ) )
	{
		m_ActiveItem=item;
		txt.FormatMessage(IDS_DELETING_n, changeNo);
		MainFrame()->UpdateStatus(txt);
	}	
	else
		delete pCmd;
}


void CDeltaTreeCtrl::OnChangeDescribe() 
{
	HTREEITEM item=GetLastSelection();  
			
	if (item == NULL)
		{ ASSERT(0); return; }

	m_EditChange= item;
	long changeNumber= GetSelectedChangeNumber();
	
	if(changeNumber > 0)
	{
		CString changeTxt;
		changeTxt.Format(_T("%ld"), changeNumber);

		CCmd_Describe *pCmd= new CCmd_Describe;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		if( pCmd->Run( P4DESCRIBE, changeTxt) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_FETCHING_CHANGELIST_DESCRIPTION) );
		else
			delete pCmd;
	}
}

LRESULT CDeltaTreeCtrl::OnP4Describe(WPARAM wParam, LPARAM lParam)
{
	CCmd_Describe *pCmd= (CCmd_Describe *) wParam;

	if(pCmd->GetSpecType() != P4JOB_SPEC)
		return OnP4ChangeDescribe( wParam, lParam);
	else
		return OnP4JobDescribe( wParam, lParam);
}

LRESULT CDeltaTreeCtrl::OnP4EndDescribe(WPARAM wParam, LPARAM lParam)
{
	CSpecDescDlg *dlg = (CSpecDescDlg *)lParam;

	if(dlg->GetViewType() != P4JOB_SPEC)
		return OnP4EndChgDescribe( wParam, lParam);
	else
		return OnP4EndJobDescribe( wParam, lParam);
}

LRESULT CDeltaTreeCtrl::OnP4ChangeDescribe(WPARAM wParam, LPARAM lParam)
{
	CCmd_Describe *pCmd= (CCmd_Describe *) wParam;

	if(!pCmd->GetError())
	{
		// Get the descriptive text
		CString desc= MakeCRs(pCmd->GetDescription());
		
		// If we are talking to a pre 2002.2 server,
		// Then add the section about affected files
		if (GET_SERVERLEVEL() < 14)
		{
			HTREEITEM child= GetChildItem(m_EditChange);
			if( child != NULL && IsAFile(child) )
			{
				desc += LoadStringResource(IDS_AFFECTEDFILES);
			}

			CString fileText;
			while( child != NULL )
			{
				if( IsAFile( child ) )
				{
					CP4FileStats *fs= (CP4FileStats *) GetLParam(child);
					if( fs->IsMyOpen() )
						fileText.Format(_T("\r\n%s#%d %s"), fs->GetFullDepotPath(),
													fs->GetHaveRev(),
													fs->GetActionStr( fs->GetMyOpenAction() ) );
					else
						fileText.Format(_T("\r\n%s#%d %s"), fs->GetFullDepotPath(),
													fs->GetHaveRev(),
													fs->GetActionStr( fs->GetOtherOpenAction() ) );

					// do reallocs in large chunks, rather than letting CString::operator +=() do it
					// in little bits.  This makes a huge speed difference if the number if items is large.
					if(desc.GetLength() + fileText.GetLength() + 1 > desc.GetAllocLength())
					{
						// grow the buffer 16k at a time, adjusting the first time to get an even 4k multiple
						int newSize = desc.GetAllocLength() + 16384;
						newSize -= newSize % 16384;
						desc.GetBuffer(newSize);
						desc.ReleaseBuffer();
					}

					desc+= fileText;
				}
				child= GetNextSiblingItem(child);
			}
		}

		int key;
		CSpecDescDlg *dlg = new CSpecDescDlg(this);
		dlg->SetIsModeless(TRUE);
		dlg->SetKey(key = pCmd->HaveServerLock()? pCmd->GetServerKey() : 0);
		dlg->SetItemName(pCmd->GetReference());
		dlg->SetDescription(desc);
		dlg->SetCaption(LoadStringResource(IDS_PERFORCE_CHANGELIST_DESCRIPTION));
		dlg->SetShowEditBtn(!key && IsMyPendingChange(m_EditChange));
		dlg->SetViewType(P4CHANGE_SPEC);
		if (!dlg->Create(IDD_SPECDESC, this))	// display the description dialog box
		{
			dlg->DestroyWindow();	// some error! clean up
			delete dlg;
		}
	}

	delete pCmd;
   	MainFrame()->ClearStatus();
	return 0;
}

LRESULT CDeltaTreeCtrl::OnP4EndChgDescribe( WPARAM wParam, LPARAM lParam )
{
	CSpecDescDlg *dlg = (CSpecDescDlg *)lParam;

	if (wParam == IDC_EDITIT)	// which button did they click to close the box?
	{
		CString ref = dlg->GetItemName();
		ASSERT(!ref.IsEmpty());

		int i;
		BOOL found = FALSE;
		long chgnbr = _ttol(ref);

		// find the HTREEITEM - user might have chged the selection
		HTREEITEM item=GetChildItem(m_MyRoot);
		while(item !=NULL && !found)
		{
			CString testtext=GetItemText(item);
			if ((i = testtext.Find(' ')) != -1)
			{
				testtext = testtext.Mid(i);
				testtext.TrimLeft();
				if ((i = testtext.Find(' ')) != -1)
					testtext = testtext.Left(i);
			}
			if(lstrcmp(ref, testtext) == 0)
			{
				found=TRUE;
				break;
			}
			item=GetNextSiblingItem(item);
		}
		if (found)
		{
			m_ChangeIsSelected=TRUE;
			m_EnableChangeSubmit=FALSE;
			m_SubmitOnlyChged=m_SubmitOnlySeled=FALSE;
			ChangeEdit(chgnbr, item);
		}
		else
		{
			CString txt;
			txt.FormatMessage(IDS_SE_ERR_FNF, ref);
			AddToStatus(txt, SV_WARNING);
		}
	}
	dlg->DestroyWindow();
	return TRUE;
}

// Following handlers for change spec editing and submitting
void CDeltaTreeCtrl::OnChangeSubmit() 
{
	m_ChangeIsSelected=TRUE;
	m_EnableChangeSubmit=TRUE;
	m_SubmitOnlyChged=GET_P4REGPTR()->GetSubmitOnlyChged();
	m_SubmitOnlySeled=IsSelectionInSubmittableChange();
	ChangeEdit();
}

void CDeltaTreeCtrl::OnChangeEdspec() 
{
	m_ChangeIsSelected=TRUE;
	m_EnableChangeSubmit=FALSE;
	m_SubmitOnlyChged=m_SubmitOnlySeled=FALSE;
	ChangeEdit();
}

void CDeltaTreeCtrl::OnChangeNew()
{
	m_ChangeIsSelected=FALSE;
	m_EnableChangeSubmit=FALSE;
	m_SubmitOnlyChged=m_SubmitOnlySeled=FALSE;
	ChangeEdit();
}

void CDeltaTreeCtrl::CallOnChangeNew()
{
	OnChangeNew();
}

void CDeltaTreeCtrl::ChangeEdit(long chgnum /*= -1*/, HTREEITEM chgItem /*= 0*/)
{
	if (m_EditInProgress)
	{
		CantDoItRightNow(IDS_EDITSTRING);
		return;
	}

	HTREEITEM item;

	// Get the relevant change number and the list of files under the change
	if (chgnum != -1)
	{
		m_EditChangeNum = chgnum;
		m_EditChange = chgItem;
	}
	else if(m_ChangeIsSelected)
	{
		item=GetLastSelection();  
		if(!IsMyPendingChange(item))
			item=GetParentItem(item);

		if (item == NULL)
			{ ASSERT(0); return; }

		m_EditChangeNum=GetChangeNumber(item);

		if(m_EditChangeNum < 0L)
			{ ASSERT(0); return; }

		m_EditChange=item;
	}
	else
	{
		m_EditChangeNum=0;
		m_EditChange=m_MyDefault;
	}

    // Empty lists
	m_FileList.RemoveAll();
	m_SelectionList.RemoveAll();
	m_FileListDefinitive = FALSE;
	
	// If they want to only set the check on changed files,
	// we must get a list of edited files into m_FileList.
	// Also if they are submitting a selection of files from the chglist
	// we must get a list of the selected files into m_SelectionList
	if (m_EnableChangeSubmit 
	 && (m_SubmitOnlyChged || m_SubmitOnlySeled || GET_SERVERLEVEL() >= 21))
	{
		CString fileName;
		int files=0;

		BOOL b20051 = FALSE;
		CP4Command *pcmd = 0;
		CGuiClient *client = 0;
		if (GET_SERVERLEVEL() >= 19)		// 2005.1 or later?
		{
			Error e;
			pcmd = new CP4Command;
			client = pcmd->GetClient();
			client->SetTrans();
			client->Init(&e);
			if( !e.Test() )
				b20051 = TRUE;
			else
				delete pcmd;
		}

		item=GetChildItem(m_EditChange);
		while(item!=NULL)
		{
			if(IsAFile(item))
			{
				fileName=GetItemText(item);
				if ((m_SubmitOnlyChged || GET_SERVERLEVEL() >= 21)	//always build list for 6.1+
				 && (!m_SubmitOnlySeled || IsSelected(item)))
				{
					if (fileName.Find(_T("<edit>")) != -1)
					{
						if (b20051)	// working with good 2005.1 or later server?
						{
							LPARAM lParam=GetLParam(item);
							CP4FileStats *stats = (CP4FileStats *) lParam;
							if (TheApp()->digestIsSame(stats, FALSE, client)
							 && stats->GetType() == stats->GetHeadType())
								m_FileList.AddTail(stats->GetFullDepotPath());
						}
						else
						{
							m_FileList.AddTail(fileName.Left(fileName.ReverseFind(_T('#'))));
							++files;
						}
					}
					if ((files > 32000) && !m_SubmitOnlySeled)
						break;
				}
				if (m_SubmitOnlySeled && IsSelected(item))
					m_SelectionList.AddTail(fileName.Left(fileName.ReverseFind(_T('#'))));
			}
			item=GetNextSiblingItem(item);
		}
		if (b20051)
		{
			delete pcmd;
			m_FileListDefinitive = TRUE;
			RunChangeEdit(0);
			return;
		}
		if (m_FileList.GetCount())
		{
		  if (GET_SERVERLEVEL() >= 14)	// 2002.2 or later?
		  {
			if (files <= 32000)
			{
				CCmd_Revert *pCmd= new CCmd_Revert;
				pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK);
				pCmd->SetAlternateReplyMsg( WM_P4DIFFCHANGEEDIT );
				CString chgnbr;
				if (m_EditChangeNum)
					chgnbr.Format(_T("%ld"), m_EditChangeNum);
				else
					chgnbr = _T("default");
				if( pCmd->Run( chgnbr, TRUE, TRUE, TRUE ) )
				{
					MainFrame()->UpdateStatus(LoadStringResource(IDS_RUNNING_DIFF));
					return;
				}
				else
					delete pCmd;
			}
			else
			{
				CString msg;
				msg.FormatMessage(IDS_TOOMANYFILE4SUBMITONLYCHGED, files);
				if (IDNO == AfxMessageBox(msg, MB_YESNO | MB_ICONEXCLAMATION))
					return;
			}
		  }
		}
	}
	RunChangeEdit(0);
}

LRESULT CDeltaTreeCtrl::OnP4DiffChangeEdit(WPARAM wParam, LPARAM lParam)
{
	CCmd_Revert *pCmd= (CCmd_Revert *) wParam;
    
	if(!pCmd->GetError())
	{
		m_FileList.RemoveAll();
		CStringList *list= pCmd->GetFileList();
		POSITION pos;
	
		if(!list->IsEmpty())  // Some filenames in the list
		{
			int i;
			for(pos=list->GetHeadPosition(); pos!=NULL;)
			{
				CString str = list->GetNext(pos);
				if (str.Find(_T(", not reverted")) != -1)
					continue;
				if ((i = str.Find(_T('#'))) != -1)
					str = str.Left(i);
				m_FileList.AddHead(str);
			}
        }
		m_FileListDefinitive = TRUE;
	}
	int key= pCmd->GetServerKey();
	delete pCmd;
	RunChangeEdit(key);
	return 0;
}

void CDeltaTreeCtrl::RunChangeEdit(int key)
{
	// At this point, context has been saved in m_EditChangeNum, m_EnableChangeSubmit,
	// and m_FileList members. So start the spec edit process.  The dialog
    // is invoked by CCmd_EditSpec

	MainFrame()->UpdateStatus( LoadStringResource(IDS_RETRIEVING_CHANGELIST_SPEC) );
	CCmd_EditSpec *pCmd= new CCmd_EditSpec;
    pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key );
    if( pCmd->Run( m_EditChangeNum, m_EnableChangeSubmit, FALSE, m_SubmitOnlyChged, 
																 m_SubmitOnlySeled) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_EDITING_CHANGELIST_SPEC) );	
	else
		delete pCmd;
}

BOOL CDeltaTreeCtrl::IsAMemeberOfFileList(CString &fileName)
{
	for (POSITION pos= m_FileList.GetHeadPosition(); pos!=NULL; )
		if (m_FileList.GetNext( pos ) == fileName)
			return TRUE;
	return FALSE;
}

BOOL CDeltaTreeCtrl::IsAMemeberOfSelectionList(CString &fileName)
{
	for (POSITION pos= m_SelectionList.GetHeadPosition(); pos!=NULL; )
		if (m_SelectionList.GetNext( pos ) == fileName)
			return TRUE;
	return FALSE;
}


// TODO:  allow this function to be called for selected file level in addition to
//        change level
void CDeltaTreeCtrl::OnChangeRevorig() 
{
    if( SERVER_BUSY() )
    {
        ASSERT(0);
        return;
    }

	HTREEITEM currItem=GetLastSelection();  
	
	// no selected item - menu enables should have prevented
	if (currItem == NULL)
		{ ASSERT(0); return; }

	// selected item not one of my changes - fix menu enables should have prevented
	if(GetParentItem(currItem) != m_MyRoot)
		{ ASSERT(0); return; }

	// Empty list
	m_StringList.RemoveAll();
	
	SET_BUSYCURSOR();
	MainFrame()->UpdateStatus(LoadStringResource(IDS_DIFFFILES));

	int files=0;

	HTREEITEM item=GetChildItem(currItem);
	while(item!=NULL)
	{
		if(IsAFile(item))
		{
			if(++files > 32000)
			{	
				AfxMessageBox(IDS_UNABLE_TO_DIFF_MORE_THAN_32000_FILES, MB_ICONEXCLAMATION);
				return;
			}
			if (GET_SERVERLEVEL() < 14)	// pre 2002.2?
			{
				CString fileName=GetItemText(item);
				fileName=fileName.Left(fileName.ReverseFind(_T('#')));  // Strip revision number
				m_StringList.AddTail(fileName);
			}
		}
		item=GetNextSiblingItem(item);
	}

	if (GET_SERVERLEVEL() >= 14)	// 2002.2 or later?  If so we can use p4 revert -an
	{
		CCmd_Revert *pCmd= new CCmd_Revert;
		pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK);
		CString chgnbr;
		m_EditChangeNum=GetChangeNumber(currItem);
		if (m_EditChangeNum)
			chgnbr.Format(_T("%ld"), m_EditChangeNum);
		else
			chgnbr = _T("default");
		if( pCmd->Run( chgnbr, TRUE, TRUE, TRUE ) )
			m_DoRevert=TRUE;
		else
		{
			MainFrame()->ClearStatus();
			delete pCmd;
		}
		return;
	}

	CCmd_Diff *pCmd= new CCmd_Diff;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK);
	if( pCmd->Run( &m_StringList, _T("-sr") ) )
	{
		if(files > 5)
			MainFrame()->UpdateStatus(LoadStringResource(IDS_RUNNING_MASSIVE_DIFF));
		else
			MainFrame()->UpdateStatus(LoadStringResource(IDS_RUNNING_DIFF));

		m_DoRevert=TRUE;
	}
	else
		delete pCmd;
}

void CDeltaTreeCtrl::OnFileDiff() 
{
	// should always have something selected
	if(GetSelectedCount()==0)
	{
		ASSERT(0);
		return;
	}

	// see if it's a chglist or file(s) that's selected
	HTREEITEM initialItem = GetSelectedItem(0);
	BOOL root;
	int level = GetItemLevel(initialItem, &root);
	if (level == 1)
	{
		// a chglist is selected, so select its files
		if (GET_SERVERLEVEL() >= 19)	// 2005.1 or later? Then select only chged files
		{
			int tot;
			if (SelectChgUnchg(TRUE, &tot))		// any file(s) get selected?
			{
				int i = tot - GetSelectedCount();	// compute nbr not selected (i.e. unchged)
				if (i)								// any unchanged? if so, tell user
				{
					CString txt;
					if(i == 1)
						txt.FormatMessage(IDS_ONECLIENTFILEDOESNOTDIFFER);
					else
						txt.FormatMessage(IDS_SEVERALCLIENTFILESDONOTDIFFER_n, i);
					AddToStatus(txt, SV_MSG);
				}
			}
			else	// all unchanged; tell user and return
			{
				AddToStatus(LoadStringResource(IDS_NONE_OF_THE_SELECTED_CLIENT_FILES_DIFFER), SV_COMPLETION);
				return;
			}
		}
		else
			SelectAllFilesInChange(initialItem, 0);	// older server: Sellect all the chglist files
		if (GetSelectedCount() < 1)
		{
			UnselectAll();
			SetSelectState(initialItem, TRUE);
			return;
		}
	}

	// get a stringlist of files and deal with any adds
	BOOL bFoundAdd = AssembleStringList( NULL, FALSE );
	if (bFoundAdd)
	{
		if ((level != 1) || m_StringList.IsEmpty())
			AddToStatus(LoadStringResource(IDS_CANTDIFFADDEDFILES), SV_WARNING);
		if (m_StringList.IsEmpty())
			return;
	}

	// see if we are diffing "a whole buncha files"
	if (m_StringList.GetCount() > _ttoi(GET_P4REGPTR()->GetWarnLimitDiff()))
	{
		CString txt;
		txt.FormatMessage(IDS_DIFF_WARNLIMIT_EXCEEDED_d, m_StringList.GetCount());
		if (IDYES != AfxMessageBox(txt, MB_YESNO))
			return;
	}

	// If the server is busy because we triggered an expand of a changelist
	// and are getting the attached jobs, wait for the server to finish
	if (level == 1)
	{
		UnselectAll();
		SetSelectState(initialItem, TRUE);	// reselect original chglist
		if (SERVER_BUSY())
		{
			int t=GET_P4REGPTR()->BusyWaitTime();
			do
			{
				Sleep(50);
				t -= 50;
			} while (SERVER_BUSY() && t > 0);
		}
	}

	// finally run the diff command against the selected file(s)
	m_DoRevert=FALSE;
	CCmd_Diff *pCmd= new CCmd_Diff;
	pCmd->Init( m_hWnd, RUN_ASYNC);
	if( pCmd->Run( &m_StringList ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_DIFFING_FILE) );
	else
		delete pCmd;
}

void CDeltaTreeCtrl::OnFiletype() 
{
	HTREEITEM item=GetLastSelection();  
	
	if (item == NULL)
		ASSERT(0);
	else if (!IsAFile(item))
		AfxMessageBox(IDS_CANTCHGFILETYPE4JOB);
	else
	{
		CFileType dlg;
		CP4FileStats *stats;
		stats= (CP4FileStats *) GetLParam(item);
		dlg.m_itemStr = stats->GetFormattedChangeFile(TRUE, TRUE);
		int i=GetSelectedCount();
		if (i > 1)
		{
			CString str = GetItemText(item);
			str = str.Mid(str.ReverseFind(_T('#')));
			str = str.Mid(str.Find(_T("<")));
			str = str.Left(str.Find(_T(">")));
			while(--i >= 0)
			{
				item=GetSelectedItem(i);
				if(!IsAFile(item))
					continue;
				CString fname=GetItemText(item);
				CString type =fname.Mid(fname.ReverseFind(_T('#')));
				type = type.Mid(type.Find(_T("<")));
				type = type.Left(type.Find(_T(">")));
				if (type != str)
				{
					dlg.m_Action = 1;
					break;
				}
			}
		}
		if (dlg.DoModal() == IDOK)
			ReopenAs(dlg.m_fileType);
	}
}

void CDeltaTreeCtrl::OnMoveFiles() 
{
	HTREEITEM item=GetLastSelection();  
	
	if (item == NULL)
		ASSERT(0);
	else
	{
		CMoveFiles dlg;
		// Get a list of my changes
		CStringList list;
		GetMyChangesList(&list);
		// Get current change item and save in m_DragFromChange
		HTREEITEM currentItem=GetLastSelection();
		m_DragFromChange=GetParentItem(currentItem);
		// Get current change number and convert to string
		long changeNo=GetChangeNumber(m_DragFromChange);
		CString curChg;
		if (!changeNo)
			curChg = LoadStringResource(IDS_CHANGE_DEFAULT);
		else
			curChg.Format(_T("%ld"), changeNo);
		// Remove current change number from list
		// as the list is copied to the dlg
		POSITION pos;
		for( pos = list.GetHeadPosition(); pos != NULL; )
		{
			int i;
			CString changeStr = dlg.m_ChangeList.GetNext( pos );
			CString chg = ((i = changeStr.Find(_T(' '), 2)) != -1) 
						? changeStr.Left(i) : changeStr;
			if (chg != curChg)
				dlg.m_ChangeList.AddHead(changeStr);
		}

		// display the dialog
		if (dlg.DoModal() == IDOK)
		{
			// Save the change number to move to
			m_DragToChangeNum=dlg.m_SelectedChange;
			m_DragToChange = m_DragToChangeNum ? FindChange(m_DragToChangeNum) : m_MyDefault;
			m_Need2Refresh = m_DragToChange ? dlg.m_Need2Refresh : TRUE;

			// Build a list of files and a list of jobs
			m_DroppedFileList.RemoveAll();
			m_DroppedJobList.RemoveAll();
			for(int i=GetSelectedCount()-1; i>=0; i--)
			{
				item=GetSelectedItem(i);
				if(!IsAFile(item))
					m_DroppedJobList.AddHead(GetItemText(item));
				else
				{
					CString fname=GetItemText(item);
					fname=fname.Left(fname.ReverseFind(_T('#')));
					m_DroppedFileList.AddHead(fname);
				}
			}
			// And finally do the actual move
			OnGotMoveLists(0, 0);
		}
	}
}


/*
	_________________________________________________________________
*/

void CDeltaTreeCtrl::OnFileLock() 
{
	LockOrUnlock( P4LOCK );
}

void CDeltaTreeCtrl::OnFileUnlock() 
{
	LockOrUnlock( P4UNLOCK );
}

void CDeltaTreeCtrl::LockOrUnlock( int which ) 
{
	if(GetSelectedCount()==0)
	{
		ASSERT(0);
		return;
	}

	AssembleStringList( );

	// Run the command for the depot window
	CCmd_ListOpStat *pCmd= new CCmd_ListOpStat;
	pCmd->Init( m_depotWnd, RUN_ASYNC, HOLD_LOCK );
	if( pCmd->Run( &m_StringList, which ) )
        MainFrame()->UpdateStatus( LoadStringResource(which == P4LOCK ? 
            IDS_REQUESTINGLOCK : IDS_REQUESTINGUNLOCK));
	else
		delete pCmd;
}


/*
	_________________________________________________________________
*/

void CDeltaTreeCtrl::OnUpdateFileSchedule(CCmdUI* pCmdUI) 
{
	BOOL root;
	BOOL rc = FALSE;
	if (!SERVER_BUSY() && GetSelectedCount() > 0)
	{
		int level = GetItemLevel(GetSelectedItem(0), &root);
		if (level == 2)
			rc = (IsMyPendingChangeFile( GetSelectedItem(0) ) && IsAFile( GetSelectedItem(0)) );
		else if (level == 1)
			rc = (IsMyPendingChange(GetSelectedItem(0)) 
					&& HasChildren(GetSelectedItem(0)) 
					&& GetSelectedCount()==1);
	}
	pCmdUI->Enable(rc);
}

void CDeltaTreeCtrl::OnFileGetWhatIf() 
{
	FileGet(TRUE);
}

void CDeltaTreeCtrl::OnFileGet()
{
	FileGet(FALSE);
}

void CDeltaTreeCtrl::FileGet(BOOL whatIf)
{
	BOOL root;
	if(GetSelectedCount()==0)
	{
		ASSERT(0);
		return;
	}

	if (GetItemLevel(GetSelectedItem(0), &root) == 2)
		AssembleStringList( );
	else
	{
		int files = 0;
		CString fileName;
		m_StringList.RemoveAll();
		HTREEITEM item=GetChildItem(GetSelectedItem(0));
		while(item!=NULL)
		{
			if(IsAFile(item))
			{
				fileName=GetItemText(item);
				fileName=fileName.Left(fileName.ReverseFind(_T('#')));  // Strip revision number
				m_StringList.AddTail(fileName);
				if(files++ > 32000)
				{	
					AfxMessageBox(IDS_UNABLE_TO_RESOLVE_MORE_THAN_32000_FILES, MB_ICONEXCLAMATION);
					return;
				}
			}
			item=GetNextSiblingItem(item);
		}
	}

	CCmd_Get *pCmd= new CCmd_Get;
	pCmd->Init( m_depotWnd, RUN_ASYNC);
	if( pCmd->Run( &m_StringList, whatIf ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_FILE_SYNC) );
	else
		delete pCmd;
}

void CDeltaTreeCtrl::OnFileRevisionhistory() 
{
	HTREEITEM currentItem=GetLastSelection();  
	if( IsAFile(currentItem) && !SERVER_BUSY() )
	{
		CString fname= GetItemText( currentItem );
	
		int pound= fname.Find(_T('#'));
		if( pound != -1 )
			fname= fname.Left( pound );

				
		CCmd_History *pCmd= new CCmd_History;
		pCmd->Init( m_depotWnd, RUN_ASYNC);
		pCmd->SetCallingWnd(m_hWnd);
		if( pCmd->Run(fname) )
		{
			MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_HISTORY) );
		}
		else
			delete pCmd;
	}
}

void CDeltaTreeCtrl::OnFileRevisionTree() 
{
	HTREEITEM currentItem=GetLastSelection();  
	if( IsAFile(currentItem) && !SERVER_BUSY() )
	{
		int i, j;
		CString path = GetItemText( currentItem );
		if ((i = path.Find(_T('#'))) != -1)
		{
			if ((j = path.Find(_T(' '), i)) != -1)
				path = path.Left(j);
			if (path[i+1] == _T('0'))
				path = path.Left(i);
		}
		TheApp()->CallP4RevisionTree( path );
	}
}

void CDeltaTreeCtrl::OnFileTimeLapseView() 
{
	HTREEITEM currentItem=GetLastSelection();  
	if( IsAFile(currentItem) && !SERVER_BUSY() && MainFrame()->HaveTLV() )
	{
		int i;
		CString path = GetItemText( currentItem );
		if (((i = path.Find(_T('#'))) != -1) && ((i = path.Find(_T(' '), i)) != -1))
			path = path.Left(i);
		TheApp()->CallP4A( path, _T(""), 0 );
	}
}

void CDeltaTreeCtrl::OnFileAnnotate() 
{
	FileAnnotate(FALSE);
}

void CDeltaTreeCtrl::OnFileAnnotateAll() 
{
	FileAnnotate(TRUE);
}

void CDeltaTreeCtrl::OnFileAnnotateChg() 
{
	FileAnnotate(FALSE, TRUE);
}

void CDeltaTreeCtrl::OnFileAnnotateChgAll() 
{
	FileAnnotate(TRUE, TRUE);
}

void CDeltaTreeCtrl::FileAnnotate(BOOL bAll, BOOL bChg/*=FALSE*/) 
{
	HTREEITEM currentItem=GetLastSelection();
	
	if (currentItem == NULL || !IsAFile(currentItem))
		ASSERT(0);
	else
	{
		CP4FileStats *fs= (CP4FileStats *) GetLParam(currentItem);
		CString itemStr= fs->GetFullDepotPath();
		
		CCmd_PrepBrowse *pCmd= new CCmd_PrepBrowse;
		pCmd->Init( m_depotWnd, RUN_ASYNC);
		pCmd->SetFileType(fs->IsTextFile() ? FST_TEXT : FST_BINARY);
		CString fType = fs->GetHeadType();
		::SendMessage(m_depotWnd, WM_SETVIEWER, 0, (LPARAM)GET_P4REGPTR()->GetEditApp());
		if( pCmd->Run( FALSE, itemStr, fType, bAll, bChg, FALSE, fs->GetHaveRev(),
			GET_P4REGPTR()->GetAnnotateWhtSpace(),
			bChg ? GET_P4REGPTR()->GetAnnotateIncInteg() : FALSE) )
		{
			MainFrame()->UpdateStatus( LoadStringResource(IDS_FETCHING_FILE) );
		}
		else
			delete pCmd;
	}
}


//	If user right clicks on a file and chooses 'Explore', run Windows Explorer
//	in the directory where that file resides on the client machine.
//	We can only do this for a specific file - not for multiple selected files
//	since they may map to different directories.

void CDeltaTreeCtrl::OnWinExplore() 
{
    BOOL root;

    if (GetSelectedCount() && 
		GetItemLevel(GetSelectedItem(0), &root)== 2 && 
        IsMyPendingChangeFile(GetSelectedItem(0)) )
    {
        HTREEITEM item = NULL;
		if (m_ContextPoint.x != -1 && m_ContextPoint.y != -1)
		{
			// find out what item was clicked to generate the last context menu
			TV_HITTESTINFO ht;
			ht.pt=m_ContextPoint;
			ScreenToClient(&ht.pt);
			ht.flags=TVHT_ONITEMLABEL | TVHT_ONITEMICON | TVHT_ONITEMBUTTON;
			item=HitTest( &ht );
		}
		if (!item || !IsSelected(item) || !IsAFile(item))
		{
			item = GetSelectedItem(0);
			if (!IsAFile(item))
				return;
		}

		CString clientPath;
        if(GetClientPath(item, clientPath))
        {
            int	i;
			CString switches;

			clientPath.Replace('/', '\\');
			if (GET_P4REGPTR()->GetExplorer()				// not using Win Explorer
			 || (::GetFileAttributes(clientPath) == -1))	// file not found
			{
				switches = _T("");
				if ((i = clientPath.ReverseFind('\\')) != -1)
					clientPath = clientPath.Left(i);
			}
			else
			{
				switches = _T("/select,");
			}

			if (clientPath.FindOneOf(_T(" &()[]{}^=;!'+,`~")) != -1)
			{
				clientPath.TrimLeft();
				clientPath.TrimRight();
				clientPath = _T('\"') + clientPath + _T('\"');
			}

            STARTUPINFO si;
            memset(&si, 0, sizeof(si));
            si.cb = sizeof(si);
            PROCESS_INFORMATION pi;
            CreateProcess(NULL, const_cast<LPTSTR>((LPCTSTR)(TheApp()->GetExplorer() 
													+ switches + clientPath)), 
                NULL, NULL, 
#ifdef UNICODE
                FALSE, DETACHED_PROCESS | NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT, 
#else
                FALSE, DETACHED_PROCESS | NORMAL_PRIORITY_CLASS, 
#endif
                MainFrame()->P4GetEnvironmentStrings(),
				NULL, &si, &pi);
        }
    }
}

//	If user right clicks on a file and chooses 'Command Prompt', run a Command Prompt
//	in the directory where that file resides on the client machine.
//	We can only do this for a specific file - not for multiple selected files
//	since they may map to different directories; otherwise run MainFrame's OnCmdPrompt()

void CDeltaTreeCtrl::OnCmdPrompt() 
{
	BOOL root;

	if (GetSelectedCount() && 
		GetItemLevel(GetSelectedItem(0), &root)== 2 && 
		IsMyPendingChangeFile(GetSelectedItem(0)) )
	{
        HTREEITEM item = NULL;
		if (m_ContextPoint.x != -1 && m_ContextPoint.y != -1)
		{
			// find out what item was clicked to generate the last context menu
			TV_HITTESTINFO ht;
			ht.pt=m_ContextPoint;
			ScreenToClient(&ht.pt);
			ht.flags=TVHT_ONITEMLABEL | TVHT_ONITEMICON | TVHT_ONITEMBUTTON;
			item=HitTest( &ht );
		}
		if (!item || !IsSelected(item) || !IsAFile(item))
		{
			item = GetSelectedItem(0);
			if (!IsAFile(item))
			{
				MainFrame()->OnCmdPromptPublic();
				return;
			}
		}

		CString clientPath;
		if(GetClientPath(item, clientPath))
		{
			int	i;
			clientPath.Replace('/', '\\');
			if ((i = clientPath.ReverseFind(_T('\\'))) != -1)
				clientPath = clientPath.Left(i);

			TCHAR	cmd[MAX_PATH+1];

			GetEnvironmentVariable(_T("ComSpec"), cmd, MAX_PATH);

			STARTUPINFO si;
			memset(&si, 0, sizeof(si));
			si.cb = sizeof(si);
			PROCESS_INFORMATION pi;
			CreateProcess(NULL, cmd, 
				NULL, NULL, 
#ifdef UNICODE
                FALSE, CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT, 
#else
				FALSE, CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS, 
#endif
				MainFrame()->P4GetEnvironmentStrings(), 
				clientPath, &si, &pi);
		}
	}
	else
		MainFrame()->OnCmdPromptPublic();
}

void CDeltaTreeCtrl::OnJobDescribe() 
{
	HTREEITEM item=GetLastSelection();  
	
	if (item == NULL)
		ASSERT(0);
	else
	{
		CString itemStr=GetItemText(item);
		itemStr.TrimLeft(); 

		CCmd_Describe *pCmd= new CCmd_Describe;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		if( pCmd->Run( P4JOB_SPEC, itemStr ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_FETCHING_JOB_SPEC) );	
		else
			delete pCmd;
	}	
}

void CDeltaTreeCtrl::OnJobEditspec() 
{
	HTREEITEM item=GetLastSelection();  
	
	if (item == NULL)
		ASSERT(0);
	else
	{
		CString itemStr=GetItemText(item);
		itemStr.TrimLeft(); 
		MainFrame()->EditJobSpec(&itemStr);
	}
}

void CDeltaTreeCtrl::OnAddjobfix() 
{
    if( SERVER_BUSY() )
    {
		EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
        ASSERT(0);
        return;
    }

	HTREEITEM currItem=GetLastSelection();  
		
	if(!IsMyPendingChange(currItem))
		currItem=GetParentItem(currItem);

	// no selected item - menu enables should have prevented
	if (currItem == NULL)
		{ EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE); ASSERT(0); return; }

	// selected item not one of my changes - fix menu enables should have prevented
	if(GetParentItem(currItem) != m_MyRoot)
		{ EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE); ASSERT(0); return; }
	
	// Record the change that is involved
	m_ActiveItem=currItem;
	m_EditChangeNum=GetSelectedChangeNumber();

	if(m_EditChangeNum < 0L)
		{ EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE); ASSERT(0); return; }

    // Ask jobs window to update itself (if required) and send WM_P4JOBS when done
    // The reply message will be handled below
    SET_APP_HALTED(TRUE);
    ::SendMessage( m_jobWnd, WM_FETCHJOBS, (WPARAM)m_EditChangeNum, (LPARAM)m_hWnd);
}


LRESULT CDeltaTreeCtrl::OnP4JobList(WPARAM wParam, LPARAM lParam)
{
    m_EditChangeNum= wParam;

    // Get the list of jobs
    CObList *jobs= (CObList *) ::SendMessage( m_jobWnd, WM_QUERYJOBS, 0, 0);
    ASSERT(jobs);
    ASSERT_KINDOF(CObList,jobs);

	CString *spec= (CString *) ::SendMessage( m_jobWnd, WM_QUERYJOBSPEC, 0, 0);
    ASSERT(spec);

	CStringArray *cols= (CStringArray *) ::SendMessage( m_jobWnd, WM_QUERYJOBCOLS, 0, 0);
    ASSERT(cols);

	CString *curr= (CString *) ::SendMessage( m_jobWnd, WM_QUERYJOBSELECTION, 0, 0);

	CJobListDlg dlg;
	dlg.SetJobFont(GetFont());
	dlg.SetJobList(jobs);
	dlg.SetJobSpec(spec);
	dlg.SetJobCols(cols);
	dlg.SetJobCurr(curr);
	CStringList *jobnames= dlg.GetSelectedJobs();

	EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
	int retcode= dlg.DoModal();
    SET_APP_HALTED(FALSE);

	// Delete the job list
	for(POSITION pos=jobs->GetHeadPosition(); pos!=NULL; )
		delete (CP4Job *) jobs->GetNext(pos);
    delete jobs;

	if (retcode == IDOK && jobnames->GetCount() > 0)
		AddJobFixes(jobnames, dlg.m_JobStatusValue.GetLength() 
					? (LPCTSTR)dlg.m_JobStatusValue : NULL);
	else if (retcode == IDRETRY)
	{
	    ::SendMessage( m_jobWnd, WM_CLEARLIST, 0, 0);   
		PostMessage(WM_COMMAND, ID_CHANGE_ADDJOBFIX, 0);
	}

	MainFrame()->ClearStatus();
    return 0;
}

void CDeltaTreeCtrl::AddJobFixes(CStringList *jobnames, LPCTSTR jobstatusvalue)
{
    POSITION pos;
    CString str;
   
	// Copy the joblist
	m_JobList.RemoveAll();
	for(pos= jobnames->GetHeadPosition(); pos != NULL; )
	{
		str= jobnames->GetNext(pos);
		m_JobList.AddHead(str);
	}

	CCmd_Fix *pCmdFix= new CCmd_Fix;
	pCmdFix->Init( m_hWnd, RUN_ASYNC, LOSE_LOCK);
	if( pCmdFix->Run( &m_JobList, m_EditChangeNum, FALSE, jobstatusvalue ) )
	{
	    MainFrame()->UpdateStatus( LoadStringResource(IDS_FIXING_JOBS) );	
	}	
	else
		delete pCmdFix;
}

void CDeltaTreeCtrl::OnRemovefix() 
{
	HTREEITEM item=GetLastSelection();  
	m_ActiveItem=item;
		
	if (item == NULL)
		ASSERT(0);
	else
	{
		// Get the change number
		BOOL underMyRoot;
		int level=GetItemLevel(item, &underMyRoot);
		if(level==2) 
		{
			HTREEITEM change=GetParentItem(item);
			ASSERT(change != NULL);
			long changeNum= GetChangeNumber(change);
			ASSERT(changeNum);

			CString itemStr=GetItemText(item);
			itemStr.TrimLeft();  // Strip leading space
			m_StringList.RemoveAll();
			m_StringList.AddHead(itemStr);

			CCmd_Fix *pCmd= new CCmd_Fix;
			pCmd->Init( m_hWnd, RUN_ASYNC);
			if( pCmd->Run( &m_StringList, changeNum, TRUE ) )
				MainFrame()->UpdateStatus( LoadStringResource(IDS_UNFIXING_JOB) );	
			else
				delete pCmd;
		}
	}
}


void CDeltaTreeCtrl::OnChgListRevert() 
{
	if (GET_SERVERLEVEL() < 19)	// if server is before 2005.1, don't try to diff - just warn
	{
		if (IDYES != AfxMessageBox(IDS_REVERTING_FILES_WILL_OVERWRITE_EDITS, 
									MB_YESNO|MB_ICONQUESTION))
			return;
	}
	else	// for 2005.2 and later servers, we can locally check to see if any files have changed
	{
		CString text;
		HTREEITEM currentItem = GetSelectedItem(0);
		int tot;
		BOOL b = SelectChgUnchg(TRUE, &tot);
		if (b)
		{
			int n = GetSelectionSetSize();
			text.FormatMessage(IDS_n_CHGED_REVERT_YESNO, n);
		}
		UnselectAll();
		SetSelectState( currentItem, TRUE );
		if (IDYES != AfxMessageBox(b ? text : LoadStringResource(IDS_ALL_UNCHGED_REVERT_YESNO), 
									MB_YESNO|MB_ICONQUESTION))
			return;
	}

	CString chg;
	long l = GetSelectedChangeNumber();
	if (l)
		chg.Format(_T("%ld"), l);
	else
		chg = _T("default");
	m_SelectionList.RemoveAll();
	m_SelectionList.AddHead(chg);
	m_SelectionList.AddTail(_T("//..."));

	m_DoRevert = FALSE;
	CCmd_Revert *pCmd= new CCmd_Revert;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK);
	if( pCmd->Run(&m_SelectionList , TRUE ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_REVERT) );
	else
		delete pCmd;
}

void CDeltaTreeCtrl::OnFileRevert() 
{
	int cnt = GetSelectedCount();
	if(!cnt)
	{
		ASSERT(0);
		return;
	}
	if (GET_P4REGPTR()->AlwaysWarnOnRevert())
	{
		AssembleStringList( );
		OnP4FileRevert(0, 0);
		return;
	}
	if(cnt == 1 && IsMyPendingChange(GetSelectedItem(0)))
	{
		if (AnyFilesInChange(GetSelectedItem(0)))
			OnChgListRevert();
		return;
	}

	AssembleStringList( );

	if (GET_SERVERLEVEL() >= 14)
	{
		// Make a new list of selected files, but don't include those opened for add.
		// We can't use the list above from AssembleStringList()
		// because this list will be cleared by the Revert -an command 
		// plus we don't have to ask about any adds.
		AssembleStringList(&m_SelectionList, FALSE);
		
		// Run the p4 revert -an command on all the selected files not opened for add
		CCmd_Revert *pCmd= new CCmd_Revert;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		pCmd->SetAlternateReplyMsg( WM_P4FILEREVERT );
		pCmd->SetNbrNonEdits(m_StringList.GetCount() - m_SelectionList.GetCount());
		if (m_SelectionList.IsEmpty())
		{
			pCmd->ClearError();
			OnP4FileRevert((WPARAM)pCmd, 0);
		}
		else if( pCmd->Run( &m_SelectionList, FALSE, TRUE, TRUE, FALSE, TRUE ) )
			MainFrame()->UpdateStatus(LoadStringResource(IDS_RUNNING_DIFF));
		else
			delete pCmd;
	}
	else
	{
		CCmd_Opened *pCmd= new CCmd_Opened;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		pCmd->SetAlternateReplyMsg( WM_P4FILEREVERT );

		if( pCmd->Run( FALSE, FALSE, -1, &m_StringList ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_FILE_INFORMATION) );
		else
			delete pCmd;
	}
}

LRESULT CDeltaTreeCtrl::OnP4Revert( WPARAM wParam, LPARAM lParam )
{
	BOOL chainedCommands=FALSE;

	CCmd_Revert *pCmd= (CCmd_Revert *) wParam;
	ASSERT_KINDOF(CCmd_Revert, pCmd);

	if( !pCmd->GetError() )
	{
		if(m_DoRevert)
		{
			chainedCommands = DoRevert(pCmd->GetFileList(), 
									   pCmd->GetServerKey(), pCmd->OnlyUnChgd());
        }
		else
		{
			int cnt = pCmd->GetFileList()->GetCount();
			if (cnt > MAX_FILESEEKS)
			{
				int key= pCmd->GetServerKey();
				chainedCommands= TRUE;
				MainFrame()->UpdateDepotandChangeViews(REDRILL, key);
			}
			else
				OnP4RevertFile(pCmd->GetFileList());

			CString chg;
			CString text;
			if (pCmd->OnlyUnChgd() && !(chg = pCmd->GetChgName()).IsEmpty() && !pCmd->IsPreview())
				text.FormatMessage(IDS_REVERTEDALL_n_FILES, cnt, chg);
			else
				text.FormatMessage(IDS_REVERTED_n_FILES, cnt);
			AddToStatus(text, SV_COMPLETION);
		}
	}
	if( !chainedCommands || MainFrame()->IsQuitting() )
		pCmd->ReleaseServerLock();
	delete pCmd;
	return (0);
}

LRESULT CDeltaTreeCtrl::OnP4FileRevert( WPARAM wParam, LPARAM lParam )
{
	BOOL	bBox = TRUE;
	BOOL	bUseDashA = FALSE;

	if (!GET_P4REGPTR()->AlwaysWarnOnRevert())
	{
		if (GET_SERVERLEVEL() >= 14)
		{
			// m_StringList still contains our revert list
			CCmd_Revert *pCmd= (CCmd_Revert *) wParam;

			if(!pCmd->GetError() 
			&& pCmd->GetFileList()->GetCount() + pCmd->NbrNonEdits() == m_StringList.GetCount())
			{
				bBox = FALSE;
				if (pCmd->NbrNonEdits() == 0)
					bUseDashA = TRUE;
			}
			delete pCmd;
		}
		else
		{
			CCmd_Opened *pCmd= (CCmd_Opened *) wParam;

			if(!pCmd->GetError())
			{
				bBox = FALSE;
				m_StringList.RemoveAll();
				CObList *list = pCmd->GetList( );
				ASSERT_KINDOF( CObList, list );
				for(POSITION pos= list->GetHeadPosition(); pos!=NULL; )
				{
					CP4FileStats *stats = ( CP4FileStats * )list->GetNext( pos );
					ASSERT_KINDOF( CP4FileStats, stats );
					m_StringList.AddHead(stats->GetFullDepotPath( ));
					if (stats->GetMyOpenAction() != F_ADD)
						bBox = TRUE;
					delete stats;
				}
			}
			else AssembleStringList( );
			delete pCmd;
		}
	}

	if (bBox)
	{
		if(AfxMessageBox(IDS_REVERTING_FILES_WILL_OVERWRITE_EDITS, MB_ICONQUESTION|MB_YESNO) != IDYES)
		{
			MainFrame()->ClearStatus();
			return (0);
		}
	}

	CCmd_ListOpStat *pCmd2= new CCmd_ListOpStat;
	pCmd2->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
	// MUST use a temp variable or the release version 
	// will return garbage from SendMessage!
	int iFlag;
	::SendMessage(m_depotWnd, WM_ISFILTEREDONOPEN, 0, (LPARAM)&iFlag);
	pCmd2->SetRedoOpenedFilter(iFlag);
	if( pCmd2->Run( &m_StringList, bUseDashA ? P4REVERTUNCHG : P4REVERT ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_REVERT) );
	else
		delete pCmd2;

	return (0);
}


int CDeltaTreeCtrl::GetItemLevel(HTREEITEM currentItem, BOOL *underMyRoot)
{
	if (currentItem == NULL)
		return 0;

	int Level=0;

	// get its parent and grandparent (tree is only 3 layers deep)
	HTREEITEM parentItem=GetParentItem(currentItem);
	HTREEITEM rootItem;
	if(parentItem != NULL)
	{
		Level++;
		rootItem=GetParentItem(parentItem);
		if(rootItem!=NULL)
			Level++;
	}
	else
		rootItem=NULL;
		
	// is the item at or under m_MyRoot?
	if(currentItem != m_MyRoot && parentItem != m_MyRoot && rootItem != m_MyRoot)
		*underMyRoot=FALSE;
	else
		*underMyRoot=TRUE;

	return Level;
}

BOOL CDeltaTreeCtrl::OKToAddSelection( HTREEITEM currentItem )
{
	if (GetParentItem(currentItem) != GetLastSelectionParent())
		return FALSE;
														
	BOOL underMyRoot;
	int Level=GetItemLevel(currentItem, &underMyRoot);

	// Multi-select of files and jobs is OK.  All other 
	// multi-select is illegal
	if(Level==2)
		return TRUE;
	else
		return FALSE;
}


/*
	_________________________________________________________________

	Called by MSTreeView during OnLButtonDown to see if the user is 
	dragging before letting the mouse button up, which would indicate
	a drag drop operation.
	_________________________________________________________________
*/

BOOL CDeltaTreeCtrl::TryDragDrop( HTREEITEM currentItem )
{
	// Only files are draggable
	BOOL underMyRoot;
	if( GetItemLevel( currentItem, &underMyRoot ) != 2 )
		return FALSE;

	// Store the change this is from
	m_DragFromChange=GetParentItem(currentItem);

	// Dont actually send data - clipboard format is all the info target requires
	/*(DYNAMIC_DOWNCAST(CDeltaView,GetView()))->*/m_OLESource.DelayRenderData( (unsigned short) m_CF_DELTA);	// for P4WIN
	/*(DYNAMIC_DOWNCAST(CDeltaView,GetView()))->*/m_OLESource.DelayRenderData( (unsigned short) CF_HDROP);	// for external programs (dragging to an editor)

	// We lie here and tell it we will only do COPY, but actually we will really do a MOVE
	// if we drop the selection on another changelist.  This lie is done to prevent other
	// external programs that can accept a drag&drop from MOVing the files; they will only
	// COPY the files, never MOVE them.  Note that this means if you drag a file to
	// Explorer, you will copy it to the new location, not move it.
	m_DragDropCtr++;
	if(/*(DYNAMIC_DOWNCAST(CDeltaView,GetView()))->*/m_OLESource.DoDragDrop(DROPEFFECT_COPY, &m_DragSourceRect, NULL) == DROPEFFECT_MOVE)
	{
		// this code is probably now obsolete since we don't call DoDragDrop() with
		// DROPEFFECT_MOVE anymore.  But I'll leave it, just in case....
		m_LastDragDropTime = GetTickCount();// record the time so we can distinguish between CF_HDROP's from this window and CF_HDROP's from an external window
		UnselectAll();
		return TRUE;
	}
	else
	{
		m_LastDragDropTime = GetTickCount();// record the time so we can distinguish between CF_HDROP's from this window and CF_HDROP's from an external window
		return FALSE;
	}
}

// This rountine provides a list of dropped file names for CF_HDROP format drag and drop
// when the Changelist pane is the source of the drag and drop.
// The return value is the length of all the file names plus number of files (this is one less
// than the length of the memory needed to render the file names).  If there is nothing to
// render, the return value is 0.
// Call this routine with a NULL to just obtain the length needed for the buffer; call it
// with a pointer to the addr of a buffer in order to load that buffer with the file names.
int CDeltaTreeCtrl::RenderFileNames(LPTSTR p)
{
	static LPTSTR	pFN = 0;	// ptr to buffer to store file names to be rendered
	static DWORD	lFN = 0;	// lgth of buffer at pFN
	static DWORD	uFN = 0;	// amt of buffer at pFN actually in use
	static DWORD	ddCtr = 0;	// counter to validate contents of buffer at pFN
	LPTSTR ptr;
	int		i;

	// If the change list pane is the drop target, don't provide a list of files
	// because files from the chglist pane to the chglist pane are NOT in CF_HDROP format.
	if ( m_DeltaIsDropTarget )
		return 0;

	if (!pFN)
	{
		 pFN = (LPTSTR)::VirtualAlloc(NULL, lFN = 4096*sizeof(TCHAR), MEM_COMMIT, PAGE_READWRITE);
		 if (!pFN)
			 return(0);	// out of memory!
	}
	else if (ddCtr == m_DragDropCtr)	// is this D&D the same as the one we have stored?
	{
		if (p)
			memcpy(p, pFN, uFN*sizeof(TCHAR));
		return uFN;
	}

	for(i=GetSelectedCount()-1, ptr = pFN, uFN = 0; i>=0; i--)
	{
		HTREEITEM item= GetSelectedItem(i);
		if( IsMyPendingChangeFile( item ) )
		{
			CString clientPath;
			if(GetClientPath(item, clientPath))
			{
				if ((ptr + clientPath.GetLength() + (2*sizeof(TCHAR))) 
						>= (pFN + (lFN/sizeof(TCHAR))))	// running out of room?
				{
					LPTSTR sav = (LPTSTR)::VirtualAlloc(NULL, (lFN + 4096)*sizeof(TCHAR), MEM_COMMIT, PAGE_READWRITE);
					if (!sav)
						return(0);	// we're in trouble - out of memory!
					memcpy(sav, pFN, uFN*sizeof(TCHAR));
					ptr = sav + (ptr - pFN);
					::VirtualFree(pFN, 0, MEM_RELEASE);
					pFN = sav;
					lFN += 4096;
				}
				lstrcpy(ptr, clientPath);
				ptr += clientPath.GetLength() + 1;
				uFN += clientPath.GetLength() + 1;
			}
		}
	}
	*ptr = _T('\0');
	ddCtr = m_DragDropCtr;
	if (p)
		memcpy(p, pFN, uFN*sizeof(TCHAR));
	return uFN;
}

// Render CF_HDROP format drag and drop data.  Note that this routine is NOT a CDeltaTreeCtrl
// routine - it is an override of COleDataSource for m_OLESource.  It calls back to
// CDeltaTreeCtrl::RenderFileNames() to render the files names - it just sets up the structure
// that will hold the rendered file names.
BOOL CP4OleDataSource::OnRenderData( LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium )
{
	int	lgth;

	if ((lgth = m_deltaTree->RenderFileNames(NULL)) == 0)
		return FALSE;
	lpStgMedium->tymed = TYMED_HGLOBAL;
	lpStgMedium->hGlobal = GlobalAlloc(GHND, sizeof(_DROPFILES) + (lgth + 2)*sizeof(TCHAR));
	LPDROPFILES lpdropfiles = (LPDROPFILES)GlobalLock(lpStgMedium->hGlobal);
	lpdropfiles->pFiles = sizeof(_DROPFILES);
	lpdropfiles->pt.x = 0;
	lpdropfiles->pt.y = 0;
	lpdropfiles->fNC = FALSE;
#ifdef UNICODE
	lpdropfiles->fWide = TRUE;
#else
	lpdropfiles->fWide = FALSE;
#endif
	BOOL rc = m_deltaTree->RenderFileNames((LPTSTR)((char*)lpdropfiles + lpdropfiles->pFiles)) 
			? TRUE : FALSE;
	GlobalUnlock(lpStgMedium->hGlobal);
	if (!rc)	// if there were no file names rendered, clean up
	{
		GlobalFree(lpStgMedium->hGlobal);
		lpStgMedium->hGlobal = 0;
	}
	return rc;
}


//////////////////////////////////////////////////////////////////////////////////
// Context menu implementation.  Due to an apparent MFC bug, two mouse click handlers
// are included to make this work.  Prolly in the next release of MFC, the default
// OnCOntextMenu linkage can be used.

void CDeltaTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	BOOL defChange;

	m_ContextPoint = point;

	GetParentFrame()->ActivateFrame();
	m_Need2Edit = FALSE;

	HTREEITEM currentItem;
    SetItemAndPoint( currentItem, point );
    ClientToScreen( &point );

	BOOL underMyRoot=FALSE;
	int Level=0;
	if(currentItem != NULL)
		Level=GetItemLevel(currentItem, &underMyRoot);

	// Create the empty menus
	CP4Menu popMenu;
	popMenu.CreatePopupMenu();
	CP4Menu editMenu;
	editMenu.CreatePopupMenu();
	CP4Menu viewMenu;
	CP4Menu annotateMenu;
	CP4Menu resolveMenu;
	resolveMenu.CreatePopupMenu( );

	// make a new selection new if reqd
	if(!IsSelected(currentItem))
	{
		UnselectAll();
		SetSelectState(currentItem, TRUE);
	}
			
	// Most options only relevant for my own open files
	if( currentItem == NULL || (!IsMyPendingChange( currentItem ) && !IsMyPendingChangeItem( currentItem )) )
	{
		if( currentItem != NULL && Level == 1)
		{
			popMenu.AppendMenu( stringsON, ID_CHANGE_DESCRIBE, LoadStringResource( IDS_DESCRIBEIT ) );
			if(HasChildren(currentItem))
				popMenu.AppendMenu( stringsON, ID_POSITIONDEPOT, LoadStringResource(IDS_FINDCHGFILESINDEPOT_F));
		}
		goto EndContext;
	}

	defChange= (Level==1 && GetChangeNumber(currentItem)==0);
				
	if(Level==1)  // Its a change
	{
		//		for all changelists with files in them. put submit first
		//
		if(HasChildren(currentItem))
		{
			popMenu.AppendMenu( stringsON, ID_CHANGE_SUBMIT );
			popMenu.AppendMenu(MF_SEPARATOR);
		}

		//		for numbered changelists only
		//
		if( !defChange)
		{
			popMenu.AppendMenu( stringsON, ID_CHANGE_EDSPEC, LoadStringResource( IDS_EDITSPEC ) );
			popMenu.AppendMenu( stringsON, ID_CHANGE_DESCRIBE, LoadStringResource( IDS_DESCRIBEIT ) );
			popMenu.AppendMenu( stringsON, ID_CHANGE_ADDJOBFIX, LoadStringResource( IDS_ADDJOBFIX ) );
		}

		// Choice for empty changes if not default
		if(!HasChildren(currentItem) && !defChange)
			popMenu.AppendMenu( stringsON, ID_CHANGE_DEL, LoadStringResource( ID_CHANGE_DEL ) );

		// Choices relevant only to non-empty change
		if(HasChildren(currentItem))
		{
			popMenu.AppendMenu( stringsON, ID_CHANGE_REVORIG, LoadStringResource( ID_CHANGE_REVORIG ) );
			popMenu.AppendMenu( stringsON, ID_FILE_DIFFHEAD, LoadStringResource(IDS_DIFFFILESAGAINSTDEPOT));	
			popMenu.AppendMenu( stringsON, ID_POSITIONDEPOT, LoadStringResource(IDS_FINDCHGFILESINDEPOT_F));	
		}
	}
	if(Level==2) // Its a file or job (jobs not supported yet)
	{
		if(AnyMyPendingChangeFiles())
		{
			if ( AnyMyInteg() || AnyMyBranch() )
			{
				if(!SERVER_BUSY() && IsEditableFile())
				{
					viewMenu.CreatePopupMenu();
					viewMenu.AppendMenu( stringsON, ID_FILE_QUICKBROWSE, 
								LoadStringResource( IDS_ASSOCVIEWER ));

					int actualMRUs=0;
					for(int i=0; i < MAX_MRU_VIEWERS; i++)
					{
						if( GET_P4REGPTR()->GetMRUViewerName(i).GetLength() > 0 )
						{
							viewMenu.AppendMenu( stringsON, ID_FILE_BROWSER_1+i, 
									CString ( _T("&") + GET_P4REGPTR()->GetMRUViewerName(i)) );
							actualMRUs++;
						}
					}

					viewMenu.AppendMenu( stringsON, ID_FILE_NEWBROWSER, 
						LoadStringResource ( IDS_OTHERVIEWER ) );

					if(!SERVER_BUSY() && viewMenu.GetMenuItemCount() > 0)
					{
						popMenu.AppendMenu(MF_POPUP, (UINT) viewMenu.GetSafeHmenu(), 
							LoadStringResource( IDS_VIEWUSING ));
					}
				}

			}
			
			if(!SERVER_BUSY() && IsEditableFile())
			{
				editMenu.AppendMenu( stringsON, ID_FILE_QUICKEDIT, 
							LoadStringResource( IDS_ASSOCEDITOR ));

				int actualMRUs=0;
				for(int i=0; i < MAX_MRU_VIEWERS; i++)
				{
					if( GET_P4REGPTR()->GetMRUViewerName(i).GetLength() > 0 )
					{
						editMenu.AppendMenu( stringsON, ID_FILE_EDITOR_1+i, 
								CString ( _T("&") + GET_P4REGPTR()->GetMRUViewerName(i)) );
						actualMRUs++;
					}
				}

				editMenu.AppendMenu( stringsON, ID_FILE_NEWEDITOR, LoadStringResource ( IDS_OTHEREDITOR ) );
				editMenu.AppendMenu( stringsON, ID_FILE_RMVEDITOR, LoadStringResource ( IDS_RMVEDITOR ));
			}

			if(!SERVER_BUSY() && editMenu.GetMenuItemCount() > 0)
			{
				popMenu.AppendMenu(MF_POPUP, (UINT) editMenu.GetSafeHmenu(), 
					LoadStringResource( IDS_EDITUSING ));
			}

			if ( AnyMyInteg() || AnyMyBranch() )
				popMenu.AppendMenu( stringsON, ID_FILE_OPENEDIT, LoadStringResource( IDS_REOPENFOREDIT ) );

			popMenu.AppendMenu( stringsON, ID_FILE_DIFFHEAD, 
				LoadStringResource(GetSelectedCount() > 1 ? IDS_DIFFFILESAGAINSTDEPOT : IDS_DIFFFILEAGAINSTDEPOT) );	

			if(!SERVER_BUSY())
			{
				popMenu.AppendMenu(stringsON, ID_FILETYPE, LoadStringResource( IDS_CHANGEFILETYPE) );
				popMenu.AppendMenu(stringsON, ID_FILE_MV2OTHERCHGLIST, LoadStringResource( IDS_FILE_MV2OTHERCHGLIST) );
			}
				
			if(AnyMyUnLocked())
				popMenu.AppendMenu( stringsON, ID_FILE_LOCK, LoadStringResource( IDS_LOCK ) );	
			if(AnyMyLock())
				popMenu.AppendMenu( stringsON, ID_FILE_UNLOCK, LoadStringResource( IDS_UNLOCK ) );	
		}
	}

	if (((Level==1) && HasChildren(currentItem)) || ((Level==2) && AnyMyPendingChangeFiles()))	// for both changes and files
	{
		resolveMenu.AppendMenu( stringsON, ID_FILE_RESOLVE, LoadStringResource( IDS_INTERACTIVELY )  );	
		resolveMenu.AppendMenu( stringsON, ID_FILE_AUTORESOLVE, LoadStringResource( IDS_AUTORESOLVE ) );	
		resolveMenu.AppendMenu( stringsON, ID_FILE_RUNMERGETOOL, LoadStringResource( IDS_RUNMERGETOOL )  );	
		resolveMenu.AppendMenu( stringsON, ID_FILE_SCHEDULE, 
				LoadStringResource(((Level==1) || (GetSelectedCount() > 1)) ? IDS_SCHEDULEFILESFORRESOLVE 
																			: IDS_SCHEDULEFILEFORRESOLVE) );
		if (Level==2 && IsAFile(currentItem) && GetSelectedCount()==1 && !SERVER_BUSY()
		 && IsMyPendingChangeItem(currentItem))
		{
			CP4FileStats *stats= (CP4FileStats *) GetLParam(currentItem);
			if(stats != NULL && (stats->IsUnresolved() || stats->IsResolved()))
			{
				resolveMenu.AppendMenu(MF_SEPARATOR);
				resolveMenu.AppendMenu( stringsON, ID_THEIRFILE_PROPERTIES, LoadStringResource( IDS_THEIRFILE_PROPERTIES )  );
				resolveMenu.AppendMenu( stringsON, ID_THEIRFILE_REVISIONHISTORY, LoadStringResource( IDS_THEIRFILE_REVISIONHISTORY )  );
				resolveMenu.AppendMenu( stringsON, ID_THEIRFILE_FINDINDEPOT, LoadStringResource( IDS_THEIRFILE_FINDINDEPOT )  );
			}
		}
		if(!SERVER_BUSY())
			popMenu.AppendMenu(MF_POPUP, (UINT) resolveMenu.GetSafeHmenu(), 
				LoadStringResource ( (Level==1) ? IDS_MENU_RESOLVE_FILES : IDS_MENU_RESOLVE ) );
	}

	if (Level == 2) // rest of a file or job
	{
		if( IsAFile(currentItem) )
		{
			if (!SERVER_BUSY() && GET_SERVERLEVEL() >= 14 && GetSelectedCount()==1)
			{
				CP4FileStats *fs= (CP4FileStats *) GetLParam( GetSelectedItem(0) );
				BOOL enable = ( fs->GetHaveRev() <= 1
								&& ( fs->GetMyOpenAction() == F_ADD 
								  || fs->GetMyOpenAction() == F_BRANCH ) ) ? FALSE : TRUE;
				if (enable)
				{
					CString fileType = fs->GetHeadType();
					enable = ((fileType.Find(_T("text")) != -1) 
							|| (fileType.Find(_T("symlink")) != -1)) ? TRUE : FALSE;
				}
				if (enable)
				{
					annotateMenu.CreatePopupMenu();
					annotateMenu.AppendMenu( stringsON, ID_FILE_ANNOTATE, LoadStringResource ( IDS_FILE_ANNOTATE ) );
					annotateMenu.AppendMenu( stringsON, ID_FILE_ANNOTATEALL, LoadStringResource ( IDS_FILE_ANNOTATEALL ) );
					annotateMenu.AppendMenu( stringsON, ID_FILE_ANNOTATECHG, LoadStringResource ( IDS_FILE_ANNOTATECHG ) );
					annotateMenu.AppendMenu( stringsON, ID_FILE_ANNOTATECHGALL, LoadStringResource ( IDS_FILE_ANNOTATECHGALL ) );
					popMenu.AppendMenu(MF_POPUP, (UINT) annotateMenu.GetSafeHmenu(), 
						LoadStringResource( IDS_ANNOTATEFILE ));
				}
			}
			popMenu.AppendMenu( stringsON, ID_FILE_PROPERTIES, LoadStringResource( IDS_PROPERTIES ) );
			if (MainFrame()->HaveP4QTree())
				popMenu.AppendMenu( stringsON, ID_FILE_REVISIONTREE,LoadStringResource( IDS_REVISIONTREE ) );
			if (MainFrame()->HaveTLV())
				popMenu.AppendMenu( stringsON, ID_FILE_ANNOTATIONS, LoadStringResource( IDS_ANNOTATIONS ));
			popMenu.AppendMenu( stringsON, ID_FILE_REVISIONHISTORY, LoadStringResource( IDS_REVISIONHISTORY ) );
			if (!GetChangeNumber(currentItem))	// is it the default change?
			{
				popMenu.AppendMenu( MF_SEPARATOR );
				popMenu.AppendMenu( stringsON, ID_CHANGE_SUBMIT, LoadStringResource( IDS_SUBMIT_SELECTED ) );
			}
			popMenu.AppendMenu( MF_SEPARATOR);
			popMenu.AppendMenu( stringsON, ID_POSITIONDEPOT, LoadStringResource( IDS_POSITIONDEPOT ) );
			BOOL b = TRUE;
			if (GET_SERVERLEVEL() >= 19)			// 2005.1 or later?
			{
				CP4FileStats *stats= (CP4FileStats *) GetLParam(currentItem);
				if (stats->GetOtherOpenAction() == 0)
					b = FALSE;
			}
			if (b)
				popMenu.AppendMenu( stringsON, ID_POSITIONCHGS, LoadStringResource( IDS_POSITIONOTHERCHG ) );
			if (GetSelectedCount()==1)
				popMenu.AppendMenu( stringsON, ID_ADD_BOOKMARK, LoadStringResource(IDS_ADD_BOOKMARK) );
			popMenu.AppendMenu( stringsON, ID_WINEXPLORE, LoadStringResource( IDS_EXPLORE ) );
			popMenu.AppendMenu( stringsON, ID_CMDPROMPT, LoadStringResource( IDS_CMDPROMPT ) );
		}

		if(AnyJobs())
		{
			// job options
			popMenu.AppendMenu( stringsON, ID_JOB_DESCRIBE, LoadStringResource( IDS_DESCRIBEFIXEDJOB ) );	
			popMenu.AppendMenu( stringsON, ID_JOB_EDITSPEC, LoadStringResource( ID_JOB_EDITSPEC ) );
			popMenu.AppendMenu( stringsON, ID_CHANGE_REMOVEFIX, LoadStringResource( IDS_UNFIXJOB ) );	
		}
		else
		{
			popMenu.AppendMenu( MF_SEPARATOR );
			popMenu.AppendMenu( stringsON, ID_FILE_REVERT, LoadStringResource( IDS_REVERT ) );	
		}
	}


EndContext:
	if( currentItem != NULL && !IsMyPendingChangeItem( currentItem ) && Level == 2)
	{
		if ( IsAFile(currentItem) )
		{
			popMenu.AppendMenu( stringsON, ID_FILE_PROPERTIES, LoadStringResource( IDS_PROPERTIES ) );
			popMenu.AppendMenu( stringsON, ID_FILE_REVISIONHISTORY, LoadStringResource( IDS_REVISIONHISTORY ) );
			popMenu.AppendMenu( stringsON, ID_POSITIONDEPOT, LoadStringResource( IDS_POSITIONDEPOT ) );
			popMenu.AppendMenu( stringsON, ID_POSITIONCHGS, LoadStringResource( IDS_POSITIONOTHERCHG ) );
		}
		else
			popMenu.AppendMenu( stringsON, ID_JOB_DESCRIBE, LoadStringResource( IDS_DESCRIBEFIXEDJOB ));
	}

	if ( Level == 1 && popMenu.GetMenuItemCount( ) > 0 )
		popMenu.AppendMenu(MF_SEPARATOR);

	if ( Level != 2 )
	{
        popMenu.AppendMenu( stringsON, ID_CHANGE_NEW, LoadStringResource( IDS_NEWCHANGELIST ) );
        popMenu.AppendMenu( stringsON, ID_SORTCHGFILESBYNAME, LoadStringResource( IDS_SORTCHGFILESBYNAME ) );
        popMenu.AppendMenu( stringsON, ID_SORTCHGFILESBYEXT, LoadStringResource( IDS_SORTCHGFILESBYEXT ) );
        popMenu.AppendMenu( stringsON, ID_SORTCHGFILESBYACTION, LoadStringResource( IDS_SORTCHGFILESBYACTION ) );
        popMenu.AppendMenu( stringsON, ID_SORTCHGFILESBYRESOLVE, LoadStringResource( IDS_SORTCHGFILESBYRESOLVE ) );
		popMenu.AppendMenu( stringsON, ID_SORTCHGSBYUSER, LoadStringResource( IDS_SORTCHGSBYUSER ) );

		if( currentItem != NULL && !IsMyPendingChange( currentItem ) && Level == 1)
		{
			popMenu.AppendMenu( MF_SEPARATOR );
			popMenu.AppendMenu( stringsON, ID_USER_SWITCHTOUSER, LoadStringResource(IDS_USER_SWITCHTOUSER));
		}
		if( currentItem != NULL && !underMyRoot && Level == 1)
			popMenu.AppendMenu( stringsON, ID_CLIENTSPEC_SWITCH, LoadStringResource(IDS_CLIENTSPEC_SWITCH));

		if( currentItem != NULL && !underMyRoot && Level == 0)
		{
			popMenu.AppendMenu( MF_SEPARATOR );
			popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_FILTER_SETVIEW, LoadStringResource(IDS_FILTER_PCO_SETVIEW));
			popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_FILTER_CLEARVIEW, LoadStringResource(IDS_FILTER_PCO_CLEARVIEW));
		}
		popMenu.AppendMenu( MF_SEPARATOR );
		popMenu.AppendMenu( stringsON, ID_VIEW_UPDATE, LoadStringResource( IDS_REFRESH ) );
	}

	MainFrame()->AddToolsToContextMenu(&popMenu);

	// Finally blast the menu onto the screen
	m_InContextMenu = TRUE;
	popMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	point.x, point.y, AfxGetMainWnd());
	m_InContextMenu = FALSE;
}


void CDeltaTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// Do nothing	
}

void CDeltaTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
    //CTreeView::OnRButtonUp(nFlags, point);
	CPoint screenPt=point;
	ClientToScreen(&screenPt);
    m_ContextContext= MOUSEHIT;
	OnContextMenu(NULL, screenPt);
}



//////////////////////////////////////////////////////////////////
// Handlers for OnUpdateUI


void CDeltaTreeCtrl::OnUpdateChangeDescribe(CCmdUI* pCmdUI) 
{
	long chgnbr = 0;
	BOOL underMyRoot;
	BOOL b = GetSelectedCount()==1 && 
			 GetItemLevel( GetSelectedItem(0), &underMyRoot ) == 1 &&
			 (chgnbr = GetSelectedChangeNumber()) > 0;

    CString txt;
	if (b)
		txt.FormatMessage(IDS_DESCRIBEPENDING_d, chgnbr);
	else
		txt.LoadString(IDS_DESCRIBESUBMITTED);
	pCmdUI->SetText ( txt );
	pCmdUI->Enable(!SERVER_BUSY() && b);
}

void CDeltaTreeCtrl::OnUpdateChgEdspec(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && !m_EditInProgress &&
					( IsSelectionSubmittableChange() ||
					  IsSelectionInSubmittableChange() ||

					  ( GetSelectedCount()==1 &&
					  IsMyPendingChange(GetSelectedItem(0)) &&
					  GetChangeNumber(GetSelectedItem(0)) != 0) ) 

					  );
}

void CDeltaTreeCtrl::OnUpdateChgDel(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText( LoadStringResource ( IDS_DELEMPTYCHANGELIST ) );
	pCmdUI->Enable(!SERVER_BUSY() && GetSelectedCount()==1 &&
					IsMyPendingChange(GetSelectedItem(0)) &&
					!HasChildren(GetSelectedItem(0)) 
					&& GetChangeNumber(GetSelectedItem(0)) != 0 );
}

void CDeltaTreeCtrl::OnUpdateChgNew(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && !m_EditInProgress);	
}

void CDeltaTreeCtrl::OnUpdateChgRevorig(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText( LoadStringResource ( IDS_REVERTUNCHANGED ) );
	pCmdUI->Enable(!SERVER_BUSY() && GetSelectedCount()==1 &&
					IsMyPendingChange(GetSelectedItem(0)) &&
					AnyFilesInChange(GetSelectedItem(0)) );	
}

void CDeltaTreeCtrl::OnUpdateChgSubmit(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText( LoadStringResource ( IDS_SUBMIT ) );
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() && !m_EditInProgress
		&& ( IsSelectionSubmittableChange()
			|| IsSelectionInSubmittableChange() ) ) );	
}

void CDeltaTreeCtrl::OnUpdateFileLock(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() && AnyMyUnLocked()) );
}

void CDeltaTreeCtrl::OnUpdateFileUnlock(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() && AnyMyLock()) );
}

void CDeltaTreeCtrl::OnUpdateFileRevisionhistory(CCmdUI* pCmdUI) 
{
	BOOL root;
	BOOL enable= (!SERVER_BUSY() && GetSelectedCount() == 1 &&
					GetItemLevel(GetSelectedItem(0), &root)== 2 && 
					IsAFile( GetSelectedItem(0)) );
	if( enable )
	{
		CP4FileStats *fs= (CP4FileStats *) GetLParam( GetSelectedItem(0) );
		if( fs->GetHaveRev() <= 1 &&
			( fs->GetMyOpenAction() == F_ADD || fs->GetMyOpenAction() == F_BRANCH ) )
			enable= FALSE;
	}
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, enable) );
}

void CDeltaTreeCtrl::OnUpdateFileAnnotate(CCmdUI* pCmdUI) 
{
	BOOL root;
	BOOL enable = !SERVER_BUSY() && GET_SERVERLEVEL() >= 14
					&& GetSelectedCount()==1
					&& GetItemLevel(GetSelectedItem(0), &root)== 2 
					&& IsAFile( GetSelectedItem(0) );
	if( enable )
	{
		CP4FileStats *fs= (CP4FileStats *) GetLParam( GetSelectedItem(0) );
		if( fs->GetHaveRev() <= 1 &&
			( fs->GetMyOpenAction() == F_ADD || fs->GetMyOpenAction() == F_BRANCH ) )
			enable= FALSE;
		if (enable)
		{
			CString fileType = fs->GetHeadType();
			enable = ((fileType.Find(_T("text")) != -1) 
					 || (fileType.Find(_T("symlink")) != -1)) ? TRUE : FALSE;
		}
	}
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, enable) );
}

void CDeltaTreeCtrl::OnUpdateWinExplore(CCmdUI* pCmdUI) 
{
	BOOL root;
	pCmdUI->Enable(GetSelectedCount() &&
					GetItemLevel(GetSelectedItem(0), &root)== 2 && 
					IsMyPendingChangeFile( GetSelectedItem(0)) );
}

void CDeltaTreeCtrl::OnUpdateCmdPrompt(CCmdUI* pCmdUI) 
{
	BOOL root;
	pCmdUI->Enable(GetSelectedCount() &&
					GetItemLevel(GetSelectedItem(0), &root)== 2 && 
					IsMyPendingChangeFile( GetSelectedItem(0)) );
}

void CDeltaTreeCtrl::OnUpdateFileGet(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() 
		&& GetSelectedCount() > 0 
		&& IsMyPendingChangeFile(GetSelectedItem(0))) );
}

void CDeltaTreeCtrl::OnUpdateFileDiffhead(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY()
		&& GetSelectedCount() > 0
		&& (IsMyPendingChangeFile(GetSelectedItem(0))
		 || (IsMyPendingChange(GetSelectedItem(0))
		  && AnyFilesInChange(GetSelectedItem(0))))));	
}

void CDeltaTreeCtrl::OnUpdateFiletype(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && GetSelectedCount() > 0 &&
					AnyMyPendingChangeFiles() );	
}

void CDeltaTreeCtrl::OnUpdateMoveFiles(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && GetSelectedCount() > 0
		&& AnyMyPendingChangeFiles()
		&& !m_EditInProgress );	
}

void CDeltaTreeCtrl::OnUpdateJobDescribe(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( OnUpdateJob(pCmdUI, IDS_DESCRIBEIT_s) );
}

void CDeltaTreeCtrl::OnUpdateJobEditspec( CCmdUI* pCmdUI ) 
{
	pCmdUI->Enable( OnUpdateJob(pCmdUI, IDS_EDITSPEC_s) );
}

BOOL CDeltaTreeCtrl::OnUpdateJob(CCmdUI* pCmdUI, int msgnbr)
{
	BOOL bEnable = FALSE;
	int cnt = GetSelectedCount();
	if (!SERVER_BUSY() && cnt >= 1 && AnyJobs())
	{
		HTREEITEM item= GetSelectedItem(0);
		
		int i = 0;
		if (cnt > 1)
		{
			BOOL underMyRoot;
			int level= GetItemLevel(item, &underMyRoot);

			if(level ==2)
			{
				for( ; i < cnt; i++)
				{
					if(GetLParam(GetSelectedItem(i)) == NULL)
					{
						item= GetSelectedItem(i);
						break;
					}
				}
			}
		}

		if (item == NULL || GetLParam(item) != NULL)
			ASSERT(0);
		else
		{
			CString itemStr=GetItemText(item);
			itemStr.TrimLeft(); 
			if (!itemStr.IsEmpty( ))
			{
				CString txt;
				txt.FormatMessage(msgnbr, TruncateString(itemStr, 50));
				pCmdUI->SetText ( txt );
				bEnable = TRUE;
			}
		}
	}
	return bEnable ;
}

void CDeltaTreeCtrl::OnUpdateAddjobfix(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && 
			  (	 (GetSelectedCount() == 1 && IsMyPendingChange(GetSelectedItem(0)) && 
				  GetChangeNumber(GetSelectedItem(0)) )  ||
			     IsSelectionInMyNumberedChange()) );
}

void CDeltaTreeCtrl::OnUpdateRemovefix(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && GetSelectedCount() == 1 &&
					AnyJobs() );	
}


void CDeltaTreeCtrl::OnUpdateFileRevert(CCmdUI* pCmdUI) 
{
	int cnt = GetSelectedCount();
	HTREEITEM currentItem = GetSelectedItem(0);
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() && cnt > 0 
		&& (AnyMyPendingChangeFiles() || (cnt == 1 
										&& IsMyPendingChange(currentItem) 
										&& AnyFilesInChange(currentItem)))) );	
}


//////////////////////////////////////////////////////////////
// Funcions for use in OnUpdateUI handlers and OnContextMenu()

BOOL CDeltaTreeCtrl::IsMyPendingChange(HTREEITEM currentItem)
{
	BOOL myChange= FALSE;
	BOOL underMyRoot;
	int level= GetItemLevel(currentItem, &underMyRoot);

	if(level==1 && underMyRoot)
	{
		if( GetImage( currentItem) != CP4ViewImageList::VI_YOUROTHERCHANGE )
			myChange= TRUE;
	}
	
	return myChange;
}

BOOL CDeltaTreeCtrl::IsOpenedForInteg(HTREEITEM currentItem)
{
	BOOL opened4integ= FALSE;
	BOOL underMyRoot;
	int level= GetItemLevel(currentItem, &underMyRoot);

	if(level==2 && underMyRoot && GetLParam(currentItem) != NULL)
	{
		CP4FileStats *fs= (CP4FileStats *) GetLParam(currentItem);
		if(fs->GetMyOpenAction()==F_INTEGRATE && fs->GetHaveRev() != 0)
		{
			opened4integ=TRUE;
		}
	}
	return opened4integ;
}

BOOL CDeltaTreeCtrl::IsOpenedForBranch(HTREEITEM currentItem)
{
	BOOL opened4branch= FALSE;
	BOOL underMyRoot;
	int level= GetItemLevel(currentItem, &underMyRoot);

	if(level==2 && underMyRoot && GetLParam(currentItem) != NULL)
	{
		CP4FileStats *fs= (CP4FileStats *) GetLParam(currentItem);
		if(fs->GetMyOpenAction()==F_BRANCH && fs->GetHaveRev() != 0)
		{
			opened4branch=TRUE;
		}
	}
	return opened4branch;
}

BOOL CDeltaTreeCtrl::IsMyPendingChangeFile(HTREEITEM currentItem)
{
	BOOL myFile= FALSE;
	BOOL underMyRoot;
	int level= GetItemLevel(currentItem, &underMyRoot);

	if(level==2 && underMyRoot && GetLParam(currentItem) != NULL)
	{
		CP4FileStats *fs= (CP4FileStats *) GetLParam(currentItem);
		if( !fs->IsOtherUserMyClient() )
			myFile= TRUE;
	}

	return myFile;
}

BOOL CDeltaTreeCtrl::IsMyPendingChangeItem(HTREEITEM currentItem)
{
	BOOL myItem= FALSE;
	BOOL underMyRoot;
	int level= GetItemLevel(currentItem, &underMyRoot);

	if(level==2 && underMyRoot)
	{
		HTREEITEM changeItem= GetParentItem(currentItem);
		if( GetImage( changeItem ) != CP4ViewImageList::VI_YOUROTHERCHANGE )
			myItem= TRUE;
	}

	return myItem;
}

BOOL CDeltaTreeCtrl::IsSelectionSubmittableChange()
{
	// True if only one item is selected and that item is a change
	// that I can be submit
	return ( GetSelectedCount()==1 && 
		     IsMyPendingChange(GetSelectedItem(0)) &&
			 AnyFilesInChange(GetSelectedItem(0)) );
}

BOOL CDeltaTreeCtrl::IsSelectionInSubmittableChange()
{
	// True if item(s) selected are in a change
	// that I can be submit
	return ( GetSelectedCount() > 0 && 
			 IsMyPendingChangeItem(GetSelectedItem(0)) &&
			 AnyFilesInChange( GetParentItem(GetSelectedItem(0)) ) );
}

BOOL CDeltaTreeCtrl::IsSelectionInMyNumberedChange()
{
	// True if item(s) selected are in my numbered (not default) change
	if( GetSelectedCount() == 0 )
		return FALSE;

	HTREEITEM parentItem=GetParentItem(GetSelectedItem(0));
	if(parentItem == NULL)
		return FALSE;
	
	return( IsMyPendingChange(parentItem) && GetChangeNumber(parentItem) > 0 );
}


BOOL CDeltaTreeCtrl::AnyMyPendingChangeFiles()
{
	BOOL myChangeFile=FALSE;
	
	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		if(IsMyPendingChangeFile(GetSelectedItem(i)))
		{
			myChangeFile=TRUE;
			break;
		}
	}
	return myChangeFile;
}

BOOL CDeltaTreeCtrl::AnyJobs()
{
	BOOL anyJobs=FALSE;
	
	if(GetSelectedCount() ==0 )
		{ return FALSE; }

	HTREEITEM firstItem= GetSelectedItem(0);
	
	BOOL underMyRoot;
	int level= GetItemLevel(firstItem, &underMyRoot);

	if(level ==2)
	{
		for(int i=GetSelectedCount()-1; i>=0; i--)
		{
			if(GetLParam(GetSelectedItem(i)) == NULL)
			{
				anyJobs=TRUE;
				break;
			}
		}
	}
	return anyJobs;
}

BOOL CDeltaTreeCtrl::AnyFilesInChange( HTREEITEM changeItem )
{
	BOOL anyFiles=FALSE;
	
	if( changeItem == NULL )
		{ return FALSE; }

	HTREEITEM child= GetChildItem( changeItem );
	while( child != NULL )
	{
		if( GetLParam( child ) > 0 )
		{
			anyFiles= TRUE;
			break;
		}
		child= GetNextSiblingItem( child );
	}
	
	return anyFiles;
}


BOOL CDeltaTreeCtrl::AnyMyInteg()
{
	BOOL anyMyInteg=FALSE;
	
	if(GetSelectedCount() ==0 )
		{ return FALSE; }

	HTREEITEM firstItem= GetSelectedItem(0);
		
	if(IsMyPendingChangeItem( firstItem ))
	{
		for(int i=GetSelectedCount()-1; i>=0; i--)
		{
			if(IsOpenedForInteg(GetSelectedItem(i)))
			{
				anyMyInteg=TRUE;
				break;
			}
		}
	}
	return anyMyInteg;
}


BOOL CDeltaTreeCtrl::AnyMyBranch()
{
	BOOL anyMyBranch=FALSE;
	
	if(GetSelectedCount() ==0 )
		{ return FALSE; }

	HTREEITEM firstItem= GetSelectedItem(0);
		
	if(IsMyPendingChangeItem( firstItem ))
	{
		for(int i=GetSelectedCount()-1; i>=0; i--)
		{
			if(IsOpenedForBranch(GetSelectedItem(i)))
			{
				anyMyBranch=TRUE;
				break;
			}
		}
	}
	return anyMyBranch;
}


BOOL CDeltaTreeCtrl::AnyMyLock()
{
	BOOL anyMyLock=FALSE;
	
	if(GetSelectedCount() ==0 )
		{ return FALSE; }

	HTREEITEM firstItem= GetSelectedItem(0);
	CP4FileStats *stats;
		
	if(IsMyPendingChangeItem( firstItem ))
	{
		for(int i=GetSelectedCount()-1; i>=0; i--)
		{
			stats= (CP4FileStats *) GetLParam(GetSelectedItem(i));
			if(stats != NULL)
			{
				if(stats->IsMyLock())
				{
					anyMyLock=TRUE;
					break;
				}
			}
		}
	}
	return anyMyLock;

}


BOOL CDeltaTreeCtrl::AnyMyUnLocked()
{
	BOOL anyMyUnLocked=FALSE;
	
	if(GetSelectedCount() ==0 )
		{ return FALSE; }

	HTREEITEM firstItem= GetSelectedItem(0);
	CP4FileStats *stats;
	
	if(IsMyPendingChangeItem(firstItem))
	{
		for(int i=GetSelectedCount()-1; i>=0; i--)
		{
			stats= (CP4FileStats *) GetLParam(GetSelectedItem(i));
			if(stats != NULL)
			{
				if(!stats->IsMyLock())
				{
					anyMyUnLocked=TRUE;
					break;
				}
			}
		}
	}
	return anyMyUnLocked;
}

// A file is consider binary (as far as this routine is concerned)
// if its base type is "binary"
// or its base type is "text" plus its storage type is C or F
BOOL CDeltaTreeCtrl::AnyBinaryFiles(BOOL bAnyResolvable/*=FALSE*/)
{
	BOOL anyMyFilesBinary=FALSE;
	
	if(GetSelectedCount() ==0 )
		{ return FALSE; }

	HTREEITEM firstItem= GetSelectedItem(0);
		
	if(IsMyPendingChangeItem( firstItem ))
	{
		int		baseType;
		int		storeType;
		BOOL	typeK;
		BOOL	typeW;
		BOOL	typeX;
		BOOL	typeO;
		BOOL	typeM;
		BOOL	typeL;
		BOOL	typeS;
		DWORD_PTR		nbrrevs;
		BOOL	unknown;
		for(int i=GetSelectedCount()-1; i>=0; i--)
		{
			TheApp()->GetFileType(GetItemText(GetSelectedItem(i)), 
							baseType, storeType, typeK, typeW, typeX, 
							typeO, typeM, typeL, typeS, nbrrevs, unknown);
			if ((baseType == 1) 
			 || (baseType == 0 && (storeType == 1 || storeType == 3))
			 || (baseType == 5 && (storeType == 1 || storeType == 3)))
			{
				if (bAnyResolvable)
				{
					CP4FileStats *stats = (CP4FileStats *) GetLParam(GetSelectedItem(i));
					if(stats != NULL)
					{
						if (!stats->IsUnresolved() && !stats->IsResolved())
							continue;
					}
				}
				anyMyFilesBinary=TRUE;
				break;
			}
		}
	}
	return anyMyFilesBinary;

}


BOOL CDeltaTreeCtrl::AnyUnresolvedFiles()
{
	BOOL anyUnresolved=FALSE;
	
	if(GetSelectedCount() ==0 )
		{ return FALSE; }

	HTREEITEM firstItem= GetSelectedItem(0);
	CP4FileStats *stats;
	
	if(IsMyPendingChangeItem(firstItem))
	{
		for(int i=GetSelectedCount()-1; i>=0; i--)
		{
			stats= (CP4FileStats *) GetLParam(GetSelectedItem(i));
			if(stats != NULL)
			{
				if(stats->IsUnresolved())
				{
					anyUnresolved=TRUE;
					break;
				}
			}
		}
	}
	return anyUnresolved;
}

BOOL CDeltaTreeCtrl::AnyResolvedFiles(BOOL bList/*=FALSE*/)
{
	BOOL anyResolved=FALSE;

	if (bList)
		m_StringList.RemoveAll();
	
	if(GetSelectedCount() ==0 )
		{ return FALSE; }

	HTREEITEM firstItem= GetSelectedItem(0);
	CP4FileStats *stats;
	
	if(IsMyPendingChangeItem(firstItem))
	{
		for(int i=GetSelectedCount()-1; i>=0; i--)
		{
			stats= (CP4FileStats *) GetLParam(GetSelectedItem(i));
			if(stats != NULL)
			{
				if(stats->IsResolved())
				{
					anyResolved=TRUE;
					if (bList)
						 m_StringList.AddHead(stats->GetFullDepotPath());
					else break;
				}
			}
		}
	}
	return anyResolved;
}

BOOL CDeltaTreeCtrl::AnyUnresolvedFilesInChg(HTREEITEM chgitem)
{
	BOOL anyUnresolved=FALSE;
	
	CP4FileStats *stats;
	HTREEITEM item=GetChildItem(chgitem);
	while(item!=NULL)
	{
		if(IsAFile(item))
		{
			stats= (CP4FileStats *) GetLParam(item);
			if(stats != NULL)
			{
				if(stats->IsUnresolved())
				{
					anyUnresolved=TRUE;
					break;
				}
			}
		}
		item=GetNextSiblingItem(item);
	}
	return anyUnresolved;
}

BOOL CDeltaTreeCtrl::AnyResolvedFilesInChg(HTREEITEM chgitem)
{
	BOOL anyResolved=FALSE;

	CP4FileStats *stats;
	HTREEITEM item=GetChildItem(chgitem);
	while(item!=NULL)
	{
		if(IsAFile(item))
		{
			stats= (CP4FileStats *) GetLParam(item);
			if(stats != NULL)
			{
				if(stats->IsResolved())
				{
					anyResolved=TRUE;
					break;
				}
			}
		}
		item=GetNextSiblingItem(item);
	}
	return anyResolved;
}

BOOL CDeltaTreeCtrl::AnyMyFilesUnresolved( )
{
	BOOL anyUnresolved=FALSE;
	
	HTREEITEM change= GetChildItem(m_MyRoot);
	HTREEITEM file;
	LPARAM lParam;

	while(change != NULL && !anyUnresolved)
	{
		file= GetChildItem(change);	
		while(file != NULL && !anyUnresolved)
		{
			lParam=GetLParam(file);
			if(lParam > 0)
            {
				CP4FileStats *stats= (CP4FileStats *) lParam;
                ASSERT_KINDOF(CP4FileStats, stats);
				if(stats->IsUnresolved())
				{
					anyUnresolved=TRUE;
					break;
				}
            }

			file=GetNextSiblingItem(file);
		}
		change=GetNextSiblingItem(change);
	}
	return anyUnresolved;
}

BOOL CDeltaTreeCtrl::AnyMyFilesResolved(BOOL bList/*=FALSE*/)
{
	BOOL anyResolved=FALSE;

	if (bList)
		m_StringList.RemoveAll();
	
	HTREEITEM change= GetChildItem(m_MyRoot);
	HTREEITEM file;
	LPARAM lParam;

	while(change != NULL && (!anyResolved || bList))
	{
		file= GetChildItem(change);	
		while(file != NULL && (!anyResolved || bList))
		{
			lParam=GetLParam(file);
			if(lParam > 0)
            {
				CP4FileStats *stats= (CP4FileStats *) lParam;
                ASSERT_KINDOF(CP4FileStats, stats);
				if(stats->IsResolved())
				{
					anyResolved=TRUE;
					if (bList)
						 m_StringList.AddHead(stats->GetFullDepotPath());
					else break;
				}
            }

			file=GetNextSiblingItem(file);
		}
		change=GetNextSiblingItem(change);
	}
	return anyResolved;
}


void CDeltaTreeCtrl::OnUpdateFileAutoresolve(CCmdUI* pCmdUI) 
{
	BOOL root;
	BOOL rc = FALSE;
	if (!SERVER_BUSY() && GetSelectedCount() > 0)
	{
		HTREEITEM item = GetSelectedItem(0);
		int level = GetItemLevel(item, &root);
		if (level == 2)
			rc = ((AnyMyFilesUnresolved() || AnyMyFilesResolved())
					&& IsMyPendingChangeFile(item)
					&& IsAFile( item) );
		else if (level == 1)
			rc = (IsMyPendingChange(item) 
					&& (AnyUnresolvedFilesInChg(item) || AnyResolvedFilesInChg(item))
					&& GetSelectedCount()==1);
	}
	pCmdUI->Enable(rc);
}

void CDeltaTreeCtrl::OnUpdateFileResolve(CCmdUI* pCmdUI) 
{
	BOOL root;
	BOOL rc = FALSE;
	if (!SERVER_BUSY() && GetSelectedCount() > 0)
	{
		HTREEITEM item = GetSelectedItem(0);
		int level = GetItemLevel(item, &root);
		if (level == 2)
			rc = ((AnyUnresolvedFiles() || AnyResolvedFiles())
					&& IsMyPendingChangeFile(item) );
		else if (level == 1)
			rc = (IsMyPendingChange(GetSelectedItem(0))
					&& (AnyUnresolvedFilesInChg(item) || AnyResolvedFilesInChg(item))
					&& GetSelectedCount()==1);
	}
	pCmdUI->Enable(rc);
}

void CDeltaTreeCtrl::OnUpdateTheirFile(CCmdUI* pCmdUI) 
{
	BOOL root;
	HTREEITEM item = GetSelectedItem(0);
	if (!SERVER_BUSY() && GetSelectedCount() == 1 && GetItemLevel(item, &root) == 2)
		OnUpdateFileResolve(pCmdUI);
	else
		pCmdUI->Enable(FALSE);
}

void CDeltaTreeCtrl::OnFileAutoresolve() 
{
	BOOL root;
	HTREEITEM initialItem = GetSelectedItem(0);
	int level = GetItemLevel(initialItem, &root);
	if (level == 1)
	{
		SelectAllFilesInChange(initialItem, 2);
		if (GetSelectedCount() < 1)
		{
			UnselectAll();
			SetSelectState(initialItem, TRUE);
			return;
		}
		else if (GetSelectedCount() > 32000)
		{	
			AfxMessageBox(IDS_UNABLE_TO_RESOLVE_MORE_THAN_32000_FILES, MB_ICONEXCLAMATION);
			UnselectAll();
			SetSelectState(initialItem, TRUE);
			return;
		}
	}

	MainFrame()->DoNotAutoPoll();

	CAutoResolveDlg dlg;

	BOOL u = AnyUnresolvedFiles();
	BOOL r = AnyResolvedFiles();
	dlg.m_NoSel2Res = (GetSelectedCount() && (u || r)) ? FALSE : TRUE;
	if (!u && r)
		dlg.m_ReResolve = TRUE;
	dlg.m_SelResolved = r;
	dlg.m_AnyResolved = AnyMyFilesResolved();
	dlg.m_ResolveFromChgList = (level == 1);
	dlg.m_pDeltaTreeCtrl = this;
	SetAppearance(TRUE, FALSE, FALSE);
	InvalidateRect( NULL, FALSE );
	UpdateWindow( );
	SetSelectAtts(TVIS_SELECTED);	// Clear the flag we set in call to SetAppearance() above

	if (dlg.DoModal() == IDOK)
	{
		if (dlg.m_AllFiles)
			 m_StringList.RemoveAll();
		else
		{
			if ((level == 1) && r && !dlg.m_ReResolve)
			{
				SelectAllFilesInChange(initialItem, 1);
				SetAppearance(TRUE, FALSE, FALSE);
				InvalidateRect( NULL, FALSE );
				UpdateWindow( );
				SetSelectAtts(TVIS_SELECTED);	// Clear the flag we set in call to SetAppearance() above
				if (GetSelectedCount() < 1)
				{
					UnselectAll();
					SetSelectState(initialItem, TRUE);
					AddToStatus(_T("0 files resolved"), SV_COMPLETION);
					MainFrame()->ResumeAutoPoll();
					return;
				}
			}
			AssembleStringList( );
		}

		CCmd_AutoResolve *pCmd= new CCmd_AutoResolve;
		pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
		if( pCmd->Run( &m_StringList, dlg.m_ResolveType, 
						dlg.m_Preview, dlg.m_ReResolve, dlg.m_TextMerge, dlg.m_ResolveWhtSp ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_AUTO_RESOLVING) );
		else
			delete pCmd;
	}
	if (level == 1)
	{
		UnselectAll();
		SetSelectState(initialItem, TRUE);
	}
	MainFrame()->ResumeAutoPoll();
}

void CDeltaTreeCtrl::OnFileResolve() 
{
	OnFileMergeResolve(FALSE);
}

void CDeltaTreeCtrl::OnFileMerge() 
{
	OnFileMergeResolve(TRUE);
}

void CDeltaTreeCtrl::OnFileMergeResolve(BOOL bRunMerge) 
{
	int r, b, havehead;

	if (!GetSelectedCount())
		return;

	m_bRunMerge = bRunMerge;
	HTREEITEM initialItem = GetSelectedItem(0);
	BOOL root;
	int level = GetItemLevel(initialItem, &root);
	if (level == 1)
	{
		SelectAllFilesInChange(initialItem, 2);
		if (GetSelectedCount() < 1)
		{
			UnselectAll();
			SetSelectState(initialItem, TRUE);
			return;
		}
		else if (GetSelectedCount() > 32000)
		{	
			AfxMessageBox(IDS_UNABLE_TO_RESOLVE_MORE_THAN_32000_FILES, MB_ICONEXCLAMATION);
			UnselectAll();
			SetSelectState(initialItem, TRUE);
			return;
		}
	}

	if (!IsMyPendingChangeItem(GetSelectedItem(0)))
	{
		if (level == 1)
		{
			UnselectAll();
			SetSelectState(initialItem, TRUE);
		}
		return;
	}
	SetAppearance(TRUE, FALSE, FALSE);
	InvalidateRect( NULL, FALSE );
	UpdateWindow( );
	SetSelectAtts(TVIS_SELECTED);	// Clear the flag we set in call to SetAppearance() above

	HTREEITEM item;
	BOOL textualMerge = FALSE;
	r = AnyResolvedFiles();
	b = AnyBinaryFiles(TRUE);
	if (b)
	{
		b = FALSE;
		textualMerge = TRUE;
		CString appName;
		for(int i=GetSelectedCount()-1; i>=0; i--)
		{
			int		baseType;
			int		storeType;
			BOOL	typeK;
			BOOL	typeW;
			BOOL	typeX;
			BOOL	typeO;
			BOOL	typeM;
			BOOL	typeL;
			BOOL	typeS;
			DWORD_PTR		nbrrevs;
			BOOL	unknown;
			int j;
			appName.Empty();
			CString filename = GetItemText(item = GetSelectedItem(i));
			TheApp()->GetFileType(filename, baseType, storeType, typeK, typeW, typeX, 
									typeO, typeM, typeL, typeS, nbrrevs, unknown);
			if ((baseType == 1) 
			 || (baseType == 0 && (storeType == 1 || storeType == 3))
			 || (baseType == 5 && (storeType == 1 || storeType == 3)))
			{
				CP4FileStats *stats = (CP4FileStats *) GetLParam(item);
				if(stats != NULL)
				{
					if (!stats->IsUnresolved() && !stats->IsResolved())
						continue;
				}
				CString extension = GetFilesExtension(filename);
				if ((j = extension.Find(_T('#'))) != -1)
					extension = extension.Left(j);
				if(!extension.IsEmpty())
					appName= GET_P4REGPTR()->GetAssociatedMerge(extension);
				if (appName.IsEmpty())
				{
					b = TRUE;
					textualMerge = FALSE;
					break;
				}
			}
		}
	}
	if (r || b)
	{
		if (GetSelectedCount() == 1)
		{
			CP4FileStats *stats= (CP4FileStats *) GetLParam(GetSelectedItem(0));
			if(stats != NULL)
				 havehead = (stats->GetHeadRev() == stats->GetHaveRev()) ? 1 : 0;
			else havehead = 0;
		}
		else havehead = 0;
		if (r && !b && havehead && !AnyUnresolvedFiles())
		{
			m_ReResolve = TRUE;
			m_TextualMerge = FALSE;
		}
		else
		{
			CResolveFlagsDlg dlg;

			dlg.m_ReResolve = r;
			dlg.m_TextualMerge = b;
			if (dlg.DoModal() == IDOK)
			{
				m_ReResolve = dlg.m_ReResolve;
				m_TextualMerge = b ? dlg.m_TextualMerge : textualMerge;
			}
			else return;
		}
	}
	else
	{
		m_ReResolve = FALSE;
		m_TextualMerge = textualMerge;
	}

	if (level == 1)
	{
		if (AnyResolvedFiles() && !m_ReResolve)
		{
			SelectAllFilesInChange(initialItem, 1);
			SetAppearance(TRUE, FALSE, FALSE);
			InvalidateRect( NULL, FALSE );
			UpdateWindow( );
			SetSelectAtts(TVIS_SELECTED);	// Clear the flag we set in call to SetAppearance() above
			if (GetSelectedCount() < 1)
			{
				UnselectAll();
				SetSelectState(initialItem, TRUE);
				AddToStatus(_T("0 files resolved"), SV_COMPLETION);
				return;
			}
		}
		// If the server is busy because we triggered an expand of a changelist
		// and are getting the attached jobs, wait for the server to finish
		if (SERVER_BUSY())
		{
			int t=GET_P4REGPTR()->BusyWaitTime();
			do
			{
				Sleep(50);
				t -= 50;
			} while (SERVER_BUSY() && t > 0);
		}
	}

	m_ResolveList.RemoveAll();
	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item = GetSelectedItem(i);
		m_ResolveList.AddHead((CObject *)item);
	}

	// fire up a resolve on the first item
	item = (HTREEITEM)(m_ResolveList.RemoveHead());
	ResolveItem(item);
	if (level == 1)
	{
		UnselectAll();
		SetSelectState(initialItem, TRUE);
	}
}

void CDeltaTreeCtrl::ResolveItem(HTREEITEM item)
{
	BOOL bHeadIsText = FALSE;
	m_ForcedResolve = FALSE;
	m_ActiveItem=item;
	CString itemStr = GetItemText(item);
	itemStr=itemStr.Left(itemStr.ReverseFind(_T('#')));  // Strip revision number
	CP4FileStats *stats = (CP4FileStats *) GetLParam(item);
	if (stats != NULL)
	{
		bHeadIsText = stats->GetHeadType().Find(_T("text")) != -1 ? TRUE : FALSE;
		if (m_ReResolve)
		{
			if (stats->IsResolved())
				m_ForcedResolve = TRUE;
		}
	}
	CCmd_Resolve *pCmd= new CCmd_Resolve;
	pCmd->Init( m_hWnd, RUN_ASYNC);
	pCmd->SetHeadIsText(bHeadIsText);
	if( pCmd->Run( itemStr, m_ForcedResolve, m_TextualMerge ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_RESOLVE) );	
	else
		delete pCmd;
}

void CDeltaTreeCtrl::SelectAllFilesInChange(HTREEITEM changeitem, int resolveFlag/*=0*/)
{
	UnselectAll();
	HTREEITEM item=GetChildItem(changeitem);
	while(item!=NULL)
	{
		BOOL bOK = IsAFile(item);
		if (resolveFlag && bOK)
		{
			bOK = FALSE;
			CP4FileStats *stats = (CP4FileStats *) GetLParam(item);
			if(stats != NULL)
			{
				if (stats->IsUnresolved() || ((resolveFlag == 2) && stats->IsResolved()))
					bOK = TRUE;
			}
		}
		if(bOK)
			SetSelectState(item, TRUE);
		item=GetNextSiblingItem(item);
	}
}

void CDeltaTreeCtrl::OnDestroy() 
{
	// Traverse tree, deleting each item's lParam
	// Cant wait till destructor, because Treeview items
	// are deleted by common ctrl before the destructor is called
	DeleteLParams(m_MyRoot);
	if (m_OthersRoot)
	{
		DeleteLParams(m_OthersRoot);
		m_OthersRoot = NULL;
	}
	CMultiSelTreeCtrl::OnDestroy();
}

void CDeltaTreeCtrl::ReopenAs(LPCTSTR newtype)
{
	if(GetSelectedCount()==0)
	{
		ASSERT(0);
		return;
	}

	AssembleStringList( );

	CCmd_ListOpStat *pCmd= new CCmd_ListOpStat;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
	if( pCmd->Run( &m_StringList, P4REOPEN, -1, newtype ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_REOPENING_FILES) );
	else
		delete pCmd;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// Support for file editing
///////////////////////////////////////////////////////////////////////////////////////////////
//
// Command UI update functions:
//		OnUpdateFileAutoedit(CCmdUI* pCmdUI) 
//
// Main Menu command handlers
//		OnFileQuickedit()		(will use associated app)
//		OnFileAutoedit()		(will start with chooser dialog)
//		----> EditFile()
//
// Context Menu command handlers
//		OnFileMRUEditor(UINT  nID)
//		OnFileNewEditor()
//		OnFileQuickedit()
//		----> EditFile()
//
// Command goes to depot window
//		EditFile()
//		----> SendMessage WM_FILEEDITTXT (or WM_FILEEDITBIN) to depot wnd, 
//						wparam= CString *path
//						lparam= 0-(MAX_MRU-1) for MRU, 
//								10 for quick edit, 
//								100 for chooser dialog
//								1000 for new editor
//
///////////////////////////////////////////////////////////////////////////////////////////////


void CDeltaTreeCtrl::OnUpdateFileAutoedit(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() && IsEditableFile()));	
}

void CDeltaTreeCtrl::OnUpdateFileAutobrowse(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() && IsEditableFile()));
}

BOOL CDeltaTreeCtrl::IsEditableFile()
{
	BOOL isEditable=FALSE;

	if(GetSelectedCount() == 1 && AnyMyPendingChangeFiles())
		isEditable=TRUE;
	
	return isEditable;
}

void CDeltaTreeCtrl::OnFileMRUEditor(UINT nID)
{
	EditFile(nID - ID_FILE_EDITOR_1, TRUE);
}

void CDeltaTreeCtrl::OnFileMRUBrowser(UINT nID)
{
	EditFile(nID - ID_FILE_BROWSER_1, FALSE);
}

void CDeltaTreeCtrl::OnFileQuickedit() 
{
	EditFile(EDIT_ASSOCVIEWER, TRUE);
}

void CDeltaTreeCtrl::OnFileQuickbrowse()
{
	EditFile(EDIT_ASSOCVIEWER, FALSE);
}

void CDeltaTreeCtrl::OnFileAutoedit() 
{
	EditFile(EDIT_CHOOSEVIEWER, TRUE);
}

void CDeltaTreeCtrl::OnFileNewEditor()
{
	EditFile(EDIT_FINDNEWVIEWER, TRUE);
}

void CDeltaTreeCtrl::OnFileNewBrowser()
{
	EditFile(EDIT_FINDNEWVIEWER, FALSE);
}

void CDeltaTreeCtrl::EditFile(int lparam, BOOL editing)
{
	HTREEITEM item= GetLastSelection();
	ASSERT(item!=NULL);

	m_Need2Edit = FALSE;
	if(item != NULL)
	{
		if(GetClientPath(item, m_ClientPath))
		{
			CP4FileStats *fs=(CP4FileStats *) GetLParam( item );
			if (editing && (IsOpenedForInteg(item) || IsOpenedForBranch(item)))
			{
				m_Msg2Send = fs->IsTextFile() ? WM_FILEEDITTXT : WM_FILEEDITBIN;
				m_Need2Edit = TRUE;
				m_SavelParam = lparam;
				OnFileOpenedit();
			}
			else
			{
				if (fs->GetMyOpenAction() == F_INTEGRATE)
					 m_Msg2Send = fs->IsTextFile() ? WM_FILEBROWSETXT : WM_FILEBROWSEBIN;
				else m_Msg2Send = fs->IsTextFile() ? WM_FILEEDITTXT : WM_FILEEDITBIN;
				::SendMessage(m_depotWnd, m_Msg2Send, (WPARAM) &m_ClientPath, lparam);
			}
		}
	}
}

void CDeltaTreeCtrl::OnUpdateRemoveViewer(CCmdUI* pCmdUI)
{
	BOOL b = FALSE;
	for(int i=0; i < MAX_MRU_VIEWERS; i++)
	{
		if( GET_P4REGPTR()->GetMRUViewerName(i).GetLength() > 0 )
		{
			b = TRUE;
			break;
		}
	}
	pCmdUI->Enable( b );
}

void CDeltaTreeCtrl::OnRemoveViewer()
{
	CRemoveViewer dlg;
	dlg.DoModal();
}


/*
	_________________________________________________________________
*/

BOOL CDeltaTreeCtrl::GetClientPath(HTREEITEM item, CString& clientPath)
{
    int key=0;

	CP4FileStats *stats= (CP4FileStats *) GetLParam( item );
    ASSERT(stats);
    ASSERT_KINDOF(CP4FileStats,stats);

	if(GET_SERVERLEVEL() >= 19)			// 2005.1 or later?
	{
		clientPath = stats->GetFullClientPath( );
        if( !clientPath.IsEmpty() )
            return TRUE;
	}

	BOOL addingFile= (stats->GetMyOpenAction() == F_ADD || stats->GetMyOpenAction() == F_BRANCH ); 

	// The server must be available, must be capable of returning a useable path,
	// and our attempt to get a server lock must succeed, else we bail
	if( SERVER_BUSY() || ( GET_SERVERLEVEL() < 4 && addingFile ) ||
		!GET_SERVER_LOCK(key))
    {
        return FALSE;
    }

	CCmd_Fstat *pCmd= new CCmd_Fstat;
	BOOL success = FALSE;
	
	CString itemStr= GetItemText(item);
	itemStr.TrimRight();
	int pound= itemStr.ReverseFind(_T('#'));
	if(pound == -1)
	{
		ASSERT(0);
		goto GetClientPathEnd;
	}

	itemStr = itemStr.Left( pound );
			
	//		file open for edit. we can't get the client path from the 
	//		depot window anymore since the new p4 dirs/fstat doesnt get
	//		all the files, but only those under the expanded subdirectory
	//		tree. and we cant use p4 where (see below) since p4 where
	//		returns the path of where the file would be if the server
	//		had gotten it. so the only save way of getting the client
	//		path is to call p4 fstat ( run it synchronously )
	//
	pCmd->Init(NULL, RUN_SYNC, HOLD_LOCK, key);
	if ( !PumpMessages( ) )
		goto GetClientPathEnd;

	pCmd->SetIncludeAddedFiles( TRUE );
	if( pCmd->Run( FALSE, itemStr, 0 ) && !pCmd->GetError() )
	{
		CObList *list = pCmd->GetFileList ( );
		ASSERT_KINDOF( CObList, list );
        ASSERT( list->GetCount() <= 1 );
		POSITION pos = list->GetHeadPosition( );
        if( pos != NULL )
        {
			CP4FileStats *stats = ( CP4FileStats * )list->GetNext( pos );
			ASSERT_KINDOF( CP4FileStats, stats );
			clientPath = stats->GetFullClientPath( );
            if( !clientPath.IsEmpty() )
                success = TRUE;
            delete stats;
		}		
	}

GetClientPathEnd:
    RELEASE_SERVER_LOCK(key);
	delete pCmd;
	return success;
}


/*
	_________________________________________________________________

	Double click action is to attempt to edit the file if its our open file
	Note that GetClientPath will fail if the server is busy
	_________________________________________________________________
*/ 

void CDeltaTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CTreeCtrl::OnLButtonDblClk(nFlags, point);
	if(nFlags & (MK_CONTROL | MK_MBUTTON| MK_RBUTTON | MK_SHIFT))
		return;

	// find out what was hit
	TV_HITTESTINFO ht;
	ht.pt=point;
	HTREEITEM currentItem=HitTest( &ht);

	if( currentItem != NULL && (ht.flags & TVHT_ONITEM) && currentItem == m_LastLButtonDown)
		OnLButtonDblClk(currentItem);
}

void CDeltaTreeCtrl::OnLButtonDblClk(HTREEITEM currentItem) 
{
	// make a new selection new if reqd
	if(!IsSelected(currentItem))
	{
		UnselectAll();
		if(currentItem != NULL)
			SetSelectState(currentItem, TRUE);
	}

	if(!SERVER_BUSY() && IsEditableFile())
	{
		switch (GET_P4REGPTR()->GetDoubleClickOption())
		{
		case 0:	// Edit
		case 1:	// open
		case 2:	// open and edit
		case 3:	// view head revision
		case 4:	// sync to head revision
		default:
		{
			CString clientPath;

			if(GetClientPath(currentItem, clientPath))
			{
				CP4FileStats *fs=(CP4FileStats *) GetLParam( currentItem );
				if (fs->GetMyOpenAction() == F_INTEGRATE)
					 m_Msg2Send = fs->IsTextFile() ? WM_FILEBROWSETXT : WM_FILEBROWSEBIN;
				else m_Msg2Send = fs->IsTextFile() ? WM_FILEEDITTXT   : WM_FILEEDITBIN;
				::SendMessage(m_depotWnd, m_Msg2Send, (WPARAM) &clientPath, EDIT_ASSOCVIEWER);
				return;
			}
		}

		case 5:	// diff versus head revision
			PostMessage(WM_COMMAND, ID_FILE_DIFFHEAD, 0);
			break;

		case 6:	// display Properties dialogbox
			PostMessage(WM_COMMAND, ID_FILE_PROPERTIES, 0);
			break;

		case 7:	// display Revision History dialogbox
			PostMessage(WM_COMMAND, ID_FILE_REVISIONHISTORY, 0);
			break;
		}
	}
	else if(!HasChildren(currentItem))
	{
		BOOL underMyRoot=FALSE;
		int level = GetItemLevel(currentItem, &underMyRoot);
		if((GetLParam(currentItem) == NULL) && (level == 2))
			OnJobDescribe();
		else
			MessageBeep(MB_OK);
	}
}


/*
	_________________________________________________________________

	at least one file is selected, user is sure, so put all selected
	file names sans revision number in the string list.
	_________________________________________________________________
*/

BOOL CDeltaTreeCtrl::AssembleStringList( CStringList *list /*=NULL*/, BOOL includeAdds /*=TRUE*/ )
{
	CString itemStr;
	BOOL bFoundFileOpenedForAdd = FALSE;

	if (!list)
		list = &m_StringList;
 
	list->RemoveAll();

	for( int i = GetSelectedCount()-1; i >= 0; i--)
	{
		itemStr = GetItemText( GetSelectedItem( i ) );
		if (itemStr.Right(5) == _T("<add>"))
		{
			bFoundFileOpenedForAdd = TRUE;
			if (!includeAdds)
				continue;
		}
		int sep = itemStr.ReverseFind( _T('#') );
		if (sep != -1)
		{
			itemStr = itemStr.Left( sep );  
			list->AddHead( itemStr );
		}
	}
	return bFoundFileOpenedForAdd;
}

BOOL CDeltaTreeCtrl::GetSelectedFiles( CStringList *list )
{
	list->RemoveAll();

	HTREEITEM cItem;
	CString itemStr;
	CString clientPath;

	for( int i = GetSelectedCount() - 1; i >= 0; i-- )
	{
		cItem = GetSelectedItem( i );
		itemStr = GetItemText( cItem );

		//		if it's not a changelist, add it to the list
		//
		if( !HasChildren( cItem ) && (itemStr.ReverseFind( _T('#') ) != -1) )
		{
			if(GetClientPath(cItem, clientPath))
				list->AddHead( clientPath );
			else
				return FALSE;
		}
	}
	return TRUE;
}


/*
	_________________________________________________________________

	for commands that will run synchronously.
	_________________________________________________________________
*/

BOOL CDeltaTreeCtrl::PumpMessages( )
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


// Support for quick copy of depot path or client path to the clipboard
//

void CDeltaTreeCtrl::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( GetSelectedCount()>=1 && IsMyPendingChangeFile( GetSelectedItem(0)));
}

void CDeltaTreeCtrl::OnUpdateEditCopyclientpath(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( GetSelectedCount()>=1 && IsMyPendingChangeFile( GetSelectedItem(0)));
}

void CDeltaTreeCtrl::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
	BOOL selectable=FALSE;
	if( GetSelectedCount() > 0 )
	{
		if( IsMyPendingChangeItem( GetSelectedItem(0)) )
			selectable= TRUE;
		else if( IsMyPendingChange( GetSelectedItem(0)) )
		{
			UINT state= CTreeCtrl::GetItemState( GetSelectedItem(0), TVIS_EXPANDED );
			if( (state & TVIS_EXPANDED) == TVIS_EXPANDED )
				selectable=TRUE;
		}
	}
	pCmdUI->Enable( selectable );
}

void CDeltaTreeCtrl::OnEditCopyclientpath() 
{
	CString txt;
	for(int i=-1; ++i < GetSelectedCount(); )
	{
		HTREEITEM item= GetSelectedItem(i);
		if( IsMyPendingChangeFile( item ) )
		{
			CString clientPath;
			if(GetClientPath(item, clientPath))
			{
				if (i)
					txt += _T("\r\n");
				txt += clientPath;
			}
		}
	}

	if (txt.IsEmpty())
		MessageBeep(MB_ICONEXCLAMATION);
	else
		CopyTextToClipboard( txt );
}

void CDeltaTreeCtrl::OnEditCopy() 
{
	CString txt;
	for(int i=-1; ++i < GetSelectedCount(); )
	{
		HTREEITEM item= GetSelectedItem(i);
		BOOL underMyRoot=FALSE;
		int level= GetItemLevel(item, &underMyRoot);

		if( level == 2 && IsAFile( item ) )
		{
			// Fetch the filename for ANY file
			CString itemStr = GetItemText( item );
			itemStr = itemStr.Left( itemStr.ReverseFind( _T('#') ) );  
			if (i)
				txt += _T("\r\n");
			txt += itemStr;
		}
	}

	if (txt.IsEmpty())
		MessageBeep(MB_ICONEXCLAMATION);
	else
		CopyTextToClipboard( txt );
}

void CDeltaTreeCtrl::OnEditSelectAll() 
{
	if( GetSelectedCount() > 0 && 
		( IsMyPendingChangeItem( GetSelectedItem(0) ) ||
		  IsMyPendingChange( GetSelectedItem(0)) ) )
	{
		HTREEITEM parent;
		if( IsMyPendingChange( GetSelectedItem(0)) )
			parent= GetSelectedItem(0);
		else
			parent= GetParentItem( GetSelectedItem(0) );

		UnselectAll();
		SetMultiSelect(TRUE);
		HTREEITEM child= GetChildItem( parent );
		while( child != NULL )
		{
			SetSelectState( child, TRUE );
			child= GetNextSiblingItem(child);
		}
		SetMultiSelect(FALSE);
		ShowNbrSelected();
	}
}

void CDeltaTreeCtrl::OnUpdateFileOpenedit(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() 
		&& (AnyMyInteg() || AnyMyBranch())) );	
}


void CDeltaTreeCtrl::OnFileOpenedit() 
{
	if(GetSelectedCount()==0)
	{
		ASSERT(0);
		return;
	}

	CString itemStr;
 
	m_StringList.RemoveAll();

	for( int i = GetSelectedCount()-1; i >= 0; i--)
	{
		if(IsOpenedForInteg(GetSelectedItem(i)) || IsOpenedForBranch(GetSelectedItem(i)))
		{
			itemStr = GetItemText( GetSelectedItem( i ) );
			itemStr = itemStr.Left( itemStr.ReverseFind( _T('#') ) );  
			m_StringList.AddHead( itemStr );
		}
	}
	if (m_StringList.IsEmpty())
		return;

	CCmd_ListOpStat *pCmd= new CCmd_ListOpStat;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
	if( pCmd->Run( &m_StringList, P4EDIT, 0 ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_OPENING_FILES_FOR_EDIT) );
	else
		delete pCmd;
}

void CDeltaTreeCtrl::OnUpdatePositionDepot(CCmdUI* pCmdUI) 
{
	BOOL underMyRoot=FALSE;
	int level = GetItemLevel(GetSelectedItem(0), &underMyRoot);

	int n = GetSelectedCount();
	BOOL b = pCmdUI->m_pParentMenu == MainFrame()->GetMenu();
	if (level == 1)
	{
		if (b)
			pCmdUI->SetText(LoadStringResource(IDS_FINDCHGFILESINDEPOT));
		pCmdUI->Enable( n == 1 && AnyFilesInChange(GetSelectedItem(0)));
	}
	else
	{
		if (b)
			pCmdUI->SetText(LoadStringResource(n < 2 ? IDS_FINDSELFILEINDEPOT 
													 : IDS_FINDSELFILESINDEPOT));
		pCmdUI->Enable( n >= 1 && (level == 2) && IsAFile(GetSelectedItem(0)) );
	}
}

//	User clicked on "Find in Depot" menu item
//	Expand the Depot Treeview to the location of that file
//
void CDeltaTreeCtrl::OnPositionDepot() 
{
	// see if it's a chglist or file(s) that's selected
	HTREEITEM initialItem = GetSelectedItem(0);
	BOOL root;
	int level = GetItemLevel(initialItem, &root);
	if (level == 1)
	{
		// a chglist is selected, so select all its files
		SelectAllFilesInChange(initialItem, 0);
		if (GetSelectedCount() < 1)
		{
			UnselectAll();
			SetSelectState(initialItem, TRUE);
			return;
		}
	}

	int n;
	int count = 0;
	if( (n = GetSelectedCount()) >=1 )
	{
		MainFrame()->SetAdd2ExpandItemList(TRUE);
		for (int i=-1; ++i < n; )
		{
			HTREEITEM item= GetSelectedItem(i);
			if( GetItemLevel(item, &root) == 2 && IsAFile( item ) )
			{
				// Fetch the filename for ANY file
				CString itemStr = GetItemText( item );
				itemStr = itemStr.Left( itemStr.ReverseFind( _T('#') ) );  // trim off rev# info
				MainFrame()->ExpandDepotString( itemStr, TRUE );
				while (MainFrame()->IsExpandDepotContinuing() || SERVER_BUSY())
				{
					if ( !MainFrame()->PumpMessages( ) )
						break;
					Sleep(250);
				}
				count++;
			}
		}
	}
	if (level == 1)
	{
		UnselectAll();
		SetSelectState(initialItem, TRUE);
	}
	if (count)
	{
		MainFrame()->SetAdd2ExpandItemList(FALSE);
		MainFrame()->SelectExpandItemList();
		if (MainFrame()->GetExpandItemListCount() < count)
		{
			CString txt;
			n = count - MainFrame()->GetExpandItemListCount();
			txt.FormatMessage(IDS_NOTALLFILESSELECTED_d, n, n==1 ? _T("") : _T("s"));
			TheApp()->StatusAdd( txt, SV_WARNING );
		}
	}
	else
		MessageBeep(MB_ICONEXCLAMATION);	// unexpected problem - should never happen
}

// this returns -1 if the change is not found or is not MY change
// if the chg is found and it is mine, its number is returned
long CDeltaTreeCtrl::PositionChgs(const CString &path, 
								  BOOL lookInMine, 
								  BOOL lookInOthers/*=TRUE*/, 
								  BOOL addToSelectionSet/*=FALSE*/)
{
	long chg;

	// clear the previous position request string
	m_PositionTo = _T("");

	// first look thru my client's changes, if requested
	if (lookInMine && ((chg = PositionToFileInChg(path, m_MyRoot,
														m_MyRoot, FALSE, addToSelectionSet)) !=-1))
		return chg;

	if (!lookInOthers)
		return -1;

	chg = -1;
	if (m_OthersRoot)
	{
		// If we get here, we didn't find it among my clients,
		// so look thru the other clients.
		// First expand the Other Clients node - which might fail,
		// and thereby trigger a p4 opened -a
		Expand(m_OthersRoot, TVE_EXPAND);
		if ((chg = PositionToFileInChg(path, m_OthersRoot, 
											 m_OthersRoot, FALSE, addToSelectionSet)) == -1)
		{
			// We didn't find it, we might have triggered a p4 opened -a, 
			// so save off the name of the thing we are seeking
			m_PositionTo = path;
		}
	}
	return chg;
}

void CDeltaTreeCtrl::OnUpdatePositionOtherChgs(CCmdUI* pCmdUI) 
{
	m_PositionTo.Empty();
	if (!m_InContextMenu)
		pCmdUI->SetText( LoadStringResource(IDS_POSITIONCHGS_CHGPANE) );

	BOOL underMyRoot=FALSE;
	BOOL b = GetSelectedCount()==1 && 
		GetItemLevel(GetSelectedItem(0), &underMyRoot) == 2 &&
		IsAFile(GetSelectedItem(0));
	if (b && GET_SERVERLEVEL() >= 19)			// 2005.1 or later?
	{
		CP4FileStats *stats= (CP4FileStats *) GetLParam(GetSelectedItem(0));
		if (stats->GetOtherOpenAction() == 0)
			b = FALSE;
	}
	pCmdUI->Enable(b);
}

void CDeltaTreeCtrl::OnPositionOtherChgs()
{
	m_PositionTo.Empty();

	if ( !IsAFile(GetSelectedItem(0)) || !m_OthersRoot )
		return;

	HTREEITEM file = GetSelectedItem(0);
	LPARAM lParam=GetLParam(file);
	if(lParam > 0)
    {
		HTREEITEM start = GetParentItem(file);
		HTREEITEM root  = GetParentItem(start);
		start = (root == m_MyRoot) ? m_OthersRoot 
			  : GetNextSiblingItem(start);
		if (!start)
			return;

		CP4FileStats *stats= (CP4FileStats *) lParam;
        ASSERT_KINDOF(CP4FileStats, stats);
		CString path = stats->GetFullDepotPath();
		// First expand the Other Clients node - which might fail,
		// and thereby trigger a p4 opened -a
		Expand(m_OthersRoot, TVE_EXPAND);
		// then try and find the file
		if (PositionToFileInChg(path, start, m_OthersRoot) == -1)
		{
			// We didn't find it, we might have triggered a p4 opened -a, 
			// so save off the name of the thing we are seeking
			m_PositionTo = path;
		}
	}
}

long CDeltaTreeCtrl::PositionToFileInChg(const CString &path, 
										 HTREEITEM start, HTREEITEM root, 
										 BOOL afterExpand/*=FALSE*/,
										 BOOL addToSelectionSet/*=FALSE*/)
{
	HTREEITEM change = !start || (start == root) 
		             ? GetChildItem(root) : start;
	HTREEITEM file;
	LPARAM lParam;
	BOOL bFile = FALSE;

	while(change != NULL)
	{
		file= GetChildItem(change);	
		while(file != NULL)
		{
			lParam=GetLParam(file);
			if(lParam > 0)
            {
				CP4FileStats *stats= (CP4FileStats *) lParam;
                ASSERT_KINDOF(CP4FileStats, stats);
				if(stats->GetFullDepotPath() == path)
				{
					Expand(root, TVE_EXPAND);
					Expand(change, TVE_EXPAND);
					if (!addToSelectionSet)
						UnselectAll();
					SetSelectState( file, TRUE );
					// set focus to pending chglist pane
					MainFrame()->SetActiveView(DYNAMIC_DOWNCAST(CView,GetParent()), TRUE);
					long chg = stats->GetOpenChangeNum();
					CString txt;
					txt.FormatMessage(chg ? IDS_FILE_FOUND_IN_CHG_d : IDS_FILE_FOUND_IN_DEFCHG, chg);
					MainFrame()->SetMessageText(txt);
					return chg;
				}
				bFile = TRUE;
            }

			file=GetNextSiblingItem(file);
		}
		change=GetNextSiblingItem(change);
	}
	if (bFile)
		MainFrame()->SetMessageText(LoadStringResource(afterExpand ? IDS_FILE_NOT_FOUND_IN_OTHERS 
																   : IDS_CHGFILE_NOT_FOUND));
	return -1;
}


void CDeltaTreeCtrl::OnUpdatePositionToPattern(CCmdUI* pCmdUI) 
{
	CString txt = LoadStringResource(IDS_POSITIONTOPATTERN);
	pCmdUI->SetText ( txt );
	pCmdUI->Enable(!SERVER_BUSY());
}

void CDeltaTreeCtrl::OnPositionToPattern() 
{
	::PostMessage(m_depotWnd, WM_COMMAND, ID_POSITIONTOPATTERN, 0);
}

BOOL CDeltaTreeCtrl::AnyInDefault()
{
	return GetChildItem(m_MyDefault) ? TRUE : FALSE;
}

BOOL CDeltaTreeCtrl::AnyNbredChg()
{
	return GetNextSiblingItem(m_MyDefault) ? TRUE : FALSE;
}

void CDeltaTreeCtrl::OnUpdateUserSwitchtouser(CCmdUI* pCmdUI) 
{
	BOOL underMyRoot;

	pCmdUI->Enable( !SERVER_BUSY() && GetSelectedCount() == 1
		&& GetItemLevel(GetSelectedItem(0), &underMyRoot) == 1
		&& !IsMyPendingChange( GetSelectedItem(0) )
		&& GetUserFromChange() != GET_P4REGPTR()->GetP4User());	
}

void CDeltaTreeCtrl::OnUserSwitchtouser() 
{
	BOOL underMyRoot;

	if ( !SERVER_BUSY() && GetSelectedCount() == 1
		&& GetItemLevel(GetSelectedItem(0), &underMyRoot) == 1)
	{
		CString user = GetUserFromChange();
		if (!user.IsEmpty())
		{
			GET_P4REGPTR()->SetP4User(user, TRUE, FALSE, FALSE);
			MainFrame()->OnPerforceOptions(FALSE, FALSE);
		}
	}
}

void CDeltaTreeCtrl::OnUpdateClientspecSwitch(CCmdUI* pCmdUI) 
{
	BOOL underMyRoot;

	pCmdUI->Enable( !SERVER_BUSY() && GetSelectedCount() == 1
		&& GetItemLevel(GetSelectedItem(0), &underMyRoot) == 1
		&& !underMyRoot);
}

void CDeltaTreeCtrl::OnClientspecSwitch() 
{
	BOOL underMyRoot;

	if ( !SERVER_BUSY() && GetSelectedCount() == 1
		&& GetItemLevel(GetSelectedItem(0), &underMyRoot) == 1
		&& !underMyRoot)
	{
		CString client = GetClientFromChange();
		if (!client.IsEmpty())
			MainFrame()->ClientSpecSwitch(client);
	}
}

CString CDeltaTreeCtrl::GetClientFromChange() 
{
	BOOL underMyRoot;
	TCHAR buf[2050];
	int  i;

	if ( GetSelectedCount() == 1
		&& GetItemLevel(GetSelectedItem(0), &underMyRoot) == 1)
	{
		TV_ITEM item;
		item.hItem=GetSelectedItem(0);
		item.mask=TVIF_TEXT;
		item.pszText = buf;
		item.cchTextMax = sizeof(buf)/sizeof(TCHAR)-1;
		if (GetItem(&item ))
		{
			CString txt = buf;
			if ((i = txt.Find(_T('@'))) != -1)
			{
				txt = txt.Right(txt.GetLength() - i - 1);
				if ((i = txt.Find(_T(' '))) != -1)
					txt = txt.Left(i);
				return txt;
			}
		}
	}
	return _T("");
}

CString CDeltaTreeCtrl::GetUserFromChange() 
{
	BOOL underMyRoot;
	TCHAR buf[2050];
	int  i;

	if ( GetSelectedCount() == 1
		&& GetItemLevel(GetSelectedItem(0), &underMyRoot) == 1)
	{
		TV_ITEM item;
		item.hItem=GetSelectedItem(0);
		item.mask=TVIF_TEXT;
		item.pszText = buf;
		item.cchTextMax = sizeof(buf)/sizeof(TCHAR)-1;
		if (GetItem(&item ))
		{
			CString txt = buf;
			if ((i = txt.Find(_T('@'))) != -1)
			{
				txt = txt.Left(i);
				if ((i = txt.ReverseFind(_T(' '))) != -1)
					txt = txt.Right(txt.GetLength() - i - 1);
				return txt;
			}
		}
	}
	return _T("");
}

void CDeltaTreeCtrl::OnUpdateFileInformation(CCmdUI* pCmdUI) 
{
	BOOL root;
	BOOL enable= (!SERVER_BUSY() && GetSelectedCount() == 1 &&
					GetItemLevel(GetSelectedItem(0), &root)== 2 && 
					IsAFile( GetSelectedItem(0)) );
	if( enable )
	{
		CP4FileStats *fs= (CP4FileStats *) GetLParam( GetSelectedItem(0) );
		if( fs->GetHaveRev() <= 1 &&
			( fs->GetMyOpenAction() == F_ADD || fs->GetMyOpenAction() == F_BRANCH ) )
			enable= FALSE;
	}
	pCmdUI->Enable( enable );
}

void CDeltaTreeCtrl::OnFileInformation() 
{
	HTREEITEM item=GetLastSelection();
	CString itemStr= GetItemText(item);
	itemStr.TrimRight();
	int pound= itemStr.ReverseFind(_T('#'));
	if(pound == -1)
	{
		ASSERT(0);
		return;
	}

	itemStr = itemStr.Left( pound );

	m_StringList.RemoveAll();
	m_StringList.AddHead(itemStr);
	
	CCmd_Opened *pCmd= new CCmd_Opened;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, 0);
	pCmd->SetAlternateReplyMsg( WM_P4FILEINFORMATION );

	if( pCmd->Run( TRUE, FALSE, -1, &m_StringList ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_FILE_INFORMATION) );
	else
		delete pCmd;
}

LRESULT CDeltaTreeCtrl::OnP4FileInformation( WPARAM wParam, LPARAM lParam )
{
	CCmd_Opened *pCmd= (CCmd_Opened *) wParam;
	
	m_StringList.RemoveAll();
	if(!pCmd->GetError())
	{
		CString thisuser=GET_P4REGPTR()->GetMyID();
				
		// Initialize the file info dialog
		CFileInfoDlg *dlg = new CFileInfoDlg(this);

		CString itemStr = *pCmd->GetDepotPath();

		if (itemStr.IsEmpty())
		{
			HTREEITEM item=GetLastSelection();
			itemStr= GetItemText(item);
			itemStr.TrimRight();
			int pound= itemStr.ReverseFind(_T('#'));
			if(pound == -1)
			{
				ASSERT(0);
				return -1;
			}

			itemStr = itemStr.Left( pound );
		}
		dlg->m_DepotPath = itemStr;

		int key= pCmd->GetServerKey();
		CCmd_Fstat *pCmd2= new CCmd_Fstat;
		
		pCmd2->Init(NULL, RUN_SYNC, HOLD_LOCK, key);
		if ( !PumpMessages( ) )
			goto CantGetFStat;

		pCmd2->SetIncludeAddedFiles( TRUE );
		if( pCmd2->Run( FALSE, itemStr, 0 ) && !pCmd2->GetError() )
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
		RELEASE_SERVER_LOCK(key);
		delete pCmd2;

		CObList *list= pCmd->GetList();
		ASSERT_KINDOF(CObList, list);

        POSITION pos= list->GetHeadPosition();
		while(pos != NULL)
		{
			CP4FileStats *fs= (CP4FileStats *) list->GetNext(pos);
			
			CString str, strUser, strAction;

			strUser= fs->GetOtherUsers();
			if( fs->IsMyOpen() && strUser.IsEmpty() )
			{
				strUser= thisuser;
				strAction= fs->GetActionStr(fs->GetMyOpenAction());
			}
			else
				strAction= fs->GetActionStr(fs->GetOtherOpenAction());

			if( fs->GetOpenChangeNum() == 0 )
                str.FormatMessage(IDS_CHANGE_DEFAULT_USER_s, strUser);
            else
                str.FormatMessage(IDS_CHANGE_n_USER_s, fs->GetOpenChangeNum(), strUser);
            str +=  _T(" (") + strAction + _T(")");

			if( fs->IsOtherLock() )
				str += LoadStringResource(IDS_STAR_LOCKED);
			
			dlg->m_StrList.AddHead( str );
			
			delete fs;
		}
		// Display the info
		if (!dlg->Create(IDD_FILE_INFORMATION, this))	// display the description dialog box
		{
			dlg->DestroyWindow();	// some error! clean up
			delete dlg;
		}
	}

	MainFrame()->ClearStatus();
	delete pCmd;
	
	return 0;
}

LRESULT CDeltaTreeCtrl::OnP4EndFileInformation( WPARAM wParam, LPARAM lParam )
{
	CFileInfoDlg *dlg = (CFileInfoDlg *)lParam;
	dlg->DestroyWindow();
	return TRUE;
}

void CDeltaTreeCtrl::OnSetFlyoverMessage(HTREEITEM currentItem)
{
	if( !GET_P4REGPTR()->ShowClientPath() || GET_SERVERLEVEL() < 19)	// earlier than 2005.1
		return;

	ASSERT( currentItem != NULL );
    LPARAM lParam= GetLParam(currentItem);
   
    // Bail out if its a changelist or a job
    if( lParam <= 0 )
    {
        ShowNbrSelected();
    }
    else
    {
		if( IsMyPendingChangeFile( currentItem ) )
		{
			CP4FileStats *fs= (CP4FileStats *) lParam;
			CString msg = fs->GetFullClientPath();
			if (msg.IsEmpty())
			{
				GetClientPath(currentItem, msg);
				fs->SetClientPath(msg);
			}
			if (!msg.IsEmpty())
			{
				CString itemStr = GetItemText(currentItem);
				CString type;
				int		baseType;
				int		storeType;
				BOOL	typeK = FALSE;
				BOOL	typeW = FALSE;
				BOOL	typeX = FALSE;
				BOOL	typeO = FALSE;
				BOOL	typeM = FALSE;
				BOOL	typeL = FALSE;
				BOOL	typeS = FALSE;
				DWORD_PTR		nbrrevs = 1;
				BOOL	unknown = FALSE;

				// convert the GetItemText() string to 5 flags
				TheApp()->GetFileType(itemStr, baseType, storeType,
											typeK, typeW, typeX, typeO, 
											typeM, typeL, typeS, nbrrevs, unknown);
				// determine the base file type
				switch(baseType)
				{
				case 0:
					type = _T("  <text");
					break;
				case 1:
					type = _T("  <binary");
					break;
				case 2:    
					type = _T("  <symlink");
					break;
				case 3:
					type = _T("  <resource");
					break;
				case 4:
					type = _T("  <apple");
					break;
				case 5:
					type = _T("  <unicode");
					break;
				case 6:
					type = _T("  <utf16");
					break;
				default:
					type = _T("  <unknown");
					break;
				}
				// determine storage type - if it's the default for the base type, do nothing
				switch(storeType)
				{
				case 1:					// +C
					if (baseType != 1)
						type += LoadStringResource(IDS_comma_FULL_COMPRESSED_VERSION_STORED);
					break;
				case 2:					// +D
					if (baseType != 0)
						type += LoadStringResource(IDS_comma_RCS_DELTAS_STORED);
					break;
				case 3:					// +F
					type += LoadStringResource(IDS_comma_FULL_FILE_STORED_PER_REV);
					break;
				default:
					break;
				}
				// Warning: if more of these modifier types are added, we may need to
				// shorten these strings so that all will fit in the status bar window.
				// Already there is a possible overflow if the local path name is long!
				if (typeO)
					type += LoadStringResource(IDS_comma_LIMITED_RCS_KEYWORD_EXPANSION);
				else if (typeK)
					type += LoadStringResource(IDS_comma_RCS_KEYWORD_EXPANSION);
				if (typeW)
					type += LoadStringResource(IDS_comma_ALWAYS_WRITABLE);
				if (typeX)
					type += LoadStringResource(IDS_comma_EXECUTABLE);
				if (typeL)
					type += LoadStringResource(IDS_comma_LOCKED);
				if (typeS)
				{
					if (nbrrevs < 2)
						type += LoadStringResource(IDS_comma_ONLY_HEAD_REV_STORED);
					else
					{
						CString str;
						str.FormatMessage(IDS_comma_ONLY_n_REVS_STORED, nbrrevs);
						type += str;
					}
				}
				if (unknown)
					type += LoadStringResource(IDS_PLUS_UNKNOWN);
				type += _T(">");
				if (type != LoadStringResource(IDS_ONLY_UNKNOWN))
					msg += type;

				if (fs->GetMyOpenAction() > 0)
					msg += _T("<") + fs->GetActionStr( fs->GetMyOpenAction() ) + _T(">");

				if( fs->IsUnresolved() )
					msg += LoadStringResource(IDS_UNRESOLVED);
				if(fs->IsMyLock())
					msg += LoadStringResource(IDS_LOCKED);

				if((fs->GetOtherOpens() > 0) || fs->IsOtherLock())
				{
					msg += LoadStringResource(IDS_OTHERUSER);
					if(fs->GetOtherOpens() > 0)
                    {
                        CString otherAction;
                        otherAction.FormatMessage(IDS_OPENFOR_s, 
							fs->GetActionStr(fs->GetOtherOpenAction()));
						msg += otherAction + _T(" by ") + fs->GetOtherUsers();
                    }
					if(fs->IsOtherLock())
						msg += _T(" ") + LoadStringResource(IDS_LOCKED);
				}

				// write the local path and the file type to the status bar
				MainFrame()->SetMessageText(msg);
			}
			else
				MainFrame()->SetMessageText(LoadStringResource(IDS_FILE_NOT_IN_CLIENT_VIEW));
		}
		else
	        MainFrame()->SetMessageText(LoadStringResource(IDS_FOR_HELP_PRESS_F1));
    }
}

int CDeltaTreeCtrl::CreateNewChangeList(int key, CString *description/*=NULL*/, BOOL autoOK/*=FALSE*/)
{
	BOOL bReleaseLock;
	if (!key)
	{
		if(SERVER_BUSY() || !GET_SERVER_LOCK(key))
		{
			ASSERT(0);
			return -1;
		}
		bReleaseLock = TRUE;
	}
	else
		bReleaseLock = FALSE;
	m_NewChgNbr = -1;
	m_NewDesc.Empty();
	CCmd_Describe *pCmd = new CCmd_Describe;
	pCmd->Init( NULL, RUN_SYNC, HOLD_LOCK, key );
	BOOL cmdStarted= pCmd->Run( P4CHANGE_SPEC, NULL );
	if(cmdStarted && !pCmd->GetError())
	{
		// if we are passed a description, insert it
		if (description && *description)
		{
			int i;
			CString cmdDesc = pCmd->GetDescription();
			if ((i = cmdDesc.Find(CCmd_EditSpec::g_blankDesc)) != -1)
			{
				cmdDesc = cmdDesc.Left(i) + *description + cmdDesc.Mid(i + lstrlen(CCmd_EditSpec::g_blankDesc));
				pCmd->SetDescription(cmdDesc);
				m_NewDesc = *description;
			}
		}
		// we create a dummy CCmd_EditSpec; it is never run,
		// it just hold the lock's key and receives the new chglist number
		CCmd_EditSpec callingCmd;
		callingCmd.Init( NULL, RUN_SYNC, HOLD_LOCK, key );
		callingCmd.SetSpecIn(pCmd->GetDescription());
		callingCmd.PreprocessChgSpec();

		CP4SpecSheet SpecSheet;
		SpecSheet.m_P4SpecDlg.SetCallingCommand(&callingCmd);
		SpecSheet.m_P4SpecDlg.SetSpec(callingCmd.GetSpecIn(), pCmd->GetSpecStr(), 
														P4CHANGE_SPEC, FALSE);
		SpecSheet.m_P4SpecDlg.SetChangeParms(FALSE, FALSE, FALSE, FALSE, FALSE, autoOK);
		if (SpecSheet.DoModal() == IDOK)
		{
			m_NewChgNbr = callingCmd.GetNewChangeNum();
			m_NewDesc   = callingCmd.GetChangeDesc();
		}
		else
		{
			m_NewChgNbr = -1;
			m_NewDesc.Empty();
		}
	}
	delete pCmd;
	if (bReleaseLock)
		RELEASE_SERVER_LOCK(key);
	return m_NewChgNbr;
}

void CDeltaTreeCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	MainFrame()->WaitAWhileToPoll( );
	CMultiSelTreeCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDeltaTreeCtrl::OnUpdateViewUpdate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && !MainFrame()->IsModlessUp());
}

void CDeltaTreeCtrl::OnViewUpdate() 
{
	// somewhat unexpected maybe, but refreshing the pending changes view
	// actually just sends a request to the depot view for a full refresh
	::PostMessage(m_depotWnd, WM_COMMAND, ID_VIEW_UPDATE_LEFT, 0);
}

void CDeltaTreeCtrl::OnPerforceOptions()
{
	MainFrame()->OnPerforceOptions(TRUE, FALSE, IDS_PAGE_CHANGELIST);
}

void CDeltaTreeCtrl::OnTheirFindInDepot()
{
	ASSERT(GetSelectedCount() == 1);
	AssembleStringList( );
	CCmd_AutoResolve *pCmd= new CCmd_AutoResolve;
	pCmd->Init( m_hWnd, RUN_ASYNC );
	pCmd->SetAlternateReplyMsg( WM_THEIRFINDINDEPOT );
	if( pCmd->Run( &m_StringList, 0, TRUE, TRUE, FALSE, 0 ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_FINDING_THEIR_FILE) );
	else
		delete pCmd;
}

LRESULT CDeltaTreeCtrl::OnP4TheirFindInDepot(WPARAM wParam, LPARAM lParam)
{
	CCmd_AutoResolve *pCmd= (CCmd_AutoResolve *) wParam;

	if( !pCmd->GetError() && !MainFrame()->IsQuitting() )
	{
		CStringList *list= pCmd->GetList();
		if (list->GetCount())
		{
			int i;
			CString theirStr = list->GetHead();
			if ((i = theirStr.Find( _T(" - vs "))) != -1)
			{
				theirStr = theirStr.Mid(i + sizeof(_T(" - vs "))/sizeof(TCHAR) -1 );
				theirStr.TrimLeft();
				theirStr = theirStr.Left( theirStr.ReverseFind( _T('#') ) );  // trim off rev# info
				MainFrame()->ExpandDepotString( theirStr, TRUE );
			}
		}
	}
	MainFrame()->ClearStatus();
	delete pCmd;
	return 0;
}

void CDeltaTreeCtrl::OnTheirHistory()
{
	ASSERT(GetSelectedCount() == 1);
	AssembleStringList( );
	CCmd_AutoResolve *pCmd= new CCmd_AutoResolve;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
	pCmd->SetAlternateReplyMsg( WM_THEIRHISTORY );
	if( pCmd->Run( &m_StringList, 0, TRUE, TRUE, FALSE, 0 ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_FINDING_THEIR_FILE) );
	else
		delete pCmd;
}

LRESULT CDeltaTreeCtrl::OnP4TheirHistory(WPARAM wParam, LPARAM lParam)
{
	CCmd_AutoResolve *pCmd= (CCmd_AutoResolve *) wParam;

	if( !pCmd->GetError() && !MainFrame()->IsQuitting() )
	{
		CStringList *list= pCmd->GetList();
		if (list->GetCount())
		{
			int i;
			int rev = -1;
			CString theirStr = list->GetHead();
			if ((i = theirStr.Find( _T(" - vs "))) != -1)
			{
				theirStr = theirStr.Mid(i + sizeof(_T(" - vs "))/sizeof(TCHAR) -1 );
				theirStr.TrimLeft();
				i = theirStr.Find( _T('#') );
				if (i != -1)
				{
					rev = _ttoi(theirStr.Right(theirStr.GetLength() - i - 1));
					if (!rev)
						rev = -1;
					theirStr = theirStr.Left(i);  // trim off rev# info
				}
				CCmd_History *pCmd2= new CCmd_History;
				pCmd2->Init( m_depotWnd, RUN_ASYNC, LOSE_LOCK, pCmd->GetServerKey());
				pCmd2->SetInitialRev(rev, theirStr);
				pCmd2->SetCallingWnd(m_hWnd);
				if( pCmd2->Run(theirStr) )
				{
					MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_HISTORY) );
				}
				else
				{
					delete pCmd2;
					pCmd->ReleaseServerLock();
				}
			}
			else
				pCmd->ReleaseServerLock();
		}
		else
			pCmd->ReleaseServerLock();
	}
	else
		pCmd->ReleaseServerLock();
	MainFrame()->ClearStatus();
	delete pCmd;
	return 0;
}

void CDeltaTreeCtrl::OnTheirProperties()
{
	ASSERT(GetSelectedCount() == 1);
	AssembleStringList( );
	CCmd_AutoResolve *pCmd= new CCmd_AutoResolve;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
	pCmd->SetAlternateReplyMsg( WM_THEIRPROPERTIES );
	if( pCmd->Run( &m_StringList, 0, TRUE, TRUE, FALSE, 0 ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_FINDING_THEIR_FILE) );
	else
		delete pCmd;
}

LRESULT CDeltaTreeCtrl::OnP4TheirProperties(WPARAM wParam, LPARAM lParam)
{
	CCmd_AutoResolve *pCmd= (CCmd_AutoResolve *) wParam;

	if( !pCmd->GetError() && !MainFrame()->IsQuitting() )
	{
		CStringList *list= pCmd->GetList();
		if (list->GetCount())
		{
			int i;
			CString theirStr = list->GetHead();
			if ((i = theirStr.Find( _T(" - vs "))) != -1)
			{
				theirStr = theirStr.Mid(i + sizeof(_T(" - vs "))/sizeof(TCHAR) -1 );
				theirStr.TrimLeft();
				if ((i = theirStr.Find(_T('#'))) != -1)
					theirStr = theirStr.Left(i);  // trim off rev# info
				m_StringList.RemoveAll();
				m_StringList.AddHead(theirStr);
				
				CCmd_Opened *pCmd2= new CCmd_Opened;
				pCmd2->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, pCmd->GetServerKey());
				pCmd2->SetAlternateReplyMsg( WM_P4FILEINFORMATION );
				pCmd2->SetDepotPath(theirStr);
				if( pCmd2->Run( TRUE, FALSE, -1, &m_StringList ) )
					MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_FILE_INFORMATION) );
				else
				{
					delete pCmd2;
					pCmd->ReleaseServerLock();
				}
			}
			else
				pCmd->ReleaseServerLock();
		}
		else
			pCmd->ReleaseServerLock();
	}
	else
		pCmd->ReleaseServerLock();
	MainFrame()->ClearStatus();
	delete pCmd;
	return 0;
}

void CDeltaTreeCtrl::CantDoItRightNow(int type)
{
	CString msg;
	msg.FormatMessage(IDS_CANTEDITCHG_INPROGRESS, LoadStringResource(type));
	AddToStatus( msg, SV_WARNING );
}

void CDeltaTreeCtrl::OnUpdateSelectChanged(CCmdUI* pCmdUI) 
{
	BOOL selectable=FALSE;
	if( GET_SERVERLEVEL() >= 19 )
	{
		int cnt;
		BOOL underMyRoot;
		HTREEITEM curitem = GetSelectedItem(0);
		if( (cnt = GetSelectedCount()) == 1 && IsMyPendingChange( curitem ))
		{
			selectable=TRUE;
		}
		else if (cnt > 0 && GetItemLevel( curitem, &underMyRoot ) == 2 
			  && underMyRoot && IsAFile( curitem ) )
		{
			selectable=TRUE;
		}
	}
	pCmdUI->Enable( selectable );
}

void CDeltaTreeCtrl::OnSelectChanged()
{
	SelectChgUnchg(TRUE);
}

void CDeltaTreeCtrl::OnSelectUnchanged()
{
	SelectChgUnchg(FALSE);
}

// returns TRUE if any file(s) selected
BOOL CDeltaTreeCtrl::SelectChgUnchg(BOOL bChged, int *totfiles/*=NULL*/)
{
	BOOL b = FALSE;
	if (totfiles)
	   *totfiles = 0;
	BOOL underMyRoot;
	HTREEITEM curitem = GetSelectedItem(0);
	if( GetItemLevel(curitem, &underMyRoot) == 2 && underMyRoot && IsAFile(curitem) )
	{
		UnselectAll();
		SetSelectState( GetParentItem(curitem), TRUE );
	}
	if( GetSelectedCount() == 1 && IsMyPendingChange( GetSelectedItem(0) ) )
	{
		SET_BUSYCURSOR();
		MainFrame()->UpdateStatus(LoadStringResource(IDS_DIFFFILES));

		HTREEITEM parent = GetSelectedItem(0);

		UnselectAll();
		SetMultiSelect(TRUE);

		UINT state= CTreeCtrl::GetItemState( curitem, TVIS_EXPANDED );
		if( (state & TVIS_EXPANDED) != TVIS_EXPANDED )
			CTreeCtrl::Expand( curitem, TVE_EXPAND );

		Error e;
		CP4Command *pcmd = new CP4Command;
		CGuiClient *client = pcmd->GetClient();
		client->SetTrans();
		client->Init(&e);
		if( e.Test() )
		{
			delete pcmd;
			return FALSE;
		}

		HTREEITEM child= GetChildItem( parent );
		while( child != NULL )
		{
			LPARAM lParam=GetLParam(child);
			if(lParam > 0)
            {
				BOOL chg = FALSE;
				CP4FileStats *stats = (CP4FileStats *) lParam;
				if (stats->GetMyOpenAction() == F_ADD || stats->GetMyOpenAction() == F_BRANCH)
					chg = TRUE;
				else if (!TheApp()->digestIsSame(stats, FALSE, client)
					  || stats->GetType() != stats->GetHeadType())
					chg = TRUE;
				if (chg	== bChged)
				{
					SetSelectState( child, TRUE );
					b = TRUE;
				}
				if (totfiles)
					++*totfiles;
			}
			child= GetNextSiblingItem(child);
		}
		delete pcmd;
		SetMultiSelect(FALSE);
		if (!b)
		{
			if (!totfiles)
				MessageBeep(0);
			SetSelectState( parent, TRUE );
			MainFrame()->SetMessageText(LoadStringResource(bChged ? IDS_NOFILESCHGED : IDS_ALLFILESCHGED));
		}
		else
			ShowNbrSelected();
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	MainFrame()->ClearStatus();
	return b;
}

LRESULT CDeltaTreeCtrl::CallOnUpdateFilterClearview(WPARAM wParam, LPARAM lParam)
{
	OnUpdateFilterClearview((CCmdUI *)lParam);
	return 0;
}

void CDeltaTreeCtrl::OnUpdateFilterClearview(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText(LoadStringResource(IDS_FILTER_PCO_CLEARVIEW));
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() && 
		GET_P4REGPTR()->GetEnablePendingChgsOtherClients() &&
		(GET_P4REGPTR()->FilterPendChgsByMyClient()
//		|| m_FilteredByClient || m_FilteredByUser
		)));
}

void CDeltaTreeCtrl::OnUpdateFilterSetview(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText(LoadStringResource(IDS_FILTER_PCO_SETVIEW));
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY()) && 
		GET_P4REGPTR()->GetEnablePendingChgsOtherClients());
}

void CDeltaTreeCtrl::OnFilterSetview()
{
	COldChgFilterDlg dlg;

	// initialize filter vars
//	dlg.m_useClient = m_FilteredByClient;
//	dlg.m_client = m_ClientFilter;
//	dlg.m_useUser = m_FilteredByUser;
//	dlg.m_user = m_UserFilter;
	dlg.m_includeIntegrations = FALSE;
	dlg.m_bPending = TRUE;

	// get selected files from depot view and convert to string
	CStringList selected;
    ::SendMessage(m_depotWnd, WM_GETSELLIST, (WPARAM) &selected, 0);
	CString selectedTxt;
	POSITION pos = selected.GetHeadPosition();
	int i;
	for(i=0; pos != NULL; i++)
	{
		CString sel = selected.GetNext(pos);
		selectedTxt += sel + _T(" ");
		dlg.m_selected.AddTail(sel);
	}
	selectedTxt.TrimRight();
	dlg.m_selectedFiles = selectedTxt;

	// make a radio button selection based on various strings
	i = GET_P4REGPTR()->FilterPendChgsByMyClient();
	if (i == 0)
		dlg.m_filterFiles = 0;
	else if (i == 1)
		dlg.m_filterFiles = 1;
	else
	{
		CString currentTxt = GET_P4REGPTR()->FilterPendChgsByPath();
		if(currentTxt.IsEmpty() || (GET_SERVERLEVEL() < 21))
			dlg.m_filterFiles = 0;
		else
		{
			if(currentTxt == selectedTxt)
				dlg.m_filterFiles = 3;
			else
				dlg.m_filterFiles = 2;
		}
	}

	if(dlg.DoModal() == IDCANCEL)
		return;

#if 0
	// get client and user filter settings
	m_FilteredByClient = dlg.m_useClient;
	GET_P4REGPTR()->SetFilteredByClient(m_FilteredByClient);
	if(m_FilteredByClient)
	{
        m_ClientFilter = dlg.m_client;
		GET_P4REGPTR()->SetClientFilter(m_ClientFilter);
	}

	m_FilteredByUser = dlg.m_useUser;
	GET_P4REGPTR()->SetFilteredByUser(m_FilteredByUser);
	if(m_FilteredByUser)
	{
		m_UserFilter = dlg.m_user;
		GET_P4REGPTR()->SetUserFilter(m_UserFilter);
	}
#endif

	// get the filter view
	switch(dlg.m_filterFiles)
	{
	case 0:	// all files
		GET_P4REGPTR()->SetFilterPendChgsByMyClient(0);
		break;
	case 1:	// my client files
		GET_P4REGPTR()->SetFilterPendChgsByMyClient(1);
		break;
	case 2:	// filespec
	 {
		// convert filespec into view stringlist
		CString filespec = dlg.m_filespec;
		if (!filespec.IsEmpty())
		{
			GET_P4REGPTR()->SetFilterPendChgsByMyClient(2);
			GET_P4REGPTR()->SetFilterPendChgsByPath(filespec);
			GET_P4REGPTR()->AddMRUChgFilter( filespec );	// save as most recently used in Reg
		}
		break;
	 }
	case 3:	// selected files
	  {
		CString filterView;
		POSITION pos = selected.GetHeadPosition();
		while (pos != NULL)
		{
			CString str = selected.GetNext(pos);
			if (dlg.m_UseClientSyntax)
			{
				// user wants to convert to client syntax
				CCmd_Where *pCmd1 = new CCmd_Where;
				pCmd1->Init(NULL, RUN_SYNC);
				if ( pCmd1->Run(str) && !pCmd1->GetError() 
				  && pCmd1->GetClientFiles()->GetCount() )
				{
					CStringList * list = pCmd1->GetClientFiles();
					POSITION pos2 = list->GetHeadPosition();
					while (pos2 != NULL)
						filterView += list->GetNext(pos2) + _T(' ');
				}
				else	// p4 where failed - use depot syntax after all
					filterView += str + _T(' ');
				delete pCmd1;
			}
			else
				filterView += str + _T(' ');
		}
		filterView.TrimRight();
		if (!filterView.IsEmpty())
		{
			GET_P4REGPTR()->SetFilterPendChgsByMyClient(2);
			GET_P4REGPTR()->SetFilterPendChgsByPath(filterView);
			GET_P4REGPTR()->AddMRUChgFilter( filterView );	// save as most recently used in Reg
		}
		break;
	  }
	}
	OnViewUpdate();
}

void CDeltaTreeCtrl::OnFilterClearview()
{
	GET_P4REGPTR()->SetFilterPendChgsByMyClient(0);
	OnViewUpdate();
}

void CDeltaTreeCtrl::OnUpdateAddBookmark(CCmdUI* pCmdUI) 
{
	BOOL root;
	BOOL rc = FALSE;
	if (!SERVER_BUSY() && GetSelectedCount() == 1)
	{
		HTREEITEM item;
		if (GetItemLevel((item = GetSelectedItem(0)), &root) == 2 && IsAFile(item))
			rc = TRUE;
	}
	pCmdUI->Enable(rc);
}

void CDeltaTreeCtrl::OnAddBookmark() 
{
	BOOL root;
	HTREEITEM item;

	if (GetSelectedCount() != 1
	 || GetItemLevel((item = GetSelectedItem(0)), &root) != 2 
	 || !IsAFile(item))
	{
		ASSERT(0);
		return;
	}
	int j;
	CString txt = GetItemText(item);
	if (txt.GetAt(0) == _T('/') && ((j = txt.ReverseFind(_T('#'))) != -1))
		txt = txt.Left(j);
	::SendMessage(m_depotWnd, WM_ADDBOOKMARK, 0, (LPARAM)(&txt));
}
