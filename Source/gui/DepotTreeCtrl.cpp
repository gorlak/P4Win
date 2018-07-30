//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


#include "stdafx.h"
#include "errno.h"
//#define TRACE_HERE
#include "p4win.h"
#include "DepotTreeCtrl.h"
#include "resource.h"
#include "MainFrm.h"
#include "Historydlg.h"
#include "P4FileStats.h"
#include <process.h>
#include "viewerdlg.h"
#include "FileInfoDlg.h"
#include "FindFilesDlg.h"
#include "ForceSyncDlg.h"
#include "RemoveViewer.h"
#include "integdlg\FileSpecPage.h"
#include "integdlg\OptionsPage.h"
#include "MsgBox.h"
#include "Diff2Dlg.h"
#include "FilterDepotDlg.h"
#include "ImageList.h"
#include "SpecDescDlg.h"
#include "BookmarkAdd.h"

#include "cmd_add.h"
#include "cmd_changes.h"
#include "cmd_delete.h"
#include "cmd_diff.h"
#include "cmd_diff2.h"
#include "cmd_get.h"
#include "cmd_files.h"
#include "cmd_fstat.h"
#include "cmd_depots.h"
#include "cmd_dirstat.h"
#include "cmd_history.h"
#include "cmd_integrate2.h"
#include "cmd_listopstat.h"
#include "cmd_opened.h"
#include "cmd_maxchange.h"
#include "cmd_prepbrowse.h"
#include "cmd_prepedit.h"
#include "cmd_refresh.h"
#include "cmd_revert.h"
#include "cmd_where.h"
#include "strops.h"

// Flags for selection set adjustments durning a drag-drop operation
#define KEEP_SELECTION    0x00
#define DELETE_EXISTING_SELECTION 0x01
#define	ADD_TO_SELECTION  0x02

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//		we use the new method of getting depot files in 98.2
//		use the old way for those lusers that have old servers
//
#define SERVER_IS_982_ORMORE (GET_SERVERLEVEL()>3)
#define NEW_DEPOT_LISTING  SERVER_IS_982_ORMORE

//		used for LPARAM of htree item.
//		the lparam of files is the item count
//		the lparam of subdirs is a negative number, showing
//		whether it is expanded or not.
//		since the code was full of if (item.lparam != -1 )
//		let's take the magic numbers out.
//

#define ITEM_IS_A_FILE_NOT_A_SUBDIR		(item.lParam>-1)
#define ITEM_IS_FILE(x) ((int) GetLParam(x) > -1)


//		to prevent littering the code with slashes
//
const CString g_sSlashes = "//";


//		we tag the directory names with a space so that they can be 
//		sorted, subdirs first, files after. use this const so we 
//		can find those blanks right away if we have to.
//
const CString g_sStupidLeadingBlank = " ";


//	Make sure the user knows why an empty folder of depot is visible.
//  Two possible causes are: 
//  1) only deleted files below, and 
//  2) its a depot that is empty or is outside the client view

/*const */CString g_TrulyEmptyDir;// = LoadStringResource(IDS_CONTAINS_NO_FILES_OR_FOLDERS);

void DepotContextMenu(CView * pDepotView)
{
	CPoint point;
	point.x = point.y = 0;
	((CDepotTreeCtrl *)pDepotView)->Call_OnContextMenu(NULL, point);
}

/////////////////////////////////////////////////////////////////////////////
// CDepotTreeCtrl

IMPLEMENT_DYNCREATE(CDepotTreeCtrl, CMultiSelTreeCtrl)

BEGIN_MESSAGE_MAP(CDepotTreeCtrl, CMultiSelTreeCtrl)
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_UPDATE_COMMAND_UI(ID_FILE_LOCK, OnUpdateFileLock)
	ON_UPDATE_COMMAND_UI(ID_FILE_UNLOCK, OnUpdateFileUnlock)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPENDELETE, OnUpdateFileOpendelete)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPENEDIT, OnUpdateFileOpenedit)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPENEDITA, OnUpdateFileOpenedit)
	ON_UPDATE_COMMAND_UI(ID_FILE_REVERT, OnUpdateFileRevert)
	ON_UPDATE_COMMAND_UI(ID_FILE_GET, OnUpdateFileGet)
	ON_UPDATE_COMMAND_UI(ID_FILE_GETWHATIF, OnUpdateGetwhatif)
	ON_UPDATE_COMMAND_UI(ID_FILE_REMOVE, OnUpdateFileRemove)
	ON_UPDATE_COMMAND_UI(ID_FILE_ANNOTATIONS, OnUpdateFileTimeLapse)
	ON_UPDATE_COMMAND_UI(ID_FILE_ANNOTATE, OnUpdateFileAnnotate)
	ON_UPDATE_COMMAND_UI(ID_FILE_ANNOTATEALL, OnUpdateFileAnnotate)
	ON_UPDATE_COMMAND_UI(ID_FILE_ANNOTATECHG, OnUpdateFileAnnotate)
	ON_UPDATE_COMMAND_UI(ID_FILE_ANNOTATECHGALL, OnUpdateFileAnnotate)
	ON_UPDATE_COMMAND_UI(ID_FILE_REVISIONHISTORY, OnUpdateFileRevisionhistory)
	ON_UPDATE_COMMAND_UI(ID_FILE_REVISIONTREE, OnUpdateFileRevisiontree)
	ON_UPDATE_COMMAND_UI(ID_FILE_DIFFHEAD, OnUpdateFileDiffhead)
	ON_UPDATE_COMMAND_UI(ID_FILE_DIFF2, OnUpdateFileDiff2)
	ON_UPDATE_COMMAND_UI(ID_POSITIONTOPATTERN, OnUpdatePositionDepot)
	ON_UPDATE_COMMAND_UI(ID_FINDNEXT, OnUpdatePositionDepotNext)
	ON_UPDATE_COMMAND_UI(ID_FINDPREV, OnUpdatePositionDepotNext)
	ON_UPDATE_COMMAND_UI(ID_POSITIONCHGS, OnUpdatePositionChgs)
	ON_UPDATE_COMMAND_UI(ID_WINEXPLORE, OnUpdateWinExplore)
	ON_UPDATE_COMMAND_UI(ID_CMDPROMPT, OnUpdateCmdPrompt)
	ON_COMMAND(ID_FILE_LOCK, OnFileLock)
	ON_COMMAND(ID_FILE_UNLOCK, OnFileUnlock)
	ON_COMMAND(ID_FILE_OPENEDIT, OnFileOpenedit)
	ON_COMMAND(ID_FILE_OPENEDITA, OnFileOpenedit)
	ON_COMMAND(ID_FILE_OPENDELETE, OnFileOpendelete)
	ON_COMMAND(ID_FILE_REVERT, OnFileRevert)
	ON_COMMAND(ID_FILE_GET, OnFileGet)
	ON_COMMAND(ID_FILE_GETWHATIF, OnFileGetwhatif)
	ON_COMMAND(ID_FILE_REMOVE, OnFileRemove)
	ON_COMMAND(ID_FILE_ANNOTATIONS, OnFileTimeLapseView)
	ON_COMMAND(ID_FILE_ANNOTATE, OnFileAnnotate)
	ON_COMMAND(ID_FILE_ANNOTATEALL, OnFileAnnotateAll)
	ON_COMMAND(ID_FILE_ANNOTATECHG, OnFileAnnotateChg)
	ON_COMMAND(ID_FILE_ANNOTATECHGALL, OnFileAnnotateChgAll)
	ON_COMMAND(ID_FILE_REVISIONHISTORY, OnFileRevisionhistory)
	ON_COMMAND(ID_FILE_REVISIONTREE, OnFileRevisionTree)
	ON_COMMAND(ID_POSITIONTOPATTERN, OnPositionDepot)
	ON_COMMAND(ID_FINDNEXT, OnPositionDepotNext)
	ON_COMMAND(ID_FINDPREV, OnPositionDepotPrev)
	ON_COMMAND(ID_POSITIONCHGS, OnPositionChgs)
	ON_COMMAND(ID_WINEXPLORE, OnWinExplore)
	ON_COMMAND(ID_CMDPROMPT, OnCmdPrompt)
	ON_UPDATE_COMMAND_UI(ID_FILE_GETCUSTOM, OnUpdateFileGetcustom)
	ON_COMMAND(ID_FILE_GETCUSTOM, OnFileGetcustom)
	ON_UPDATE_COMMAND_UI(ID_FILE_REFRESH, OnUpdateFileRefresh)
	ON_COMMAND(ID_FILE_REFRESH, OnFileRefresh)
	ON_UPDATE_COMMAND_UI(ID_FILE_FORCESYNCTOHEAD, OnUpdateFileForceToHead)
	ON_COMMAND(ID_FILE_FORCESYNCTOHEAD, OnFileForceToHead)
	ON_COMMAND(ID_FILE_DIFFHEAD, OnFileDiffhead)
	ON_COMMAND(ID_FILE_DIFF2, OnFileDiff2)
	ON_UPDATE_COMMAND_UI(ID_FILE_PROPERTIES, OnUpdateFileInformation)
	ON_COMMAND(ID_FILE_PROPERTIES, OnFileInformation)
	ON_UPDATE_COMMAND_UI(ID_FILEDROP_EDIT, OnUpdateFiledropEdit)
	ON_COMMAND(ID_FILEDROP_EDIT, OnFiledropEdit)
	ON_UPDATE_COMMAND_UI(ID_FILEDROP_DELETE, OnUpdateFiledropDelete)
	ON_COMMAND(ID_FILEDROP_DELETE, OnFiledropDelete)
	ON_UPDATE_COMMAND_UI(ID_FILEDROP_CANCEL, OnUpdateFiledropCancel)
	ON_COMMAND(ID_FILEDROP_CANCEL, OnFiledropCancel)
	ON_UPDATE_COMMAND_UI(ID_FILE_AUTOEDIT, OnUpdateFileAutoedit)
	ON_COMMAND(ID_FILE_AUTOEDIT, OnFileAutoedit)
	ON_COMMAND(ID_FILE_QUICKEDIT, OnFileQuickedit)
	ON_UPDATE_COMMAND_UI(ID_FILE_AUTOBROWSE, OnUpdateFileAutobrowse)
	ON_COMMAND(ID_FILE_AUTOBROWSE, OnFileAutobrowse)
	ON_COMMAND(ID_FILE_QUICKBROWSE, OnFileQuickbrowse)
	ON_UPDATE_COMMAND_UI(ID_JOB_SETFILEFILTER, OnUpdateFilterJobview)
	ON_COMMAND(ID_JOB_SETFILEFILTER, OnFilterJobview)
	ON_UPDATE_COMMAND_UI(ID_JOB_SETFILEFILTERINTEG, OnUpdateFilterJobview)
	ON_COMMAND(ID_JOB_SETFILEFILTERINTEG, OnFilterJobviewInteg)
	ON_UPDATE_COMMAND_UI(ID_FILE_RENAME, OnUpdateFileRename)
	ON_COMMAND(ID_FILE_RENAME, OnFileRename)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_SUBMIT, OnUpdateFileSubmit)
	ON_COMMAND(ID_CHANGE_SUBMIT, OnFileSubmit)
	ON_UPDATE_COMMAND_UI(ID_FILE_INTEGRATE, OnUpdateFileIntegrate)
	ON_COMMAND(ID_FILE_INTEGRATE, OnFileIntegrate)
	ON_COMMAND(ID_FILE_INTEGSPEC, OnFileIntegspec)
	ON_COMMAND(ID_VIEW_UPDATE_LEFT, OnViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_FILE_RECOVER, OnUpdateFileRecover)
	ON_COMMAND(ID_FILE_RECOVER, OnFileRecover)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYCLIENTPATH, OnUpdateEditCopyclientpath)
	ON_COMMAND(ID_EDIT_COPYCLIENTPATH, OnEditCopyclientpath)
	ON_COMMAND(ID_DIFFSDSE, OnDiff_sd_se)
	ON_UPDATE_COMMAND_UI(ID_DIFFSDSE, OnUpdateDiff_sd_se)
	ON_UPDATE_COMMAND_UI(ID_ADD_TOVIEW, OnUpdateAddToClientView)
	ON_COMMAND(ID_ADD_TOVIEW, OnAddToClientView)
	ON_UPDATE_COMMAND_UI(ID_ADD_REVIEWS, OnUpdateAddReviews)
	ON_COMMAND(ID_ADD_REVIEWS, OnAddReviews)
	ON_UPDATE_COMMAND_UI(ID_FILE_RMVBROWSER, OnUpdateRemoveViewer)
	ON_COMMAND(ID_FILE_RMVBROWSER, OnRemoveViewer)
	ON_UPDATE_COMMAND_UI(ID_FILE_RMVEDITOR, OnUpdateRemoveViewer)
	ON_COMMAND(ID_FILE_RMVEDITOR, OnRemoveViewer)
	ON_UPDATE_COMMAND_UI(ID_FILE_QUICKEDIT, OnUpdateFileAutoedit)
	ON_UPDATE_COMMAND_UI(ID_FILE_QUICKBROWSE, OnUpdateFileAutobrowse)
	ON_UPDATE_COMMAND_UI(ID_FILE_INTEGSPEC, OnUpdateFileIntegrate)
	ON_UPDATE_COMMAND_UI(ID_FINDFILEUNDERFOLDER, OnUpdateFindFileUnderFolder)
	ON_COMMAND(ID_FINDFILEUNDERFOLDER, OnFindFileUnderFolder)
	ON_UPDATE_COMMAND_UI(ID_ADD_BOOKMARK, OnUpdateAddBookmark)
	ON_COMMAND(ID_ADD_BOOKMARK, OnAddBookmark)
	ON_UPDATE_COMMAND_UI(ID_FILE_DELETE, OnUpdateFileDelete)
	ON_COMMAND(ID_FILE_DELETE, OnFileDelete)
	ON_UPDATE_COMMAND_UI(ID_FILE_ADD, OnUpdateFileAdd)
	ON_COMMAND(ID_FILE_ADD, OnFileAdd)
	ON_COMMAND_RANGE(ID_FILE_BROWSER_1, ID_FILE_BROWSER_1+MAX_MRU_VIEWERS-1, OnFileMRUBrowse)
	ON_COMMAND_RANGE(ID_FILE_EDITOR_1, ID_FILE_EDITOR_1+MAX_MRU_VIEWERS-1, OnFileMRUEditor)
	ON_COMMAND(ID_FILE_NEWBROWSER, OnFileNewBrowser)
	ON_COMMAND(ID_FILE_NEWEDITOR, OnFileNewEditor)
	ON_COMMAND(ID_SHOWDELETED, OnShowDeletedFiles )
	ON_UPDATE_COMMAND_UI(ID_VIEW_UPDATE_LEFT, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILTEREDVIEW, OnUpdateViewFilteredview)
	ON_COMMAND(ID_VIEW_FILTEREDVIEW, OnViewFilteredview)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CLEARFILTER, OnUpdateViewClearfilter)
	ON_COMMAND(ID_VIEW_CLEARFILTER, OnViewClearfilter)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SETVIEW, OnUpdateFilterSetview)
	ON_COMMAND(ID_FILTER_SETVIEW, OnFilterSetview)
	ON_UPDATE_COMMAND_UI(ID_FILTER_CLEARVIEW, OnUpdateFilterClearview)
	ON_COMMAND(ID_FILTER_CLEARVIEW, OnFilterClearview)
	ON_UPDATE_COMMAND_UI(ID_LABELFILTER_SETVIEW, OnUpdateLabelFilterSetview)
	ON_COMMAND(ID_LABELFILTER_SETVIEW, OnLabelFilterSetview)
	ON_UPDATE_COMMAND_UI(ID_LABELFILTER_SETVIEWREV, OnUpdateLabelFilterSetviewRev)
	ON_COMMAND(ID_LABELFILTER_SETVIEWREV, OnLabelFilterSetviewRev)
	ON_UPDATE_COMMAND_UI(ID_LABELFILTER_CLEARVIEW, OnUpdateLabelFilterClearview)
	ON_COMMAND(ID_LABELFILTER_CLEARVIEW, OnLabelFilterClearview)
	ON_COMMAND(ID_PERFORCE_OPTIONS, OnPerforceOptions)
	ON_MESSAGE(WM_FILEEDITTXT, OnEditFileTxt )
	ON_MESSAGE(WM_FILEEDITBIN, OnEditFileBin )
	ON_MESSAGE(WM_FILEBROWSETXT, OnBrowseFileTxt )  
	ON_MESSAGE(WM_FILEBROWSEBIN, OnBrowseFileBin )  
	ON_MESSAGE(WM_P4LISTOPSTAT, OnP4ListOp )
	ON_MESSAGE(WM_P4DIFF, OnP4Diff )
	ON_MESSAGE(WM_P4ERROR, OnP4Error )
	ON_MESSAGE(WM_P4FSTAT, OnP4FStat )
    ON_MESSAGE(WM_P4DIRSTAT, OnP4DirStat )
	ON_MESSAGE(WM_P4EXPANDSUBDIR, OnP4ExpandTree )
    ON_MESSAGE(WM_P4DEPOTS, OnP4Depots )
	ON_MESSAGE(WM_P4INTEGRATE2, OnP4Integ )
	ON_MESSAGE(WM_P4PREPBROWSE, OnP4PrepBrowse )
	ON_MESSAGE(WM_P4PREPEDIT, OnP4PrepEdit )
	ON_MESSAGE(WM_P4HISTORY, OnP4History )
	ON_MESSAGE(WM_P4ENDHISTORY, OnP4EndHistory )
	ON_MESSAGE(WM_P4GET, OnP4Get )
	ON_MESSAGE(WM_P4REFRESH, OnP4Refresh )
	ON_MESSAGE(WM_P4ADD, OnP4Add )
	ON_MESSAGE(WM_P4RECOVER, OnP4Recover )
	ON_MESSAGE(WM_UPDATEOPEN, OnP4UpdateOpen )
	ON_MESSAGE(WM_GETSELCOUNT, OnGetSelectedCount )
	ON_MESSAGE(WM_GETSELLIST, OnGetSelectedList )
	ON_MESSAGE(WM_BRANCHINTEG, OnBranchIntegrate )
	ON_MESSAGE(WM_CHANGELISTINTEG, OnChangelistIntegrate )
	ON_MESSAGE(WM_DOINTEGRATE1, OnIntegrate1 )
	ON_MESSAGE(WM_DOCUSTOMGET, OnDoGetCustom )
	ON_MESSAGE(WM_P4FILEINFORMATION, OnP4FileInformation )
	ON_MESSAGE(WM_P4ENDFILEINFORMATION, OnP4EndFileInformation )
	ON_MESSAGE(WM_P4FILES, OnP4Files )
	ON_MESSAGE(WM_P4OPENED, OnP4Opened )
	ON_MESSAGE(WM_DROPTARGET, OnDropTarget)
	ON_MESSAGE(WM_VIEWHEAD, OnViewHead )
	ON_MESSAGE(WM_ISFILTEREDONOPEN, IsFilteredOnOpen )
	ON_MESSAGE(WM_REDOOPENEDFILTER, OnRedoOpendList )
	ON_MESSAGE(WM_SETADDFSTATS, OnSetAddFstats )
	ON_MESSAGE(WM_GETADDFSTATS, OnGetAddFstats )
	ON_MESSAGE(WM_P4DIFF2, OnP4Diff2 )
	ON_MESSAGE(WM_P4ENDDESCRIBE, OnP4EndDiff2 )
	ON_MESSAGE(WM_P4ENDDIFF2OUTPUT, OnP4EndDiff2Output )
	ON_MESSAGE(WM_P4ENDFINDFILES, OnEndPositionDepot )
	ON_MESSAGE(WM_P4FILEREVERT, OnP4FileRevert )
	ON_MESSAGE(WM_ADDBOOKMARK, OnAddBookmarkMsg )
	ON_MESSAGE(WM_BROWSECALLBACK1, OnIntegBranchBrowseCallBack)
	ON_MESSAGE(WM_SETVIEWER, OnSetViewer)
END_MESSAGE_MAP()

CDepotTreeCtrl::CDepotTreeCtrl()
{
	m_OLESource.SetTreeCtrl(this);
	m_DepotIsDropTarget = FALSE;
	m_DragDropCtr= 0;

	g_TrulyEmptyDir = LoadStringResource(IDS_CONTAINS_NO_FILES_OR_FOLDERS);

	m_CF_DELTA=RegisterClipboardFormat(LoadStringResource(IDS_DRAGFROMDELTA));
	m_CF_DEPOT=RegisterClipboardFormat(LoadStringResource(IDS_DRAGFROMDEPOT));

	m_Need2Filter = FALSE;
	m_ExpandDepotContinue = m_Add2ExpandItemList = m_JustExpanded = FALSE;
	m_DropTargetFlag = 0;

	m_CustomGetDlg = 0;

	m_FilterDepot = m_P4Files_Deselect = FALSE;
	m_DepotFilterType = -1;
	m_DepotFilterPort = m_DepotFilterClient = "";

	m_ContextPoint.x = m_ContextPoint.y = -1;
	m_InContextMenu = FALSE;
	m_SkipSyncDialog = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CDepotTreeCtrl diagnostics

#ifdef _DEBUG
void CDepotTreeCtrl::AssertValid() const
{
	CMultiSelTreeCtrl::AssertValid();
}

void CDepotTreeCtrl::Dump(CDumpContext& dc) const
{
	CMultiSelTreeCtrl::Dump(dc);
}
#endif //_DEBUG

/*
	_________________________________________________________________

	wipe out entire depot tree and start fresh with the depot at the root.
	_________________________________________________________________
*/

void CDepotTreeCtrl::Clear()
{
	XTRACE(_T("Clear()\n"));

    SetRedraw(FALSE);
	DeleteAllItems( );
    SetRedraw(TRUE);
    m_ItemCount = m_DepotCount = 0;
	m_FSColl.DestroyAll( );

	// We also need to empty out the Fstats info
	// for files opened for add
	Empty_FstatsAdds();

	m_LastPath = g_sSlashes;
	m_Root = m_LastPathItem = TVI_ROOT;
	SetAdd2ExpandItemList(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// CDepotTreeCtrl tree control access functions

// This is a recursive function!
// It searches a whole tree looking for a file with the name given in 'lookingfor'
// Traversing a file tree just begs for a fully resursive routine. 
// But if there are too many files, the stack overflows and we crash. 
// So recurse when we change levels and loop at the same level
// to dramatically reduce the stack usage.
HTREEITEM CDepotTreeCtrl::CheckItem(CString lookingfor, HTREEITEM item, BOOL useRevNum, int revNum)
{
	int lgth = lookingfor.GetLength();
	if (!HasChildren(item))
	{
		while (item)	// loop on items at the same level
		{
			BOOL b;
			HTREEITEM prevItem;

			CP4FileStats *stats= m_FSColl.GetStats(GetLParam(item));
			if (IsBadWritePtr(stats, sizeof(CP4FileStats)))
			{
				b = FALSE;
			}
			else
			{
				LPCTSTR path = lookingfor.GetAt(0) == _T('/') ? stats->GetFullDepotPath() 
					                                          : stats->GetFullClientPath();
				if (IsBadWritePtr((LPVOID)path, lgth))
					b = FALSE;
				else
					b = nCompare(path, lookingfor, lgth) == 0;
				if (b && useRevNum)
					b = revNum == stats->GetHaveRev();
			}
			if (b)
				return item;
			item = TreeView_GetNextSibling(m_hWnd, prevItem = item);
			if (prevItem == item)
				item = NULL;
			else while (item && HasChildren(item))
			{
				item = TreeView_GetChild(m_hWnd, item);
				if (item)
				{
					// recurse to go down a level
					item = CheckItem(lookingfor, item, useRevNum, revNum);
				}
				else
				{
					item = TreeView_GetNextSibling(m_hWnd, prevItem = item);
					if (prevItem == item)
						item = NULL;
				}
			}
		}
	}
	else
	{
		// before exploring the next level, check everything at this level
		HTREEITEM nextitem = TreeView_GetNextSibling(m_hWnd, item);
		if (nextitem)
		{
			// hopefully we can resurse here without blowing the stack
			// since we will only be recursing for directories, not files
			if ((nextitem = CheckItem(lookingfor, nextitem, useRevNum, revNum)) != NULL)
				return nextitem;
		}
		item = TreeView_GetChild(m_hWnd, item);
		if (item)
		{
			// recurse to go down a level
			item = CheckItem(lookingfor, item, useRevNum, revNum);
		}
	}
	return item;
}

HTREEITEM CDepotTreeCtrl::FindItem(CString path, CString fname, BOOL useRevNum)
{
    // If the file is under a folder known to be empty, immediately return NULL for
    // not found.  Note: before each series of FindItem() calls, m_LastChildlessPath 
    // should be set to an impossible path, by calling InitFindItem()    

    if( nCompare( path, m_LastChildlessPath, m_LastChildlessPath.GetLength()) == 0 )
        return NULL;

	HTREEITEM item=m_Root;
	CString temp;
	int start=2;   // Right after '//'
	int last;
	m_LastPath.Empty();
	m_LastPathItem=m_Root;
    BOOL isDepotNode;

	if (FindMBCS(path, m_SlashChar) == -1)
	{
		if (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
		{
			long revNum = -5;
			CString findwhat = path + fname;
			if ((last = findwhat.Find(_T('#'))) != -1)
			{
				revNum = _tstol(findwhat.Mid(last+1));
				findwhat = findwhat.Left(last);
			}
			item=TreeView_GetNextItem(m_hWnd, TVI_ROOT, TVGN_ROOT);
			return CheckItem(findwhat, item, useRevNum, revNum);
		}
	}

	while(1)
	{
		////////////
		// Get next chunk of path, break if no more path left
		temp=path.Mid(start);
		last=FindMBCS(temp, m_SlashChar);
		start+=(last+1);
		if(last ==-1)
			break;		
		
		////////////
		// Get the first item at m_LastPath node in tree
		if(item==m_Root)
		{
			item=TreeView_GetNextItem(m_hWnd, TVI_ROOT, TVGN_ROOT);
			temp = g_sSlashes + temp.Left(last);
            isDepotNode=TRUE;
		}
		else
        {
			item=TreeView_GetChild(m_hWnd, item);   // Get first child
            temp = g_sStupidLeadingBlank + temp.Left(last);	
		    isDepotNode=FALSE;
        }

        ///////////
        // If there are NO leaves at this node, save the path to the empty
        // node, so subsequent searches for files under this path can be
        // expedited
        if( item == NULL )
        {
            m_LastChildlessPath= m_LastPath;
            if(m_LastPathItem == m_Root)
				m_LastChildlessPath= g_sSlashes;
        }

		///////////
		// For each leaf at this node, compare path
		while(item != NULL)
        {
            CString itemTxt= GetItemName(item);
            if( Compare(itemTxt,temp) == 0 ||
                            Compare(itemTxt,temp+g_TrulyEmptyDir) == 0 )
                break;
			item= TreeView_GetNextSibling(m_hWnd, item);
        }
		

		//////////
		// A NULL item means we looked at all leaves, and didnt get a match
		// In other words, we didnt find anything, so return a NULL for not found
		if(item==NULL)
		{
			if(m_LastPathItem == m_Root)
				m_LastPath= g_sSlashes;
			return NULL;
		}

		//////////
		// We have a match, at least up to current item, so update
		// last path and parent
        if( isDepotNode )
            m_LastPath= temp;
        else
		    m_LastPath+= temp.Mid(1);

		m_LastPath+=_T("/");
		m_LastPathItem=item;
	}

	// If item is still m_Root, we found nothing
	if(item==m_Root)
	{
		m_LastPath= g_sSlashes;
		m_LastPathItem=m_Root;
	}

	// Should be at the correct directory node
	item=TreeView_GetChild(m_hWnd, item);   // Get first child
	
    ///////////
    // If there are NO leaves at this node, save the path to the empty
    // node, so subsequent searches for files under this path can be
    // expedited
    if( item == NULL )
    {
        m_LastChildlessPath= m_LastPath;
    	return NULL;
    }

	int startRev=fname.ReverseFind(_T('#'));
	if(startRev == -1)
	{
		if(!useRevNum)
			startRev=fname.GetLength();
		else
		{
			ASSERT(0);    // Wanted to search by revision # but didnt provide revision number
			return NULL;
		}
	}

	if(useRevNum)	// do the compare outside the loop for speed - this gets called a lot
	{
		while(1)
		{
#ifdef _DEBUG
			temp=GetItemName(item);
#endif
			if(Compare(fname, GetItemName(item)) == 0)
				break;                      // found it
		
			item= TreeView_GetNextSibling(m_hWnd, item);
			if(item==NULL)
				return NULL;
		}
	}
	else
	{
		while(1)
		{
			temp=GetItemName(item);
			
			// compare everything except the revision
			if(nCompare(fname, temp, max(startRev, temp.GetLength())) == 0)
				break;                      // found it

			item= TreeView_GetNextSibling(m_hWnd, item);
			if(item==NULL)
				return NULL;
		}
	}
	
	return item;
}


void CDepotTreeCtrl::RecordTreeExploration()
{
    // Hose out the lists of explored and expanded files
    m_StringList.RemoveAll();
    m_ExpandedNodeList.RemoveAll();

    // Record the first visible item
    HTREEITEM item= TreeView_GetFirstVisible(m_hWnd);

    if( item != NULL )
        m_FirstVisibleNodeText= GetItemPath( TreeView_GetFirstVisible(m_hWnd) ); 
    else
        m_FirstVisibleNodeText.Empty();

	XTRACE(_T("Recorded first vis node=%s\n"), m_FirstVisibleNodeText);

    // Rummage through the tree, recording all folders that have been explored,
    // and noting those that are still expanded
    RecordFolderExploration(m_Root, _T(""));
}

void CDepotTreeCtrl::RecordFolderExploration(HTREEITEM parentItem, LPCTSTR path)
{
    TCHAR buf[ LONGPATH + 1 ];
    HTREEITEM item;

    if( parentItem == m_Root)
		item=TreeView_GetRoot(m_hWnd);
	else
		item=TreeView_GetChild(m_hWnd, parentItem);

    while(item != NULL)
	{
        TV_ITEM tvItem;
	    tvItem.hItem=item;
        tvItem.pszText = buf;         
        tvItem.cchTextMax = LONGPATH ;  
        tvItem.stateMask= TVIS_EXPANDED ;
	    tvItem.mask=TVIF_STATE | TVIF_PARAM | TVIF_HANDLE | TVIF_TEXT;
	    TreeView_GetItem(m_hWnd, &tvItem );	
	       
        if( tvItem.lParam == FOLDER_ALREADY_EXPANDED && (tvItem.state & TVIS_EXPANDED) == TVIS_EXPANDED )
        {
			// Get the foldername, minus leading spaces and trailing empty dir text
			TCHAR slashChar;
            CString folder;
			if( buf[0] == _T(' ') )
				folder= buf+1;
			else
				folder= buf;
            
			int junkStart= folder.Find( g_TrulyEmptyDir );
			if( junkStart != -1 )
				folder= folder.Left(junkStart);

            CString fullPath= path;

            if( !fullPath.IsEmpty() )
			{
				slashChar = fullPath.GetAt(0) == _T('/') ? _T('/') : _T('\\');
				if ( fullPath.GetLength() > 2 && fullPath.GetAt(2) == _T('\\') )
					TrimRightMBCS(fullPath, _T("\\"));
                fullPath += slashChar;
			}
			else
			{
				slashChar = folder.GetAt(0) == _T('/') ? _T('/') : _T('\\');
				if ( folder.GetLength() > 2 && folder.GetAt(2) == _T('\\') )
					TrimRightMBCS(folder, _T("\\"));
			}

            fullPath += folder;

            m_StringList.AddHead( CString( fullPath + slashChar + _T("*")) );
            m_ExpandedNodeList.AddHead( fullPath );

            RecordFolderExploration( item, fullPath );
		}
        
		item= TreeView_GetNextSibling(m_hWnd, item);
	}
}


/*
	_________________________________________________________________
*/

HTREEITEM CDepotTreeCtrl::FindFolder(LPCTSTR folderName)
{
	HTREEITEM item=m_LastPathItem;
	CString temp=folderName;
	CString temp2= temp+g_TrulyEmptyDir;
		
	if(m_LastPathItem == m_Root)
		item=TreeView_GetNextItem(m_hWnd, m_LastPathItem, TVGN_ROOT);
	else
		item=TreeView_GetNextItem(m_hWnd, m_LastPathItem, TVGN_CHILD);
	
	
	///////////
	// For each leaf compare the folder name

	while(item != NULL)
	{
		CString itemText= GetItemText(item);
		if(Compare(itemText,temp) == 0  || Compare(itemText,temp2) == 0 )
			break;

		item= TreeView_GetNextSibling(m_hWnd, item);
	}

	return item;
}


/*
	_________________________________________________________________

	Same as GetItemText, but strip off revision info
	_________________________________________________________________
*/

CString CDepotTreeCtrl::GetItemName(HTREEITEM curr_item)
{
	CString temp=GetItemText(curr_item);
	int i=temp.ReverseFind(_T('#'));
	if (i > 0 && temp.GetAt(i-1) == _T(' '))
		// the '#' is preceded by a space, so its i-1
 		return temp.Left(i-1); 
	else
		return temp;
}

/*
	_________________________________________________________________

	Same as GetItemName, but return have revision number as a CString
	_________________________________________________________________
*/

CString CDepotTreeCtrl::GetItemRev(HTREEITEM curr_item)
{
	CString temp=GetItemText(curr_item);
	int i = temp.ReverseFind(_T('#'));
	if(i != -1)
	{
 		temp = temp.Mid(i+1);
		i = temp.Find(_T('/'));
		if (i != -1)
			temp = temp.Left(i);
	}
	else
		temp.Empty();
	return temp;
}

/*
	_________________________________________________________________

	Same as GetItemName, but return head revision number as a CString
	_________________________________________________________________
*/

CString CDepotTreeCtrl::GetItemHeadRev(HTREEITEM curr_item)
{
	CString temp=GetItemText(curr_item);
	int i = temp.ReverseFind(_T('#'));
	if(i != -1)
	{
 		temp = temp.Mid(i+1);
		i = temp.Find(_T('/'));
		if (i != -1)
		{
			temp = temp.Mid(i+1);
			i = temp.Find(_T(' '));
			if (i != -1)
				temp = temp.Left(i);
		}
		else
			temp.Empty();
	}
	else
		temp.Empty();
	return temp;
}

/*
	_________________________________________________________________

	Return the item's file type
	_________________________________________________________________
*/

CString CDepotTreeCtrl::GetItemType(HTREEITEM curr_item)
{
	CString type = _T("");
	if (GET_P4REGPTR()->ShowFileType())
	{
		CString temp = GetItemText(curr_item);
		int i = temp.ReverseFind(_T('#'));
		if(i != -1)
		{
 			temp = temp.Mid(i+1);
			i = temp.Find(_T(' '));
			if (i != -1)
			{
				type = temp.Mid(i+1);
				i = type.Find(_T(' '));
				if (i != -1)
					type = type.Left(i);
				type.TrimLeft(_T("< "));
				type.TrimRight(_T("> "));
			}
		}
	}
	if (type.IsEmpty())
	{
		TV_ITEM item;
		item.hItem=curr_item;
		item.mask=TVIF_HANDLE| TVIF_CHILDREN | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			CP4FileStats *fs=m_FSColl.GetStats((int) item.lParam);
			if (fs && fs->IsKindOf(RUNTIME_CLASS(CP4FileStats)))
				type = fs->GetHeadType();
		}
	}
	if (type.IsEmpty())
		type = _T("unknown");	// don't know
	return type;
}



/*
	_________________________________________________________________
	
	climb all the way up the tree (to "//depot" or whatever it's
	called in the server), getting the path pieces and concatenating 
	them with a single slash between.
	_________________________________________________________________
*/

CString CDepotTreeCtrl::GetItemPath(HTREEITEM item)
{
	ASSERT( item != NULL );

	CString fullPath;
	HTREEITEM htParent = item;

	if(ITEM_IS_FILE(item))
		fullPath = GetItemName( htParent );
	else
	{
		fullPath = GetItemText( htParent ) + m_SlashChar;
		if( fullPath[0] == _T(' ') )
			fullPath = fullPath.Mid(1);
	}

	while ( (htParent = TreeView_GetParent( m_hWnd, htParent ) ) != 0 )
	{
		fullPath = GetItemText( htParent ) + m_SlashChar + fullPath;
		if( fullPath[0] == _T(' ') )
			fullPath = fullPath.Mid(1);
	}

	// if we are doing local view, we need to check for the
	// C:\ case so that we don't wind up with things like 
	// C:\\dirname\file.ext - which is wrong
	if (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT
	 && TheApp( )->m_ClientRoot.GetLength( ) == 3
	 && fullPath.GetAt(2) == _T('\\')
	 && fullPath.GetAt(3) == _T('\\'))
	{
		// for some reason, this has to be done
		// in multiple statements using a temp
		// variable (called endpart) - as a
		// single line, the compile does the
		// wrong thing and spits out garbage
		CString endpart = fullPath.Mid(4);
		fullPath = fullPath.GetAt(0);
		fullPath += _T(":\\");
		fullPath += endpart;
	}
	return fullPath;
}

/*	_________________________________________________________________

	Like previous routine, but for current selection rather than a given item.
	_________________________________________________________________
*/	

CString CDepotTreeCtrl::GetCurrentItemPath() 
{
	if( GetSelectedCount() )
	{
		TVITEM	tvItem;
		tvItem.state = 0;         
		tvItem.stateMask = TVIS_EXPANDED;         
		tvItem.mask = TVIF_STATE;
		tvItem.hItem = GetSelectedItem(0);
		CString path = GetItemPath(tvItem.hItem);
		if (TreeView_GetItem(m_hWnd, &tvItem) && !(tvItem.state & TVIS_EXPANDED))	// get the item and its expansion state
			path.TrimRight(_T('/'));
		return path;
	}
	return _T("");
}

// Convert a local syntax path into a depot syntax path
// There are several way to call this routine:
//	1) item == 0 && localPath == 0 -> GetItemDepotSyntax(current selection)
//	2) item != 0 && localPath == 0 -> Slowest, GetItemDepotSyntax(for given item)
//	3) item == 0 && localPath != 0 -> Must be a client file, GetItemDepotSyntax(for given file)
//	4) item != 0 && localPath != 0 -> Fastest, but item must be for localPath
CString CDepotTreeCtrl::GetItemDepotSyntax(HTREEITEM item, CString *localPath)
{
	CString depotPath;
	if (!localPath)
	{
		if (!item)
		{
			depotPath = GetCurrentItemPath();
			item = GetSelectedItem(0);
		}
		else
		{
			depotPath = GetItemPath(item);
		}
	}
	else if (!item)
		depotPath = *localPath;

	if( item && ITEM_IS_FILE(item) )
	{
		CP4FileStats *fs= m_FSColl.GetStats(GetLParam(item));
		if( fs->InClientView() )
			return(fs->GetFullDepotPath());
	}

	if (depotPath.IsEmpty())
		depotPath = GetItemPath(item);
	BOOL bAddSl = FALSE;
	if (depotPath.GetAt(depotPath.GetLength()-1) == _T('\\'))
	{
		depotPath.TrimRight(_T('\\'));
		bAddSl = TRUE;
	}
	if (depotPath.FindOneOf(_T("@#%*")) != -1)
	{
		StrBuf b;
		StrBuf f;
		f << CharFromCString(depotPath);
		StrPtr *p = &f;
		StrOps::WildToStr(*p, b);
		depotPath = CharToCString(b.Value());
	}
	CCmd_Where *pCmd1 = new CCmd_Where;
	pCmd1->Init(NULL, RUN_SYNC);
	if ( pCmd1->Run(depotPath) && !pCmd1->GetError() 
		&& pCmd1->GetDepotFiles()->GetCount() )
	{
		depotPath = pCmd1->GetDepotSyntax();
		if (bAddSl)
			depotPath += _T('/');
	}
	else if (bAddSl)
		depotPath += _T('\\');
	delete pCmd1;
	return depotPath;
}

/*
	_________________________________________________________________
*/

HTREEITEM CDepotTreeCtrl::Insert( LPCTSTR text, int imageIndex
							 , LPARAM lParam, HTREEITEM hParent)
{
    CString temp;
#ifdef _DEBUG 	// Asserts to sniff for the deadly QQ bug
	temp = text;
	ASSERT( temp.Find( QQBUG_JOB000458 ) == -1 );
#endif

	// Make sure the parent is now displayed with the '+' button,
	// since the tree control does a "steel trap" after cChildren
	// is set for any node and will no longer attempt to compute
	// the child count for itself. Also make sure we dont have the
	// bailing wire message, g_sTrulyEmptyDir in the parent's name.
	if(hParent != m_Root)
	{
		SetChildCount(hParent, 1);
		temp= GetItemText(hParent);
		int offendingtxt;
		if( (offendingtxt= temp.Find(g_TrulyEmptyDir)) != -1)
			SetItemText(hParent, temp.Left(offendingtxt));
	}

	TV_INSERTSTRUCT tree_insert;
	// If its a folder or depot we are inserting, make sure it has a '+' sign on the button
	if( imageIndex == CP4ViewImageList::VI_FOLDER ||
		imageIndex == CP4ViewImageList::VI_OPENFOLDER ||
		imageIndex == CP4ViewImageList::VI_DEPOT ||
		imageIndex == CP4ViewImageList::VI_REMOTEDEPOT )
	{
		tree_insert.item.cChildren=1;
		tree_insert.item.mask= TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;
	}
	else
		tree_insert.item.mask= TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	if (GET_P4REGPTR()->SortByExtension())
		tree_insert.hInsertAfter = (*text == _T(' ')) ? TVI_SORT : SortItemByExtension(text, hParent);
	else
		tree_insert.hInsertAfter = TVI_SORT;
	tree_insert.hParent = hParent;
	tree_insert.item.iImage = imageIndex;
	tree_insert.item.iSelectedImage = imageIndex;
	tree_insert.item.pszText = const_cast<LPTSTR>(text);
	tree_insert.item.cchTextMax = lstrlen( text );

	//		lparam is used in two different ways:
	//			1. for files, it's the m_ItemCount
	//			2. for subdirs in 98.2, it's either EXPAND_FOLDER
	//				or FOLDER_ALREADY_EXPANDED.
	//				EXPAND_FOLDER is used to trigger a p4 dirs and fstat.
	//
	tree_insert.item.lParam = lParam;   

	return( TreeView_InsertItem( m_hWnd, &tree_insert ) );
}

HTREEITEM CDepotTreeCtrl::SortItemByExtension(LPCTSTR text, HTREEITEM hParent)
{
	HTREEITEM item=TreeView_GetNextItem(m_hWnd, hParent, TVGN_CHILD);
	if (!item)
		return TVI_FIRST;

	CString myText = text;
	int i;
	CString myExt = _T("");
	if ((i = myText.ReverseFind(_T('#'))) > 0)
		myText = myText.Left(i-1);
	if ((i = myText.ReverseFind(_T('.'))) != -1) 
		myExt = myText.Right(myText.GetLength() - i - 1);
	CString itemText;
	CString itemExt;
	HTREEITEM prevItem = TVI_FIRST;
	while(item != NULL)
	{
		itemText= GetItemText(item);
		itemExt = _T("");
		if (itemText.GetAt(0) != _T(' '))
		{
			if ((i = itemText.ReverseFind(_T('#'))) > 0)
				itemText = itemText.Left(i-1);
			if ((i = itemText.ReverseFind(_T('.'))) != -1)
				itemExt = itemText.Right(itemText.GetLength() - i - 1);
			if ((i = itemExt.CompareNoCase(myExt)) > 0)
				return prevItem;
			if (!i && (itemText.CompareNoCase(myText) > 0))
				return prevItem;
		}
		prevItem = item;
		item= TreeView_GetNextSibling(m_hWnd, item);
	}
	return TVI_LAST;
}


HTREEITEM CDepotTreeCtrl::VerifySubdir(CString path, HTREEITEM startItem)
{
	HTREEITEM item=startItem;
	HTREEITEM parent;

	CString temp;
	int start=2;   // Right after '//'
	int last;
	
	while(1)
	{
		// Extract the next subdir name
		temp=path.Mid(start);
		last=temp.Find(_T("/"));
		start+=(last+1);
		if(last ==-1)
			break;  //at end of path
		temp = g_sStupidLeadingBlank + temp.Left(last);
		
		// Search children of current item for matching subdir name
		parent=item;
		item=TreeView_GetChild(m_hWnd, parent);   
		while(item != NULL && GetItemName(item) != temp)
			item= TreeView_GetNextSibling(m_hWnd, item);

		if(item == NULL)  
			// subdir name not found - so add it
			item=Insert ( temp, CP4ViewImageList::VI_FOLDER, (LPARAM) -1, parent);
	}

	return item;
}


/*
	_________________________________________________________________
*/

LRESULT CDepotTreeCtrl::OnP4Error(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


/*
	_________________________________________________________________
*/

LRESULT CDepotTreeCtrl::OnP4Diff(WPARAM wParam, LPARAM lParam)
{
	CCmd_Diff *pCmd= (CCmd_Diff *) wParam;

	if (pCmd->GetSflag() == _T('d') || pCmd->GetSflag() == _T('e'))
		return OnP4Diff_sd_se(wParam, lParam);

	if( !pCmd->GetError() && pCmd->GetDiffRunCount() == 0)
	{
		if (pCmd->GetDiffNbrFiles() == 1)
        {
            CString msg;
            msg.FormatMessage(IDS_CLIENTFILE_s_DOESNOTDIFFER, pCmd->GetDiffFileName());
			AddToStatus(msg, SV_COMPLETION);
        }
		else if (pCmd->GetDiffErrCount() == 0)
			AddToStatus(LoadStringResource(IDS_NONE_OF_THE_SELECTED_CLIENT_FILES_DIFFER), SV_COMPLETION);
	}
	MainFrame()->ClearStatus();         
	delete pCmd;
	return 0;
}


/*
	_________________________________________________________________
*/

LRESULT CDepotTreeCtrl::OnP4Get(WPARAM wParam, LPARAM lParam)
{
	CCmd_Get *pCmd= (CCmd_Get *) wParam;
    ASSERT_KINDOF(CCmd_Get,pCmd);
	BOOL bNeed2Refresh = FALSE;
	BOOL bGet = FALSE;
	BOOL bRecover = FALSE;
	CString txt;
	int i;

	if(!pCmd->GetError())
	{
		CStringList *getlist= pCmd->GetGetList();
		ASSERT_KINDOF(CStringList,getlist);
		CStringList *removelist= pCmd->GetRemoveList();
		ASSERT_KINDOF(CStringList,removelist);

		if(!pCmd->IsWhatIf())
		{
            // Update the trees (unless we've hit max file seeks; then just refresh)
			if ((getlist->GetCount() > MAX_FILESEEKS || removelist->GetCount() > MAX_FILESEEKS)
			 && (!pCmd->IsRunIntegAfterSync() && !pCmd->IsOpenAfterSync() && !pCmd->GetRecover()->GetCount()))
			{
				bNeed2Refresh = TRUE;
			}
			else
			{
				// Temporarily disable redraws for both windows
				SetRedraw(FALSE);
				::SendMessage(m_changeWnd, WM_SETREDRAW, FALSE, 0);

	            // Update the trees
				ProcessGetListResults(WM_P4GET, getlist);
				ProcessGetListResults(WM_P4UNGET, removelist);

				// Re-enable redraws for both windows
				SetRedraw(TRUE);
				::SendMessage(m_changeWnd, WM_SETREDRAW, TRUE, 0);

				// And finally, make sure both windows redraw
				RedrawWindow();
				::RedrawWindow( m_changeWnd, NULL, NULL, RDW_INVALIDATE );
			}

			if(getlist->GetCount() >0)
			{
				txt.FormatMessage(IDS_COPIED_n_FILES_TO_CLIENT, getlist->GetCount());
				if (pCmd->GetNumberAdded())
				{
					CString str;
					str.FormatMessage(IDS_n_WERE_ADDED, pCmd->GetNumberAdded());
					txt += str;
				}
				AddToStatus(txt, SV_COMPLETION);
				if (!bNeed2Refresh)
				{
					for(POSITION pos = getlist->GetHeadPosition(); pos != NULL; )
					{
						if (getlist->GetNext( pos ).Find(_T(" added as ")) >= 0
							&& !pCmd->IsOpeningForEdit())
						{
							// found something new was added - signal to start a refresh
							bNeed2Refresh = TRUE;
							break;
						}
					}
				}
			}
			else
				AddToStatus(LoadStringResource(IDS_SYNC_COPIED_NO_FILES_TO_CLIENT), SV_COMPLETION);
		
			if(removelist->GetCount() >0)
			{
				txt.FormatMessage(IDS_REMOVED_n_FILES_FROM_CLIENT, removelist->GetCount());
				AddToStatus(txt, SV_COMPLETION);
			}
			else
				AddToStatus(LoadStringResource(IDS_REMOVED_NO_FILES_FROM_CLIENT), SV_COMPLETION);
		}
		else
		{
			if(getlist->GetCount() >0)
			{
				txt.FormatMessage(IDS_SYNC_WOULD_COPY_n_FILES_TO_CLIENT, getlist->GetCount());
				if (pCmd->GetNumberAdded())
				{
					CString str;
					str.FormatMessage(IDS_n_WOULD_BE_ADDED, pCmd->GetNumberAdded());
					txt += str;
				}
				AddToStatus(txt, SV_COMPLETION);
			}
			else
				AddToStatus(LoadStringResource(IDS_SYNC_WOULD_COPY_NO_FILES_TO_CLIENT), SV_COMPLETION);
		
			if(removelist->GetCount() >0)
			{
				txt.FormatMessage(IDS_SYNC_WOULD_REMOVE_n_FILES_FROM_CLIENT, removelist->GetCount());
				AddToStatus(txt, SV_COMPLETION);
			}
			else
				AddToStatus(LoadStringResource(IDS_SYNC_WOULD_REMOVE_NO_FILES_FROM_CLIENT), SV_COMPLETION);
		}
		if (((i = pCmd->ReiterateWarnings()) == 0) 
		 || ((pCmd->IsRunIntegAfterSync()) 
		  && (IDYES == AfxMessageBox(IDS_THERE_WERE_WARNINGS_REPORTED_DURING_THE_SYNC, MB_YESNO|MB_ICONQUESTION))))
		{
			if (pCmd->IsRunIntegAfterSync())
			{
				if (OnIntegrate3())
					bNeed2Refresh = FALSE;	// the integration completion routine will do the refresh
			}
			else if (pCmd->IsOpenAfterSync())
			{
				bGet = TRUE;
				if(getlist->GetCount() == 0)
					m_SkipSyncDialog = TRUE;
				// May have to reslect all that were selected when sync command fired.
				// This undoes any fiddling the user might have done while waiting.
				CDWordArray *selset = pCmd->GetSelectionSet();	// get original sel set
				int cnt = selset->GetSize();
				BOOL b = cnt == GetSelectedCount();
				if (b)
				{	// same number in each list; are the lists the same?
					for(int index=-1; ++index < cnt; )
					{
						HTREEITEM item = (HTREEITEM) selset->GetAt(index);
						if (GetSelectedItem(index) != item)
						{
							b = FALSE;	// lists are different - have to reselect
							break;
						}
					}
				}
				if (!b)
				{	// the lists differ; have to reselect
					UnselectAll();
					for(int index=-1; ++index < cnt; )
					{
						HTREEITEM item = (HTREEITEM) selset->GetAt(index);
						SetSelectState(item, TRUE);
					}
				}
			}
			else if (pCmd->GetRecover()->GetCount() > 0)
			{
				m_StringList.RemoveAll();
				CStringList *recoverList = pCmd->GetRecover();
				for(POSITION pos = recoverList->GetHeadPosition(); pos != NULL; )
					m_StringList.AddHead(recoverList->GetNext( pos ));
				bRecover = TRUE;
			}
		}
		else if (i)
		{
			if (pCmd->IsOpenAfterSync())
				AfxMessageBox(IDS_SYNC_WARNINGS_CANTEDIT, MB_ICONSTOP);
			else if (pCmd->GetRecover()->GetCount() > 0)
				AfxMessageBox(IDS_SYNC_WARNINGS_CANTRECOVER, MB_ICONSTOP);
			// We need to do a refresh to update our internal stats and the screen
			bNeed2Refresh = TRUE;
		}
		HWND hwnd = pCmd->GetRevHistWnd();
		if (hwnd)
			::PostMessage(hwnd, WM_UPDATEHAVEREV, 0, pCmd->GetRevReq());
	}
	else if (pCmd->IsRunIntegAfterSync())	// got an error; do we need to re-enable integ dialog?
	{
		EnterCriticalSection(&MainFrame()->CriticalSection); 
		if (m_IntegWizard)
			m_IntegWizard->SendMessage(WM_ENABLEDISABLE, 0, TRUE);
		LeaveCriticalSection(&MainFrame()->CriticalSection);
	}

	MainFrame()->ClearStatus();         
	delete pCmd;
	
	if (bGet)
		OnFiledropEdit();
	else if (bRecover)
		OnFileRecoverProceed();
	else if (bNeed2Refresh)
		OnViewUpdate();
	return 0;
}


/*
	_________________________________________________________________

	wait, p4 refresh was superseded by p4 sync -f -- see too onFilerefresh
	test for a server 98.1 or greater: how? SERVERLEVEL>2?
	_________________________________________________________________
*/

LRESULT CDepotTreeCtrl::OnP4Refresh(WPARAM wParam, LPARAM lParam)
{
	CCmd_Refresh *pCmd= (CCmd_Refresh *) wParam;
	
	if(!pCmd->GetError())
		AddToStatus(LoadStringResource(IDS_FILE_REFRESH_COMPLETED), SV_COMPLETION);

	delete pCmd;
	MainFrame()->ClearStatus();         

	return 0;
}


/*
	_________________________________________________________________
*/

LRESULT CDepotTreeCtrl::OnP4History(WPARAM wParam, LPARAM lParam)
{
	CCmd_History *pCmd= (CCmd_History *) wParam;
	CString txt;

	// if just want rev hist dialog, minimize main window
	if (!TheApp()->m_RevHistPath.IsEmpty())
		MainFrame()->ShowWindow(SW_SHOWMINIMIZED);

	if(!pCmd->GetError())
	{
		// Get the file type for the head rev
		CP4FileStats *fs= pCmd->GetFileStats();
		CString ftype= fs->GetHeadType();
		BOOL isText= fs->IsTextFile();

		CHistory *hist= pCmd->GetHistory();
		if(hist->GetRevisionCount() > 0)
		{
			BOOL enableShowIntegs = pCmd->GetEnableShowIntegs();
			CHistoryDlg *dlg= new CHistoryDlg(this);
			if (pCmd->IsAFile())
			{
				dlg->Init(hist, ftype, isText, m_hWnd, fs->GetHaveRev(), fs->IsMyOpen(),
							pCmd->GetInitialRev(), pCmd->GetInitialName(), 
							pCmd->HaveServerLock() ? pCmd->GetServerKey() : 0);
			}
			else
			{
				dlg->Init(hist, ftype = _T("text"), TRUE, m_hWnd, -1, FALSE,
							pCmd->GetInitialRev(), pCmd->GetInitialName(), 
							pCmd->HaveServerLock() ? pCmd->GetServerKey() : 0);
			}
			dlg->SetEnableShowIntegs(enableShowIntegs);
			dlg->SetCallingWnd(pCmd->GetCallingWnd());
			dlg->SetCallingCommand(pCmd);
			// Display the Rev Hist dialog
			if (!dlg->Create(IDD_HISTORY, this))	// display the description dialog box
			{
				dlg->DestroyWindow();	// some error! clean up
				delete dlg;
			}
		}
		delete fs;
	}
	else
	{
		CString errTxt = pCmd->GetErrorText();
		if ((errTxt.Find(_T("Remote depot 'db.rev' database access failed")) != -1)
		  && GET_P4REGPTR()->GetFetchCompleteHist())
		{
			if(!GET_P4REGPTR()->SetFetchCompleteHist( 0 ) )
				AfxMessageBox( IDS_BAD_REGISTRY,  MB_ICONSTOP );
			else
			{
				TheApp()->StatusAdd(LoadStringResource(IDS_REMOTE_DOWN_TURNING_OFF_INTEGS),SV_WARNING);
				::PostMessage(m_hWnd, WM_COMMAND, ID_FILE_REVISIONHISTORY, 0);
			}
		}
		else if (!TheApp()->m_RevHistPath.IsEmpty())
		{
			CString txt;
			txt.FormatMessage(IDS_CANTRUNREVHIST_s_s, TheApp()->m_RevHistPath, errTxt);
			AfxMessageBox( txt,  MB_ICONSTOP );
		}
	}
	MainFrame()->ClearStatus();
	if(pCmd->GetError())
		::SetFocus(pCmd->GetCallingWnd());
	return 0;
}

LRESULT CDepotTreeCtrl::OnP4EndHistory( WPARAM wParam, LPARAM lParam )
{
	CHistoryDlg *dlg = (CHistoryDlg *)lParam;
	BOOL enableShowIntegs = dlg->GetEnableShowIntegs();
	HWND hRerun = NULL;

	if (dlg->m_Rerun 
		|| (dlg->m_CompleteHist != GET_P4REGPTR()->GetFetchCompleteHist() 
		 && enableShowIntegs))
	{
		hRerun = dlg->GetCallingWnd();
		TheApp()->m_RevHistMore = dlg->m_RevHistCount;
		int n = GET_P4REGPTR()->GetFetchHistCount();
		if (!dlg->m_More && n > 0)
		{
			TheApp()->m_RevHistMore -= n;
			if (TheApp()->m_RevHistMore < 0)
				TheApp()->m_RevHistMore = 0;
		}
		if (enableShowIntegs && !GET_P4REGPTR()->SetFetchCompleteHist( dlg->m_CompleteHist ) )
			AfxMessageBox( IDS_BAD_REGISTRY,  MB_ICONSTOP );
	}
	CCmd_History *pCmd = dlg->GetCallingCommand();
	delete pCmd;
	dlg->DestroyWindow();

	if (hRerun)
		::PostMessage(hRerun, WM_COMMAND, 
					  hRerun == MainFrame()->m_hWnd ? ID_FILE_REVISIONHISTORYCONT 
					                                : ID_FILE_REVISIONHISTORY, 0);
	else if (!TheApp()->m_RevHistPath.IsEmpty())
		::PostMessage(MainFrame()->m_hWnd, WM_COMMAND, ID_APP_EXIT, 0);

	return TRUE;
}

/*
	_________________________________________________________________
*/

LRESULT CDepotTreeCtrl::OnP4Integ(WPARAM wParam, LPARAM lParam)
{
	BOOL bRunInteg2 = FALSE;
	BOOL chainedCommands= FALSE;
	CCmd_Integrate2 *pCmd= (CCmd_Integrate2 *) wParam;

	// Did anything actually get integrated (and this is not a preview)?
	EnterCriticalSection(&MainFrame()->CriticalSection); 
	if (pCmd->GetActualCount() && !pCmd->IsPreview() && !pCmd->HitFatalError())
	{
		if (m_IntegWizard)	// if the dialog is still up, close it
		{
			m_IntegWizard->DestroyWindow();	 // deletes m_IntegWizard
			m_IntegWizard = 0;
			MainFrame()->SetModelessUp(FALSE);
			m_StringListSv.RemoveAll();		// saved list now no longer useful
		}
	}
	else if (m_IntegWizard)
		m_IntegWizard->SendMessage(WM_ENABLEDISABLE, 0, TRUE);
	LeaveCriticalSection(&MainFrame()->CriticalSection);

	if(!pCmd->GetError() && (!pCmd->IsRename() || pCmd->IsOK2rename()))
	{
		CObList *list= pCmd->GetTargetList();
		ASSERT_KINDOF(CObList,list);
		if (list->GetCount() > MAX_FILESEEKS && !pCmd->HitMaxFileSeeks())
		{
			POSITION pos= list->GetHeadPosition();
			while(pos != NULL)
			{
				// delete the filestats info
				CP4FileStats *stats = (CP4FileStats *) list->GetNext(pos);
				ASSERT_KINDOF(CP4FileStats, stats);
				delete stats;
			}
			pCmd->SetHitMaxFileSeeks(TRUE);
		}

		if(!pCmd->IsPreview())
		{
			CString text;
			text.FormatMessage(IDS_OPENED_n_FILES_FOR_INTEGRATE, pCmd->GetActualCount());
			AddToStatus(text, SV_COMPLETION);
		}

		if(!pCmd->IsPreview() && pCmd->HitMaxFileSeeks())
		{
			// Too much was added for seeking out each file for an att update
			// to be efficient.  CCmd_Integrate2 thus did not create a target
			// list for us.  Just start a full update.
			int key= pCmd->GetServerKey();
			chainedCommands= TRUE;
			MainFrame()->UpdateDepotandChangeViews(REDRILL, key);
		}
		else if(pCmd->IsPreview())
		{
			// The list is empty for previews
			ASSERT( list->GetCount() == 0 );
			AddToStatus(LoadStringResource(IDS_INTEGRATE_PREVIEW_COMPLETED), SV_COMPLETION);
			if (pCmd->IsRunSyncAfterPreview())
				bRunInteg2 = TRUE;
		}
		else
		{
			 // Temporarily disable redraws for both windows
            SetRedraw(FALSE);
            ::SendMessage(m_changeWnd, WM_SETREDRAW, FALSE, 0);

			POSITION pos= list->GetHeadPosition();
            InitFindItem();
			while(pos != NULL)
			{
				// Get the filestats info
				CP4FileStats *stats = (CP4FileStats *) list->GetNext(pos);
				ASSERT_KINDOF(CP4FileStats, stats);

				// Find the item
				HTREEITEM item;
				item=FindItem(stats->GetDepotDir(), stats->GetDepotFilename(), FALSE);
				if(item==NULL)
					// It is quite common for get or unget to reference a file
					// that is not in our tree, due to client view or deleted
					// file viewing options
					XTRACE(_T("OnP4Integ() item not found %s\n"), stats->GetFullDepotPath());
				else
				{
					// Update its properties
					int index=GetLParam(item);
					CP4FileStats *fs=m_FSColl.GetStats(index);
					
					// Note that the open action could be a delete, since integ
					// can result in file deletes as well as branch and integ
					// open actions
					fs->SetOpenAction(stats->GetMyOpenAction(), FALSE);
					fs->SetHeadType(stats->GetHeadType());
					fs->SetType(stats->GetType());
					fs->SetOpenChangeNum(stats->GetOpenChangeNum());
							
					// Update the image in this window
					SetImage(item, TheApp()->GetFileImageIndex(fs));
				}

				// Update changes window whether or not we found file here
				::SendMessage(m_changeWnd, WM_UPDATEOPEN, (WPARAM) stats, P4INTEG);

				// Delete the filestats object
				delete stats;

			} // while

			   // Re-enable redraws for both windows
            SetRedraw(TRUE);
            ::SendMessage(m_changeWnd, WM_SETREDRAW, TRUE, 0);

            // And finally, make sure both windows redraw
	        RedrawWindow();
            ::RedrawWindow( m_changeWnd, NULL, NULL, RDW_INVALIDATE );
		}
		
	} // !error
	else if (pCmd->IsRename() && !pCmd->IsOK2rename())
	{
		int chgnbr = pCmd->GetTargetChange();
		CString txt;
		txt.FormatMessage(IDS_RENAME_NOT_POSSIBLE, pCmd->GetErrorText());
		AddToStatus(txt, SV_ERROR);
		if (chgnbr && (chgnbr == pCmd->GetNewChangeNbr()))
		{
			CString msg;
			msg.FormatMessage(IDS_DELETE_NEW_CHGLIST, txt, chgnbr);
			CCmd_Delete *pCmd2= new CCmd_Delete;
			txt.Format(_T("%d"), chgnbr);
			pCmd2->Init( m_changeWnd, RUN_ASYNC, HOLD_LOCK, pCmd->GetServerKey() );
			pCmd2->SetIgnoreActiveItem(TRUE);
			if( pCmd2->Run( P4CHANGE_DEL, txt ) )
			{
				txt.FormatMessage(IDS_DELETING_n, chgnbr);
				MainFrame()->UpdateStatus(txt);
			}	
			else
				delete pCmd2;
		}
	}

	if(!chainedCommands || MainFrame()->IsQuitting())
		pCmd->ReleaseServerLock();

	if (bRunInteg2 && !MainFrame()->IsQuitting())
	{
		EnterCriticalSection(&MainFrame()->CriticalSection); 
		if (m_IntegWizard)
			m_IntegWizard->SendMessage(WM_ENABLEDISABLE, 0, FALSE);
		LeaveCriticalSection(&MainFrame()->CriticalSection);

		OnIntegrate2(pCmd->GetPreviewTargetList());
	}

	delete pCmd;

	return 0;
}

LRESULT CDepotTreeCtrl::OnP4Add(WPARAM wParam, LPARAM lParam)
{
	BOOL chainedCommands= FALSE;
	CCmd_Add *pCmd= (CCmd_Add *) wParam;
	ASSERT_KINDOF(CCmd_Add, pCmd);

	if(!pCmd->GetError() && pCmd->HitMaxFileSeeks() )
	{
		// Too much was added for seeking out each file for an att update
		// to be efficient.  Just start a full update.
		int key= pCmd->GetServerKey();
		chainedCommands= TRUE;
		MainFrame()->UpdateDepotandChangeViews(REDRILL, key);
	}
	else if(!pCmd->GetError())
	{
		// Temporarily disable redraws
		SetRedraw(FALSE);

		// Get the results list, and update any files that are
		// present in the DepotTree
		HTREEITEM item;
		CObList const *list=pCmd->GetList();

		InitFindItem();
		for(POSITION pos= list->GetHeadPosition(); pos!=NULL; )
		{
			CP4FileStats *fs= (CP4FileStats *) list->GetNext(pos);

			// Try to find the item
			item=FindItem(fs->GetDepotDir(), fs->GetDepotFilename(), FALSE);
			if(item==NULL)
				// It is not unusual to NOT find the file, since the user
				// could have initiated the command by clicking a folder
				// that had not yet been explored
				XTRACE(_T("OnP4Recover() item not found %s\n"), fs->GetFullDepotPath());
			else
			{
				// Update its properties in accordance with the command.  Remember
				// that CCmd_ListOpStat will return a sparse set of information.
				// In general only the file name and revision are present.

				CP4FileStats *treefs= m_FSColl.GetStats(GetLParam(item));
						
				treefs->SetNotInDepot(TRUE);
				treefs->SetOpenAction(fs->GetMyOpenAction(), FALSE);
				treefs->SetHaveRev(fs->GetHaveRev());
				treefs->SetOpenChangeNum(fs->GetOpenChangeNum());

				// Update the image in this window
				SetImage(item, TheApp()->GetFileImageIndex(treefs));
			
			}// if found
			
		}//for each file
	
		 // Re-enable redraw
        SetRedraw(TRUE);
        RedrawWindow();

         // And finally, re-send the WM_P4ADD to the changelist pane
	    ::SendMessage(m_changeWnd, WM_P4ADD, wParam, lParam);
    	MainFrame()->ClearStatus();         

		// The server lock release and deletion of the command are
		// both handled by the changes window
		return 0;
	}
	else
    	MainFrame()->ClearStatus();         
	

	if( !chainedCommands || MainFrame()->IsQuitting() )
		pCmd->ReleaseServerLock();
	delete pCmd;
		
	return 0;
}


LRESULT CDepotTreeCtrl::OnP4Recover(WPARAM wParam, LPARAM lParam)
{
	BOOL chainedCommands= FALSE;
	CCmd_Add *pCmd= (CCmd_Add *) wParam;
	ASSERT_KINDOF(CCmd_Add, pCmd);

	if(!pCmd->GetError())
	{
		CString text;
		text.FormatMessage(IDS_RECOVERED_n_FILES, pCmd->GetAddedFileCount());
		AddToStatus(text,SV_COMPLETION);
	}
	
	if(!pCmd->GetError() && pCmd->HitMaxFileSeeks() )
	{
		// Too much was added for seeking out each file for an att update
		// to be efficient.  Just start a full update.
		int key= pCmd->GetServerKey();
		chainedCommands= TRUE;
		MainFrame()->UpdateDepotandChangeViews(REDRILL, key);
	}
	else if(!pCmd->GetError())
	{
		// Temporarily disable redraws
		SetRedraw(FALSE);

		// Get the results list, and update any files that are
		// present in the DepotTree
		HTREEITEM item;
		CObList const *list=pCmd->GetList();

		InitFindItem();
		for(POSITION pos= list->GetHeadPosition(); pos!=NULL; )
		{
			CP4FileStats *fs= (CP4FileStats *) list->GetNext(pos);

			// Try to find the item
			item=FindItem(fs->GetDepotDir(), fs->GetDepotFilename(), FALSE);
			if(item==NULL)
				// It is not unusual to NOT find the file, since the user
				// could have initiated the command by clicking a folder
				// that had not yet been explored
				XTRACE(_T("OnP4Recover() item not found %s\n"), fs->GetFullDepotPath());
			else
			{
				// Update its properties in accordance with the command.  Remember
				// that CCmd_ListOpStat will return a sparse set of information.
				// In general only the file name and revision are present, though the
				// rev is not present for LOCK, UNLOCK.  For EDIT and DELETE we will
				// have additional info as returned by a call to ostat.

				CP4FileStats *treefs= m_FSColl.GetStats(GetLParam(item));
						
				treefs->SetNotInDepot(fs->IsNotInDepot());
				treefs->SetOpenAction(fs->GetMyOpenAction(), FALSE);
				if (fs->GetHaveRev() != 0)
					treefs->SetHaveRev(fs->GetHaveRev());
				treefs->SetOpenChangeNum(fs->GetOpenChangeNum());

				SetImage(item, TheApp()->GetFileImageIndex(treefs));
			
			}// if found
			
		}//for each file
	
		 // Re-enable redraw
        SetRedraw(TRUE);
        RedrawWindow();

         // And finally, re-send the WM_P4ADD to the changelist pane
	    ::SendMessage(m_changeWnd, WM_P4ADD, wParam, lParam);
    	MainFrame()->ClearStatus();         

		// The server lock release and deletion of the command are
		// both handled by the changes window
		return 0;
	}
	else
    	MainFrame()->ClearStatus();         
	

	if( !chainedCommands || MainFrame()->IsQuitting() )
		pCmd->ReleaseServerLock();
	delete pCmd;
		
	return 0;
}


/*
	_________________________________________________________________
*/

LRESULT CDepotTreeCtrl::OnP4ListOp(WPARAM wParam, LPARAM lParam)
{
	BOOL chainedCommands= FALSE;
	int  iRedoOpenedFilter = 0;
	CCmd_ListOpStat *pCmd= (CCmd_ListOpStat *) wParam;
	
	if(!pCmd->GetError())
	{
		iRedoOpenedFilter = pCmd->GetRedoOpenedFilter();
		if( pCmd->GetOpenAfterDelete() )
		{
			int selectedChange = pCmd->GetSelectedChange();
			int key= pCmd->GetServerKey();
			chainedCommands= TRUE;
			CCmd_ListOpStat *pCmd2= new CCmd_ListOpStat;
			pCmd2->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key );

			if( pCmd2->Run( &m_StringList2, P4EDIT, selectedChange ) )
				MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUEST_OPEN_EDIT) );
			else
				delete pCmd2;
		}
		else if( pCmd->HitMaxFileSeeks() || pCmd->HitMaxFileStats())
		{
			// Too much was added (or the chglist is too big)
			// for seeking out each file for an att update
			// to be efficient.  Just start a full update.
			int key= pCmd->GetServerKey();
			chainedCommands= TRUE;
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
		}
		else
		{
			 // Temporarily disable redraws for both windows
			SetRedraw(FALSE);
			::SendMessage(m_changeWnd, WM_SETREDRAW, FALSE, 0);

			switch(pCmd->GetCommand())
			{
			case P4REVERT:
			case P4VIRTREVERT:
			case P4REVERTUNCHG:
				chainedCommands = OnP4RevertFile(pCmd);
				if (chainedCommands)
					iRedoOpenedFilter = 0;
				break;
			case P4ADD:
			case P4EDIT:
			case P4DELETE:
			case P4LOCK:
			case P4UNLOCK:
				ProcessStatListResults(wParam);
				break;
			default:
				ASSERT(0);
			}						   

			// Re-enable redraws for both windows
			SetRedraw(TRUE);
			::SendMessage(m_changeWnd, WM_SETREDRAW, TRUE, 0);

			 // And finally, make sure both windows redraw
			RedrawWindow();
			::RedrawWindow( m_changeWnd, NULL, NULL, RDW_INVALIDATE );
		}
	}

	pCmd->DeleteStatList();

	if( !chainedCommands || MainFrame()->IsQuitting() )
		pCmd->ReleaseServerLock();

	BOOL bOutputError = pCmd->GetOutputErrFlag();
	delete pCmd;
	
	if (iRedoOpenedFilter)
		OnRedoOpendList((WPARAM)iRedoOpenedFilter, 0);
	else if (bOutputError)
		::PostMessage(m_hWnd, WM_COMMAND, ID_VIEW_UPDATE, 0);
	return 0;
}


/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::ProcessStatListResults(WPARAM wParam)
{
	CCmd_ListOpStat *pCmd= (CCmd_ListOpStat *) wParam;
	ASSERT_KINDOF(CCmd_ListOpStat, pCmd);
	
	// Get the results list, and update any files that are
	// present in the DepotTree
	HTREEITEM item;
	CObList *list=pCmd->GetStatList();

    InitFindItem();
	for(POSITION pos= list->GetHeadPosition(); pos!=NULL; )
	{
		CP4FileStats *fs= (CP4FileStats *) list->GetNext(pos);

		// Try to find the item
		item=FindItem(fs->GetDepotDir(), fs->GetDepotFilename(), FALSE);
		if(item==NULL)
			// It is not unusual to NOT find the file, since the user
			// could have initiated the command by clicking a folder
			// that had not yet been explored
			XTRACE(_T("ProcessListResults() item not found %s\n"), fs->GetFullDepotPath());
		else
		{
			// Update its properties in accordance with the command.  Remember
			// that CCmd_ListOpStat will return a sparse set of information.
			// In general only the file name and revision are present, though the
			// rev is not present for LOCK, UNLOCK.  For EDIT and DELETE we will
			// have additional info as returned by a call to ostat.

			CP4FileStats *treefs= m_FSColl.GetStats(GetLParam(item));
					
			switch(pCmd->GetCommand())
			{
			case P4ADD:
				treefs->SetNotInDepot(fs->IsNotInDepot());
			case P4EDIT:
			case P4DELETE:
				treefs->SetOpenAction(fs->GetMyOpenAction(), FALSE);
				treefs->SetHaveRev(fs->GetHaveRev());
				treefs->SetOpenChangeNum(fs->GetOpenChangeNum());
				break;
						
			case P4LOCK:
				treefs->SetLocked(TRUE, FALSE);
				break;
			case P4UNLOCK:
				treefs->SetLocked(FALSE, FALSE);
				break;
			default:
				ASSERT(0);
			}

			// Update the image in this window
			if (treefs->GetHeadRev() != fs->GetHeadRev() && fs->GetHeadRev())
			{
				treefs->SetHeadRev(fs->GetHeadRev());
				SetItemText(item, fs->GetFormattedFilename(GET_P4REGPTR()->ShowFileType()));
			}
			SetImage(item, TheApp()->GetFileImageIndex(treefs));
			
		}// if found

		// Always notify the changes window
		::SendMessage(m_changeWnd, WM_UPDATEOPEN, (WPARAM) fs, (LPARAM) pCmd->GetCommand());
		
		MainFrame()->ClearStatus();
	
	}//for each file
}

void CDepotTreeCtrl::ProcessGetListResults(UINT command, CStringList *list)
{
	CString path, name, listRow, fname, fRev;
	long rev;
	DWORD index;
	HTREEITEM item;
	int lastSlash;
	POSITION pos;
	CP4FileStats *fs;
		
	pos=list->GetHeadPosition();
    InitFindItem();
	for(int i=0; i < list->GetCount(); i++)
	{
		BOOL differentRev=FALSE;

		listRow=fname=list->GetNext(pos);

		int separator, pound;
		BOOL rowError=FALSE;

		//////////////
		// Separate the filename from the action description
		// For all operations but lock and unlock, this amounts to a
		// quick search for '#'.  In the case of the lock commands, 
		// there is no revision number, so search for the action text itself
	
		pound= listRow.Find(_T('#'));   
		if(pound == -1)
		{
			rowError=TRUE;
			break;
		}

		separator= pound+1;
		int len= listRow.GetLength();
		for( ; separator < len ; separator++)
		{
			if(listRow[separator]==_T(' ') && listRow[separator+1]==_T('-') && listRow[separator+2]==_T(' '))
			break;
		}
		if(separator==len)
			rowError=TRUE;

		if(rowError)
		{ 
			// doesnt look like a valid row, report it and skip it
			ASSERT(0); 
			AddToStatus(listRow, SV_WARNING);
			continue; 
		}	
					
		fRev=fname.Mid(pound+1);
		rev=_ttol(fRev);
		
		fname=fname.Left(separator);    // full name w/ revision
		lastSlash=fname.ReverseFind(_T('/')); 
		path=fname.Left(lastSlash+1);	// path with trailing /
		name=fname.Mid(lastSlash+1);	// file name with revision

		// Find the item
		item=FindItem(path, name, FALSE);
		if(item==NULL)
		{
			// If the user did a GET from a folder, we wont be finding
			// files unless we already explored the folder
			XTRACE(_T("ProcessGetListResults() item not found %s\n"), listRow);
			fs= (CP4FileStats *) new CP4FileStats;
			fs->SetDepotPath(fname.Left(pound));  
			fs->SetHaveRev(rev);

			// Send the filename to the changes window for update
			::SendMessage(m_changeWnd, WM_SETUNRESOLVED, (WPARAM) fs, 0); 
			delete fs;
		}
		else
		{
			// Update its properties
			index=GetLParam(item);
			fs=m_FSColl.GetStats(index);
			// We need to do this because depot paths are not stored after
			// files are added to the tree
			fs->SetDepotPath(fname.Left(pound));  

			CString digest = _T("");
			switch(command)
			{
			case WM_P4GET:
				if(rev != fs->GetHaveRev())
					differentRev=TRUE;
				fs->SetHaveRev(rev);
				TheApp()->localDigest(fs, &digest);
				fs->SetDigest(&digest);
				break;
			case WM_P4UNGET:
				fs->SetHaveRev(0);
				fs->SetDigest(&digest);
				break;
			default:
				ASSERT(0);
			}

			// Rule: Only show deleted file if user has opted to see deleted files
			// Exception: If the head rev is deleted and user has a rev < head, then
			//            we always show the file
			if(command == WM_P4UNGET && fs->GetHeadAction() == F_DELETE && 
				!GET_P4REGPTR()->ShowDeleted() && fs->GetHaveRev() == 0 ) 
			{
						DeleteLeaf(item);
			}
			else
			{
				// Update the image in this window
				SetImage(item, TheApp()->GetFileImageIndex(fs));
			
				// A get will require update of the file reve and possibly file type info
				SetItemText(item, fs->GetFormattedFilename(GET_P4REGPTR()->ShowFileType()));
				if(differentRev && fs->IsMyOpen())
					// Send the filename to the changes window for update
					::SendMessage(m_changeWnd, WM_SETUNRESOLVED, (WPARAM) fs, 0); 
			}
		} // if item found
	} // for each item
	
	MainFrame()->ClearStatus();
}



/*
	_________________________________________________________________
*/

LRESULT CDepotTreeCtrl::OnP4UpdateOpen(WPARAM wParam, LPARAM lParam)
{
	XTRACE(_T("OnP4UpdateOpen() wParam=%ld lParam=%ld\n"), wParam, lParam);

	// Changes view sends this after reverting a file
	CString *fileName= (CString *) wParam;

	int lastSlash=fileName->ReverseFind(_T('/'));
	ASSERT(lastSlash != -1);

	CString path=fileName->Left(lastSlash+1);
	CString file=fileName->Mid(lastSlash+1);
	
    InitFindItem();
	HTREEITEM item=FindItem(path, file, FALSE);

	if(item == NULL)
		// F_ADD and some F_INTEG files wont be in this view! 
		return 0;
		
	DWORD index=GetLParam(item);	
	CP4FileStats *fs=m_FSColl.GetStats(index);
	
	if(fs->GetHeadRev() || GET_P4REGPTR()->ShowEntireDepot() == SDF_LOCALTREE)
	{
		fs->SetLocked(FALSE, FALSE);
		fs->SetOpenAction(0, FALSE);
		fs->SetOpenChangeNum(0);
		if (!fs->GetHeadRev() && fs->IsNotInDepot())
			fs->SetHaveRev(0);
		SetImage(item, TheApp()->GetFileImageIndex(fs));
	}
	else
	{
		// A stranded file was reverted, and will no longer be in the 
		// client view of the depot
		ASSERT(GET_P4REGPTR()->ShowEntireDepot() != SDF_DEPOT);
	}
		
	return 0;
}


/*
	_________________________________________________________________
*/

BOOL CDepotTreeCtrl::OnP4RevertFile(CCmd_ListOpStat *pCmd)
{
	CStringList *list = pCmd->GetList();
	CStringList reverted;
	CString listRow, fname;
	POSITION pos;
	
	pos=list->GetHeadPosition();
	for(int i=0; i < list->GetCount(); i++)
	{
		listRow=list->GetNext(pos);
		if(listRow.Find(_T("reverted")) != -1 || listRow.Find(_T("was add, abandoned")) != -1)
		{
			fname= listRow.Left(listRow.ReverseFind(_T('#')));
			reverted.AddHead(fname);
					
			// Send the message to update our own window
			SendMessage(WM_UPDATEOPEN, (WPARAM) &fname, 0);
		}
	}
	
	// Make sure changes window gets updated
	if(reverted.GetCount() > 0)
	{
		::SendMessage(m_changeWnd, WM_REVERTLIST, (WPARAM) &reverted, 0);
		::RedrawWindow(m_changeWnd, NULL, NULL, RDW_INVALIDATE);
	}

	if (pCmd->GetRevertUnchgAfter())
	{
		CCmd_ListOpStat *pCmd2= new CCmd_ListOpStat;
		pCmd2->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, pCmd->GetServerKey() );
		pCmd2->SetNbrChgedFilesReverted(list->GetCount());
		pCmd2->SetRedoOpenedFilter(pCmd->GetRedoOpenedFilter());
		if( pCmd2->Run( &m_StringList2, P4REVERTUNCHG ) )
		{
			MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_REVERT) );
			return TRUE;
		}
		else
		{
			delete pCmd2;
			MainFrame()->UpdateDepotandChangeViews(REDRILL, pCmd->GetServerKey());
		}
	}
	else
		MainFrame()->ClearStatus();

	return FALSE;
}


/*
	_________________________________________________________________

	Start the refresh of the changes window, called at the end of
	a CStat operation.  Can be called directly from OnP4Depots for 
	a full refresh, otherwise called after files and folders are fetched
	_________________________________________________________________
*/

void CDepotTreeCtrl::StartChangeWndUpdate(int key)
{
	XTRACE(_T("StartChangeWndUpdate()\n"));

	if( !m_ClearedChangeWnd )
		::SendMessage(m_changeWnd, WM_INITTREE, 0, 0);

	MainFrame()->UpdateStatus(LoadStringResource(IDS_CHECKING_PENDING_CHANGES));

	// And start the refresh of the changes tree
	CString clistr = GET_P4REGPTR()->GetP4Client();
	CCmd_Changes *pCmd= new CCmd_Changes;
	pCmd->Init( m_changeWnd, RUN_ASYNC, HOLD_LOCK, key);
	if( !pCmd->Run( PENDING_CHANGES, 
			(GET_SERVERLEVEL() >= 19 && GET_P4REGPTR()->GetUseLongChglistDesc() > 31) ? 2 : 0, 
			NULL, 0, FALSE, NULL,
			GET_P4REGPTR()->GetEnablePendingChgsOtherClients() 
				&& !GET_P4REGPTR()->FilterPendChgsByMyClient() ? NULL : &clistr ) )
	{
		RELEASE_SERVER_LOCK(key);
		MainFrame()->SetLastUpdateTime(UPDATE_FAILED);
		MainFrame()->ClearStatus();
		delete pCmd;
	}
}



/*
	_________________________________________________________________

	We just got a list of dirs and files, because a tree node is expanding.  So put the
    dirs into the tree and call fstat to get the files at the same level in
    the tree.
	_________________________________________________________________
*/

LRESULT CDepotTreeCtrl::OnP4ExpandTree ( WPARAM wParam, LPARAM lParam )
{
	XTRACE(_T("OnP4Dirs() wParam=%ld lParam=%ld\n"), wParam, lParam);

	CCmd_DirStat *pCmd= ( CCmd_DirStat * ) wParam;
	ASSERT_KINDOF(CCmd_DirStat, pCmd);

	// Data used for triggering an auto expand 
	// of a folder that contains only a single subfolder
	HTREEITEM lastDir = NULL;
	int nbrDirs = 0;
	int children = 0;

	// Set the context
	m_LastPathItem= pCmd->GetItemRef();
	m_LastPath = pCmd->GetTextRef();
	m_UpdateType= UPDATE_EXPAND;

	if (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
		m_LastPath.Replace(_T('/'), _T('\\'));

	// We passed in a 'new' stringlist, so delete it
	delete pCmd->GetSpecList();

	MainFrame()->ClearStatus();

	if(pCmd->GetError( ) )
	{
		MainFrame()->SetLastUpdateTime(UPDATE_FAILED);
		if( m_LastPathItem != NULL )
		{
			SetLParam( m_LastPathItem, EXPAND_FOLDER );
			SetChildCount( m_LastPathItem, 1 );
		}
		m_ExpandDepotContinue = FALSE;
	}
	else
	{
		POSITION pos;
		SetRedraw(FALSE);

		HTREEITEM iNode = NULL;

		// First insert all directories
		CStringList *dirs= pCmd->GetDirs();
		pos= dirs->GetHeadPosition();
		while(pos != NULL)
		{
			// Get the dir
			CString dir= dirs->GetNext(pos) + m_SlashChar;
			if (InsertDir( dir ))
			{
				iNode = TreeView_GetParent( m_hWnd, m_LastPathItem );
				lastDir = m_LastPathItem;
				children++;
				nbrDirs++;
			}
		}

		// Then insert all files
		CObList const *files= pCmd->GetFiles();
		ASSERT_KINDOF(CObList, files);

		BOOL bSbyE = FALSE;
		if (GET_P4REGPTR()->SortByExtension()
		 && (files->GetCount() > _ttoi(GET_P4REGPTR()->GetExtSortMax())))
		{
			GET_P4REGPTR()->SetSortByExtension( FALSE );
			CString txt;
			txt.FormatMessage(IDS_TOO_MANY_TO_SORT_BYEXT_n, files->GetCount());
			AddToStatus( txt, SV_WARNING );
			bSbyE = TRUE;
		}

		pos= files->GetHeadPosition();
		while(pos != NULL)
		{
			// Get the filestats
			CP4FileStats *stats= (CP4FileStats *) files->GetNext(pos);
			ASSERT_KINDOF(CP4FileStats, stats);

			// Just another successfully retrieved row - do NOT delete pCmd
			if(stats->GetHeadAction() != F_DELETE || GET_P4REGPTR()->ShowDeleted() || stats->GetHaveRev() != 0 )
			{
				InsertFromFstat(stats);
				iNode= m_LastPathItem;
				children++;
			}
			else
				delete stats;
		}

		if (bSbyE)
			GET_P4REGPTR()->SetSortByExtension( TRUE );

		if( children )
		{
			if( iNode != NULL )
			{
				if (TreeView_Expand( m_hWnd, iNode, TVE_EXPAND ))
					SetLParam( iNode, FOLDER_ALREADY_EXPANDED);
			}
			else
				ASSERT(0); // Why didnt iNode get set?
		}
		else
		{
			// Leave the node ready to try expanding
			if( m_LastPathItem != NULL )
			{
				SetLParam( m_LastPathItem, EXPAND_FOLDER );
				CString txt= GetItemText( m_LastPathItem );
				if( txt.Find( g_TrulyEmptyDir ) == -1 )
				{
					txt+= g_TrulyEmptyDir;
					SetItemText( m_LastPathItem, txt );
				}
				SetChildCount( m_LastPathItem, 0 );
			}
			else
				ASSERT(0);
		}

		SetRedraw(TRUE);
		RedrawWindow();
	} // if error

    delete pCmd;

	// if we are in the process of expanding down a branch of the depot tree
	// to a given path (m_ExpandPath), now that we have finished expanding a
	// node, we need to restart ExpandDepotString() with a FALSE to indicate
	// that it should continue from where it left off.
	if (m_ExpandDepotContinue)
	{
		 ExpandDepotString(m_ExpandPath, FALSE);
	}
	// else if there is only a single subfolder in this folder, expand it
	else if (nbrDirs == children && nbrDirs == 1 && GET_P4REGPTR()->AutoTreeExpand())
	{
		if (!TreeView_Expand( m_hWnd, lastDir, TVE_EXPAND ))
			ApplySelectAtts(GetSelectAtts());
	}
	else
	{
		ApplySelectAtts(GetSelectAtts());
	}
	return 0;
}



/*
	_________________________________________________________________
*/

LRESULT CDepotTreeCtrl::OnP4DirStat(WPARAM wParam, LPARAM lParam)
{
	XTRACE(_T("OnP4DirStat() wParam=%ld lParam=%ld\n"), wParam, lParam);
    CCmd_DirStat *pCmd;

    if(lParam == 0)   // completion
	{
        pCmd= (CCmd_DirStat *) wParam;
	    ASSERT_KINDOF(CCmd_DirStat, pCmd);

		if(pCmd->GetError() || MainFrame()->IsQuitting())
		{
			pCmd->ReleaseServerLock();
			MainFrame()->SetLastUpdateTime(UPDATE_FAILED);
			MainFrame()->ClearStatus();
			delete pCmd;
			return 0;
		}
    }
    else
    {
        // This command does not return intermediate results
        ASSERT(0);
        return 0;
    }

	int key= pCmd->GetServerKey( );
	BOOL bNoExpand = TRUE;
    POSITION pos;
    SetRedraw(FALSE);

    // Set the context
    m_LastPathItem= TVI_ROOT;
    m_LastPath = g_sSlashes;
	m_UpdateType= UPDATE_REDRILL;

    // First insert all directories
    CStringList *dirs= pCmd->GetDirs();
    pos= dirs->GetHeadPosition();
	while(pos != NULL && m_DepotCount)
	{
		// Get the dir
		CString dir= dirs->GetNext(pos) + m_SlashChar;
		InsertDir( dir );
    }

    // Then insert all files
    CObList const *files= pCmd->GetFiles();
    ASSERT_KINDOF(CObList, files);
    		
	BOOL bSbyE = FALSE;
	if (GET_P4REGPTR()->SortByExtension()
	 && (files->GetCount() > _ttoi(GET_P4REGPTR()->GetExtSortMax())))
	{
		GET_P4REGPTR()->SetSortByExtension( FALSE );
		CString txt;
		txt.FormatMessage(IDS_TOO_MANY_TO_SORT_BYEXT_n, files->GetCount());
		AddToStatus( txt, SV_WARNING );
		bSbyE = TRUE;
	}

	pos= files->GetHeadPosition();
	while(pos != NULL && m_DepotCount)
	{
		// Get the filestats
		CP4FileStats *stats= (CP4FileStats *) files->GetNext(pos);
		ASSERT_KINDOF(CP4FileStats, stats);

		// Just another successfully retrieved row - do NOT delete pCmd
		if(stats->GetHeadAction() != F_DELETE || GET_P4REGPTR()->ShowDeleted() || stats->GetHaveRev() != 0 )
		{
			InsertFromFstat(stats);
			if( m_LastPathItem != NULL && m_LastPathItem != TVI_ROOT)
			{
				if (GetLParam( m_LastPathItem) != FOLDER_ALREADY_EXPANDED)
				{
					XTRACE(_T("OnP4DirStat() marking %s expanded\n"), 
							   m_LastPathItem != TVI_ROOT ? GetItemPath(m_LastPathItem) : _T("ROOT"));
					SetLParam( m_LastPathItem, FOLDER_ALREADY_EXPANDED);
				}
			}
		}
		else
			delete stats;
	} 

	if (bSbyE)
		GET_P4REGPTR()->SetSortByExtension( TRUE );
	   
    // Then expand all previously expanded nodes that still exist
    pos= m_ExpandedNodeList.GetHeadPosition();
    while(pos != NULL && m_DepotCount)
	{
        int commonLength;
		CString nodeName= m_ExpandedNodeList.GetNext(pos);
		if (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
		{
			if ((nodeName.GetLength() == 2) && (nodeName.GetAt(1) == _T(':')))
				nodeName += _T('\\');
			m_LastPathItem = TreeView_GetRoot(m_hWnd);
			m_LastPath = TheApp()->m_ClientRoot;
			m_LastPath.TrimRight(_T("\\"));	/* to handle c:\ */
			m_LastPath += _T('\\');
			commonLength = m_LastPath.GetLength();
		}
		else
		{
	        m_LastPathItem= TVI_ROOT;
			m_LastPath = g_sSlashes;
			commonLength = 2;
		}
		if (nodeName.GetAt(1) == _T(':'))
			nodeName.Replace(_T('/'), _T('\\'));
		else
			ReplaceMBCS(nodeName, _T('\\'), _T('/'));
		if (((m_SlashChar == _T('/'))  && (nodeName.GetAt(1) == _T(':')))
		 || ((m_SlashChar == _T('\\')) && (nodeName.GetAt(0) == _T('/'))))
		{
			// the node to expand syntax doesn't match our tree syntax now
			// so just bail after setting 'bNoExpand' so that the reselection
			// of any previous selections will know that it must expand nodes
			bNoExpand = FALSE;
			continue;
		}

        FindParentDirectory( nodeName, commonLength );
		if( m_LastPathItem != NULL && m_LastPathItem != TVI_ROOT)
        {
            XTRACE(_T("OnP4DirStat() expanding=%s\n"), nodeName);
			SetLParam( m_LastPathItem, FOLDER_ALREADY_EXPANDED);
            if(!TreeView_Expand( m_hWnd, m_LastPathItem, TVE_EXPAND ))
			{
				XTRACE(_T("OnP4DirStat() expanding failed\n"));
				SetLParam( m_LastPathItem, EXPAND_FOLDER);
			}
        }
	}

    // Then, see if the previous first visible node is in the tree
    // and if so, scroll it into view
    HTREEITEM firstItem= NULL;

    int len= m_FirstVisibleNodeText.GetLength();
    if( len > 0 )
    {
        if( m_FirstVisibleNodeText[len-1] == m_SlashChar )
        {
            // It's a depot or folder, so initialize m_LastPath and m_LastPathItem
            // and then try to find the parent folder
            m_LastPathItem = m_Root;
            m_LastPath= g_sSlashes;
            int commonLength= m_LastPath.GetLength();
            FindParentDirectory( m_FirstVisibleNodeText, commonLength );

            if( Compare( m_LastPath, m_FirstVisibleNodeText ) == 0 )
                firstItem= m_LastPathItem;
        }
        else
        {
            // It's a file, so split apart the path and filename and then
            // try a FindItem() call
            CString path= m_FirstVisibleNodeText.Left(ReverseFindMBCS(m_FirstVisibleNodeText, m_SlashChar)+1);
            CString name= m_FirstVisibleNodeText.Mid(ReverseFindMBCS(m_FirstVisibleNodeText, m_SlashChar)+1);
			
			InitFindItem();
            firstItem= FindItem( path, name, FALSE );
        }
    }

    if( firstItem != NULL )
        ScrollToFirstItem( firstItem );

	// Now if there were any items selected, reslect them
	if  (!m_SavedSelectionSet.IsEmpty())
	{
		int i;
		int count = m_SavedSelectionSet.GetCount();
		CView* pView = MainFrame()->GetActiveView ();
		HTREEITEM item = NULL;
		HTREEITEM itemPrv = NULL;
		CDWordArray treeItems;
		TCHAR   lastSlashChr = m_SavedSelectionSet.GetTail().GetAt(0) == _T('/') ? _T('/') : _T('\\');
		int     lastSlashCur = -2;
		int     lastSlashPrv = -2;
		CString lastSlashStr = "";
		CString prevPath = "";
		do
		{
			BOOL bQuick;
			CString itemPath;
			itemPath = m_SavedSelectionSet.GetHead();
			m_SavedSelectionSet.RemoveHead();

			// Do a quick check to see if we are trying to find
			// it in the same folder - if so, set a flag
			lastSlashCur = ReverseFindMBCS(itemPath, lastSlashChr);
			if (lastSlashPrv == lastSlashCur
			 && itemPath.Left(lastSlashCur) == lastSlashStr)
			{
				bQuick = itemPrv != NULL;
			}
			else
			{
				bQuick = FALSE;
				if (lastSlashCur > 0)
				{
					lastSlashStr = itemPath.Left(lastSlashCur);
					lastSlashPrv = lastSlashCur;
				}
			}

			// Now clear away any trailing ...s
			if (itemPath.Right(3) == _T("..."))
			{
				if (itemPath.Right(4) == _T("/..."))
					itemPath = itemPath.Left(itemPath.GetLength() - 4);
				else if (itemPath.Right(4) == _T("\\..."))
					itemPath = itemPath.Left(itemPath.GetLength() - 4);
			}

			// Can we use the quick method?
			if (bQuick)
			{
				// look for it amongst our siblings
				BOOL bUp = itemPath.CompareNoCase(prevPath) < 0;
				if ((itemPrv = FindDepotSibling(itemPath, itemPrv, bUp)) != NULL)
				{
					treeItems.Add((DWORD)itemPrv);
					prevPath = itemPath;
					continue;
				}
			}

			// Go find it in the depot
			ExpandDepotString(itemPath, TRUE, bNoExpand, key, TRUE);
			if (count == 1)									// If only one thing was selected
				break;										//	whatever we found is good enough
			if (!GetSelectedCount())						// If nothing got selected
			{												//	we don't want to persue this
				ASSERT(0);									//	or the GetSelectedItem() will
				continue;									//	return a NULL
			}
			item = GetSelectedItem(0);
			CString itemText = GetItemText(item);			// Yields last element of found path
			if ((i = itemText.ReverseFind(_T('#'))) != -1)	// If there is a # after the item name
				itemText = itemText.Left(i);				//	grab only the actual name
			itemText.TrimLeft();
			itemText.TrimRight();
			TCHAR slashChar = itemPath.GetAt(0) == _T('/') ? _T('/') : _T('\\');
			CString lastElem;
			if ((i = ReverseFindMBCS(itemPath, slashChar)) != -1)	// If there is a / in the full depot path
				lastElem = itemPath.Mid(i+1);						//	grab only the last element
			else
				lastElem = itemPath;
			lastElem.TrimLeft();
			lastElem.TrimRight();
			if (lastElem == itemText)		 // Did we actually find the thing we were looking for?
			{
				treeItems.Add((DWORD)item ); //  remember it cuz future calls to ExpandDepotString() will deselect it
				itemPrv  = item;
				prevPath = itemPath;
			}
		} while (!m_SavedSelectionSet.IsEmpty());

		// Get the update of the changes window underway before we set all the selections
		// because some code in StartChangeWndUpdate() causes the selections to be reset
		StartChangeWndUpdate( key );
		if (count > 1)
		{
			UnselectAll();
			for (i = treeItems.GetSize(); i--; )
			{
				item = (HTREEITEM)treeItems.GetAt(i);
				SetSelectState( item, TRUE );
			}
			// Make sure selection set is properly displayed
			SetAppearance(FALSE, TRUE, FALSE);
		}
		else if (GetSelectedCount())
		{
			item = GetSelectedItem(0);
		}
	    if( item != NULL )
	        TreeView_EnsureVisible( m_hWnd, item );
		MainFrame()->SetActiveView(pView, TRUE);
	}
	else
	{
		// Finally, get the update of the changes window underway
		StartChangeWndUpdate( key );
	}

	SetRedraw(TRUE);
	MainFrame()->ClearStatus();
	delete pCmd;

	return 0;
}


/*
	_________________________________________________________________
*/

LRESULT CDepotTreeCtrl::OnP4FStat(WPARAM wParam, LPARAM lParam)
{
	XTRACE(_T("OnP4FStat() wParam=%ld lParam=%ld\n"), wParam, lParam);
    CCmd_Fstat *pCmd;

    if(lParam == 0)   // completion
	{
        pCmd= (CCmd_Fstat *) wParam;
	    ASSERT_KINDOF(CCmd_Fstat, pCmd);

		if(pCmd->GetError() || MainFrame()->IsQuitting())
		{
			pCmd->ReleaseServerLock();
			MainFrame()->SetLastUpdateTime(UPDATE_FAILED);
			MainFrame()->ClearStatus();
			delete pCmd;
			return 0;
		}
   
             	
		// We only run fstat for a full update against a 97.3 server
		ASSERT( m_UpdateType == UPDATE_FULL && GET_SERVERLEVEL() < 4 );
		
        int key= pCmd->GetServerKey( );
		StartChangeWndUpdate( key );
		
		MainFrame()->ClearStatus();
		delete pCmd;
		return 0;
	}
	else
	{
        // For postings of CP4FileStats lists, we need the list 
        // as well as a pointer to the command, so a primitive
        // wrapper is used
        CFstatWrapper *pWrap= (CFstatWrapper *) wParam;
        pCmd= (CCmd_Fstat *) pWrap->pCmd;
	    ASSERT_KINDOF(CCmd_Fstat, pCmd);
		CObList *list= (CObList *) pWrap->pList;
		ASSERT_KINDOF(CObList, list);

        // Get the context from the command
        m_LastPathItem= pCmd->GetItemRef();
        m_LastPath= pCmd->GetTextRef();
        m_UpdateType= pCmd->GetUpdateType();
		
		POSITION pos= list->GetHeadPosition();

        SetRedraw(FALSE);
        InitFindItem();
		while(pos != NULL)
		{
			// Get the filestats and insert it
			CP4FileStats *stats= (CP4FileStats *) list->GetNext(pos);
			ASSERT_KINDOF(CP4FileStats, stats);
		
			// Just another successfully retrieved row - do NOT delete pCmd
			InsertFromFstat(stats);

		} // while row batch not done

        SetRedraw(TRUE);

		delete list;
        delete pWrap;  // do NOT delete pCmd, since it's still running!
		return 0;
	} // a batch of rows, we'll be called again so dont delete pCmd
}


/*
	_________________________________________________________________
*/


// Delete a leaf from the tree, making sure childless parent(s) are not left behind
void CDepotTreeCtrl::DeleteLeaf(HTREEITEM item)
{
	XTRACE(_T("DeleteLeaf()\n"));

	ASSERT(item != NULL);
	ASSERT(item != m_Root);

	HTREEITEM parent=TreeView_GetParent(m_hWnd, item);
	HTREEITEM child;

	// First delete the item number from the filestats obj, so 
    // tree-related functions don't attempt to monkey with a 
    // non-existent tree node

    
    if( ITEM_IS_FILE(item) )
    {
        CP4FileStats *fs= (CP4FileStats *) m_FSColl.GetStats( GetLParam(item) );
        ASSERT_KINDOF(CP4FileStats, fs);
        fs->SetUserParam( NULL );
    }
    else
        ASSERT(0);

	TreeView_DeleteItem(m_hWnd, item);

	// Make sure childless parent directori(es) are not left behind
	while(parent != NULL && TreeView_GetChild(m_hWnd, parent) == NULL )
	{
		child=parent;
		parent=TreeView_GetParent(m_hWnd, child);
		TreeView_DeleteItem(m_hWnd, child);
	}	
}


BOOL CDepotTreeCtrl::FindDirInDepotFilter(LPCTSTR path)
{
	long lgth = lstrlen(path);
	CString str;
	for( POSITION pos= m_DepotFilterList.GetHeadPosition(); pos != NULL; )
	{
		str = m_DepotFilterList.GetNext(pos);
		if (str.GetLength() >= lgth)
		{
			str = str.Left(lgth);
			if (Compare(str, path) == 0)
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CDepotTreeCtrl::InsertDir( CString &path )
{
	if (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
	{
		path.TrimRight(_T('/'));
		if (m_LastPathItem == TVI_ROOT)
		{
		    m_LastPathItem = TreeView_GetRoot(m_hWnd);
			m_LastPath = TheApp()->m_ClientRoot;
		}
		TrimRightMBCS(m_LastPath, _T("\\/"));
		m_LastPath += _T('\\');
	}

	XTRACE(_T("InsertDir() path=%s\n"), path);

	int commonLength= nCommon(path, m_LastPath);

	// back up the tree to the last common node
	int end;
	TCHAR slashbuf[2];
	slashbuf[0] = m_SlashChar;
	slashbuf[1] = _T('\0');
	while(m_LastPath.GetLength() > commonLength)
	{
		if (m_LastPath == CString(slashbuf))
			break;
	    m_LastPathItem= TreeView_GetParent(m_hWnd, m_LastPathItem);
		TrimRightMBCS(m_LastPath, slashbuf);
		end = ReverseFindMBCS(m_LastPath, m_SlashChar);
		m_LastPath=m_LastPath.Left(end+1);
        commonLength= nCommon(path, m_LastPath);
	}

	ASSERT( commonLength < path.GetLength() || path == m_LastPath); 

	// If we are filtering the depot, look for the Dir in the filter list
	// Note: have to do this after m_LastPath is set correctly
	if (m_FilterDepot && !FindDirInDepotFilter(path))
		return FALSE;

	// May add to commonLength if folders found.  Will also remove
	// g_TrulyEmptyDir text if found, since presumably it is now
	// a parent directory
	FindParentDirectory(path, commonLength);

    // The search for parent directory should at least find a depot node:
    if(commonLength < path.GetLength())
	{
        // Add subdirs as required
		while( path.GetLength() > m_LastPath.GetLength() )
		{
			// Directories are prefixed w/ a space so they sort to the top
			// Yes, a grim fix, but better than putting all sort info into lParam
			// and using SortChildrenCB() 
			CString temp= path.Mid(m_LastPath.GetLength());
            if( FindMBCS(temp, m_SlashChar) > 0 ) 
			    temp=temp.Left(FindMBCS(temp, m_SlashChar));

            XTRACE(_T("Insert dir: path=%s m_LastPath=%s\n"), path, m_LastPath);

            // The parent of the folder being added clearly has been explored
            SetLParam( m_LastPathItem, FOLDER_ALREADY_EXPANDED);

			if(m_LastPathItem==TVI_ROOT || m_LastPathItem==NULL)
				m_LastPathItem=Insert(_T("//")+temp, CP4ViewImageList::VI_DEPOT, 
                                           EXPAND_FOLDER, TVI_ROOT);
			else
				m_LastPathItem=Insert(_T(" ")+temp, CP4ViewImageList::VI_FOLDER, 
                                           EXPAND_FOLDER, m_LastPathItem);
                
            m_LastPath += temp;
	    	m_LastPath += m_SlashChar;
         } //while
    } // if commonlength < path.GetLength
	return TRUE;
}


BOOL CDepotTreeCtrl::FindFileInDepotFilter(LPCTSTR path)
{
	for( POSITION pos= m_DepotFilterList.GetHeadPosition(); pos != NULL; )
	{
		if (Compare(m_DepotFilterList.GetNext(pos),path) == 0)
			return TRUE;
	}
	return FALSE;
}

// This routine assumes m_LastPath is valid
LRESULT CDepotTreeCtrl::InsertFromFstat( CP4FileStats *stats )
{
    // Under 98.2 API, insertion of files under unexplored folders is avoided, so
    // the folders can be properly explored later.  Under any api, insertion of 
    // files is skipped when the file is deleted and showdeleted is not true.
    BOOL skipFileInsert=FALSE;

	CString temp;
    CString path;

	// full depot paths that are the empty string mean ingnore this record
	if (!stats->GetFullDepotPath()[0])
	{
		skipFileInsert=TRUE;
		goto AfterPath;
	}
	else
	{
		XTRACE(_T("InsertFromFstat() path=%s file=%s\n"), stats->GetDepotDir(), stats->GetDepotFilename());
		XTRACE(_T("InsertFromFstat() deleting=%d\n"), (stats->GetHeadAction()==F_DELETE));
	}

	// Rule: Only show deleted file if user has opted to see deleted files
	// Exception: If the head rev is deleted and user has a rev < head, then
	//            we always show the file
	if (stats->GetHeadAction() == F_DELETE && !GET_P4REGPTR()->ShowDeleted()
		&& stats->GetHaveRev() == 0 ) 
	{
		skipFileInsert=TRUE;
		goto AfterPath;
	}

	// if we are only showing files not in the depot
	// skip any file with a head action
	if (MainFrame()->m_ShowOnlyNotInDepot && stats->GetHeadAction())
	{
		skipFileInsert=TRUE;
		goto AfterPath;
	}

	// If we are filtering the depot, look for the file in the filter list
	if (m_FilterDepot 
	 && !FindFileInDepotFilter(m_SlashChar == _T('/') ? stats->GetFullDepotPath() 
													  : stats->GetFullClientPath()))
	{
		skipFileInsert=TRUE;
		goto AfterPath;
	}

	// Go up the tree from m_LastPath as required to find a common point, and
	// set m_LastParentItem to point to that node.  Then add subdirs as required,
    // except that if m_UpdateType== UPDATE_INC_982API we will add no more than
    // one node deep.

    path = GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT 
		 ? stats->GetClientDir() : stats->GetDepotDir();
		
	if(Compare(path, m_LastPath)!=0)
	{
		if (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
		{
			if (m_LastPathItem == TVI_ROOT)
			{
				m_LastPathItem = TreeView_GetRoot(m_hWnd);
				m_LastPath = TheApp()->m_ClientRoot;
			}
			TrimRightMBCS(m_LastPath, _T("\\/"));
			m_LastPath += _T('\\');
		}

		int commonLength= nCommon(path, m_LastPath);

		// back up the tree to the last common node
		int end;
		while(m_LastPath.GetLength() > commonLength)
		{
			m_LastPathItem= TreeView_GetParent(m_hWnd,m_LastPathItem);
			m_LastPath = m_LastPath.Left(m_LastPath.GetLength()-1);
			end = ReverseFindMBCS(m_LastPath, m_SlashChar);
			ASSERT(end != -1);
			m_LastPath=m_LastPath.Left(end+1);
		}

        commonLength= m_LastPath.GetLength();

		if(commonLength < path.GetLength())
			// May add to commonLength if folders found.  Will also remove
			// g_TrulyEmptyDir text if found, since presumably it is now
			// a parent directory
			FindParentDirectory(path, commonLength);

        if(commonLength < path.GetLength())
		{
			// Add subdirs as required
			while( path.GetLength() > m_LastPath.GetLength() )
			{
				// Directories are prefixed w/ a space so they sort to the top
				// Yes, a grim fix, but better than putting all sort info into lParam
				// and using SortChildrenCB() 
				temp=path.Mid(m_LastPath.GetLength());
				temp=temp.Left(FindMBCS(temp, m_SlashChar));

                XTRACE(_T("Insert dir: path=%s m_LastPath=%s\n"), path, m_LastPath);

				if(m_LastPathItem==TVI_ROOT || m_LastPathItem==NULL)
					m_LastPathItem=Insert(_T("//")+temp, CP4ViewImageList::VI_DEPOT, 
                                            EXPAND_FOLDER, TVI_ROOT);
				else
					m_LastPathItem=Insert(_T(" ")+temp, CP4ViewImageList::VI_FOLDER, 
                                            EXPAND_FOLDER, m_LastPathItem);
                
                m_LastPath+=temp;
				m_LastPath+=m_SlashChar;
             
            } //while
        } // if commonlength < path.GetLength
    } // if path mismatch

AfterPath:
    if( skipFileInsert )
    {
        delete stats;
    }
    else
    {
	    //		Add the file. make sure it lives under a subdirectory
	    //
	    m_FSColl.SetStats( m_ItemCount, stats );

	    HTREEITEM newItem = Insert( stats->GetFormattedFilename( GET_P4REGPTR()->ShowFileType( )),
		                    TheApp( )->GetFileImageIndex( stats ), 	m_ItemCount, m_LastPathItem );

        m_ItemCount++;
	    stats->SetUserParam( (LPARAM) newItem );

	    if ( ! NEW_DEPOT_LISTING && m_ItemCount == 1 )
		    TreeView_Expand( m_hWnd, TreeView_GetRoot( m_hWnd ), TVE_EXPAND );

        #ifdef _DEBUG
	        // Asserts to sniff for the deadly QQ bug
	        CString temp2(GetItemText(newItem));
	        ASSERT(temp.Find( QQBUG_JOB000458 ) == -1);
        #endif
    }
	return 0;
}


void CDepotTreeCtrl::FindParentDirectory(CString path, int& commonLength)
{
	// We already backed up the tree till the path was only as long as commonLength
	// Now try to drill down for the rest of the path, since the folders may be
	// present in the tree.  The Compare() function will consider whether the server
	// is or isnt case sensitive.

	HTREEITEM item;

	while( path.GetLength() > m_LastPath.GetLength() )
	{
        // skip over chars till we hit a slash or the end of path
		int i = path.Find(m_SlashChar, commonLength);
		if(i == -1)
			i = path.GetLength();

		CString dirname= path.Mid(commonLength, i-commonLength);

        // Note that Win95 will bite it if you call TreeView_GetChild at the root.  Must
        // be some sort of 16 bit overflow inside brittleware.
        if( m_LastPathItem == m_Root)
		    item=TreeView_GetRoot(m_hWnd);
	    else
		    item=TreeView_GetChild(m_hWnd, m_LastPathItem);

		int skipAmt = (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT) 
					? TheApp( )->m_ClientRoot.GetLength( ) : 2;
		while(item != NULL)
		{
			CString txt;
            if( IsDepot(item) )
                // Skip over leading "//"
                txt=GetItemText(item).Mid(skipAmt);  
            else
                // Skip over the leading space
                txt=GetItemText(item).Mid(1);

			if(Compare(dirname,txt) == 0 || Compare(dirname+g_TrulyEmptyDir,txt)==0)
			{
				// Then store the dir name and tree item
				m_LastPathItem=item;
				m_LastPath+= dirname + m_SlashChar ;

				// Set new commonlength and get out of inner loop
				commonLength=m_LastPath.GetLength();
				break;  
			}
			item= TreeView_GetNextSibling(m_hWnd, item);
		}
                
        // We didnt get a match at this subdir level, so
        // the search is over
        if(item==NULL)
			break;
    } // while
    return;
}


/*
	_________________________________________________________________

	VIEWUPDATE STEP I:  
	Maxchanges is called to find out what the head change number of
	the server is.
	
	the first overloaded function is the protected member, and called when 
	the user hits F5 or the toolbar button.  it will attempt to do a full
    refresh of explored folders, re-expanding folders as required.
    
    the second overload is for when the mainframe calls us directly, and 
    supports incremental refresh for use after a change is submitted, etc
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnViewUpdate() 
{
	XTRACE(_T("OnViewUpdate()\n"));

    if( GET_SERVERLEVEL() == 3)
        OnViewUpdate( NO_REDRILL );
    else
	    OnViewUpdate( REDRILL );
}

void CDepotTreeCtrl::OnViewUpdate( BOOL redrill, int key /*=0*/ )
{
	if (MainFrame()->IsQuitting())
	{
		if (key)
			RELEASE_SERVER_LOCK(key);
		return;
	}

	// Make sure that autopolling gets turned back on if it was off
	MainFrame()->ResumeAutoPoll();

	// Save our selection set
	AssembleStringList( &m_SavedSelectionSet, FALSE, FALSE, TRUE );

	// Don't change the slach char before the selection set is assembled!
	TCHAR oldSlashChar = m_SlashChar;
	m_SlashChar = (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT) ? _T('\\') : _T('/');

	// If we are filtering the depot and changed the port or client,
	// we may need to clear the filter
	if (m_FilterDepot)
	{
		if (m_DepotFilterPort != GET_P4REGPTR()->GetP4Port())
			ClearDepotFilter(FALSE);
		else if ((m_DepotFilterClient != GET_P4REGPTR()->GetP4Client())
			  && (m_DepotFilterType == DFT_MYOPENED))
			ClearDepotFilter(FALSE);
	}

	if (m_SlashChar != oldSlashChar)
	{
		if (m_FilterDepot && (m_DepotFilterType == DFT_LIST))
			ConvertDepotFilterList(key);
	}

	m_ExpandDepotContinue = FALSE;
	if( redrill && GET_SERVERLEVEL() > 3 )
	{
		XTRACE(_T("OnViewUpdate - redrill\n"));
		m_UpdateType= UPDATE_REDRILL;
	}
	else
	{
		XTRACE(_T("OnViewUpdate - full\n"));
		m_UpdateType= UPDATE_FULL;
	}

    m_RunningUpdate= FALSE;
    m_ClearedChangeWnd= FALSE;

	if( m_UpdateType == UPDATE_REDRILL )
	{
		// Only 98.2 and higher servers support 'p4 dirs' to redrill
		ASSERT( GET_SERVERLEVEL() > 3 );
		RecordTreeExploration();
	}
	else
	{
		//	For full updates, we always clear any cached server info
		//  since connect parms may have been changed
		CLEAR_SERVERINFO();
	}

	// Clear both panes now, so if there is an error along the way
	// we dont leave rubble on the screen
	Clear();
	CString client= GET_P4REGPTR()->GetP4Client();
	if( !client.IsEmpty() )
	{
		m_ClearedChangeWnd= TRUE;
		::SendMessage(m_changeWnd, WM_INITTREE, 0, 0);
	}

	// Every refresh starts with fetching the depot list
	CCmd_Depots *pCmd = new CCmd_Depots;
	    
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key );
	BOOL ret = pCmd->Run();

	if( ret )
	{
		MainFrame()->UpdateStatus(LoadStringResource(IDS_REQUESTING_DEPOTS));
	}
	else
	{
		if( key )
			RELEASE_SERVER_LOCK( key );        
		delete pCmd;
		MainFrame()->ClearStatus();
	}
}


/*
	_________________________________________________________________
	
	VIEWUPDATE STEP II:
	We now have a list of local depots and a list of remote depots.  We also
	know what serverlevel we are running against.

	Put the depots into the tree and, depending upon serverlevel and update type,
	proceed to fetch more files and folders or jump to the pending changelist pane
	update  
	_________________________________________________________________
*/

LRESULT CDepotTreeCtrl::OnP4Depots ( WPARAM wParam, LPARAM lParam )
{
	XTRACE(_T("OnP4Depots() wParam=%ld lParam=%ld\n"), wParam, lParam);

	// Clear the menu if required adter deleting the pCmd
	BOOL bRmvMRUPcu = FALSE;

	// Re-enable folder expansion, now that we have the root of the
	// tree on hand
	m_RunningUpdate= FALSE;
    
	CCmd_Depots *pCmd= ( CCmd_Depots * ) wParam;
	ASSERT_KINDOF(CCmd_Depots, pCmd);

    // Before testing for an error, try to get the mainframe caption set
    MainFrame()->UpdateCaption( ) ;	

	if(pCmd->GetError( ) || MainFrame()->IsQuitting())
	{
		pCmd->ReleaseServerLock();
		MainFrame()->SetLastUpdateTime(UPDATE_FAILED);
		MainFrame()->ClearStatus();
	
        // If PWD is busted, try the update again
        if(GET_PWD_ERROR() && !pCmd->PWDDlgCancelled())
        {
            delete pCmd;
            OnViewUpdate(NO_REDRILL);
            return 0;
        }
        
		if (!LoadStringResource(IDS_UNABLE_TO_GET_CLIENT_DESCRIPTION).Compare(pCmd->GetErrorText()))
			bRmvMRUPcu = TRUE;	// remove the PCU because it has total failure

		goto depotend;
	}
	else
	{
		if (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
		{
#ifdef	_DEBUG
			CString root = TheApp()->m_ClientRoot;
#endif
			if (TheApp()->m_ClientRoot.GetAt(1) != _T(':'))
			{
				int key;
				if( (key = pCmd->GetServerKey( )) != 0 )
					RELEASE_SERVER_LOCK( key );        
				::PostMessage(MainFrame()->m_hWnd, WM_COMMAND, ID_VIEW_CLIENTVIEW, 0);
				AddToStatus( LoadStringResource(IDS_NOLOCALFORNULLROOT), SV_WARNING );
				goto depotend;
			}
		}

        CString base, folderName;
        POSITION pos;

		m_LocalDepotList.RemoveAll();
		CStringList *list= pCmd->GetLocalDepotList();
	    for ( pos= list->GetHeadPosition(); pos != NULL; )
	    {
		    folderName= list->GetNext(pos);
			m_LocalDepotList.AddTail(folderName);
            m_LastPathItem= m_Root;
            m_LastPath= g_sSlashes;
		    if( !FindFolder(folderName) )
		    {
			    // Change the m_LastPathItem so an error dialog
			    // doesnt spoil the fun when Cmd_dirstat finishes
			    m_LastPathItem = Insert( folderName, 
				    CP4ViewImageList::VI_DEPOT, EXPAND_FOLDER, TVI_ROOT );
			    m_DepotCount++;
                m_LastPath= folderName + _T("/");
            }
		}

		m_RemoteDepotList.RemoveAll();
		list= pCmd->GetRemoteDepotList();
		for ( pos= list->GetHeadPosition(); pos != NULL; )
	    {
		    folderName= list->GetNext(pos);
			m_RemoteDepotList.AddTail(folderName);
			// we ignore remote depots for local view other than keeping the
			// above list of them so that we can know what menu items to add/enable
			if (GET_P4REGPTR()->ShowEntireDepot() > SDF_DEPOT)
				continue;
            m_LastPathItem= m_Root;
            m_LastPath= g_sSlashes;
		    if( !FindFolder(folderName) )
		    {
			    // Change the m_LastPathItem so an error dialog
			    // doesnt spoil the fun when Cmd_dirstat finishes
			    m_LastPathItem = Insert( folderName, 
				    CP4ViewImageList::VI_REMOTEDEPOT, EXPAND_FOLDER, TVI_ROOT );
			    m_DepotCount++;
                m_LastPath= folderName + _T("/");
            }
		}

		if( m_DepotCount == 0 && GET_SERVERLEVEL() > 3 )
		{
            //	If no depots were found, there is nothing to display in the depot pane.  
            //  Try to provide a usefull message to the user, and proceed to the
            //  update of the pending changelists pane.
            int msgId;

			if( GET_P4REGPTR()->ShowEntireDepot() > SDF_DEPOT )
			{
				if( GET_P4REGPTR()->ShowDeleted() )
				{
					if( GET_P4REGPTR()->ShowEntireDepot() == SDF_DEPOT )
						msgId = IDS_THERE_ARE_NO_FILES_IN_THE_DEPOT;
					else
						msgId = IDS_THERE_ARE_NO_FILES_IN_YOUR_CLIENT_VIEW;
				}
				else
				{
					if( GET_P4REGPTR()->ShowEntireDepot() == SDF_DEPOT )
						msgId = IDS_THERE_ARE_NO_UNDELETED_FILES_IN_THE_DEPOT;
					else
						msgId = IDS_THERE_ARE_NO_UNDELETED_FILES_IN_YOUR_CLIENT_VIEW;
				}
			}
			else
			{
				if( GET_P4REGPTR()->ShowEntireDepot() == SDF_LOCALTREE )
					msgId = IDS_THERE_ARE_NO_FILES_UNDER_YOUR_CLIENT_ROOT;
				else
					msgId = IDS_THERE_ARE_NO_PERFORCE_FILES_UNDER_YOUR_CLIENT_ROOT;
			}
            
            AddToStatus( LoadStringResource(msgId), SV_WARNING ); 
            AfxMessageBox( msgId, MB_ICONINFORMATION );
		}
		
        // Make sure the window redraws
        SetRedraw(TRUE);
        RedrawWindow();

        int key= pCmd->GetServerKey( );

		MainFrame()->ResumeAutoPoll();	// just to clear the flag - we know we can poll now
        switch( m_UpdateType )
        {
		case UPDATE_FULL:
			if( NEW_DEPOT_LISTING )
				StartChangeWndUpdate( key);
			else
				RunCStat( key );
		    break;
        case UPDATE_REDRILL:
			RunRedrill( key );
		    break;
        default:
            ASSERT(0);
		}
       
	} // if error
depotend:
    delete pCmd;
	if (bRmvMRUPcu)	// Must delete pCmd before removing MRU PCU
	{
		CMenu *pMenu= MainFrame()->GetMenu();
		if (pMenu != NULL)
		{
			LPCTSTR p = GET_P4REGPTR()->GetP4Port();
			LPCTSTR c = GET_P4REGPTR()->GetP4Client();
			LPCTSTR u = GET_P4REGPTR()->GetP4User();
			if (p && *p && c && *c && u && *u)
			{
				LPCTSTR q;
				for (q = p; *++q; )
				{
					if (*q == _T(' '))
						break;
				}
				CString txt;
				txt.Format(*q ? _T("\"%s\" %s %s") : _T("%s %s %s"), p, c, u);
				GET_P4REGPTR()->RmvMRUPcu( txt );
			}
			MainFrame()->loadMRUPcuMenuItems(pMenu);
		}
	}
	return 0;
}


void CDepotTreeCtrl::RunCStat(int key)
{
	// VIEWUPDATE STEP III 
	
	ASSERT( m_UpdateType == UPDATE_FULL && GET_SERVERLEVEL() < 4 );

	CString spec;
	// The filespec varies for client-view-only display
	if(GET_P4REGPTR()->ShowEntireDepot() == SDF_DEPOT)
		spec=_T("//...");
	else
		spec.Format(_T("//%s/..."), GET_P4REGPTR()->GetP4Client());

	CCmd_Fstat *pCmd= new CCmd_Fstat;

    // Store our context, so when this command returns we know where we are
    pCmd->SetItemRef(m_LastPathItem);
    pCmd->SetTextRef(m_LastPath);
    pCmd->SetUpdateType(m_UpdateType);

	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key);
    if( pCmd->Run( FALSE, LPCTSTR(spec), GET_P4REGPTR()->ShowEntireDepot() == SDF_DEPOT ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_UPDATING) );
	else
	{
		RELEASE_SERVER_LOCK(key);
		delete pCmd;
	}
}


void CDepotTreeCtrl::RunRedrill( int key )
{
	// VIEWUPDATE STEP III, for UPDATE_REDRILL 
	
	ASSERT( m_UpdateType == UPDATE_REDRILL );

	// If this is local syntax, 
	// we have to have the root as one of our directories
	// or we won't find all the dirs and files
	// We also want to remove any entires in the list
	// that are not under our root
	if (GET_P4REGPTR()->ShowEntireDepot() > SDF_DEPOT)
	{
		BOOL found = FALSE;
		CString rootbare = TheApp()->m_ClientRoot;
		rootbare.TrimRight(_T("\\"));				/* to handle c:\ */
		rootbare += _T("\\");
		int barelgth = rootbare.GetLength();
		CString rootdir = rootbare + _T("*");
		if ( m_StringList.GetCount() )
		{
			CString str;
			CString tmp;
			CStringList templist;
			for (POSITION pos = m_StringList.GetHeadPosition(); pos != NULL; )
			{
				str = m_StringList.GetNext(pos);
				tmp = str.Left(barelgth);
				if (!tmp.CompareNoCase(rootbare))	// under new root?
					templist.AddHead(str);			//	yes - add to list
			}
			m_StringList.RemoveAll();
			if ( templist.GetCount() )
			{
				for (POSITION pos = templist.GetHeadPosition(); pos != NULL; )
				{
					str = templist.GetNext(pos);
					if ( rootdir == str )			// is this the root?
						found = TRUE;				//	yes - remember we found it
					m_StringList.AddHead(str);
				}
			}
		}
		if (!found)
			m_StringList.AddHead( rootdir );
	}

    if( m_StringList.GetCount() == 0 )
    {
		// No need to run dirstat, but need to update changes window anyway
		StartChangeWndUpdate(key);
	}
    else  
    {
        CCmd_DirStat *pCmd= new CCmd_DirStat;

		pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key);
		if( pCmd->Run( &m_StringList, GET_P4REGPTR()->ShowEntireDepot() == SDF_DEPOT) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_UPDATING));
		else
		{
			RELEASE_SERVER_LOCK(key);
			delete pCmd;
		}
	}
}


/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFileDiffhead() 
{
	HTREEITEM item=GetLastSelection();  
	
	if (item == NULL)
		ASSERT(0);
	else
	{
		CString itemStr=GetItemPath(item);
		if (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
		{
			CP4FileStats *fs = m_FSColl.GetStats(GetLParam(item));
			itemStr = fs->GetFullDepotPath();
		}
		m_StringList.RemoveAll();
		m_StringList.AddHead(LPCTSTR(itemStr));

		CCmd_Diff *pCmd= new CCmd_Diff;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		if( pCmd->Run( &m_StringList, IsOpened(item) ? NULL : _T("-f") ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_DIFFING_FILE) );
		else
			delete pCmd;
	}	
}

/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFileDiff2() 
{
	int i;
	HTREEITEM item1 = GetSelectedItem(0);
	HTREEITEM item2 = (GetSelectedCount() > 1) ? GetSelectedItem(1) : item1;
	i = item1 == item2 ? 0 : 1;
	
	if ((item1 == NULL) || (item2 == NULL))
	{
		ASSERT(0);
		return;
	}
	BOOL isFiles = ITEM_IS_FILE(GetSelectedItem(0)) && ITEM_IS_FILE(GetSelectedItem(i));
	CString txt;
	CString rev1, rev2;
	CString ft1,  ft2;
	CDiff2Dlg dlg;

	dlg.m_Edit1 = GetItemPath(item1);
	dlg.m_Edit2 = GetItemPath(item2);

	if (isFiles)
	{
 		dlg.m_RevNbr1 = dlg.m_HaveRev1 = rev1 = GetItemRev(item1);
 		dlg.m_RevNbr2 = dlg.m_HaveRev2 = rev2 = GetItemRev(item2);

		rev1 = GetItemHeadRev(item1);
		rev2 = GetItemHeadRev(item2);

		CP4FileStats *fs;
		fs = m_FSColl.GetStats(GetLParam(item1));
		dlg.m_HeadRev1 = (fs->GetHeadAction() == F_DELETE) ? 0 : _tstoi(rev1);
		if (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
		{
			CString dPath = fs->GetFullDepotPath();
			if (dPath.Find(_T('%')))
				dlg.m_Edit1 = dPath;
		}
		fs = m_FSColl.GetStats(GetLParam(item2));
		dlg.m_HeadRev2 = (fs->GetHeadAction() == F_DELETE) ? 0 : _tstoi(rev2);
		if (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
		{
			CString dPath = fs->GetFullDepotPath();
			if (dPath.Find(_T('%')))
				dlg.m_Edit2 = dPath;
		}

		ft1 = GetItemType(item1);
		ft2 = GetItemType(item2);
		// assume all unknown types are text
		if (ft1 == _T("unknown"))
			ft1 = (ft2 == _T("unknown")) ? _T("text") : ft2;
		if (ft2 == _T("unknown"))
			ft2 = ft1;

		if (!GET_P4REGPTR()->GetDiffAppIsBinary())
		{
			if (((ft1.Find(_T("text")) != -1) && (ft2.Find(_T("binary")) != -1))
			 || ((ft1.Find(_T("binary")) != -1) && (ft2.Find(_T("text")) != -1)))
			{
				txt.FormatMessage(IDS_ONLY_DIFF_SAME_TYPES_s_n_s_s_n_s, 
					dlg.m_Edit1, _tstoi(rev1), ft1,
					dlg.m_Edit2, _tstoi(rev2), ft2);
				AfxMessageBox(txt);
				return;
			}
		}
	}
	else	// dirs, not files
	{
		dlg.m_Edit1 += _T("...");
		dlg.m_Edit2 += _T("...");
		dlg.m_IsFolders = TRUE;
	}

	SET_APP_HALTED(TRUE);
	int rc=dlg.DoModal();  
	SET_APP_HALTED(FALSE);
	if (rc == IDOK)
	{
		int r1, r2;

		if (dlg.m_RevRadio1 == 1)
		{
			if (isFiles)
			{
				rev1 = dlg.m_HaveRev1;
				r1 = _tstoi(rev1);
				if ((i = dlg.m_Edit1.Find(_T('#'))) != -1)
					dlg.m_Edit1 = dlg.m_Edit1.Left(i);
			}
			else
			{
				dlg.m_Edit1 += _T("#have");
				r1 = 0;
			}
		}
		else if (dlg.m_RevRadio1 == 2)
		{
			rev1 = dlg.m_RevNbr1;
			r1 = _tstoi(rev1);
			if ((i = dlg.m_Edit1.Find(_T('#'))) != -1)
				dlg.m_Edit1 = dlg.m_Edit1.Left(i);
		}
		else
		{
			r1 = 0 - _tstoi(rev1); // make negative as a signal to only use as part of temp name
			// dlg.m_Edit1 is already set with either nothing or @xxx at the end
		}
		if (dlg.m_RevRadio2 == 1)
		{
			if (isFiles)
			{
				rev2 = dlg.m_HaveRev2;
				r2 = _tstoi(rev2);
				if ((i = dlg.m_Edit2.Find(_T('#'))) != -1)
					dlg.m_Edit2 = dlg.m_Edit2.Left(i);
			}
			else
			{
				dlg.m_Edit2 += _T("#have");
				r2 = 0;
			}
		}
		else if (dlg.m_RevRadio2 == 2)
		{
			rev2 = dlg.m_RevNbr2;
			r2 = _tstoi(rev2);
			if ((i = dlg.m_Edit2.Find(_T('#'))) != -1)
				dlg.m_Edit2 = dlg.m_Edit2.Left(i);
		}
		else
		{
			r2 = 0 - _tstoi(rev2); // make negative as a signal to only use as part of temp name
			// dlg.m_Edit2 is already set with either nothing or @xxx at the end
			// if same basename and rev#, we must change so tempfiles don't step on each other
			if (r2 && (r2 == r1))
			{
				int i;
				CString str1 = _T("");
				CString str2 = _T("");
				if ((i = dlg.m_Edit1.ReverseFind(_T('/'))) != -1)
					str1 = dlg.m_Edit1.Mid(i+1);
				else if ((i = dlg.m_Edit1.ReverseFind(_T('\\'))) != -1)
					str1 = dlg.m_Edit1.Mid(i+1);
				if ((i = dlg.m_Edit2.ReverseFind(_T('/'))) != -1)
					str2 = dlg.m_Edit2.Mid(i+1);
				else if ((i = dlg.m_Edit2.ReverseFind(_T('\\'))) != -1)
					str2 = dlg.m_Edit2.Mid(i+1);
				if (!str1.CompareNoCase(str2))
					r2--;
			}
		}
		BOOL bDoIt;
		if (dlg.m_Edit1.GetAt(0) == _T('@') && dlg.m_Edit2.GetAt(0) == _T('@'))
		{
			dlg.m_Edit1 = _T("//...") + dlg.m_Edit1;
			dlg.m_Edit2 = _T("//...") + dlg.m_Edit2;
			bDoIt = TRUE;
		}
		else if ((dlg.m_Edit1.Find(_T("/..."))  != -1 && dlg.m_Edit2.Find(_T("/..."))  != -1)
			  || (dlg.m_Edit1.Find(_T("\\...")) != -1 && dlg.m_Edit2.Find(_T("\\...")) != -1))
			bDoIt = TRUE;
		else
			bDoIt = FALSE;
		if (bDoIt && (r1 || r2))
		{
			CString str;	// have to use a temp variable or Format() yields garbage
			if (r1)
			{
				str.Format(_T("%s#%d"), dlg.m_Edit1, r1);
				dlg.m_Edit1 = str;
			}
			if (r2)
			{
				str.Format(_T("%s#%d"), dlg.m_Edit2, r2);
				dlg.m_Edit2 = str;
			}
		}
		CCmd_Diff2 *pCmd= new CCmd_Diff2;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		pCmd->SetOutput2Dlg(GET_P4REGPTR()->Diff2InDialog());
		if( pCmd->Run( dlg.m_Edit1, dlg.m_Edit2, r1, r2, ft1, ft2, 
						dlg.m_RevRadio1 == 1, dlg.m_RevRadio2 == 1, bDoIt) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_DIFFING_FILES) );
		else
			delete pCmd;
	}
}

LRESULT CDepotTreeCtrl::OnP4Diff2(WPARAM wParam, LPARAM lParam)
{
	CCmd_Diff2 *pCmd= (CCmd_Diff2 *) wParam;
	CString msg= pCmd->GetInfoText();
	if( ! msg.IsEmpty() )
	{
		if (pCmd->IsOutput2Dlg())
		{
			int key;
			m_Diff2dlg = new CDiff2Output(this);
			m_Diff2dlg->SetKey(key = pCmd->HaveServerLock()? pCmd->GetServerKey() : 0);
			m_Diff2dlg->SetMsg( msg );
			CStringArray names;
			names.Add(pCmd->GetFileName(0));
			names.Add(pCmd->GetFileName(1));
			m_Diff2dlg->SetNames(&names);
			CString caption = LoadStringResource(IDS_DIFF2FOLDERS) + pCmd->GetFileName(0) + _T(" <> ") + pCmd->GetFileName(1);
			m_Diff2dlg->SetCaption( caption );
			m_Diff2dlg->SetFont(GetFont());
			if (!m_Diff2dlg->Create(IDD_DIFF2OUTPUT, this))	// display the diff2 output dialog box
			{
				m_Diff2dlg->DestroyWindow();	// some error! clean up
				delete m_Diff2dlg;
			}
		}
		else
		{
			AfxMessageBox( msg, MB_ICONINFORMATION);
		}
	}
	MainFrame()->UpdateStatus(_T(""));
	delete pCmd;
	return 0;
}

LRESULT CDepotTreeCtrl::OnP4EndDiff2( WPARAM wParam, LPARAM lParam )
{
	CSpecDescDlg *dlg = (CSpecDescDlg *)lParam;
	dlg->DestroyWindow();
	if (m_Diff2dlg)
		m_Diff2dlg->SetFocus();
	return TRUE;
}

LRESULT CDepotTreeCtrl::OnP4EndDiff2Output( WPARAM wParam, LPARAM lParam )
{
	CDiff2Output *dlg = (CDiff2Output *)lParam;
	dlg->DestroyWindow();
	m_Diff2dlg = 0;
	return TRUE;
}

/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFileRevisionTree() 
{
	HTREEITEM item=GetLastSelection();  
	
	if (item == NULL)
		ASSERT(0);
	else
	{
		CString itemStr = GetItemPath(item);

		// Are we in local syntax?
		if (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
		{
			// use depot syntax
			CP4FileStats *fs=m_FSColl.GetStats(GetLParam(item));
			itemStr = fs->GetFullDepotPath();
		}

		TheApp()->CallP4RevisionTree(itemStr);
	}
}


/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFiledropEdit() 
{
	if(GetSelectedCount() == 0)
	{
		ASSERT(0);
		return;
	}

    int key=0;
	BOOL hitMax = m_SkipSyncDialog;

	// Files were just dropped into a change in the changes window
	if(	AnyNotCurrent() && !m_SkipSyncDialog)
	{
		// Prevent anything from running while dlg is up
        if(SERVER_BUSY() || !GET_SERVER_LOCK(key) )
			return;

		if ( AnyDeleted() )
		{
			AfxMessageBox(IDS_ONE_OR_MORE_FILES_DELETED, MB_ICONEXCLAMATION);
			RELEASE_SERVER_LOCK(key);
			return;
		}
		
		int rc;
		if((rc = MsgBox(IDS_ONE_OR_MORE_FILES_IS_NOT_THE_HEAD_REVISION,
					MB_ICONEXCLAMATION | MB_DEFBUTTON3, IDC_BUTTON3)) == IDC_BUTTON3)
		{
			RELEASE_SERVER_LOCK(key);
			return;
		}
		else if (rc == IDC_BUTTON2)
		{
			RELEASE_SERVER_LOCK(key);
			m_StringList.RemoveAll();

			HTREEITEM cItem;
			CString itemStr;
			CP4FileStats *fs;
			int i, j;

			for(i=GetSelectedCount()-1; i>=0; i--)
			{
				cItem=GetSelectedItem(i);	// Next selected item

				TV_ITEM item;
				item.hItem=cItem;
				item.mask=TVIF_HANDLE| TVIF_CHILDREN;
				TreeView_GetItem(m_hWnd, &item );	
				if(!(ITEM_IS_A_FILE_NOT_A_SUBDIR))
					continue;
				fs=m_FSColl.GetStats((int) item.lParam);
				if(fs->GetHaveRev() < fs->GetHeadRev())
				{
					BOOL b =  TheApp()->m_HasPlusMapping;
					CString dPath = fs->GetFullDepotPath();
					if (!b && dPath.Find(_T('%')) != -1)
						b = TRUE;
					itemStr = b ? dPath : fs->GetFullClientPath();
					m_StringList.AddHead(itemStr);
				}
			}
			if (!m_StringList.IsEmpty())
			{
				CCmd_Get *pCmd= new CCmd_Get;
				pCmd->Init( m_hWnd, RUN_ASYNC);
				pCmd->SetOpenAfterSync(TRUE);
				for(i=-1, j=GetSelectedCount(); ++i < j; )
					pCmd->Add2SelSet(GetSelectedItem(i));	// Save list of selected items in cmd
				if( pCmd->Run( &m_StringList, FALSE ) )
					MainFrame()->UpdateStatus( LoadStringResource(IDS_FILE_SYNC) );
				else
					delete pCmd;
			}
			return;
		}
	}
	m_SkipSyncDialog = FALSE;

	if(	GET_P4REGPTR()->GetWarnAlreadyOpened() && AnyOtherOpened())
	{
		if(AfxMessageBox(IDS_ONE_OR_MORE_FILES_HAVE_BEEN_OPENED_BY_OTHER_USERS, 
						MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
		{
			RELEASE_SERVER_LOCK(key);
			return;
		}
	}

	// Maybe too many files were dropped?
	int fCount=LeafSelectedCount();
	if(	(fCount > _ttoi(GET_P4REGPTR()->GetWarnLimitOpen())) || (fCount < 0) )
	{
		// Prevent anything from running while dlg is up
		if( key==0 && (SERVER_BUSY() || !GET_SERVER_LOCK(key)) )
			return;
		
		CString txt;
		if (fCount > 0)
		{
			txt.FormatMessage(IDS_YOU_ARE_ATTEMPTING_TO_OPEN_n_FILES, fCount);
		}
		else
		{
			txt = LoadStringResource(IDS_YOU_ARE_ATTEMPTING_TO_OPEN_ALL_THE_FILES);
		}
		if(AfxMessageBox(txt, MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
		{
			RELEASE_SERVER_LOCK(key);
			return;
		}
	}

	if (((GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALTREE)
	 && !AnyHaveChildren() && AnyAddable()) || AnyRecoverable())
	{
		AssembleStringList( &m_StringList, FALSE, FALSE, TRUE );
		m_StringList2.RemoveAll();
		m_SelectionList.RemoveAll();
		for (POSITION pos= m_StringList.GetHeadPosition(); pos!=NULL; )
		{
			CString path = m_StringList.GetNext(pos);
			if (path.GetAt(0) == _T('/'))
				m_SelectionList.AddTail(path);	// if we have depot syntax, don't add it
			else
				m_StringList2.AddTail(path);
		}

		CCmd_Add *pCmd= new CCmd_Add;
		if( key==0 )
			pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
		else
			// Pass in the server lock if we have it
			pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key );
		pCmd->SetOpenAction(2);
		pCmd->SetHitMaxFileSeeks(FALSE);
		if( pCmd->Run( m_OpenUnderChangeNumber, &m_StringList2,  
							m_SelectionList.IsEmpty() ? NULL : &m_SelectionList) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_ADDING_FILES) );
		else
			delete pCmd;
	}
	else
	{
		AssembleStringList( &m_StringList );
		CCmd_ListOpStat *pCmd= new CCmd_ListOpStat;
		if( key==0 )
			pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
		else
			// Pass in the server lock if we have it
			pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key );

		pCmd->SetHitMaxFileSeeks(hitMax);
		if( pCmd->Run( &m_StringList, P4EDIT, m_OpenUnderChangeNumber ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUEST_OPEN_EDIT) );
		else
		{
			if( key != 0 )
				RELEASE_SERVER_LOCK(key);
			delete pCmd;
		}
	}
}


/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFileOpenedit() 
{	
	m_OpenUnderChangeNumber=0;
	OnFiledropEdit();	
}


/*
	_________________________________________________________________

	Do nothing - same as hitting escape to make menu go away
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFiledropCancel() 
{
}


void CDepotTreeCtrl::OnFileRecover() 
{
	if(GetSelectedCount() == 0)
	{
		ASSERT(0);
		return;
	}

	// At least one file is selected and user is sure, so proceed with recover
	m_StringList.RemoveAll();
	m_StringList2.RemoveAll();

	HTREEITEM item;
	CString itemStr;
	CP4FileStats *fs;
	int warnCount=0;

	for( int i = GetSelectedCount() - 1; i >= 0; i-- )
	{
		item = GetSelectedItem( i );
		if( HasChildren( item ) )
			ASSERT(0);
		else
		{
			itemStr = GetItemPath( item );
			fs=m_FSColl.GetStats( GetLParam(item) );
			if( fs->GetHeadAction() == F_DELETE && 
				fs->GetMyOpenAction()==0 && 
				fs->GetHeadRev() > 0 &&
				fs->GetHaveRev() < fs->GetHeadRev() &&
				fs->InClientView())
			{
				BOOL b = TheApp()->m_HasPlusMapping;
				CString dPath = fs->GetFullDepotPath();
				if (!b && dPath.Find(_T('%')) != -1)
					b = TRUE;
				m_StringList.AddHead( b ? fs->GetFullClientPath() : itemStr );
				if (fs->GetHaveRev() == 0)
				{
					itemStr.Format(_T("%s#%ld"), 
							b ? dPath : fs->GetFullClientPath(),
							fs->GetHeadRev()-1);
					m_StringList2.AddHead( itemStr );
				}
			}
			else
			{
				CString warning;
				warning.FormatMessage(IDS_UNABLE_TO_RECOVER_FILE_s, itemStr);
				AddToStatus(warning, SV_WARNING);
				warnCount++;
			}
		}
	}

	int proceed= IDYES;
	if(warnCount)
	{
		if (warnCount == GetSelectedCount())
			proceed = IDNO;
		else
		{
			CString message;
			message.FormatMessage( IDS_n_OF_n_SELECTED_FILES_CANNOT_BE_RECOVERED,
							warnCount, GetSelectedCount() );
			proceed= AfxMessageBox( message, MB_ICONEXCLAMATION | MB_YESNO );
		}
	}

	if( proceed == IDYES && !m_StringList.IsEmpty() )
	{
		if (m_StringList2.IsEmpty())
			OnFileRecoverProceed();
		else
		{
			CCmd_Get *pCmd= new CCmd_Get;
			pCmd->Init( m_hWnd, RUN_ASYNC );
			// Save the entire list of files to be recovered in the sync command
			for(POSITION pos = m_StringList.GetHeadPosition(); pos != NULL; )
				pCmd->Add2Recover(m_StringList.GetNext( pos ));
			// Run the sync command on the files that need syncing
			if( pCmd->Run( &m_StringList2, FALSE ) )
				MainFrame()->UpdateStatus( LoadStringResource(IDS_FILE_SYNC) );
			else
				delete pCmd;
		}
	}
}

void CDepotTreeCtrl::OnFileRecoverProceed()
{
	CCmd_Add *pCmd= new CCmd_Add;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
	pCmd->SetAlternateReplyMsg( WM_P4RECOVER );
	
	if( pCmd->Run( 0, &m_StringList ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_RECOVER) );
	else
		delete pCmd;
}

/*
	_________________________________________________________________
*/

BOOL CDepotTreeCtrl::AllOpenedForAdd()
{
	if (GET_P4REGPTR( )->ShowEntireDepot( ) != SDF_LOCALTREE)
		return FALSE;

	BOOL allOpenedForAdd=TRUE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE| TVIF_CHILDREN | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if(fs->GetMyOpenAction() && fs->GetMyOpenAction() != F_ADD)
			{
				allOpenedForAdd=FALSE;
				break;
			}
		}
	}
	return allOpenedForAdd;
}

void CDepotTreeCtrl::OnFileRevert() 
{
	if(GetSelectedCount() == 0)
	{
		ASSERT(0);
		return;
	}

	BOOL b;
	if ( ((((b=AllOpenedForAdd())==FALSE) && GET_SERVERLEVEL() < 14)) 
	  || GET_P4REGPTR()->AlwaysWarnOnRevert() )
	{
		if(AfxMessageBox(IDS_REVERTING_FILES_WILL_OVERWRITE_EDITS, MB_ICONQUESTION|MB_YESNO) != IDYES)
			return;
		b = TRUE;
	}

	// At least one file is selected, 
	// and user is sure or running against new server,
	// so proceed with revert.
	m_StringList.RemoveAll();
	AssembleStringList( &m_StringList, TRUE );

	CString itemStr;
	
	if (GET_SERVERLEVEL() >= 14 && !b)
	{
		// Copy m_StringList to m_SelectionList
		// because CCmd_Revert will clear what is passed to it.
		// We need the m_StringList in OnP4FileRevert()
		m_SelectionList.RemoveAll();
		for (POSITION pos= m_StringList.GetHeadPosition(); pos!=NULL; )
			m_SelectionList.AddTail(m_StringList.GetNext(pos));
		
		// Run the p4 revert -an command on all the selected files
		CCmd_Revert *pCmd= new CCmd_Revert;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		pCmd->SetAlternateReplyMsg( WM_P4FILEREVERT );
		if( pCmd->Run( &m_SelectionList, FALSE, TRUE, TRUE, FALSE, TRUE ) )
			MainFrame()->UpdateStatus(LoadStringResource(IDS_RUNNING_DIFF));
		else
			delete pCmd;
	}
	else
	{
		CCmd_ListOpStat *pCmd= new CCmd_ListOpStat;
		pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
		int iFlag;
		IsFilteredOnOpen(0, (LPARAM)&iFlag);
		pCmd->SetRedoOpenedFilter(iFlag);
		if( pCmd->Run( &m_StringList, P4REVERT ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_REVERT) );
		else
			delete pCmd;
	}
}

LRESULT CDepotTreeCtrl::OnP4FileRevert( WPARAM wParam, LPARAM lParam )
{
	BOOL	bBox = TRUE;
	BOOL	bUseDashA = FALSE;
	ASSERT(GET_SERVERLEVEL() >= 14);

	// m_StringList still contains our revert list
	CCmd_Revert *pCmdR= (CCmd_Revert *) wParam;

	if(!pCmdR->GetError() 
		&& pCmdR->GetFileList()->GetCount() + pCmdR->NbrNonEdits() == m_StringList.GetCount())
	{
		bBox = FALSE;
		if (pCmdR->NbrNonEdits() == 0)
			bUseDashA = TRUE;
	}
	delete pCmdR;

	if (bBox)
	{
		if(AfxMessageBox(IDS_REVERTING_FILES_WILL_OVERWRITE_EDITS, MB_ICONQUESTION|MB_YESNO) != IDYES)
		{
			MainFrame()->ClearStatus();
			return (0);
		}
	}

	CCmd_ListOpStat *pCmd= new CCmd_ListOpStat;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
	int iFlag;
	IsFilteredOnOpen(0, (LPARAM)&iFlag);
	pCmd->SetRedoOpenedFilter(iFlag);
	if( pCmd->Run( &m_StringList, bUseDashA ? P4REVERTUNCHG : P4REVERT ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_REVERT) );
	else
		delete pCmd;

	return (0);
}


/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFileLock() 
{
	LockAndUnlock( P4LOCK );
}


void CDepotTreeCtrl::OnFileUnlock() 
{
	LockAndUnlock( P4UNLOCK );
}


void CDepotTreeCtrl::LockAndUnlock( int which ) 
{
	if(GetSelectedCount() == 0)
	{
		ASSERT(0);
		return;
	}

	AssembleStringList( &m_StringList );
	
	CCmd_ListOpStat *pCmd= new CCmd_ListOpStat;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );

	if( pCmd->Run( &m_StringList, which ) )
        MainFrame()->UpdateStatus(LoadStringResource(which == P4LOCK ? IDS_REQUESTINGLOCK : IDS_REQUESTINGUNLOCK));
	else
		delete pCmd;
}


/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFiledropDelete() 
{
	if(GetSelectedCount() == 0)
	{
		ASSERT(0);
		return;
	}

    int key=0;

	// Maybe too many files were dropped?
	int fCount=LeafSelectedCount();
	if(	(fCount > 50) || (fCount < 0) )
	{
		// Prevent anything from running while dlg is up
		if( SERVER_BUSY() && !GET_SERVER_LOCK(key) )
			return;
		
		CString txt;
		if (fCount > 0)
		{
			txt.FormatMessage(IDS_YOU_ARE_ATTEMPTING_TO_DELETE_n_FILES, fCount);
		}
		else
		{
			txt = LoadStringResource(IDS_YOU_ARE_ATTEMPTING_TO_DELETE_ALL_THE_FILES);
		}
		if(AfxMessageBox(txt, MB_OKCANCEL | MB_ICONEXCLAMATION) == IDCANCEL)
		{
			RELEASE_SERVER_LOCK(key);
			return;
		}
	}

	AssembleStringList( &m_StringList );

	CCmd_ListOpStat *pCmd= new CCmd_ListOpStat;

    if( key == 0 )
	    pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
    else
        // Pass in the server lock if we have it
        pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key);

	if( pCmd->Run( &m_StringList, P4DELETE, m_OpenUnderChangeNumber ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_DELETE) );
	else
    {
        if( key != 0 )
            RELEASE_SERVER_LOCK( key );
		delete pCmd;
    }

}


/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFileOpendelete() 
{
	m_OpenUnderChangeNumber=0;
	OnFiledropDelete();	
}


/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFileTimeLapseView() 
{
	HTREEITEM currentItem=GetLastSelection();
	
	if (currentItem == NULL)
		ASSERT(0);
	else
	{
		CString itemStr= GetItemPath(currentItem);
		
		// Are we in local syntax?
		if (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
		{
			// use depot syntax
			CP4FileStats *fs=m_FSColl.GetStats(GetLParam(currentItem));
			itemStr = fs->GetFullDepotPath();
		}

		TheApp()->CallP4A(itemStr, _T(""), 0);	// use p4v.exe for annotate
	}
}

void CDepotTreeCtrl::OnFileAnnotate() 
{
	FileAnnotate(FALSE);
}

void CDepotTreeCtrl::OnFileAnnotateAll() 
{
	FileAnnotate(TRUE);
}

void CDepotTreeCtrl::OnFileAnnotateChg() 
{
	FileAnnotate(FALSE, TRUE);
}

void CDepotTreeCtrl::OnFileAnnotateChgAll() 
{
	FileAnnotate(TRUE, TRUE);
}

void CDepotTreeCtrl::FileAnnotate(BOOL bAll, BOOL bChg/*=FALSE*/) 
{
	HTREEITEM currentItem=GetLastSelection();
	
	if (currentItem == NULL)
		ASSERT(0);
	else
	{
		CString itemStr= GetItemPath(currentItem);
		
		// Are we in local syntax?
		if (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
		{
			// use depot syntax
			CP4FileStats *fs=m_FSColl.GetStats(GetLParam(currentItem));
			itemStr = fs->GetFullDepotPath();
		}

		DWORD index=GetLParam(currentItem);
		CP4FileStats *fs=m_FSColl.GetStats(index);
		CCmd_PrepBrowse *pCmd= new CCmd_PrepBrowse;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		pCmd->SetFileType(fs->IsTextFile() ? FST_TEXT : FST_BINARY);
		CString fType = fs->GetHeadType();
		if( pCmd->Run( FALSE, itemStr, fType, bAll, bChg, FALSE, fs->GetHeadRev(),
			GET_P4REGPTR()->GetAnnotateWhtSpace(),
			bChg ? GET_P4REGPTR()->GetAnnotateIncInteg() : FALSE) )
		{
			MainFrame()->UpdateStatus( LoadStringResource(IDS_FETCHING_FILE) );
			m_Viewer=GET_P4REGPTR()->GetEditApp();
		}
		else
			delete pCmd;
	}
}

LRESULT CDepotTreeCtrl::OnSetViewer( WPARAM wParam, LPARAM lParam )
{
	m_Viewer = (LPCTSTR)lParam;
	return 0;
}

/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFileRevisionhistory() 
{
	HTREEITEM currentItem=GetLastSelection();  
	
	if (currentItem == NULL)
		ASSERT(0);
	else
	{
		BOOL isAfile;
		CString itemStr= GetItemPath(currentItem);
		if (itemStr.GetAt(itemStr.GetLength() -1) == m_SlashChar)
		{
			itemStr += _T("...");
			isAfile = FALSE;
		}
		else
			isAfile = TRUE;
		
		// Are we in local syntax?
		if (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT && isAfile)
		{
			// use depot syntax
			CP4FileStats *fs=m_FSColl.GetStats(GetLParam(currentItem));
			itemStr = fs->GetFullDepotPath();
		}

		CCmd_History *pCmd= new CCmd_History;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		pCmd->SetCallingWnd(m_hWnd);
		if (!isAfile)
		{
			pCmd->OverrideFetchCompleteHist(0);
			pCmd->OverrideFetchHistCount(25);
			pCmd->SetEnableShowIntegs(FALSE);
		}
		pCmd->SetIsAFile(isAfile);
		pCmd->SetInitialRev(-1, itemStr);
		if( pCmd->Run(itemStr) )
		{
			MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_HISTORY) );
		}
		else
			delete pCmd;
		
	}
}

/*	_________________________________________________________________

	Handle "Find in Changelist" menu item.  
	Notify the pending changelist window of the name of
	the file in depot syntax to be positioned to.
	Go via MainFrame since it knows the changelist window ptr
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnPositionChgs() 
{
	HTREEITEM currentItem=GetLastSelection();  
	
	if ((currentItem == NULL) || !ITEM_IS_FILE(currentItem))
	{
		ASSERT(0);
	}
	else
	{
		CP4FileStats *fs=m_FSColl.GetStats( GetLParam(currentItem) );
		ASSERT_KINDOF(CP4FileStats, fs);
		MainFrame()->PositionChgs( fs->GetFullDepotPath(), fs->GetMyOpenAction() );
	}
}

/*	_________________________________________________________________

	Handle "Find in Depot" menu item.  This routine also gets
	called by the completion routine for p4 files (OnP4Files) if p4 files
	returns more that one file.

	Bring up a dialogbox that allows the user to 
	1) enter a possibly ambigeous filename to find in the Depot tree  or
	2) select from a list of files that satisfy a previously given ambigeous filename
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnPositionDepot() 
{
	m_Need2Filter = FALSE;
	CFindFilesDlg *dlg= new CFindFilesDlg(this);
	dlg->Init(m_P4Files_FileSpec, &m_P4Files_List, this, m_P4Files_Deselect);
	if (!dlg->Create(IDD_P4FILES, this))	// display the find files dialog box
	{
		dlg->DestroyWindow();	// some error! clean up
		delete dlg;
	}
}

LRESULT CDepotTreeCtrl::OnEndPositionDepot( WPARAM wParam, LPARAM lParam ) 
{
	CFindFilesDlg *dlg = (CFindFilesDlg *)lParam;

	m_P4Files_Deselect = FALSE;
	switch(dlg->m_ExitCode)
	{
	case 3:
			EmptyDepotFilter();					// empty out any old filter info
			m_Need2Filter = TRUE;
	case 1:	// focus was on the edit field at the top when they clicked the OK button,
		{	//  start a p4 files on the possibly ambigeous filename given
			RunP4Files(dlg->GetEditString());
			break;
		}
	case 2:	// the focus was on the listbox when they clicked the OK button,
		{	// expand the depot treeview to find the item selected in the listbox
			ExpandDepotString(dlg->GetListString(), TRUE);
			if (!dlg->m_CloseAfterFind)
				return TRUE;
			break;
		}
	case 4:	// they clicked on the Filter Depot button while the focus was in the listbox;
		{	// the filter has been loaded, so fire up a refresh
			OnViewUpdate();
			break;
		}
	default: // they canceled
		{
			break;
		}
	}
	dlg->DestroyWindow();
	return TRUE;
}

void CDepotTreeCtrl::OnPositionDepotNext() 
{
	if (m_P4Files_List.IsEmpty())
	{
		MessageBeep(0);
		return;
	}
	CString depotPath = GetCurrentItemPath();
	if (depotPath.GetAt(0) != _T('/'))
	{
		CString str = depotPath;
		str.TrimRight(_T("\\ "));
		if (str == TheApp()->m_ClientRoot)
		{
			ExpandDepotString(m_P4Files_List.GetHead(), TRUE);
			return;
		}
		depotPath = GetItemDepotSyntax(GetSelectedItem(0), &depotPath);
	}
	int i;
	for( POSITION pos= m_P4Files_List.GetHeadPosition(); pos != NULL; )
	{
		CString str = m_P4Files_List.GetNext(pos);
		if ((i = fCompare(depotPath, str, GET_P4REGPTR()->SortByExtension())) <= 0)
		{
			if (i)
			{
				if ((i = str.Find(_T('#'))) != -1)
					str = str.Left(i);
				str.TrimRight();
			}
			if (fCompare(depotPath, str, GET_P4REGPTR()->SortByExtension()) >= 0)
				continue;
			ExpandDepotString(str, TRUE);
			return;
		}
	}
	MessageBeep(0);
}
void CDepotTreeCtrl::OnPositionDepotPrev() 
{
	if (m_P4Files_List.IsEmpty())
	{
		MessageBeep(0);
		return;
	}
	CString depotPath = GetCurrentItemPath();
	if (depotPath.GetAt(0) != _T('/'))
	{
		CString str = depotPath;
		str.TrimRight(_T("\\ "));
		if (str == TheApp()->m_ClientRoot)
		{
			MessageBeep(0);
			return;
		}
		depotPath = GetItemDepotSyntax(GetSelectedItem(0), &depotPath);
	}
	int i;
	for( POSITION pos= m_P4Files_List.GetTailPosition(); pos != NULL; )
	{
		CString str = m_P4Files_List.GetPrev(pos);
		if ((i = fCompare(depotPath, str, GET_P4REGPTR()->SortByExtension())) >= 0)
		{
			ExpandDepotString(str, TRUE);
			return;
		}
	}
	MessageBeep(0);
}

void CDepotTreeCtrl::RunP4Files(CString str)
{
	m_P4Files_FileSpec = str;
	CCmd_Files *pCmdFiles= new CCmd_Files;
	pCmdFiles->Init( m_hWnd, RUN_ASYNC);
	if( pCmdFiles->Run(m_P4Files_FileSpec) )
	{
		MainFrame()->UpdateStatus( LoadStringResource(IDS_RUNNING_P4_FILES) );
		m_pCmdFiles = (void *)pCmdFiles;
	}
	else
	{
		delete pCmdFiles;
		RedrawWindow();
		MainFrame()->ClearStatus();
	}
}

/*	_________________________________________________________________

	Completion routine for p4 files

	1)	no files found - nothing to do
	2)	1 file found - expand the depot treeview to find the file
	3)	more than 1 file found - bring up the "Find in Depot"
		dialogbox again with all the found files in the listbox
	_________________________________________________________________
*/

LRESULT CDepotTreeCtrl::OnP4Files(WPARAM wParam, LPARAM lParam)
{
	CCmd_Files *pCmdFiles= (CCmd_Files *)m_pCmdFiles;

	m_P4Files_List.RemoveAll();		// empty the files list
	CStringList *list = pCmdFiles->GetFileList();
	switch(list->GetCount())
	{
	case 0:			//	no matching files found, nothing to do
		break;
	case 1:			//	1 file found - expand the depot treeview to find the file
		if (!m_Need2Filter)
		{
			CString path = list->GetHead();
			int i = path.Find(_T('#'));
			if (i != -1)
			{
				if (!GET_P4REGPTR()->ShowDeleted() && (path.Find(_T(" - delete change "), i) != -1))
				{
					AddToStatus(path, SV_WARNING);
					break;
				}
				path = path.Left(i);
			}
			ExpandDepotString(path, TRUE);
			break;
		}
	default:		//	more than 1 file found - load the file list
		if (GET_P4REGPTR()->ShowDeleted())
		{
			for( POSITION pos = list->GetHeadPosition(); pos != NULL; )
				m_P4Files_List.AddHead(list->GetNext( pos ));
		}
		else
		{
            int i = -1;
            CString path;
			for( POSITION pos = list->GetHeadPosition(); pos != NULL; )
			{
				path = list->GetNext( pos );
				if (((i = path.Find(_T('#'))) == -1) 
				 || (path.Find(_T(" - delete change "), i) == -1))
					m_P4Files_List.AddHead(path);
			}
			if (m_P4Files_List.GetCount() == 1 && !m_Need2Filter)
			{
                // if after removing deletes, there was only one file left,
                // just expand the depot to find it
                path = m_P4Files_List.GetHead();
				m_P4Files_List.RemoveAll();
                i = path.Find(_T('#'));
                if(i != -1)
                    path = path.Left(i);
				ExpandDepotString(path, TRUE);	
			}
			else if (!m_P4Files_List.GetCount())
				AddToStatus(LoadStringResource(IDS_ONLY_DELETED_FILES_WERE_FOUND), SV_WARNING);
		}
		break;
	}
	delete pCmdFiles;
	MainFrame()->ClearStatus();
	if (!m_P4Files_List.IsEmpty())	// if there is anyting in the files list,
	{
		if (m_Need2Filter)				// if they want to filter the depot view
		{
			LoadDepotFilterList(&m_P4Files_List);	// load the depot filter list
			m_FilterDepot=TRUE;
			m_DepotFilterPort = GET_P4REGPTR()->GetP4Port();
			m_DepotFilterClient = GET_P4REGPTR()->GetP4Client();
			if (m_DepotFilterType == DFT_LIST)
				GetNextFilesFromFilterList();		// [which will turn off m_Need2Filter and call OnViewUpdate() when done]
			else
			{
				m_Need2Filter = FALSE;
				OnViewUpdate();							//	and refresh the depot pane
			}
		}
		else
			OnPositionDepot();			// or bring up the "Find in Depot" dialog box again
	}
	return 0;
}

/*
	_________________________________________________________________
*/

LRESULT CDepotTreeCtrl::OnDropTarget(WPARAM wParam, LPARAM lParam)
{
	m_DropTargetFlag = wParam;
	m_DropTargetPt.x = LOWORD(lParam);
	m_DropTargetPt.y = HIWORD(lParam);
	return 0;
}

/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnShowDeletedFiles() 
{
	BOOL showDeleted = !GET_P4REGPTR()->ShowDeleted();
	GET_P4REGPTR()->SetShowDeleted( showDeleted );
	MainFrame()->SetDepotCaption( GET_P4REGPTR()->ShowEntireDepot() );
	OnViewUpdate( TRUE );
}

/*
	_________________________________________________________________
*/


//	If user right clicks on a file and chooses 'Explore', run Windows Explorer in
//	the directory where that file resides on the client machine. If we have any
//	trouble determining the directory, use the client root.
void CDepotTreeCtrl::OnWinExplore() 
{
	CString itemStr;
	CString switches = _T("");
	HTREEITEM currentItem = NULL;

	if (m_ContextPoint.x != -1 && m_ContextPoint.y != -1)
	{
		// find out what item was clicked to generate the last context menu
		TV_HITTESTINFO ht;
		ht.pt=m_ContextPoint;
		ScreenToClient(&ht.pt);
		ht.flags=TVHT_ONITEMLABEL | TVHT_ONITEMICON | TVHT_ONITEMBUTTON;
		currentItem=HitTest( &ht );
	}
	if (!currentItem || !IsSelected(currentItem))
		 currentItem=GetLastSelection();
	
	if (currentItem == NULL)
	{
		itemStr = TheApp()->m_ClientRoot;
	}
	else if (ITEM_IS_FILE(currentItem))
	{
		DWORD index=GetLParam(currentItem);
		CP4FileStats *fs=m_FSColl.GetStats(index);
		itemStr= fs->GetFullClientPath();
		itemStr.Replace(_T('/'), _T('\\'));
		if (GET_P4REGPTR()->GetExplorer()			// not using Win Explorer
		 || (::GetFileAttributes(itemStr) == -1))	// file not found
		{
			int i;
			if ((i = ReverseFindMBCS(itemStr, _T('\\'))) != -1)
				itemStr = itemStr.Left(i);
		}
		else
		{
			switches = _T("/select,");
		}
	}
	else
	{
		itemStr = GetItemPath(currentItem);
		if (itemStr.GetAt(0) == _T('/'))
		{
			// the itemStr is in depot syntax
			itemStr.TrimRight(_T('/'));
			// first check for the root
			if (itemStr.Find(_T('/'), 2) == -1)
			{
				itemStr = TheApp()->m_ClientRoot;
			}
			else
			{
				// if wasn't the root
				// so now run p4 where on the string and see what we get
				BOOL bOK = FALSE;
				CCmd_Where *pCmd1 = new CCmd_Where;
				pCmd1->Init(NULL, RUN_SYNC);
				if ( pCmd1->Run(itemStr) && !pCmd1->GetError() 
				  && pCmd1->GetDepotFiles()->GetCount() )
				{
					itemStr = pCmd1->GetLocalSyntax();
					bOK = TRUE;
				}
				delete pCmd1;
				if (!bOK)
					itemStr = TheApp()->m_ClientRoot;
			}
		}
		if (itemStr.GetLength() > 3)
			TrimRightMBCS(itemStr, _T("\\"));
	}
	if (itemStr.GetLength() < 3)
		itemStr += "\\";
	if (::GetFileAttributes(itemStr) == -1)
		itemStr = TheApp()->m_ClientRoot;
	if (itemStr.IsEmpty() || itemStr == _T("null"))
		itemStr = _T("C:\\");

	if (itemStr.FindOneOf(_T(" &()[]{}^=;!'+,`~")) != -1)
	{
		itemStr.TrimLeft();
		itemStr.TrimRight();
		itemStr = _T('\"') + itemStr + _T('\"');
	}

    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    CreateProcess(NULL, const_cast<LPTSTR>((LPCTSTR)(TheApp()->GetExplorer()+switches+itemStr)), 
					NULL, NULL, 
#ifdef UNICODE
					FALSE, DETACHED_PROCESS | NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT, 
#else
					FALSE, DETACHED_PROCESS | NORMAL_PRIORITY_CLASS, 
#endif
					MainFrame()->P4GetEnvironmentStrings(), 
					NULL, &si, &pi);
}

/*
	_________________________________________________________________
*/

//	If user right clicks on a file and chooses 'Command Prompt', run a Command Prompt
//	in the directory where that file resides on the client machine.
//	We can only do this for a specific file - not for a directory since it may map
//	to multiple local folders - in that case let MainFrame handle it.

void CDepotTreeCtrl::OnCmdPrompt() 
{
	if (!(GetSelectedCount() && ((!AnyHaveChildren() && AnyInView()) 
								|| GetSelectedCount()==1)))
	{
		MainFrame()->OnCmdPromptPublic();
		return;
	}

	int i;
	CString itemStr;
	HTREEITEM currentItem = NULL;

	if (m_ContextPoint.x != -1 && m_ContextPoint.y != -1)
	{
		// find out what item was clicked to generate the last context menu
		TV_HITTESTINFO ht;
		ht.pt=m_ContextPoint;
		ScreenToClient(&ht.pt);
		ht.flags=TVHT_ONITEMLABEL | TVHT_ONITEMICON | TVHT_ONITEMBUTTON;
		currentItem=HitTest( &ht );
	}
	if (!currentItem || !IsSelected(currentItem))
		 currentItem=GetLastSelection();  

	if (ITEM_IS_FILE(currentItem))
	{
		DWORD index=GetLParam(currentItem);
		CP4FileStats *fs=m_FSColl.GetStats(index);
		itemStr= fs->GetFullClientPath();
		itemStr.Replace(_T('/'), _T('\\'));
		if ((i = ReverseFindMBCS(itemStr, _T('\\'))) != -1)
			itemStr = itemStr.Left(i);
	}
	else
	{
		itemStr = GetItemPath(currentItem);
		if (itemStr.GetAt(0) == _T('/'))
		{
			// the itemStr is in depot syntax
			itemStr.TrimRight(_T('/'));
			// first check for the root
			if (itemStr.Find(_T('/'), 2) == -1)
			{
				itemStr = TheApp()->m_ClientRoot;
			}
			else
			{
				// if wasn't the root
				// so now run p4 where on the string and see what we get
				BOOL bOK = FALSE;
				CCmd_Where *pCmd1 = new CCmd_Where;
				pCmd1->Init(NULL, RUN_SYNC);
				if ( pCmd1->Run(itemStr) && !pCmd1->GetError() 
				  && pCmd1->GetDepotFiles()->GetCount() )
				{
					itemStr = pCmd1->GetLocalSyntax();
					bOK = TRUE;
				}
				delete pCmd1;
				if (!bOK)
					itemStr = TheApp()->m_ClientRoot;
			}
		}
		if (itemStr.GetLength() > 3)
			TrimRightMBCS(itemStr, _T("\\"));
	}
	if (itemStr.GetLength() < 3)
		itemStr += "\\";
	if (::GetFileAttributes(itemStr) == -1)
		itemStr = TheApp()->m_ClientRoot;
	if (itemStr.IsEmpty())
		itemStr = _T("C:\\");

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
					itemStr, &si, &pi);
}


/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFileGetwhatif() 
{
	FileGet(TRUE, FALSE, FALSE);
}


/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFileGet() 
{
	FileGet(FALSE, FALSE, FALSE);
}


/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFileGetcustom() 
{
	if (MainFrame()->IsModlessUp())
		return;

	int cnt;
	if((cnt = GetSelectedCount()) == 0)
	{
		ASSERT(0);
		return;
	}

	m_CustomGetDlg = new CCustomGetDlg(this);
	if (!m_CustomGetDlg)
	{
		ASSERT(0);
		AfxMessageBox(IDS_COULD_NOT_CREATE_CUSTOM_SYNC_DIALOG_BOX, MB_ICONSTOP);
		return;
	}
	MainFrame()->SetModelessUp(TRUE);

	if (cnt == 1 && AnyHaveChildren())
		cnt = 100;
	m_CustomGetDlg->m_NbrSel = cnt;
	if (!m_CustomGetDlg->Create(IDD_CUSTOMGET, this))
	{
		delete m_CustomGetDlg;
		MainFrame()->SetModelessUp(FALSE);
	}
}

LRESULT CDepotTreeCtrl::OnDoGetCustom(WPARAM wParam, LPARAM lParam) 
{
	BOOL preview=FALSE;
	switch(wParam)
	{
	case IDCANCEL:
		break;

	case IDGETFORCEPREVIEW:
		preview=TRUE;
		// fall thru
	case IDGETFORCE:
		ASSERT(lParam);
		FileGet(preview, TRUE, FALSE, (LPCTSTR)lParam); 
		break;

	case IDGETPREVIEW:
		preview=TRUE;
		// fall thru
	case IDGET:
		ASSERT(lParam);
		FileGet(preview, FALSE, FALSE, (LPCTSTR)lParam); 
		break;
	default:
		ASSERT(0);
	}
	if (m_CustomGetDlg && (wParam != IDGETPREVIEW) && (wParam != IDGETFORCEPREVIEW))
	{
		m_CustomGetDlg->DestroyWindow();	// deletes m_CustomGetDlg
		m_CustomGetDlg = 0;
		MainFrame()->SetModelessUp(FALSE);
	}
	return 0;
}


/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::FileGet(BOOL whatIf, BOOL force, BOOL removeFiles, LPCTSTR qualifier)
{
	if(GetSelectedCount()==0)
	{
		ASSERT(0);
		return;
	}

	m_StringList.RemoveAll();

	HTREEITEM cItem;
	CString itemStr;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		cItem=GetSelectedItem(i);  // Next selected item

		TV_ITEM item;
		item.hItem=cItem;
		item.mask=TVIF_HANDLE| TVIF_CHILDREN;
		TreeView_GetItem(m_hWnd, &item );	
		itemStr= GetItemPath(cItem);
		if(item.cChildren == 1) // a directory
			itemStr+=_T("...");
		else
		{
			int i;
			if ((itemStr.GetAt(1) == _T(':')) && ((i = itemStr.Find(g_TrulyEmptyDir)) != -1))
			{
				itemStr = itemStr.Left(i);
				itemStr.TrimRight();
				itemStr += _T("\\...");
			}
			else
			{
				CP4FileStats *stats= m_FSColl.GetStats(GetLParam(cItem));
				BOOL b =  TheApp()->m_HasPlusMapping;
				CString dPath = stats->GetFullDepotPath();
				if (!b && dPath.Find(_T('%')) != -1)
					b = TRUE;
				itemStr = b ? dPath : stats->GetFullClientPath();
			}
		}
		if(removeFiles)
			itemStr+=_T("#none");
		else 
			itemStr+=qualifier;  //  '@label' or '@changenum'

		m_StringList.AddHead(itemStr);
	}
	
	CCmd_Get *pCmd= new CCmd_Get;
	pCmd->Init( m_hWnd, RUN_ASYNC);
	if( pCmd->Run( &m_StringList, whatIf, force ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_FILE_SYNC) );
	else
		delete pCmd;
}


/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFileRefresh()
{
	if(GetSelectedCount()==0)
	{
		ASSERT(0);
		return;
	}

	AssembleStringList ( &m_StringList, TheApp()->m_HasPlusMapping );

	//		98.1 changed the command to recopy. before: p4 refresh. after: p4 sync -f
	//
	if ( GET_SERVERLEVEL( ) > 3 )
	{
        // Add a "#have" to each filespec
        POSITION pos= m_StringList.GetHeadPosition();
        while( pos != NULL )
        {
			int i;
            POSITION oldPos= pos;
            CString filespec= m_StringList.GetNext(pos);
			if (filespec.GetAt(1) == _T(':') && ((i = filespec.Find(g_TrulyEmptyDir)) != -1))
			{
				filespec = filespec.Left(i);
				filespec.TrimRight();
				filespec += _T("\\...");
			}
            m_StringList.SetAt(oldPos, filespec+_T("#have"));
        }

		CCmd_Get *pCmd= new CCmd_Get;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		if( pCmd->Run( &m_StringList, FALSE, TRUE ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_FILE_SYNC) );
		else
			delete pCmd;
	}
	else
	{
		CCmd_Refresh *pCmd= new CCmd_Refresh;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		if(	pCmd->Run( &m_StringList ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_FILE_REFRESH) );
		else
			delete pCmd;
	}
}

void CDepotTreeCtrl::OnFileForceToHead()
{
	if(GetSelectedCount()==0)
	{
		ASSERT(0);
		return;
	}

	AssembleStringList ( &m_StringList, TheApp()->m_HasPlusMapping );

	//		98.1 changed the command to recopy. before: p4 refresh. after: p4 sync -f
	//
	if ( GET_SERVERLEVEL( ) > 3 )
	{
        // Add a "#head" to each filespec
        POSITION pos= m_StringList.GetHeadPosition();
        while( pos != NULL )
        {
			int i;
            POSITION oldPos= pos;
            CString filespec= m_StringList.GetNext(pos);
			if (filespec.GetAt(1) == _T(':') && ((i = filespec.Find(g_TrulyEmptyDir)) != -1))
			{
				filespec = filespec.Left(i);
				filespec.TrimRight();
				filespec += _T("\\...");
			}
            m_StringList.SetAt(oldPos, filespec+_T("#head"));
        }

		CCmd_Get *pCmd= new CCmd_Get;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		if( pCmd->Run( &m_StringList, FALSE, TRUE ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_FILE_SYNC) );
		else
			delete pCmd;
	}
	else
		MessageBeep(0);
}


/*
	_________________________________________________________________

	removes files you're sync'd to, but don't have opened.
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFileRemove() 
{
	FileGet(FALSE, FALSE, TRUE);
}



/*
	_________________________________________________________________

	Called by MSTreeView during OnLButtonDown to see if the user is 
	dragging before letting the mouse button up, which would indicate
	a drag drop operation.
	_________________________________________________________________
*/

BOOL CDepotTreeCtrl::TryDragDrop( HTREEITEM currentItem )
{
	// Dont actually send data - clipboard format is all the info target requires
	m_OLESource.DelayRenderData( (unsigned short) m_CF_DEPOT);
	m_OLESource.DelayRenderData( (unsigned short) CF_HDROP);	// for external programs (dragging to an editor)
	m_SelectOnDrag= KEEP_SELECTION;
	m_DropTargetFlag=0;
	m_DragDropCtr++;

	if(m_OLESource.DoDragDrop(DROPEFFECT_COPY, &m_DragSourceRect, NULL) == DROPEFFECT_COPY)
	{
		if (m_DropTargetFlag == PENDINGCHG)
		{
			HTREEITEM item;
			CString itemStr;
			// changes view accepted a drop - so get the target change number
			CPoint point;
			m_OpenUnderChangeNumber= ::SendMessage(m_changeWnd, WM_GETDRAGTOCHANGENUM, (WPARAM) &point, 0);

			// and make a list of args for p4 add to use
			m_StringList.RemoveAll();
			for(int i=GetSelectedCount()-1; i>=0; i--)
			{
				item=GetSelectedItem(i);
				itemStr= GetItemPath(item);
				m_StringList.AddHead(itemStr);
			}
			
			OnFiledropEdit();
			return TRUE;
		}
		else if (m_DropTargetFlag == SUBMITTEDCHG)
		{
			// Post a message to the Submitted Changelist Window so it sets the filter
			::SendMessage(m_oldChgWnd, WM_COMMAND, MAKEWPARAM(ID_FILTER_SETVIEW_DROP, 0), 0);
			return TRUE;
		}
		else if (m_DropTargetFlag == LABELVIEW)
		{
			// Either post a message to the Label Window or pop up a menu
			switch (GET_P4REGPTR()->GetLabelDragDropOption())
			{
			case LDD_ADD:
				::SendMessage(m_labelWnd, WM_COMMAND, MAKEWPARAM(ID_LABEL_SYNC, 0), 0);
				break;
			case LDD_DELETE:
				::SendMessage(m_labelWnd, WM_COMMAND, MAKEWPARAM(ID_LABEL_DELETEFILES, 0), 0);
				break;
			case LDD_SYNC:
				::SendMessage(m_labelWnd, WM_COMMAND, MAKEWPARAM(ID_LABEL_SYNC_CLIENT, 0), 0);
				break;
			case LDD_ADD2VIEW:
				::SendMessage(m_labelWnd, WM_COMMAND, MAKEWPARAM(ID_LABEL_ADDTOLABELVIEW, 0), 0);
				break;
			case LDD_FILTER:
				::SendMessage(m_labelWnd, WM_COMMAND, MAKEWPARAM(ID_LABELFILTER_SETVIEW, 0), 0);
				break;
			case LDD_FILTERREV:
				::SendMessage(m_labelWnd, WM_COMMAND, MAKEWPARAM(ID_LABELFILTER_SETVIEWREV, 0), 0);
				break;
			default:
				LabelDropMenu(TRUE);
				break;
			}
			return TRUE;
		}
		else if (m_DropTargetFlag == LABELNOSEL)
		{
			// Either post a message to the Label Window or pop up a menu
			switch (GET_P4REGPTR()->GetLabelDragDropOption())
			{
			case LDD_ADD:
			case LDD_DELETE:
			case LDD_SYNC:
				break;			// nothing was selected in the labelview
			case LDD_FILTER:
				::SendMessage(m_labelWnd, WM_COMMAND, MAKEWPARAM(ID_LABELFILTER_SETVIEW, 0), 0);
				break;
			case LDD_FILTERREV:
				::SendMessage(m_labelWnd, WM_COMMAND, MAKEWPARAM(ID_LABELFILTER_SETVIEWREV, 0), 0);
				break;
			default:
				LabelDropMenu(FALSE);
				break;
			}
			return TRUE;
		}
		else if (m_DropTargetFlag == CLIENTVIEW)
		{
			// Post a message to the Client Window so it adds the file(s) to the view
			OnAddToClientView();
			return TRUE;
		}
		else if (m_DropTargetFlag == USERVIEW)
		{
			// Post a message to the User Window so it adds the review file(s)
			OnAddReviews();
			return TRUE;
		}
		else if (m_DropTargetFlag == JOBVIEW)
		{
			// Post a message to the Job Window so it sets the filter
			OnFilterJobview();
			return TRUE;
		}
	}
	return FALSE;
}
	

/*
	_________________________________________________________________

	This window never accepts drops - just do this so drags 
	originated here dont have the ugly cant
	_________________________________________________________________
*/

int CDepotTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMultiSelTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetIndent(15);
	SetScrollTime(10);
	SetImageList(TheApp()->GetImageList(), TVSIL_NORMAL);

	
	return 0;
}


/*
	_________________________________________________________________
*/

DROPEFFECT CDepotTreeCtrl::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	m_DepotIsDropTarget=TRUE;
	m_DropEffect=DROPEFFECT_NONE;
	m_DragDataFormat=0;

	if(pDataObject->IsDataAvailable( (unsigned short) m_CF_DEPOT))
	{
		// Right-drag support

	    // We NEVER come in here with just DELETE_EXISTING_SELECTION, because
	    // that would leave us with an empty selection set
	    ASSERT( m_SelectOnDrag == KEEP_SELECTION ||
			    m_SelectOnDrag == ADD_TO_SELECTION ||
			    m_SelectOnDrag == (DELETE_EXISTING_SELECTION | ADD_TO_SELECTION) );

	    ASSERT( m_DragFromItem != NULL);

		if( m_SelectOnDrag & DELETE_EXISTING_SELECTION)
			UnselectAll();

		if( m_SelectOnDrag & ADD_TO_SELECTION )
			SetSelectState(m_DragFromItem, TRUE);

		m_DropEffect=DROPEFFECT_COPY;
	}
#ifdef UNICODE
	else if(pDataObject->IsDataAvailable( (unsigned short) CF_UNICODETEXT))
	{
		m_DropEffect = DROPEFFECT_COPY;
		m_DragDataFormat = CF_UNICODETEXT;
	}
#else
	else if(pDataObject->IsDataAvailable( (unsigned short) CF_TEXT))
	{
		m_DropEffect = DROPEFFECT_COPY;
		m_DragDataFormat = CF_TEXT;
	}
#endif
	m_DepotIsDropTarget=FALSE;
	return m_DropEffect;
}


/*
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnDragLeave() 
{
	m_PendingDeselect=FALSE;
}


/*
	_________________________________________________________________
*/

DROPEFFECT CDepotTreeCtrl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	if(pDataObject->IsDataAvailable( (unsigned short) m_CF_DEPOT))
	{
		// Left-drag support.  Dont clear pending deselect until the cursor
		// actually moves!
		CPoint pt= point;
		ClientToScreen( &pt );
		if( !m_DragSourceRect.PtInRect( pt ) )
			m_PendingDeselect=FALSE;
	}
	return m_DropEffect;
}

/*
	_________________________________________________________________
*/

BOOL CDepotTreeCtrl::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	CString fname;
	
	if(SERVER_BUSY())
	{
		// OnDragEnter() and OnDragOver() should avoid a drop at 
		// the wrong time!
		ASSERT(0);
		return FALSE;
	}
	
	m_DepotIsDropTarget=TRUE;
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
			if (itemStr.GetAt(0) != _T('/') && itemStr.GetAt(1) != _T(':'))
			{
				int i;
				if ((i = itemStr.Find(_T("//"))) > 0)
				{
					// we have something like "STATUS: 3:33:26 Sync preview: //depot/main/www/perforce/customers/bycountry.html#16 - updating c:\workspac\main\www\perforce\customers\bycountry.html"
					// so throw away everything before the "//"
					itemStr = itemStr.Right(itemStr.GetLength() - i);
					// if we find a 2nd "//", throw it and the following chars away
					if ((i = itemStr.Find(_T("//"), 2)) > 0)
						itemStr = itemStr.Left(i);
					itemStr.TrimRight();
				}
				else if ((i = itemStr.Find(_T(":\\"))) > 0)
				{
					// we have something like "STATUS: 11:48:04 Executing p4 where c:\workspac\P4CONFIG"
					// so throw away everything before the "c:\"
					itemStr = itemStr.Right(itemStr.GetLength() - i + 1);
					// if we find a 2nd "c:\", throw it and the following chars away
					if ((i = itemStr.Find(_T(":\\"), 3)) > 0)
						itemStr = itemStr.Left(i-1);
					itemStr.TrimRight();
				}
			}
			if ((itemStr.FindOneOf(_T("?*")) != -1) || (itemStr.Find(_T("...")) != -1))
				RunP4Files(itemStr);
			else
				ExpandDepotString( itemStr, TRUE );
		}
		m_DepotIsDropTarget=FALSE;
		return TRUE;
	}
	// Return false, so depot window doesnt start a file-open operation
	m_DepotIsDropTarget=FALSE;
	return FALSE;
}

/*
	_________________________________________________________________

	Context menu implementation.  Due to an apparent MFC bug, 
	two mouse click handlers are included to make this work.  
	Prolly in the next release of MFC, the default OnContextMenu 
	linkage can be used.
	_________________________________________________________________
*/
void CDepotTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// Save mouse location for last context menu
	m_ContextPoint = point;

	//		make sure window is active
	//
	GetParentFrame()->ActivateFrame();

	BOOL isDepot = FALSE ;

	m_ExpandDepotContinue = FALSE;

	//		find out what was hit, select it if it hasn't been
	//
	CString text;
	HTREEITEM currentItem;
	if (!pWnd && !point.x && !point.y)
	{
		currentItem = NULL;
		CRect rect;
//		MainFrame()->GetHLSplitter()->GetWindowRect(rect);
		int i = GetSystemMetrics(SM_CXSIZEFRAME) + 2;
		point.x = rect.left+i;
		point.y = rect.top+i;
	}
	else
	{
		SetItemAndPoint( currentItem, point );
		ClientToScreen( &point );

		if ( currentItem != NULL )
		{
			text = GetItemText ( currentItem );
			if ( text.Find( g_sSlashes ) == 0 )
				isDepot = TRUE;
		}

		if( !IsSelected( currentItem ))
		{
			UnselectAll( );
			if( currentItem != NULL )
				SetSelectState( currentItem, TRUE );
		}
	}

	//		create an empty context menu and some submenus
	//
	CP4Menu popMenu;
	popMenu.CreatePopupMenu();

	CP4Menu getMenu;
	getMenu.CreatePopupMenu();

	CP4Menu integMenu;
	integMenu.CreatePopupMenu();

	CP4Menu viewMenu;
	viewMenu.CreatePopupMenu();

	CP4Menu editMenu;
	editMenu.CreatePopupMenu();

	//		start putting in items in the menus
	//
	if( currentItem != NULL && currentItem != m_Root )
	{
		if ( !text.IsEmpty( ) && text.Find ( g_TrulyEmptyDir ) != -1 )
		{
			CP4Menu showdelMenu;
			showdelMenu.CreatePopupMenu();
			if (GET_P4REGPTR( )->ShowEntireDepot( ) != SDF_LOCALTREE )
				showdelMenu.AppendMenu( stringsON, ID_FILE_ADD, LoadStringResource( IDS_FILE_ADD ) );
			if (GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALP4
			 || GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALTREE)
			{
				getMenu.AppendMenu( stringsON, ID_FILE_GET, LoadStringResource( IDS_SYNCTOHEAD ) );
				getMenu.AppendMenu( stringsON, ID_FILE_GETWHATIF, LoadStringResource( IDS_SYNCTOHEADPREVIEW ) );
				getMenu.AppendMenu( stringsON, ID_FILE_GETCUSTOM );
				getMenu.AppendMenu( stringsON, ID_FILE_REFRESH );
				getMenu.AppendMenu( stringsON, ID_FILE_FORCESYNCTOHEAD, LoadStringResource( IDS_FILE_FORCESYNCTOHEAD ) );
				showdelMenu.AppendMenu(MF_POPUP, (UINT) getMenu.GetSafeHmenu(), LoadStringResource( IDS_SYNC ) );
				showdelMenu.AppendMenu(MF_SEPARATOR);
			}
			showdelMenu.AppendMenu( stringsON, ID_SHOWDELETED, LoadStringResource( IDS_SHOWDELITEMS ) );
			showdelMenu.TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON,point.x, point.y, AfxGetMainWnd() );
			return;
		}

		BOOL anyChildren= AnyHaveChildren();
		BOOL anyInview = AnyInView( );
		BOOL anyAddable  = AnyAddable();
		BOOL anyEditable = AnyEditable();
		BOOL anyNotCurrent = AnyNotCurrent( );
		BOOL allNotInDepot = AllNotInDepot( );
		BOOL needToAddSeparator = FALSE;

		// a single directory
		if( GetSelectedCount()==1 && !ITEM_IS_FILE(currentItem) )
		{
			popMenu.AppendMenu( stringsON, ID_FILE_ADD, LoadStringResource( IDS_FILE_ADD ) );
			needToAddSeparator = TRUE;
			point.y -= GetSystemMetrics(SM_CYMENUSIZE) * 2;
		}
		// a directory or a number of files, but not the root of the depot
		if( anyChildren || ( anyInview && anyNotCurrent )) 
		{
			getMenu.AppendMenu( stringsON, ID_FILE_GET, LoadStringResource( IDS_SYNCTOHEAD ) );
			getMenu.AppendMenu( stringsON, ID_FILE_GETWHATIF, LoadStringResource( IDS_SYNCTOHEADPREVIEW ) );
		}

		if( anyChildren || ( anyInview && !allNotInDepot ))
		{
			getMenu.AppendMenu( stringsON, ID_FILE_GETCUSTOM );
			getMenu.AppendMenu( stringsON, ID_FILE_REFRESH );
			getMenu.AppendMenu( stringsON, ID_FILE_FORCESYNCTOHEAD, LoadStringResource( IDS_FILE_FORCESYNCTOHEAD ) );
		}

		BOOL okToAddSeparator = FALSE;	// only used for 1st Separator
		if(!SERVER_BUSY() && getMenu.GetMenuItemCount() > 0
		 && ( (anyChildren && !m_FilterDepot) || (!anyChildren && (anyInview || AnyRemoveable()))))
		{
			if (needToAddSeparator)
			{
				needToAddSeparator = FALSE;
				popMenu.AppendMenu(MF_SEPARATOR);
			}
			popMenu.AppendMenu(MF_POPUP, (UINT) getMenu.GetSafeHmenu(), LoadStringResource( IDS_SYNC ) );
			okToAddSeparator = TRUE;
		}

		BOOL isRemoteFolder = anyChildren && IsInRemoteDepot(&(CString(GetCurrentItemPath())));

		if( (anyChildren && !m_FilterDepot && !isRemoteFolder) 
		 || (!anyChildren && (anyEditable || anyAddable || AnyOpenedForInteg()) ) )
		{
			DWORD id;
			okToAddSeparator = TRUE;
			if (anyChildren || GET_P4REGPTR()->ShowEntireDepot() != SDF_LOCALTREE)
				id = IDS_CHKOUTFOREDIT;
			else if (anyAddable && anyEditable)
				id = IDS_CHKOUTFOREDITORADD;
			else
			{
				id = anyAddable ? IDS_CHKOUTFORADD : IDS_CHKOUTFOREDIT;
				okToAddSeparator = anyEditable;
			}
			if (needToAddSeparator)
			{
				needToAddSeparator = FALSE;
				popMenu.AppendMenu(MF_SEPARATOR);
			}
			popMenu.AppendMenu( stringsON, 
				id == IDS_CHKOUTFORADD ? ID_FILE_OPENEDITA : ID_FILE_OPENEDIT, 
				LoadStringResource(id));
		}

		integMenu.AppendMenu( stringsON, ID_FILE_INTEGSPEC, LoadStringResource( IDS_USINGFILESPEC ) );
		integMenu.AppendMenu( stringsON, ID_FILE_INTEGRATE, LoadStringResource( IDS_USINGBRANCHSPEC ) );
		if(!SERVER_BUSY() && ( (anyChildren && !m_FilterDepot) || !anyChildren ) 
						  && ( anyChildren || !allNotInDepot ))
		{
			if (needToAddSeparator)
			{
				needToAddSeparator = FALSE;
				popMenu.AppendMenu(MF_SEPARATOR);
			}
			popMenu.AppendMenu(MF_POPUP, (UINT) integMenu.GetSafeHmenu(), LoadStringResource( IDS_INTEGRATE ) );
			if ((anyChildren || (anyEditable && anyInview)) && !isRemoteFolder)
				popMenu.AppendMenu( stringsON, ID_FILE_RENAME, LoadStringResource( IDS_RENAME_DOTS ) );
		}

		if(GetSelectedCount()==1 && !anyChildren && ITEM_IS_FILE(GetSelectedItem(0)))
		{
		    HTREEITEM item = GetSelectedItem(0);
			int index=GetLParam(item);
			CP4FileStats *fs=m_FSColl.GetStats(index);
			if (fs->GetMyOpenAction())
				popMenu.AppendMenu( stringsON, ID_CHANGE_SUBMIT, LoadStringResource( IDS_SUBMIT_DOTS ) );
		}

		if(popMenu.GetMenuItemCount() > 0 && okToAddSeparator)
			popMenu.AppendMenu(MF_SEPARATOR);

		if( (anyChildren && !m_FilterDepot && !isRemoteFolder) || (!anyChildren && anyEditable) )
		{
			popMenu.AppendMenu( stringsON, ID_FILE_OPENDELETE, LoadStringResource( IDS_CHKOUTFORDEL ) );
		}
		else if ( !anyChildren && AllAddable() )
		{
			popMenu.AppendMenu(MF_SEPARATOR);
			popMenu.AppendMenu( stringsON, ID_FILE_DELETE, LoadStringResource( IDS_FILE_DELETE ) );
			if(GetSelectedCount()==2)
			{
				popMenu.AppendMenu(MF_SEPARATOR);
				popMenu.AppendMenu( stringsON, ID_FILE_DIFF2, LoadStringResource(IDS_FILE_DIFF2) );
			}
		}

		if(!SERVER_BUSY() && ( (anyChildren && !m_FilterDepot) 
			               || ((anyInview || AnyRemoveable()) && !allNotInDepot && !anyChildren)))
		{
			popMenu.AppendMenu( stringsON, ID_FILE_REMOVE, LoadStringResource( IDS_RMVFROMCLIENT ) );
		}

		if( !anyChildren && AnyRecoverable() )
			popMenu.AppendMenu( stringsON, ID_FILE_RECOVER, LoadStringResource( IDS_RECOVERDELFILE ) );

		if( (anyChildren && !m_FilterDepot && !isRemoteFolder)  ||  (!anyChildren && AnyLockable()) )
			popMenu.AppendMenu( stringsON, ID_FILE_LOCK, LoadStringResource( IDS_LOCK ));
		if( (anyChildren && !m_FilterDepot && !isRemoteFolder)  ||  (!anyChildren && AnyUnlockable())  )
			popMenu.AppendMenu( stringsON, ID_FILE_UNLOCK, LoadStringResource( IDS_UNLOCK ));
		if( !anyChildren )  // if no directories
		{
			if(AnyOpened())
				popMenu.AppendMenu( stringsON, ID_FILE_REVERT, LoadStringResource( IDS_REVERT ));
		}
		
		if(GetSelectedCount()==1 && !anyChildren && ITEM_IS_FILE(GetSelectedItem(0)))
		{
			// Only one selected item and its a file
			if(popMenu.GetMenuItemCount() > 0)
				popMenu.AppendMenu(MF_SEPARATOR);

			TV_ITEM item;
			CP4FileStats *fs;
			item.hItem=GetSelectedItem(0);
			item.mask=TVIF_HANDLE| TVIF_CHILDREN | TVIF_PARAM;
			TreeView_GetItem(m_hWnd, &item );
			fs=m_FSColl.GetStats((int) item.lParam);

			if (!allNotInDepot)
			{
				popMenu.AppendMenu( stringsON, ID_FILE_DIFFHEAD, 
					LoadStringResource( AnyOpened() ? IDS_DIFFVSDEPOT : IDS_DIFFVSHEAD ) );
				popMenu.AppendMenu( stringsON, ID_FILE_DIFF2, LoadStringResource(IDS_FILE_DIFF2) );
			}
			
			UINT uFlags = MF_POPUP;
			int bAnn = MainFrame()->HaveTLV();
			if (bAnn && IsInRemoteDepot(&(CString(fs->GetFullDepotPath()))))
				bAnn = 0;
			if(!SERVER_BUSY() && !IsDeleted(currentItem))
			{
				// Build the edit and view submenues simultaneously.  We need two
				// nearly identical menues because the command IDs must be different
				// and there is no simple way to record which main menu item
				// popped the submenu.  Obviously the CMenu could be subclassed and
				// have a member that did something with the parent menu ID, but that
 				// hardly seems simpler than just repeating a few lines of code.
				//
				viewMenu.AppendMenu( stringsON, ID_FILE_QUICKBROWSE, LoadStringResource( IDS_ASSOCVIEWER ) );
				editMenu.AppendMenu( stringsON, ID_FILE_QUICKEDIT, LoadStringResource( IDS_ASSOCEDITOR ) );

				int actualMRUs=0;
				for(int i=0; i < MAX_MRU_VIEWERS; i++)
				{
					if( GET_P4REGPTR()->GetMRUViewerName(i).GetLength() > 0 )
					{
						CString viewer = GET_P4REGPTR()->GetMRUViewerName(i);
						viewMenu.AppendMenu( stringsON, ID_FILE_BROWSER_1+i, CString ( _T("&") + viewer ) );
						editMenu.AppendMenu( stringsON, ID_FILE_EDITOR_1+i, CString ( _T("&") + viewer ) );
						actualMRUs++;
					}
				}

				viewMenu.AppendMenu( stringsON, ID_FILE_NEWBROWSER, LoadStringResource ( IDS_OTHERVIEWER ) );
				editMenu.AppendMenu( stringsON, ID_FILE_NEWEDITOR, LoadStringResource ( IDS_OTHEREDITOR ));
				viewMenu.AppendMenu( stringsON, ID_FILE_RMVBROWSER, LoadStringResource ( IDS_RMVVIEWER ) );
				editMenu.AppendMenu( stringsON, ID_FILE_RMVEDITOR, LoadStringResource ( IDS_RMVEDITOR ));
				if (GET_SERVERLEVEL() >= 14)
				{
					viewMenu.AppendMenu(MF_SEPARATOR);
					viewMenu.AppendMenu( stringsON, ID_FILE_ANNOTATE, LoadStringResource ( IDS_FILE_ANNOTATE ) );
					viewMenu.AppendMenu( stringsON, ID_FILE_ANNOTATEALL, LoadStringResource ( IDS_FILE_ANNOTATEALL ) );
					viewMenu.AppendMenu( stringsON, ID_FILE_ANNOTATECHG, LoadStringResource ( IDS_FILE_ANNOTATECHG ) );
					viewMenu.AppendMenu( stringsON, ID_FILE_ANNOTATECHGALL, LoadStringResource ( IDS_FILE_ANNOTATECHGALL ) );
				}
			}
			else
			{
				if (GET_SERVERLEVEL() >= 14)
				{
					viewMenu.AppendMenu( stringsON, ID_FILE_ANNOTATEALL, LoadStringResource ( IDS_FILE_ANNOTATEALL ) );
					viewMenu.AppendMenu( stringsON, ID_FILE_ANNOTATECHGALL, LoadStringResource ( IDS_FILE_ANNOTATECHGALL ) );
				}
				else
					uFlags = MF_GRAYED | MF_DISABLED | MF_POPUP;
			}

			if(!SERVER_BUSY())
			{
				UINT i = !fs->IsMyOpen() && (fs->GetHaveRev() < fs->GetHeadRev())
					   ? IDS_VIEWDEPOTVERUSING : IDS_VIEWDEPOTVERUSINGHEAD;
				popMenu.AppendMenu(uFlags, (UINT)viewMenu.GetSafeHmenu(), LoadStringResource(i));
			}

			uFlags = IsDeleted(currentItem) ? MF_GRAYED | MF_DISABLED | MF_POPUP : MF_POPUP;
			if(!SERVER_BUSY() && GetSelectedCount()==1 && IsInView() && ITEM_IS_FILE(GetSelectedItem(0)))
			{
				BOOL b = TRUE;
				if (!anyEditable && !anyAddable)
				{
					b = !fs->IsOtherOpenExclusive();
					if (b)
						b = !IsInRemoteDepot(&(CString(fs->GetFullDepotPath())));
				}
				if(b)
					popMenu.AppendMenu(uFlags, (UINT) editMenu.GetSafeHmenu(), 
							LoadStringResource(anyEditable || anyAddable 
												? IDS_OPENANDEDITUSING : IDS_EDITUSING));
			}
			
			popMenu.AppendMenu( stringsON, ID_FILE_PROPERTIES, LoadStringResource( IDS_PROPERTIES ) );
			if (!allNotInDepot)
			{
				if (MainFrame()->HaveP4QTree())
					popMenu.AppendMenu( stringsON, ID_FILE_REVISIONTREE, LoadStringResource( IDS_REVISIONTREE ));
				if (bAnn == 1)
					popMenu.AppendMenu( stringsON, ID_FILE_ANNOTATIONS, LoadStringResource( IDS_ANNOTATIONS ));
				popMenu.AppendMenu( stringsON, ID_FILE_REVISIONHISTORY, LoadStringResource( IDS_REVISIONHISTORY ));
			}
		}
		else if(GetSelectedCount()==2 
			 && ((!anyChildren && ITEM_IS_FILE(GetSelectedItem(0)) && ITEM_IS_FILE(GetSelectedItem(1)))
			  || (anyChildren && !ITEM_IS_FILE(GetSelectedItem(0)) && !ITEM_IS_FILE(GetSelectedItem(1)))))
		{
			// Two selected items and they are both files or both dirs
			if (!allNotInDepot && !IsDepot(GetSelectedItem(0)) && !IsDepot(GetSelectedItem(1)))
			{
				if(popMenu.GetMenuItemCount() > 0)
					popMenu.AppendMenu(MF_SEPARATOR);
				popMenu.AppendMenu( stringsON, ID_FILE_DIFF2, 
					LoadStringResource(anyChildren ? IDS_FILE_DIFF2DIRS : IDS_FILE_DIFF2) );
			}
		}

		if( !anyChildren || GetSelectedCount()==1 )  // if no directories or only a single directory
		{
			if (anyChildren && !IsDepot(GetSelectedItem(0)))
			{
				popMenu.AppendMenu(MF_SEPARATOR);
				popMenu.AppendMenu( stringsON, ID_FILE_DIFF2, LoadStringResource(IDS_FILE_DIFF2DIRS) );
			}
			popMenu.AppendMenu( MF_SEPARATOR);
			if (GetSelectedCount()==1)
			{
				if ((AnyOpened() || AnyOtherOpened()) && ITEM_IS_FILE(GetSelectedItem(0)))
					popMenu.AppendMenu( stringsON, ID_POSITIONCHGS, LoadStringResource( IDS_POSITIONCHGS ) );
                popMenu.AppendMenu( stringsON, ID_ADD_BOOKMARK, LoadStringResource(IDS_ADD_BOOKMARK) );
			}
			popMenu.AppendMenu( stringsON, ID_WINEXPLORE, LoadStringResource( IDS_EXPLORE ) );
			popMenu.AppendMenu( stringsON, ID_CMDPROMPT, LoadStringResource( IDS_CMDPROMPT ) );
		}
	}

	if (GetSelectedCount()==1 && currentItem != NULL)
	{
		if (AnyHaveChildren())  // if is a directory
		{
			if(popMenu.GetMenuItemCount() > 0)
				popMenu.AppendMenu( MF_SEPARATOR );
			popMenu.AppendMenu( stringsON, ID_FINDFILEUNDERFOLDER, LoadStringResource(IDS_FINDFILEUNDERFOLDER) );
		}
	}

	MainFrame()->AddToolsToContextMenu(&popMenu);

	m_InContextMenu = TRUE;
	popMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	point.x, point.y, AfxGetMainWnd());
	m_InContextMenu = FALSE;
}

/*
	_________________________________________________________________

	Right click context menu is broken in CTreeViews - 
	a dbl right click is what it wants.  
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CPoint screenPt=point;
	ClientToScreen(&screenPt);
    m_ContextContext= MOUSEHIT;
	OnContextMenu(NULL, screenPt);
}


/*
	_________________________________________________________________

	Mimic explorer behavior for right-drag operations

	1) Find out what got clicked
	
	2) If there are no current selections, select item
		Else if current item is not selected
			If there are selections and no key flags, 
				replace current selections when drag starts
			If there are selections and ctrl or shift is down, 
				add to current selections when drag starts
				
	3) On drop success, pop a context menu over the target change, 
	to allow open for edit vs open for delete
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// make sure window is active
	GetParentFrame()->ActivateFrame();

	// Step 1: find out what was hit
	TV_HITTESTINFO ht;
	ht.pt=point;
	HTREEITEM currentItem=TreeView_HitTest( m_hWnd, &ht	);
	if(currentItem==NULL)
	{
		UnselectAll();
		return;
	}
		
	// Step 2: make selections or pending selections
	//         Note: m_DragFromItem and m_SelectOnDrag and GetLastSelection()
	//               will be used in OnDragEnter() to make necessary
	//               adjustments to the selection set
	m_SelectOnDrag= KEEP_SELECTION;
	m_DragFromItem= currentItem;

	if(GetSelectedCount()==0)
		SetSelectState(currentItem, TRUE);
	else if(!IsSelected(currentItem))
	{
		if( (nFlags & MK_SHIFT || nFlags & MK_CONTROL) &&
				TreeView_GetParent(m_hWnd, currentItem) == GetLastSelectionParent())
			m_SelectOnDrag= ADD_TO_SELECTION;
		else
			m_SelectOnDrag= ADD_TO_SELECTION | DELETE_EXISTING_SELECTION;
	}

	
	// Step 3: Start possible drag-drop action
	RECT rect;
	TreeView_GetItemRect(m_hWnd, currentItem, &rect, TRUE);
	ClientToScreen(&rect);
		
	// Dont actually send data - clipboard format is all the info target requires
	m_OLESource.DelayRenderData( (unsigned short) m_CF_DEPOT);
		
	if(m_OLESource.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE, &rect, NULL) == DROPEFFECT_COPY)
	{
		if (m_DropTargetFlag == PENDINGCHG)
		{
			// changes view accepted a drop - so get the target change number
			CPoint screen;
			m_OpenUnderChangeNumber= ::SendMessage(m_changeWnd, WM_GETDRAGTOCHANGENUM, (WPARAM) &screen, 0);
			
			// create an empty context menu
			CP4Menu popMenu;
			popMenu.CreatePopupMenu();

			popMenu.AppendMenu( stringsON, ID_FILEDROP_EDIT, LoadStringResource( IDS_CHKOUTFOREDIT ) );
			popMenu.AppendMenu( stringsON, ID_FILEDROP_DELETE, LoadStringResource( IDS_CHKOUTFORDEL ) );
			popMenu.AppendMenu(MF_SEPARATOR);
			popMenu.AppendMenu( stringsON, ID_FILEDROP_CANCEL, LoadStringResource( IDS_ampCANCEL ) );

			// Pause the auto refresh timer
			SET_APP_HALTED(TRUE);

			// Finally blast the menu onto the screen
			popMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	screen.x, screen.y, AfxGetMainWnd());

			// Make sure selection set is properly displayed
			SetAppearance(FALSE, TRUE, FALSE);

			//Release the auto refresh timer
			SET_APP_HALTED(FALSE);
		}
		else if (m_DropTargetFlag == SUBMITTEDCHG)
		{
			// submitted changes view accepted a drop
			// create a context menu
			::SendMessage(m_oldChgWnd, WM_COMMAND, MAKEWPARAM(ID_FILTER_SETVIEW, 0), 0);
		}
		else if (m_DropTargetFlag == LABELVIEW)
		{
			LabelDropMenu(TRUE);
		}
		else if (m_DropTargetFlag == LABELNOSEL)
		{
			LabelDropMenu(FALSE);
		}
		else if (m_DropTargetFlag == USERVIEW)
		{
			MessageBeep(0);
		}
		else MessageBeep(0);
	}
	else
		// Drag-drop ate the wm_rbuttonup
		OnRButtonUp(nFlags, point); 
}

void CDepotTreeCtrl::LabelDropMenu(BOOL bLabelSelected)
{
	// label view accepted a drop
	// tell the label view to create a context menu
	::SendMessage(m_labelWnd, WM_LABELDROPMENU, (WPARAM)bLabelSelected, (LPARAM)&m_DropTargetPt);

}

//////////////////////////////////////////////////////////////////////////////////////
// On update UI handlers

void CDepotTreeCtrl::OnUpdateFileLock(CCmdUI* pCmdUI) 
{
	BOOL anyChildren= AnyHaveChildren();
	BOOL isRemoteFolder = anyChildren && IsInRemoteDepot(&(CString(GetCurrentItemPath())));
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() 
		&& ((anyChildren && !m_FilterDepot && !isRemoteFolder) || (!anyChildren && AnyLockable()))));
}

void CDepotTreeCtrl::OnUpdateFileUnlock(CCmdUI* pCmdUI) 
{
	BOOL anyChildren= AnyHaveChildren();
	BOOL isRemoteFolder = anyChildren && IsInRemoteDepot(&(CString(GetCurrentItemPath())));
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() 
		&& ((anyChildren && !m_FilterDepot && !isRemoteFolder) || (!anyChildren && AnyUnlockable()))));
}

void CDepotTreeCtrl::OnUpdateFileInformation(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && GetSelectedCount()==1 && 
				   !IsSelected(m_Root) && 
				   ITEM_IS_FILE(GetSelectedItem(0)) );	
}

void CDepotTreeCtrl::OnUpdateFileOpendelete(CCmdUI* pCmdUI) 
{
	BOOL anyChildren= AnyHaveChildren();
	BOOL isRemoteFolder = anyChildren && IsInRemoteDepot(&(CString(GetCurrentItemPath())));
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() 
		&& ((anyChildren && !m_FilterDepot && !isRemoteFolder) || (!anyChildren && AnyEditable()))));
}

void CDepotTreeCtrl::OnUpdateFileOpenedit(CCmdUI* pCmdUI) 
{
	BOOL anyChildren = AnyHaveChildren();
	BOOL anyEditable = anyChildren ? TRUE : AnyEditable();
	BOOL anyAddable  = anyChildren ? TRUE : AnyAddable();
	BOOL isRemoteFolder = anyChildren && IsInRemoteDepot(&(CString(GetCurrentItemPath())));

	DWORD id;
	if (anyChildren || GET_P4REGPTR()->ShowEntireDepot() != SDF_LOCALTREE)
		id = m_InContextMenu ? IDS_CHKOUTFOREDIT : IDS_CHKOUTFOREDIT_CTRLE;
	else if (anyAddable && anyEditable)
		id = m_InContextMenu ? IDS_CHKOUTFOREDITORADD : IDS_CHKOUTFOREDITORADD_CTRLE;
	else if (anyAddable)
		id = IDS_CHKOUTFORADD;
	else
		id = m_InContextMenu ? IDS_CHKOUTFOREDIT : IDS_CHKOUTFOREDIT_CTRLE;
	pCmdUI->SetText(LoadStringResource(id));

	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() 
					&& ((anyChildren && !m_FilterDepot && !isRemoteFolder) 
									|| (!anyChildren && (anyEditable 
													  || anyAddable 
													  || AnyOpenedForInteg())))));	
}

void CDepotTreeCtrl::OnUpdateFiledropEdit(CCmdUI* pCmdUI) 
{
	BOOL anyChildren= AnyHaveChildren();
	BOOL isRemoteFolder = anyChildren && IsInRemoteDepot(&(CString(GetCurrentItemPath())));
	pCmdUI->Enable(!SERVER_BUSY() && ((anyChildren && !m_FilterDepot && !isRemoteFolder)
								   || (!anyChildren && AnyEditable())));
}

void CDepotTreeCtrl::OnUpdateFiledropDelete(CCmdUI* pCmdUI) 
{
	BOOL anyChildren= AnyHaveChildren();
	BOOL isRemoteFolder = anyChildren && IsInRemoteDepot(&(CString(GetCurrentItemPath())));
	pCmdUI->Enable(!SERVER_BUSY() && ((anyChildren && !m_FilterDepot && !isRemoteFolder)
								   || (!anyChildren && AnyEditable())));
}

void CDepotTreeCtrl::OnUpdateFiledropCancel(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

void CDepotTreeCtrl::OnUpdateFileRecover(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && GET_SERVERLEVEL() > 5 && 
		AnyRecoverable() && !AnyHaveChildren()  );	
}

void CDepotTreeCtrl::OnUpdateFileRevert(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() 
											&& AnyOpened() && !AnyHaveChildren()));
}

void CDepotTreeCtrl::OnUpdateFileRename(CCmdUI* pCmdUI) 
{
	BOOL anyChildren= AnyHaveChildren();
	BOOL isRemoteFolder = anyChildren && IsInRemoteDepot(&(CString(GetCurrentItemPath())));
	pCmdUI->Enable(!SERVER_BUSY() 
		&& ((anyChildren && !m_FilterDepot && !isRemoteFolder)
		 || (!anyChildren && (AnyInView() && !AllNotInDepot())))
		&& !IsSelected(m_Root) 
		&& !MainFrame()->IsModlessUp()
		&& !MainFrame()->IsPendChgEditInProgress()
		&& !AnyInRemoteDepot() );
}

void CDepotTreeCtrl::OnUpdateFileIntegrate(CCmdUI* pCmdUI) 
{
	BOOL anyChildren= AnyHaveChildren();
	pCmdUI->Enable(!SERVER_BUSY() 
		&& ((anyChildren && !m_FilterDepot) || !anyChildren)
		&& GetSelectedCount()
		&& !IsSelected(m_Root) 
		&& !MainFrame()->IsModlessUp()
		&& !MainFrame()->IsPendChgEditInProgress() );	
}

void CDepotTreeCtrl::OnUpdateFileGet(CCmdUI* pCmdUI) 
{
	BOOL anyChildren= AnyHaveChildren();
	BOOL b = !SERVER_BUSY() && !IsSelected(m_Root)
		&& ((anyChildren && !m_FilterDepot) || (!anyChildren && AnyNotCurrent() && AnyInView()));
	if (!b && !anyChildren && !ITEM_IS_FILE(GetSelectedItem(0)) 
		&& (GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALTREE 
		 || GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALP4))
	{
		CString text = GetItemText( GetSelectedItem(0) );
		b = text.Find( g_TrulyEmptyDir ) != -1 ? TRUE : FALSE;
	}
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, b));
}

void CDepotTreeCtrl::OnUpdateFileGetcustom(CCmdUI* pCmdUI) 
{
	// Can be called even if all files up to date, since we
	// may be getting at an old change number or label
	//
	pCmdUI->SetText( ( SERVER_IS_982_ORMORE ) 
			? LoadStringResource( IDS_SYNCTORLCD )
			: LoadStringResource( IDS_SYNCTORLC ) );
	BOOL anyChildren= AnyHaveChildren();
	BOOL b = !SERVER_BUSY() 
					&& !IsSelected(m_Root)
					&& GetSelectedCount() > 0 
					&& ((anyChildren && !m_FilterDepot) || (!anyChildren && AnyInView() && !AllNotInDepot()))
					&& !MainFrame()->IsModlessUp();
	if (!b && !anyChildren && !ITEM_IS_FILE(GetSelectedItem(0)) 
		&& (GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALTREE 
		 || GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALP4))
	{
		CString text = GetItemText( GetSelectedItem(0) );
		b = text.Find( g_TrulyEmptyDir ) != -1 ? TRUE : FALSE;
	}
	pCmdUI->Enable(b);
}

void CDepotTreeCtrl::OnUpdateGetwhatif(CCmdUI* pCmdUI) 
{
	BOOL anyChildren= AnyHaveChildren();
	BOOL b = !SERVER_BUSY() && !IsSelected(m_Root) &&
		((anyChildren && !m_FilterDepot) || (!anyChildren && AnyInView() && AnyNotCurrent()));
	if (!b && !anyChildren && !ITEM_IS_FILE(GetSelectedItem(0)) 
		&& (GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALTREE 
		 || GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALP4))
	{
		CString text = GetItemText( GetSelectedItem(0) );
		b = text.Find( g_TrulyEmptyDir ) != -1 ? TRUE : FALSE;
	}
	pCmdUI->Enable(b);
}

void CDepotTreeCtrl::OnUpdateFileRemove(CCmdUI* pCmdUI) 
{
	BOOL anyChildren= AnyHaveChildren();
	pCmdUI->Enable(!SERVER_BUSY() && !IsSelected(m_Root) &&
		((anyChildren && !m_FilterDepot) || (!anyChildren && AnyRemoveable())) );
}

void CDepotTreeCtrl::OnUpdateFileTimeLapse(CCmdUI* pCmdUI) 
{
	UpdateFileAnnotate(pCmdUI, TRUE);
}

void CDepotTreeCtrl::OnUpdateFileAnnotate(CCmdUI* pCmdUI) 
{
	UpdateFileAnnotate(pCmdUI, TRUE);
}

void CDepotTreeCtrl::UpdateFileAnnotate(CCmdUI* pCmdUI, BOOL bUnicodeOK) 
{
	BOOL enable = !SERVER_BUSY() && GET_SERVERLEVEL() >= 14
					&& GetSelectedCount()==1
					&& !IsSelected(m_Root) 
					&& ITEM_IS_FILE(GetSelectedItem(0))
					&& !AllNotInDepot();
	if( enable )
	{
		CP4FileStats *fs=m_FSColl.GetStats(GetLParam(GetSelectedItem(0)));
		CString fileType = fs->GetHeadType();
		enable = ((fileType.Find(_T("text")) != -1) 
			   || (fileType.Find(_T("symlink")) != -1)
			   || (bUnicodeOK && ((fileType.Find(_T("unicode")) != -1) 
							   || (fileType.Find(_T("utf16")) != -1)))) ? TRUE : FALSE;
	}
	pCmdUI->Enable(enable);
}

void CDepotTreeCtrl::OnUpdateFileRevisionhistory(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY()
					&& GetSelectedCount()==1
					&& !IsSelected(m_Root) 
#if 0	// define this to allow Rev Hist of folders
					&& (!ITEM_IS_FILE(GetSelectedItem(0)) || !AllNotInDepot())));
#else
					&& ITEM_IS_FILE(GetSelectedItem(0))
					&& !AllNotInDepot()));
#endif
}

void CDepotTreeCtrl::OnUpdateFileRevisiontree(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY()
					&& GetSelectedCount()==1
					&& !IsSelected(m_Root) 
					&& ITEM_IS_FILE(GetSelectedItem(0))
					&& !AllNotInDepot()));
}

void CDepotTreeCtrl::OnUpdatePositionDepot(CCmdUI* pCmdUI) 
{
	CString txt = LoadStringResource(IDS_POSITIONTOPATTERN);
	pCmdUI->SetText ( txt );
	pCmdUI->Enable(!SERVER_BUSY());
}

// This works for both Next and Prev
void CDepotTreeCtrl::OnUpdatePositionDepotNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && !m_P4Files_List.IsEmpty() 
		&& GetSelectedCount() && !GET_P4REGPTR()->SortByExtension());
}

void CDepotTreeCtrl::OnUpdatePositionChgs(CCmdUI* pCmdUI) 
{
	if (!m_InContextMenu)
		pCmdUI->SetText( LoadStringResource(IDS_POSITIONCHGS_DPTPANE) );
	pCmdUI->Enable(!SERVER_BUSY() && GetSelectedCount()==1 && (AnyOpened() || AnyOtherOpened()));
}

void CDepotTreeCtrl::OnUpdateShowDeletedFiles(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY());
	pCmdUI->SetCheck(GET_P4REGPTR()->ShowDeleted());
}

void CDepotTreeCtrl::OnUpdateWinExplore(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetSelectedCount() 
					&& ((!AnyHaveChildren() && AnyInView()) 
					   || GetSelectedCount()==1));
}

void CDepotTreeCtrl::OnUpdateCmdPrompt(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((GetSelectedCount() 
				&& ((!AnyHaveChildren() && AnyInView()) 
				   || GetSelectedCount()==1))
			|| !SERVER_BUSY());
}

void CDepotTreeCtrl::OnUpdateFileDiffhead(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY()
		&& GetSelectedCount()==1
		&& !IsSelected(m_Root) && !AnyHaveChildren()
		&& (AnyOpened() || AnyRemoveable())));	
}

void CDepotTreeCtrl::OnUpdateFileDiff2(CCmdUI* pCmdUI) 
{
	int cnt = GetSelectedCount();
	pCmdUI->Enable(!SERVER_BUSY() && cnt && !IsSelected(m_Root) && !IsDepot(GetSelectedItem(0))
		        && ((cnt==1)
				 || (cnt==2 && !IsDepot(GetSelectedItem(1))
				  && (ITEM_IS_FILE(GetSelectedItem(0)) == ITEM_IS_FILE(GetSelectedItem(1))))));
}

void CDepotTreeCtrl::OnUpdateFileRefresh(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText( LoadStringResource( IDS_REDOSYNCTOSAME ) );

	BOOL anyChildren= AnyHaveChildren();
	// Can be called for any files
	BOOL b = !SERVER_BUSY() && GetSelectedCount() > 0
		&& ((anyChildren && !m_FilterDepot) || (!anyChildren && AnyInView() && !AllNotInDepot()));
	if (!b && !anyChildren && !ITEM_IS_FILE(GetSelectedItem(0)) 
		&& (GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALTREE 
		 || GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALP4))
	{
		CString text = GetItemText( GetSelectedItem(0) );
		b = text.Find( g_TrulyEmptyDir ) != -1 ? TRUE : FALSE;
	}
	pCmdUI->Enable(b);
}

void CDepotTreeCtrl::OnUpdateFileForceToHead(CCmdUI* pCmdUI) 
{
	BOOL anyChildren= AnyHaveChildren();
	// Can be called for any files
	BOOL b = !SERVER_BUSY() && GET_SERVERLEVEL( ) > 3 && GetSelectedCount() > 0
		&& ((anyChildren && !m_FilterDepot) || (!anyChildren && AnyInView() && !AllNotInDepot()));
	if (!b && !anyChildren && !ITEM_IS_FILE(GetSelectedItem(0)) 
		&& (GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALTREE 
		 || GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALP4))
	{
		CString text = GetItemText( GetSelectedItem(0) );
		b = text.Find( g_TrulyEmptyDir ) != -1 ? TRUE : FALSE;
	}
	pCmdUI->Enable(b);
}

void CDepotTreeCtrl::OnUpdateFileSubmit(CCmdUI* pCmdUI) 
{
	BOOL b = FALSE;
	HTREEITEM item;
	if(GetSelectedCount()==1 && ITEM_IS_FILE(item = GetSelectedItem(0)) && !AnyHaveChildren())
	{
		int index=GetLParam(item);
		CP4FileStats *fs=m_FSColl.GetStats(index);
		if (fs->GetMyOpenAction())
			b = !SERVER_BUSY();
	}
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, b));
}


////////////////////////////////////////////////////////
// Utility functions

int CDepotTreeCtrl::LeafSelectedCount()
{
	int rc;
	int count=0;

	for(int i=GetSelectedCount()-1; i >= 0; i--)
	{
		HTREEITEM item= GetSelectedItem(i);
		rc= GetLeafCount(item);
		if (rc < 0)
			return(-1);	// count cannot be determined at this time
		count+= rc;
	}
	return count;
}

// Warning: next function recurses
int CDepotTreeCtrl::GetLeafCount(HTREEITEM item)
{
	int rc;
	int count=0;
	if(HasChildren(item))
	{
		HTREEITEM cItem= TreeView_GetChild(m_hWnd, item);
		if (cItem == NULL)
			return(-1);	// count cannot be determined at this time - item never expanded
		while(cItem != NULL)
		{
			rc= GetLeafCount(cItem);
			if (rc < 0)
				return(-1);	// count cannot be determined at this time
			count+= rc;
			cItem= TreeView_GetNextSibling(m_hWnd, cItem);
		}
	}
	else
		count=1;

	return count;
}


BOOL CDepotTreeCtrl::AnyHaveChildren()
{
	int idleFlag;
	if (((idleFlag = TheApp()->m_IdleFlag) != 0)
	  && (idleFlag == m_FlgHaveChildren))
		return m_AnyHaveChildren;
	m_FlgHaveChildren = idleFlag;

	m_AnyHaveChildren=FALSE;
	TV_ITEM item;

	for( int i=GetSelectedCount()-1; i>=0; i-- )
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE| TVIF_CHILDREN | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );	
		if( item.cChildren ==1 )   // a directory
		{
			m_AnyHaveChildren=TRUE;
			break;
		}
	}
	return m_AnyHaveChildren;
}

BOOL CDepotTreeCtrl::HasChildren(HTREEITEM currentItem)
{
	BOOL hasChildren=FALSE;
	TV_ITEM item;

	if(currentItem != NULL)
	{
		item.hItem=currentItem;
		item.mask=TVIF_HANDLE| TVIF_CHILDREN | TVIF_PARAM;
		if(TreeView_GetItem(m_hWnd, &item ) && item.cChildren ==1)   // a directory
			hasChildren=TRUE;
	}
	return hasChildren;
}


BOOL CDepotTreeCtrl::AnyRemoveable()
{
	BOOL current=FALSE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if(fs->GetHaveRev() > 0 && fs->GetMyOpenAction()==0)
			{
				current=TRUE;
				break;
			}
		}
	}
	return current;
}

BOOL CDepotTreeCtrl::AnyInView()
{
	int idleFlag;
	if (((idleFlag = TheApp()->m_IdleFlag) != 0)
	  && (idleFlag == m_FlgInView))
		return m_AnyInView;
	m_FlgInView = idleFlag;

	m_AnyInView=FALSE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if(fs->InClientView())
			{
				m_AnyInView=TRUE;
				break;
			}
		}
	}
	return m_AnyInView;
}

BOOL CDepotTreeCtrl::AllInView()
{
	int idleFlag;
	if (((idleFlag = TheApp()->m_IdleFlag) != 0)
	  && (idleFlag == m_FlgAllInView))
		return m_AllInView;
	m_FlgAllInView = idleFlag;

	m_AllInView=TRUE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if(!fs->InClientView())
			{
				m_AllInView=FALSE;
				break;
			}
		}
	}
	return m_AllInView;
}

BOOL CDepotTreeCtrl::AnyNotCurrent()
{
	int idleFlag;
	if (((idleFlag = TheApp()->m_IdleFlag) != 0)
	  && (idleFlag == m_FlgNotCurrent))
		return m_AnyNotCurrent;
	m_FlgNotCurrent = idleFlag;

	m_AnyNotCurrent=FALSE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if(fs->GetHaveRev() < fs->GetHeadRev())
			{
				m_AnyNotCurrent=TRUE;
				break;
			}
		}
	}
	return m_AnyNotCurrent;
}


BOOL CDepotTreeCtrl::AllNotInDepot()
{
	if (GET_P4REGPTR( )->ShowEntireDepot( ) != SDF_LOCALTREE)
		return FALSE;

	int idleFlag;
	if (((idleFlag = TheApp()->m_IdleFlag) != 0)
	  && (idleFlag == m_FlgAllNotInDepot))
		return m_AllNotInDepot;
	m_FlgAllNotInDepot = idleFlag;

	m_AllNotInDepot=TRUE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if(!fs->IsNotInDepot())
			{
				m_AllNotInDepot=FALSE;
				break;
			}
		}
	}
	return m_AllNotInDepot;
}


BOOL CDepotTreeCtrl::AnyInRemoteDepot()
{
	if (m_RemoteDepotList.IsEmpty())
		return FALSE;

	BOOL anyInRemote=FALSE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			CString path = fs->GetFullDepotPath();
			if(IsInRemoteDepot(&path))
			{
				anyInRemote=TRUE;
				break;
			}
		}
	}
	return anyInRemote;
}


BOOL CDepotTreeCtrl::AnyAddable()
{
	if (GET_P4REGPTR( )->ShowEntireDepot( ) != SDF_LOCALTREE)
		return FALSE;

	int idleFlag;
	if (((idleFlag = TheApp()->m_IdleFlag) != 0)
	  && (idleFlag == m_FlgAddable))
		return m_AnyAddable;
	m_FlgAddable = idleFlag;

	m_AnyAddable=FALSE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE| TVIF_CHILDREN | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if(fs->GetMyOpenAction()== 0 && fs->IsNotInDepot())
			{
				m_AnyAddable=TRUE;
				break;
			}
		}
	}
	return m_AnyAddable;
}


BOOL CDepotTreeCtrl::AllAddable()
{
	if (GET_P4REGPTR( )->ShowEntireDepot( ) != SDF_LOCALTREE)
		return FALSE;

	int idleFlag;
	if (((idleFlag = TheApp()->m_IdleFlag) != 0)
	  && (idleFlag == m_FlgAllAddable))
		return m_AllAddable;
	m_FlgAllAddable = idleFlag;

	m_AllAddable=TRUE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if(fs->GetMyOpenAction() || !fs->IsNotInDepot())
			{
				m_AllAddable=FALSE;
				break;
			}
		}
		else
		{
			m_AllAddable=FALSE;
			break;
		}
	}
	return m_AllAddable;
}


BOOL CDepotTreeCtrl::AnyEditable()
{
	int idleFlag;
	if (((idleFlag = TheApp()->m_IdleFlag) != 0)
	  && (idleFlag == m_FlgEditable))
		return m_AnyEditable;
	m_FlgEditable = idleFlag;

	m_AnyEditable=FALSE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE| TVIF_CHILDREN | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if(fs->GetMyOpenAction()==0 && fs->GetHeadRev() != 0 
				&& fs->GetHaveRev() != 0 && fs->InClientView()
				&& !fs->IsOtherOpenExclusive()
				&& !IsInRemoteDepot(&(CString(fs->GetFullDepotPath()))))
			{
				m_AnyEditable=TRUE;
				break;
			}
		}
	}
	return m_AnyEditable;
}


BOOL CDepotTreeCtrl::AnyOpenedForInteg()
{
	BOOL editable=FALSE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE| TVIF_CHILDREN | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if(fs->GetMyOpenAction()==F_INTEGRATE && fs->GetHeadRev() != 0 
				&& fs->GetHaveRev() != 0 && fs->InClientView())
			{
				editable=TRUE;
				break;
			}
		}
	}
	return editable;
}


BOOL CDepotTreeCtrl::AnyLockable()
{
	int idleFlag;
	if (((idleFlag = TheApp()->m_IdleFlag) != 0)
	  && (idleFlag == m_FlgLockable))
		return m_AnyLockable;
	m_FlgLockable = idleFlag;

	m_AnyLockable=FALSE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE| TVIF_CHILDREN | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if(fs->GetMyOpenAction() > 0 && !fs->IsMyLock() && !fs->IsOtherLock() )
			{
				m_AnyLockable=TRUE;
				break;
			}
		}
	}
	return m_AnyLockable;
}

BOOL CDepotTreeCtrl::AnyRecoverable()
{
	BOOL recoverable=FALSE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE| TVIF_CHILDREN | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if( fs->GetHeadAction() == F_DELETE && 
				fs->GetMyOpenAction()==0 && 
				fs->GetHeadRev() != 0 && /*fs->GetHaveRev() != 0 &&*/
				fs->GetHaveRev() < fs->GetHeadRev() && 
				fs->InClientView())
			{
				recoverable=TRUE;
				break;
			}
		}
	}
	return recoverable;
}

BOOL CDepotTreeCtrl::AnyUnlockable()
{
	int idleFlag;
	if (((idleFlag = TheApp()->m_IdleFlag) != 0)
	  && (idleFlag == m_FlgUnlockable))
		return m_AnyUnlockable;
	m_FlgUnlockable = idleFlag;

	m_AnyUnlockable=FALSE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE| TVIF_CHILDREN | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if( fs->IsMyLock() )
			{
				m_AnyUnlockable=TRUE;
				break;
			}
		}
	}
	return m_AnyUnlockable;
}

BOOL CDepotTreeCtrl::AnyOpened()
{
	int idleFlag;
	if (((idleFlag = TheApp()->m_IdleFlag) != 0)
	  && (idleFlag == m_FlgOpened))
		return m_AnyOpened;
	m_FlgOpened = idleFlag;

	m_AnyOpened=FALSE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE| TVIF_CHILDREN | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if(fs->GetMyOpenAction() > 0)
			{
				m_AnyOpened=TRUE;
				break;
			}
		}
	}
	return m_AnyOpened;
}

BOOL CDepotTreeCtrl::AnyDeleted()
{
	BOOL someDeleted=FALSE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if ((fs->GetHeadAction() == F_DELETE) && (fs->GetHaveRev() == 0))
			{
				someDeleted=TRUE;
				break;
			}
		}
	}
	return someDeleted;
}

BOOL CDepotTreeCtrl::IsDeleted(HTREEITEM currentItem)
{
	BOOL deleted=FALSE;
	TV_ITEM item;
	CP4FileStats *fs;

	if(currentItem != NULL)
	{
		item.hItem=currentItem;
		item.mask=TVIF_HANDLE| TVIF_CHILDREN | TVIF_PARAM;

		if(TreeView_GetItem(m_hWnd, &item ) && ITEM_IS_A_FILE_NOT_A_SUBDIR)
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if ((fs->GetHeadAction() == F_DELETE) && (fs->GetHaveRev() == 0))
				deleted=TRUE;
		}
	}
	return deleted;
}

BOOL CDepotTreeCtrl::IsOpened(HTREEITEM currentItem)
{
	BOOL opened=FALSE;
	TV_ITEM item;
	CP4FileStats *fs;

	if(currentItem != NULL)
	{
		item.hItem=currentItem;
		item.mask=TVIF_HANDLE| TVIF_CHILDREN | TVIF_PARAM;

		if(TreeView_GetItem(m_hWnd, &item ) && ITEM_IS_A_FILE_NOT_A_SUBDIR)
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if(fs->GetMyOpenAction() > 0)
				opened=TRUE;
		}
	}
	return opened;
}

BOOL CDepotTreeCtrl::AnyOtherOpened()
{
	BOOL opened=FALSE;
	TV_ITEM item;
	CP4FileStats *fs;

	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		item.hItem=GetSelectedItem(i);
		item.mask=TVIF_HANDLE| TVIF_CHILDREN | TVIF_PARAM;
		TreeView_GetItem(m_hWnd, &item );
		if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
		{
			fs=m_FSColl.GetStats((int) item.lParam);
			if(fs->GetOtherOpenAction() > 0)
			{
				opened=TRUE;
				break;
			}
		}
	}
	return opened;
}

BOOL CDepotTreeCtrl::IsInView()
{
	BOOL InView=FALSE;
	TV_ITEM item;
	CP4FileStats *fs;

	// Is the one and only selection a file stranded outside
	// the current client view? 
	if(GetSelectedCount() != 1)
		{ ASSERT(0); return InView; }

	item.hItem=GetSelectedItem(0);
	item.mask=TVIF_HANDLE | TVIF_PARAM;
	TreeView_GetItem(m_hWnd, &item );
	if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
	{
		fs=m_FSColl.GetStats((int) item.lParam);
		if(fs->InClientView())
			InView=TRUE;
		
	}
	return InView;
}


/*
	_________________________________________________________________

	Produce a dialog showing fstat and opened file information for
	the selected file.  This info is all retrieved from the depot
	and pending changes windows.
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnFileInformation() 
{
	// Find the file and get its name 
	HTREEITEM currentItem=GetLastSelection(); 
	if (currentItem == NULL || GetSelectedCount() > 1 || HasChildren( currentItem) )
	{
		ASSERT(0);
		return;
	}

	AssembleStringList( &m_StringList );
	
	CCmd_Opened *pCmd= new CCmd_Opened;
	pCmd->Init( m_hWnd, RUN_ASYNC);
	pCmd->SetItemRef( currentItem );
	pCmd->SetAlternateReplyMsg( WM_P4FILEINFORMATION );

	if( pCmd->Run( TRUE, FALSE, -1, &m_StringList ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_FILE_INFORMATION) );
	else
		delete pCmd;
}


LRESULT CDepotTreeCtrl::OnP4FileInformation( WPARAM wParam, LPARAM lParam )
{
	CCmd_Opened *pCmd= (CCmd_Opened *) wParam;
	
	if(!pCmd->GetError())
	{
		HTREEITEM currentItem= pCmd->GetItemRef();

		// Initialize the file info dialog
		CFileInfoDlg *dlg = new CFileInfoDlg(this);

		CP4FileStats *stats= m_FSColl.GetStats(GetLParam(currentItem));
		dlg->m_DepotPath= stats->GetFullDepotPath();
		dlg->m_ClientPath= stats->GetFullClientPath();
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
		CString thisuser=GET_P4REGPTR()->GetMyID();
		
		if(stats->IsMyLock())
			dlg->m_LockedBy= thisuser;
		
		// Add to list other users who have this file open
		//
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
				str += _T(" ") + LoadStringResource(IDS_STAR_LOCKED);
			
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

LRESULT CDepotTreeCtrl::OnP4EndFileInformation( WPARAM wParam, LPARAM lParam )
{
	CFileInfoDlg *dlg = (CFileInfoDlg *)lParam;
	dlg->DestroyWindow();
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Support for file viewing and editing
///////////////////////////////////////////////////////////////////////////////////////////////
//
// Command UI update functions:
//		OnUpdateFileAutoBrowse(CCmdUI* pCmdUI) 
//
//		OnUpdateFileAutoedit(CCmdUI* pCmdUI) 
//		----> AutoEditPossible(BOOL *mustWaitForServer)
//
// Main Menu command handlers
//		OnFileQuickedit()		(will use associated app)
//		----> RunAssocViewer(BOOL editing)
//
//		OnFileAutoedit()		(will start with chooser dialog)
//		OnFileAutobrowse()			(will start with chooser dialog)
//		----> ChooseAndRunViewer(BOOL editing)
//
// Context Menu command handlers
//		OnFileMRUEditor(UINT  nID)
//		OnFileMRUBrowser(UINT  nID)
//		----> RunMRUViewer(UINT  nID, BOOL editing)
//
//		OnFileNewEditor()
//		OnFileNewBrowser()		
//		----> FindAndRunNewViewer(BOOL editing)
//
//		OnFileQuickedit()
//		OnFileQuickbrowse()	(will use associated app)
//		----> RunAssocViewer(BOOL editing)
//
// Command goes to server
//		GetViewerFile(LPCTSTR appPath)
//		----> CP4::PrepBrowse(HWND replywnd, LPCTSTR depotPathRev) (only 1st time)
//		----> CP4::PrepEdit(HWND replywnd, LPCTSTR depotPath) (only if reqd)
//		----> straight to RunViewer() if we already have file on client
//
// Application spawn actually happens here
//		OnP4PrepareForViewer(WPARAM wParam, LPARAM lParam)
//		----> RunViewer()
//			calls viewer dlg if error spawning app
//
///////////////////////////////////////////////////////////////////////////////////////////////


void CDepotTreeCtrl::OnUpdateFileAutobrowse(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY()
		&& GetSelectedCount()==1
		&& !IsSelected(m_Root)
		&& ITEM_IS_FILE(GetSelectedItem(0))
		&& !IsDeleted(GetSelectedItem(0))));
}

void CDepotTreeCtrl::OnUpdateFileAutoedit(CCmdUI* pCmdUI) 
{
	BOOL mustWaitForServer;
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, AutoEditPossible(&mustWaitForServer)
		&& !mustWaitForServer && !SERVER_BUSY()
		&& !IsDeleted(GetSelectedItem(0))));
}

BOOL CDepotTreeCtrl::AutoEditPossible(BOOL *mustWaitForServer)
{
	BOOL canEdit=FALSE;
	
	if(GetSelectedCount() == 1 && !IsSelected(m_Root) && !AnyHaveChildren() )
	{
		// If we already have it open for edit, there should
		// be no trouble editing it
		if(IsOpened(GetSelectedItem(0)))
		{
			canEdit=TRUE;
			*mustWaitForServer=FALSE;
		}
		// If in client view, we should be able to edit. Server must not
		// be busy because an edit and possibly a get will be required.
		// Also must not be +l filetype that is opened by someone else.
		else 
		{
			TV_ITEM item;
			CP4FileStats *fs;

			item.hItem=GetSelectedItem(0);
			item.mask=TVIF_HANDLE| TVIF_CHILDREN | TVIF_PARAM;
			TreeView_GetItem(m_hWnd, &item );
			if ( ITEM_IS_A_FILE_NOT_A_SUBDIR )
			{
				fs=m_FSColl.GetStats((int) item.lParam);
				if(fs->InClientView() && !fs->IsOtherOpenExclusive())
				{
					canEdit=TRUE;
					*mustWaitForServer=SERVER_BUSY();
				}
			}
		}
	}
	return canEdit;
}

///////////
// auto run  means to allow choosing of the viewer app, and then automatically 
//			get the head rev and fire up that viewer app

void CDepotTreeCtrl::OnFileAutoedit() 
{
	BOOL mustWaitForServer;
	if(!AutoEditPossible(&mustWaitForServer) || mustWaitForServer)
		return;

	if(	GET_P4REGPTR()->GetWarnAlreadyOpened() && AnyOtherOpened())
	{
		// Prevent anything from running while dlg is up
		SET_APP_HALTED(TRUE);
		if(AfxMessageBox(IDS_ONE_OR_MORE_FILES_HAVE_BEEN_OPENED_BY_OTHER_USERS, 
						MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
		{
			SET_APP_HALTED(FALSE);
			return;
		}
		SET_APP_HALTED(FALSE);
	}

	m_ViewItem=GetLastSelection();
	m_ViewFilePath.Empty();
	m_Editing=TRUE;
	ChooseAndRunViewer();
}

void CDepotTreeCtrl::OnFileAutobrowse() 
{
	m_ViewItem=GetLastSelection(); 
	m_ViewFilePath.Empty();
	m_Editing=FALSE;
	ChooseAndRunViewer();
}

void CDepotTreeCtrl::ChooseAndRunViewer()
{
	if (m_ViewItem == NULL && m_ViewFilePath.IsEmpty())
		ASSERT(0);
	else
	{
		// Ask the user to pick a viewer
		CViewerDlg dlg;
		if(dlg.DoModal() == IDCANCEL)
			return;

		m_Viewer=dlg.GetViewer();
		if(m_Viewer != _T("SHELLEXEC"))
			GET_P4REGPTR()->AddMRUViewer(m_Viewer);

		PrepareForViewer();
	}
}

//////////
// MRU versions will use the selected MRU viewer and will then automatically
//			get the head rev and run the viewer app

void CDepotTreeCtrl::OnFileMRUEditor(UINT  nID)
{
	BOOL mustWaitForServer;
	if(!AutoEditPossible(&mustWaitForServer) || mustWaitForServer)
		return;

	if(	GET_P4REGPTR()->GetWarnAlreadyOpened() && AnyOtherOpened())
	{
		if(AfxMessageBox(IDS_ONE_OR_MORE_FILES_HAVE_BEEN_OPENED_BY_OTHER_USERS, 
						MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
			return;
	}

	m_ViewItem=GetLastSelection(); 
	m_ViewFilePath.Empty();
	m_Editing=TRUE;
	RunMRUViewer(nID - ID_FILE_EDITOR_1);
}

void CDepotTreeCtrl::OnFileMRUBrowse(UINT  nID)
{
	m_ViewItem=GetLastSelection(); 
	m_ViewFilePath.Empty();
	m_Editing=FALSE;
	RunMRUViewer(nID - ID_FILE_BROWSER_1);
}

void CDepotTreeCtrl::RunMRUViewer(UINT nID)
{
	ASSERT(nID >= 0);

	if (m_ViewItem == NULL && m_ViewFilePath.IsEmpty())
		ASSERT(0);
	else
	{
		m_Viewer= GET_P4REGPTR()->GetMRUViewer( nID );
		GET_P4REGPTR()->AddMRUViewer(m_Viewer);
		PrepareForViewer();
	}
}


//////////
// New Editor versions will run common file dialog to find a viewer, add that
//		viewer to the MRU list, then viewer and will then automatically
//			get the head rev and run the viewer app

void CDepotTreeCtrl::OnFileNewEditor()
{
	BOOL mustWaitForServer;
	if(!AutoEditPossible(&mustWaitForServer) || mustWaitForServer)
		return;

	if(	GET_P4REGPTR()->GetWarnAlreadyOpened() && AnyOtherOpened())
	{
		if(AfxMessageBox(IDS_ONE_OR_MORE_FILES_HAVE_BEEN_OPENED_BY_OTHER_USERS, 
						MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
			return;
	}

	m_ViewItem=GetLastSelection(); 
	m_ViewFilePath.Empty();
	m_Editing=TRUE;
	FindAndRunNewViewer();
}

void CDepotTreeCtrl::OnFileNewBrowser()
{
	m_ViewItem=GetLastSelection(); 
	m_ViewFilePath.Empty();
	m_Editing=FALSE;
	FindAndRunNewViewer();
}

void CDepotTreeCtrl::FindAndRunNewViewer()
{
	if (m_ViewItem == NULL && m_ViewFilePath.IsEmpty())
		ASSERT(0);
	else
	{
		// Fire up a common dlg to find new file
		CFileDialog fDlg(TRUE, _T("exe"), NULL,  
			OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST,
			LoadStringResource(IDS_VIEWER_FILTER), this,
			MainFrame()->m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME)); 

		// Set the dlg caption
        CString title = LoadStringResource(IDS_FIND_FILE_EDITOR_VIEWER);
		fDlg.m_ofn.lpstrTitle = title;
		// We dont need no stinking file title
		fDlg.m_ofn.lpstrFileTitle=NULL;

		if(fDlg.DoModal() == IDOK)
		{
			m_Viewer= fDlg.GetPathName();
			if(m_Viewer.GetLength() > 0)
			{
				// User selected a valid viewer, so try to run it
				GET_P4REGPTR()->AddMRUViewer(m_Viewer);
				RunMRUViewer(0);
			}
		}
	}
}

void CDepotTreeCtrl::OnUpdateRemoveViewer(CCmdUI* pCmdUI)
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

void CDepotTreeCtrl::OnRemoveViewer()
{
	CRemoveViewer dlg;
	dlg.DoModal();
}


//////////
// Run the associated viewer on the head rev - view only

LRESULT CDepotTreeCtrl::OnViewHead(WPARAM wParam, LPARAM lParam)
{
	m_Viewer= _T("SHELLEXEC");

	// Fetch the head revision of the file to a temp filename
	CString itemStr= (TCHAR*)wParam;
	CP4FileStats *fs = (CP4FileStats *)lParam;
	CCmd_PrepBrowse *pCmd= new CCmd_PrepBrowse;
	pCmd->Init( m_hWnd, RUN_ASYNC);
	pCmd->SetFileType(fs->IsTextFile() ? FST_TEXT : FST_BINARY);
	CString fType = fs->GetHeadType();
	if( pCmd->Run( itemStr, fType, fs->GetHeadRev(), TheApp()->m_bNoCRLF ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_FETCHING_FILE) );
	else
		delete pCmd;
	return 0;
}

//////////
// QuickEdit versions will automatically get the head rev and run the associated viewer app

void CDepotTreeCtrl::OnFileQuickedit() 
{
	BOOL mustWaitForServer;
	if(!AutoEditPossible(&mustWaitForServer) || mustWaitForServer)
		return;
	
	if(	GET_P4REGPTR()->GetWarnAlreadyOpened() && AnyOtherOpened())
	{
		if(AfxMessageBox(IDS_ONE_OR_MORE_FILES_HAVE_BEEN_OPENED_BY_OTHER_USERS, 
						MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
			return;
	}

	m_ViewItem=GetLastSelection(); 
	m_ViewFilePath.Empty();
	m_Editing=TRUE;
	RunAssocViewer();
}

void CDepotTreeCtrl::OnFileQuickbrowse()
{
	m_ViewItem=GetLastSelection(); 
	m_ViewFilePath.Empty();
	m_Editing=FALSE;
	RunAssocViewer();
}

void CDepotTreeCtrl::RunAssocViewer()
{
	if (m_ViewItem == NULL && m_ViewFilePath.IsEmpty())
		ASSERT(0);
	else
	{
		m_Viewer=_T("SHELLEXEC");
		PrepareForViewer();
	}
}


void CDepotTreeCtrl::PrepareForViewer()
{
	ASSERT(!m_Viewer.IsEmpty());

	// Shortcut for edit requests coming from Delta view, because
	// there is no file lookup required, and we know for certain
	// that the file is open; plus we don't have access to 'fs'
	if( m_ViewItem == NULL && !m_ViewFilePath.IsEmpty() )
	{
		if (m_Editing)
			RunViewer();
		else	// it's opened for integrate
		{
			int i;
			DWORD errorCode  = 0;
            CString tempPath = GET_P4REGPTR()->GetTempDir();
            CString fileName = m_ViewFilePath;
            CString filePath;

			if ((i = ReverseFindMBCS(fileName, _T('\\'))) != -1)
				fileName = fileName.Mid(i+1);
	    	for( i=0; i< 100; i++)
	        {
		        filePath.Format(_T("%s\\ReadOnly-%d-Rev-Integ-%s"), tempPath, i, fileName);
				int j;
				while ((j = filePath.Find(':', 2)) != -1)
					filePath.SetAt(j, '_');
				while ((j = filePath.FindOneOf(_T("/*?\"<>|"))) != -1)
					filePath.SetAt(j, '_');
		        if( CopyFile(m_ViewFilePath, filePath, TRUE) )
                {
                    // Verify that it is readonly
					m_ViewFilePath = filePath;
                    SetFileAttributes( m_ViewFilePath, FILE_ATTRIBUTE_READONLY );
                    RunViewer();
                    return;
                };
				errorCode = GetLastError();
				if ((errorCode == ERROR_FILE_NOT_FOUND) 
				 || (errorCode == ERROR_PATH_NOT_FOUND) 
				 || (errorCode == ERROR_HANDLE_DISK_FULL))
					break;
	        }
			if (errorCode == ERROR_HANDLE_DISK_FULL)
			{
				// after 100 tries, we couldn't open the temp file
				CString ErrorTxt;
				ErrorTxt.FormatMessage(IDS_DISKFULL_OPENING_TEMP_FILE_s, m_ViewFilePath);
				AddToStatus(ErrorTxt, SV_ERROR);
			}
		}
		return;
	}

	ASSERT(m_ViewItem != NULL);
	DWORD index=GetLParam(m_ViewItem);
	CP4FileStats *fs=m_FSColl.GetStats(index);
	if ((fs->GetHeadAction() == F_DELETE) && (fs->GetHaveRev() == 0))
	{
		MessageBeep(0);
		return;
	}

	m_ViewFileIsText= fs->IsTextFile();
	CString itemStr= GetItemPath(m_ViewItem);

	if ((m_Editing && fs->IsMyOpen())
	 || ((GET_P4REGPTR()->ShowEntireDepot() == SDF_LOCALTREE) && !m_Editing && !fs->GetHeadRev()))
	{
		if((fs->GetMyOpenAction()==F_INTEGRATE || fs->GetMyOpenAction()==F_BRANCH)
		 && fs->GetHaveRev() != 0)
		{
			m_StringList.RemoveAll();
			m_StringList.AddHead(fs->GetFullClientPath());
			if(!SERVER_BUSY())
			{	
				CCmd_ListOpStat *pCmd= new CCmd_ListOpStat;
				pCmd->Init( m_changeWnd, RUN_ASYNC, HOLD_LOCK );
				if( pCmd->Run( &m_StringList, P4EDIT, 0 ) )
				{
					MainFrame()->UpdateStatus( LoadStringResource(IDS_OPENING_FILES_FOR_EDIT) );
					m_ViewFilePath= fs->GetFullClientPath();
					RunViewer();
				}
				else
					delete pCmd;
			}
			else
				AddToStatus(LoadStringResource(IDS_SERVER_BUSY_UNABLE_TO_CHECK_OUT_FILE), SV_WARNING);	
		}
		else
		{
			// We already have the file open, just set viewfilepath 
			// and fire up the view app
			m_ViewFilePath= fs->GetFullClientPath();
			RunViewer();
		}
	}
	else if(m_Editing)
	{
		m_ViewFilePath= fs->GetFullClientPath();

		int getHead= IDNO;

		// See if the user wants to fetch the 
		// head revision of the file first
		if(fs->GetHeadRev() > fs->GetHaveRev())
			 getHead=AfxMessageBox( 
						IDS_CLIENT_FILE_NOT_HEAD__SYNC_TO_HEAD,
						MB_ICONEXCLAMATION | MB_YESNOCANCEL );

		switch( getHead )
		{
		case IDNO:
		case IDYES:
			if(!SERVER_BUSY())
			{	
				m_OpenUnderChangeNumber=0;
				CCmd_PrepEdit *pCmd= new CCmd_PrepEdit;
				pCmd->Init( m_hWnd, RUN_ASYNC, LOSE_LOCK);
				pCmd->SetWarnIfLocked(TRUE);
				if( pCmd->Run( itemStr, getHead == IDYES, fs->IsNotInDepot() ) )
					MainFrame()->UpdateStatus( LoadStringResource(IDS_EDITING_HEAD_REV) );
				else
					delete pCmd;
			}
			else
				AddToStatus(LoadStringResource(IDS_SERVER_BUSY_UNABLE_TO_CHECK_OUT_FILE), SV_WARNING);	

		case IDCANCEL:
		default:
			// User cancelled edit
			return;
		} //switch
	} 
	else
	{
        // If browsing a file that we HAVE on the client AND IT'S NOT OPEN or IT'S OPEN FOR INTEGRATE, 
		// copy the file to a temp file and then fire up the viewer.
		// Use a temp file to ensure that the browse app gets a readonly file,
		// (browse is a read only activity)

		BOOL bMD5ok = TRUE;
		BOOL bUseLive = FALSE;
        if( fs->GetHaveRev() > 0 && !CString(fs->GetFullClientPath()).IsEmpty() 
			&& (!fs->IsMyOpen() || (fs->GetMyOpenAction() == F_INTEGRATE))
			&& ((bMD5ok = TheApp()->digestIsSame(fs)) == TRUE
			 || ((bUseLive = FileExtUsesLiveFileToView(fs->GetFullClientPath())) == TRUE)
			  && (GET_P4REGPTR()->GetUseTempForView() == 2)))
        {
			if ((GET_P4REGPTR()->GetUseTempForView() == 1) 
			 || ((GET_P4REGPTR()->GetUseTempForView() == 2) 
			  && !FileExtUsesLiveFileToView(fs->GetFullClientPath())))
			{
				DWORD errorCode = 0;
				int fileRev= fs->GetHaveRev();
				CString fileName= GetItemName(m_ViewItem);
				CString tempPath= GET_P4REGPTR()->GetTempDir();

	    		for( int i=0; i< 100; i++)
				{
					m_ViewFilePath.Format(_T("%s\\ReadOnly-%d-Rev-%d-%s"), tempPath, i, fileRev, fileName);
					int j;
					while ((j = m_ViewFilePath.Find(':', 2)) != -1)
						m_ViewFilePath.SetAt(j, '_');
					while ((j = m_ViewFilePath.FindOneOf(_T("/*?\"<>|"))) != -1)
						m_ViewFilePath.SetAt(j, '_');
					if( CopyFile(fs->GetFullClientPath(), m_ViewFilePath, TRUE) )
					{
						// Verify that it is readonly
						SetFileAttributes( m_ViewFilePath, FILE_ATTRIBUTE_READONLY );
						RunViewer();
						return;
					};
					errorCode = GetLastError();
					if ((errorCode == ERROR_FILE_NOT_FOUND) 
					 || (errorCode == ERROR_PATH_NOT_FOUND) 
					 || (errorCode == ERROR_HANDLE_DISK_FULL))
						break;
				}
				if (errorCode == ERROR_HANDLE_DISK_FULL)
				{
					// after 100 tries, we couldn't open the temp file
					CString ErrorTxt;
					ErrorTxt.FormatMessage(IDS_DISKFULL_OPENING_TEMP_FILE_s, m_ViewFilePath);
					AddToStatus(ErrorTxt, SV_ERROR);
					return;
				}
			}
			else
			{
				if (!bMD5ok && bUseLive)
					TheApp()->StatusAdd(LoadStringResource(IDS_USINGLOCALBUTITSCHGED), SV_WARNING);
				m_ViewFilePath = fs->GetFullClientPath();
				RunViewer();
				return;
			}
        }

		// The file is not on the client OR NOT OPENED OR USER WANTS THE DEPOT VERSION,
		// so fetch the head revision of the file to a temp filename
		CCmd_PrepBrowse *pCmd= new CCmd_PrepBrowse;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		pCmd->SetFileType(fs->IsTextFile() ? FST_TEXT : FST_BINARY);
		CString fType = fs->GetHeadType();
		if( pCmd->Run( itemStr, fType, fs->GetHeadRev(), TheApp()->m_bNoCRLF ),
			fs->IsTextFile() ? FST_TEXT : FST_BINARY )
		{
			MainFrame()->UpdateStatus( LoadStringResource(IDS_FETCHING_FILE) );
		}
		else
		{
			delete pCmd;
		}
	}
}


BOOL CDepotTreeCtrl::FileExtUsesLiveFileToView(LPCTSTR path)
{
	int i, j, k;
	CString pathstr = path;
	if ((i = pathstr.ReverseFind(_T('.'))) != -1)
	{
		CString ext;
		ext = pathstr.Mid(i+1);
		if (!ext.IsEmpty())
		{
			CString extList = GET_P4REGPTR()->GetUseTempForExts();
			for (i=0; !extList.IsEmpty(); )
			{
				if ((j = extList.Find(ext)) == -1)
					break;
				k = j + ext.GetLength();
				if (((j-- == 0)                 || extList.GetAt(j) == _T(','))
				 && ((k >= extList.GetLength()) || extList.GetAt(k) == _T(',')))
					return TRUE;
				extList = extList.Mid(k+1);
			}
		}
	}
	return FALSE;
}

// Message handlers
LRESULT CDepotTreeCtrl::OnP4PrepEdit(WPARAM wParam, LPARAM lParam)
{
	CCmd_PrepEdit *pCmd= (CCmd_PrepEdit *) wParam;
	MainFrame()->ClearStatus();
	
	if(!pCmd->GetError())
		RunViewer();
	
	delete pCmd;
	return 0;
}

LRESULT CDepotTreeCtrl::OnP4PrepBrowse(WPARAM wParam, LPARAM lParam)
{
	CCmd_PrepBrowse *pCmd= (CCmd_PrepBrowse *) wParam;
	MainFrame()->ClearStatus();

	if(!pCmd->GetError())
	{
		if (pCmd->IsAnnotating() && pCmd->UseP4A())
		{
			TheApp()->CallP4A(pCmd->GetTempName(), pCmd->GetTempFilelog(), pCmd->GetFileRev());
		}
		else
		{
			m_ViewFilePath= pCmd->GetTempName();
			m_ViewFileIsText= pCmd->GetFileType() == FST_TEXT;
			RunViewer();
		}
	}
	
	delete pCmd;
	return 0;
}
		
		
void CDepotTreeCtrl::RunViewer()
{
	// First, get the file extension, if any, and find out if
	// its a text file
	CString extension;
	int slash= ReverseFindMBCS(m_ViewFilePath, _T('\\'));
	if(slash != -1)
		extension=m_ViewFilePath.Mid(slash+1);
	else
		extension=m_ViewFilePath;

	int dot= extension.ReverseFind(_T('.'));
	if(dot == -1)
		extension.Empty();
	else
		extension=extension.Mid(dot+1);

	CString viewFilePath = m_ViewFilePath;

	// We have the file, viewFilePath, try to display it
	while(1)
	{
		if(m_Viewer == _T("SHELLEXEC"))
		{
			if (extension.IsEmpty())
				viewFilePath += _T('.');	// So Windows won't get confused!

			CString assocViewer;

			// First, see if there a P4win file association
			if(!extension.IsEmpty())
				assocViewer= GET_P4REGPTR()->GetAssociatedApp(extension);
			
			// If we still havent found a viewer, set viewer to default text app
			// if user wishes to ignore windows associations
			if(assocViewer.IsEmpty() && m_ViewFileIsText && GET_P4REGPTR()->GetIgnoreWinAssoc())
				assocViewer= GET_P4REGPTR()->GetEditApp();
			
			// Let windows take a crack at finding a viewer
			if(assocViewer.IsEmpty() /*&& !extension.IsEmpty()*/ )
			{
				// Quick check for executeable extension, 
				// which will make ShellExec try to run the file
				HINSTANCE hinst=0;
				if( extension.CompareNoCase(_T("com")) != 0 && extension.CompareNoCase(_T("exe")) != 0 &&
					extension.CompareNoCase(_T("bat")) != 0 && extension.CompareNoCase(_T("cmd")) != 0)
				{
					// first try the non-standard M$ IDE's - Have to be done first they're so BAAAD
					if(!extension.IsEmpty())
					{	// give VS .NET (non-standard!) a try.
						hinst= ShellExecute( m_hWnd, _T("Open.VisualStudio.7.1"), viewFilePath, NULL, NULL, SW_SHOWNORMAL);
						if( (int) hinst > 32 ) 
							break;  // successfull VS .NET editor launch
						if( (int) hinst == SE_ERR_NOASSOC)	// give MSDEV (non-standard!) a try
						{
							hinst= ShellExecute( m_hWnd, _T("&Open with MSDEV"), viewFilePath, NULL, NULL, SW_SHOWNORMAL);
							if( (int) hinst > 32 ) 
								break;  // successfull MSDEV editor launch
						}
					}
					if( m_Editing && (!GET_P4REGPTR()->GetUseOpenForEdit() 
								   || !extUsesOpen(extension)))
					{
						hinst= ShellExecute( m_hWnd, _T("edit"), viewFilePath, NULL, NULL, SW_SHOWNORMAL);
						if( (int) hinst > 32 ) 
							break;  // successfull editor launch
					}
					if(!extension.IsEmpty() || !m_Editing)
					{
						hinst= ShellExecute( m_hWnd, _T("open"), viewFilePath, NULL, NULL, SW_SHOWNORMAL);
						if( (int) hinst > 32)
							break;  // successfull viewer launch
					}
				}
				else if (!extension.CompareNoCase(_T("exe")))
				{
					// Use ShellExecute() rather than CreateProcess()
					// because it gives better messages in case of an error
					// Also this appears to be what WindosExplorer uses.
					hinst= ShellExecute( m_hWnd, _T("open"), viewFilePath, NULL, NULL, SW_SHOWNORMAL);
					if( (int) hinst > 32)
						break;	// successfully spawned program
				}
			}

			// Remove any '.' we appended so that Windows wouldn't get confused,
			// because some editors get confused if given a trailing '.'
			if (extension.IsEmpty())
				viewFilePath.TrimRight(_T('.'));

			// If windows doesnt have an associated viewer for a text file, we use the 
			// default text editor
			if (assocViewer.IsEmpty() && (m_ViewFileIsText || !extension.CompareNoCase(_T("bat"))))
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
		msg.FormatMessage(IDS_UNABLE_TO_LAUNCH_VIEWER_s, m_ViewFilePath);
		if(AfxMessageBox(msg, MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
			break;

		// Try to find an alternate viewer
		CViewerDlg dlg;
		if(dlg.DoModal() == IDCANCEL)
			break;

		m_Viewer=dlg.GetViewer();
		if(m_Viewer != _T("SHELLEXEC"))
			GET_P4REGPTR()->AddMRUViewer(m_Viewer);
	}
}

BOOL CDepotTreeCtrl::extUsesOpen(CString extension)
{
	if (extension.IsEmpty())
		return FALSE;

	extension.MakeLower();
	CString exts = CString(GET_P4REGPTR()->GetUseOpenForEditExts()) + _T(",");
	int i;
	if ((i = exts.Find(extension)) != -1)
	{
		if ((exts.GetAt(i + extension.GetLength()) == _T(',')) 
		 && (!i || exts.GetAt(i-1) == _T(',')))
			return TRUE;
	}
	return FALSE;
}

/*
	_________________________________________________________________
*/

BOOL CDepotTreeCtrl::IsDepot( HTREEITEM hItem )
{
	HTREEITEM p = TreeView_GetParent( m_hWnd, hItem ) ;
	if ( p == TVI_ROOT  || p == NULL )
		return TRUE;
	else
		return FALSE;
}


/*
	_________________________________________________________________
	
	Double-click will edit the local file if its open for
	edit, and will run quick browse for unopened files
	_________________________________________________________________
*/

void CDepotTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// must clear this flag first!
	m_JustExpanded = FALSE;

	CTreeCtrl::OnLButtonDblClk(nFlags, point);
	if( nFlags & (MK_CONTROL | MK_MBUTTON | MK_RBUTTON | MK_SHIFT ))
		return;

	// Handle any expand messages first
	MSG msg;
	while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
	{
		//		get out if app is terminating
		//
		if ( msg.message == WM_QUIT )
			return;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	// if we generated a node expand in the above loop,
	// then there is nothing more for us to do.
	if (m_JustExpanded)
	{
		m_JustExpanded = FALSE;
		return;
	}

	//		find out what was hit
	//
	TV_HITTESTINFO ht;
	ht.pt=point;
	HTREEITEM currentItem=TreeView_HitTest( m_hWnd, &ht	);

	//		if clicked on non-tree part of pane, skip it
	//
	if( currentItem != NULL && (ht.flags & TVHT_ONITEM) )
		OnLButtonDblClk(currentItem);
}

void CDepotTreeCtrl::OnLButtonDblClk(HTREEITEM currentItem)
{
	//		clicked on a depot. expand it, but only for the new 98.2
	//
	if ( NEW_DEPOT_LISTING )
	{
		if ( IsDepot( currentItem ) )
		{
			ExpandTree( currentItem );
			return;
		}	
	}

	//		empty directory? go no further
	//
	CString empty = GetItemText ( currentItem );
	if ( empty.Find ( g_TrulyEmptyDir ) != -1 )
	{
		return;
	}

	//		make a new selection new if reqd
	//
	if(!IsSelected(currentItem))
	{
		UnselectAll();
		if(currentItem != NULL)
			SetSelectState(currentItem, TRUE);
	}

	if(GetSelectedCount() == 1 && GetSelectedItem(0) != m_Root && ITEM_IS_FILE(GetSelectedItem(0)) )
	{
		switch (GET_P4REGPTR()->GetDoubleClickOption())
		{
		case 0:	// view unopened, edit opened
		default:
			// If we already have it open for edit,
			// there should be no trouble editing it
			if(IsOpened(GetSelectedItem(0)) && !AnyOpenedForInteg())
				OnFileQuickedit();
			else if(SERVER_BUSY())
				MessageBeep(MB_OK);
			else
				OnFileQuickbrowse();
			return;

		case 1:	// open
			PostMessage(WM_COMMAND, ID_FILE_OPENEDIT, 0);
			break;

		case 2:	// open and edit
			PostMessage(WM_COMMAND, ID_FILE_QUICKEDIT, 0);
			break;

		case 3:	// view head revision
		{
			CString itemStr= GetItemPath(currentItem);
			DWORD index=GetLParam(currentItem);
			CP4FileStats *fs=m_FSColl.GetStats(index);
			SendMessage(WM_VIEWHEAD, (WPARAM)(itemStr.GetBuffer(32)), (LPARAM)fs);
			itemStr.ReleaseBuffer();
			break;
		}

		case 4:	// sync to head revision
			PostMessage(WM_COMMAND, ID_FILE_GET, 0);
			break;

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
}


////////////////////////
// Handlers for edit requests from the changes window

LRESULT CDepotTreeCtrl::OnEditFileTxt(WPARAM wParam, LPARAM lParam)
{
	m_ViewFileIsText= TRUE;
	return OnEditFile(wParam, lParam);
}

LRESULT CDepotTreeCtrl::OnEditFileBin(WPARAM wParam, LPARAM lParam)
{
	m_ViewFileIsText= FALSE;
	return OnEditFile(wParam, lParam);
}

LRESULT CDepotTreeCtrl::OnEditFile(WPARAM wParam, LPARAM lParam)
{
	CString *clientPath= (CString *) wParam;
	ASSERT(!clientPath->IsEmpty());

	m_ViewItem=NULL;
	m_ViewFilePath= *clientPath;
	m_Editing=TRUE;


	if(lParam==EDIT_FINDNEWVIEWER)
		FindAndRunNewViewer();
	else if(lParam==EDIT_CHOOSEVIEWER)
		ChooseAndRunViewer();
	else if(lParam==EDIT_ASSOCVIEWER)
		RunAssocViewer();
	else if(lParam >= 0 && lParam < MAX_MRU_VIEWERS)
		RunMRUViewer(lParam);
	else
	{
		ASSERT(0);
		return 0;
	}
	
	return 0;
}


////////////////////////
// Handlers for browse requests from the changes window

LRESULT CDepotTreeCtrl::OnBrowseFileTxt(WPARAM wParam, LPARAM lParam)
{
	m_ViewFileIsText= TRUE;
	return OnBrowseFile(wParam, lParam);
}

LRESULT CDepotTreeCtrl::OnBrowseFileBin(WPARAM wParam, LPARAM lParam)
{
	m_ViewFileIsText= FALSE;
	return OnBrowseFile(wParam, lParam);
}

LRESULT CDepotTreeCtrl::OnBrowseFile(WPARAM wParam, LPARAM lParam)
{
	CString *clientPath= (CString *) wParam;
	ASSERT(!clientPath->IsEmpty());

	m_ViewItem=NULL;
	m_ViewFilePath= *clientPath;
	m_Editing=FALSE;


	if(lParam==EDIT_FINDNEWVIEWER)
		FindAndRunNewViewer();
	else if(lParam==EDIT_CHOOSEVIEWER)
		ChooseAndRunViewer();
	else if(lParam==EDIT_ASSOCVIEWER)
		RunAssocViewer();
	else if(lParam >= 0 && lParam < MAX_MRU_VIEWERS)
		RunMRUViewer(lParam);
	else
	{
		ASSERT(0);
		return 0;
	}
	
	return 0;
}


/*
	_________________________________________________________________

	Message handlers to allow another window to request info
	about our selected files set
	_________________________________________________________________
*/

LRESULT CDepotTreeCtrl::OnGetSelectedCount(WPARAM wParam, LPARAM lParam)
{
	return GetSelectedCount();
}
	

/*
	_________________________________________________________________
*/

LRESULT CDepotTreeCtrl::OnGetSelectedList(WPARAM wParam, LPARAM lParam)
{
	CStringList *list= (CStringList *) wParam;
	ASSERT_KINDOF(CStringList, list);

	AssembleStringList( list, FALSE, lParam ? TRUE : FALSE );

	return (LRESULT) list;
}

//////////////////////
// Handle the Pending & Submitted Changelist Set/Clear Filter menu command
void CDepotTreeCtrl::OnUpdateFilterSetview(CCmdUI* pCmdUI) 
{
	int msg;
	BOOL b;
	HWND hwnd = MainFrame()->GetRightHandWnd();
	if (hwnd == ::GetParent(m_changeWnd))
	{
		msg = IDS_FILTER_PCO_SETVIEW;
		b = TRUE;
	}
	else if (hwnd == ::GetParent(m_oldChgWnd))
	{
		msg = IDS_FILTER_SETVIEW;
		b = TRUE;
	}
	else
	{
		msg = IDS_FILTER_SETVIEW;
		b = FALSE;
	}
	pCmdUI->SetText(LoadStringResource(msg));
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() && b) );
}

void CDepotTreeCtrl::OnFilterSetview() 
{
	HWND hwnd = MainFrame()->GetRightHandWnd();
	if (hwnd == ::GetParent(m_changeWnd))
		hwnd = m_changeWnd;
	else if (hwnd == ::GetParent(m_oldChgWnd))
		hwnd = m_oldChgWnd;
	else
		return;

	ASSERT(IsWindow(hwnd));
	::SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_FILTER_SETVIEW, 0), 0);
}

void CDepotTreeCtrl::OnUpdateFilterClearview(CCmdUI* pCmdUI) 
{
	BOOL b = TRUE;
	HWND hwnd = MainFrame()->GetRightHandWnd();
	if (hwnd == ::GetParent(m_changeWnd))
		hwnd = m_changeWnd;
	else if (hwnd == ::GetParent(m_oldChgWnd))
		hwnd = m_oldChgWnd;
	else
	{
		hwnd = m_oldChgWnd;
		b = FALSE;
	}

	ASSERT(IsWindow(hwnd));
	::SendMessage(hwnd, WM_SUBCHGOUFC, 0, (LPARAM)pCmdUI);
	if (!b)
		pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, FALSE) );
}

void CDepotTreeCtrl::OnFilterClearview() 
{
	HWND hwnd = MainFrame()->GetRightHandWnd();
	if (hwnd == ::GetParent(m_changeWnd))
		hwnd = m_changeWnd;
	else if (hwnd == ::GetParent(m_oldChgWnd))
		hwnd = m_oldChgWnd;
	else
		return;

	ASSERT(IsWindow(hwnd));
	::SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_FILTER_CLEARVIEW, 0), 0);
}

//////////////////////
// Handle the Label-Set Filter menu command
//
void CDepotTreeCtrl::OnUpdateLabelFilterSetview(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() 
		&& (GET_SERVERLEVEL() >= 11) && GetSelectedCount()));
}

void CDepotTreeCtrl::OnLabelFilterSetview() 
{
	ASSERT(IsWindow(m_labelWnd));
	::SendMessage(m_labelWnd, WM_COMMAND, MAKEWPARAM(ID_LABELFILTER_SETVIEW, 0), 0);
}

void CDepotTreeCtrl::OnUpdateLabelFilterSetviewRev(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() 
		&& (GET_SERVERLEVEL() >= 11) && GetSelectedCount()));
}

void CDepotTreeCtrl::OnLabelFilterSetviewRev() 
{
	ASSERT(IsWindow(m_labelWnd));
	::SendMessage(m_labelWnd, WM_COMMAND, MAKEWPARAM(ID_LABELFILTER_SETVIEWREV, 0), 0);
}

void CDepotTreeCtrl::OnUpdateLabelFilterClearview(CCmdUI* pCmdUI) 
{
	ASSERT(IsWindow(m_labelWnd));
	::SendMessage(m_labelWnd, WM_LABELOUFC, 0, (LPARAM)pCmdUI);
}

void CDepotTreeCtrl::OnLabelFilterClearview() 
{
	ASSERT(IsWindow(m_labelWnd));
	::SendMessage(m_labelWnd, WM_COMMAND, MAKEWPARAM(ID_LABELFILTER_CLEARVIEW, 0), 0);
}

//////////////////////
// More drag and drop challenged menu items
//
void CDepotTreeCtrl::OnUpdateAddToClientView(CCmdUI* pCmdUI) 
{
	CString txt;
    txt.FormatMessage(IDS_ADD_FILES_TO_CLIENT_s_VIEW, GET_P4REGPTR()->GetP4Client());
	pCmdUI->SetText ( txt );
	pCmdUI->Enable( !SERVER_BUSY() && GetSelectedCount()
		&& GET_P4REGPTR()->ShowEntireDepot() <= SDF_DEPOT );
}

void CDepotTreeCtrl::OnAddToClientView() 
{
	::SendMessage(m_clientWnd, WM_COMMAND, MAKEWPARAM(ID_ADD_TOVIEW, 0), 0);
}

void CDepotTreeCtrl::OnUpdateAddReviews(CCmdUI* pCmdUI) 
{
	CString txt;
    txt.FormatMessage(IDS_ADD_FILES_TO_USER_s_REVIEWS, GET_P4REGPTR()->GetP4User());
	pCmdUI->SetText ( txt );
	pCmdUI->Enable( !SERVER_BUSY() && GetSelectedCount() 
		&& GET_P4REGPTR()->ShowEntireDepot() <= SDF_DEPOT );
}

void CDepotTreeCtrl::OnAddReviews() 
{
	::SendMessage(m_userWnd, WM_COMMAND, MAKEWPARAM(ID_ADD_REVIEWS, 0), 0);
}

void CDepotTreeCtrl::OnUpdateFilterJobview(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SERVER_BUSY() && GetSelectedCount() );
}

void CDepotTreeCtrl::OnFilterJobview() 
{
	ASSERT(IsWindow(m_jobWnd));

	::SendMessage(m_jobWnd, WM_COMMAND, MAKEWPARAM(ID_JOB_SETFILEFILTER, 0), 0);
}

void CDepotTreeCtrl::OnFilterJobviewInteg() 
{
	ASSERT(IsWindow(m_jobWnd));

	::SendMessage(m_jobWnd, WM_COMMAND, MAKEWPARAM(ID_JOB_SETFILEFILTERINTEG, 0), 0);
}


/////////////////////
//
//

void CDepotTreeCtrl::OnFileSubmit()
{
	HTREEITEM item;
	if(ITEM_IS_FILE(item = GetSelectedItem(0)) && !AnyHaveChildren())
	{
		int index=GetLParam(item);
		CP4FileStats *fs=m_FSColl.GetStats(index);
		if (fs->GetMyOpenAction())
		{
			long chgnbr = MainFrame()->PositionChgs( fs->GetFullDepotPath(), fs->GetMyOpenAction() );
			while (SERVER_BUSY())
				Sleep(200);
			if (chgnbr > 0)
				::SendMessage(m_changeWnd, WM_COMMAND, ID_EDIT_SELECT_ALL, 0);
			::SendMessage(m_changeWnd, WM_COMMAND, ID_CHANGE_SUBMIT, 0);
		}
	}
}

void CDepotTreeCtrl::OnUpdateFindFileUnderFolder(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SERVER_BUSY() && GetSelectedCount()==1 && AnyHaveChildren());
}

void CDepotTreeCtrl::OnFindFileUnderFolder() 
{
	m_P4Files_FileSpec = GetItemPath(GetSelectedItem(0)) + "...";
	m_P4Files_List.RemoveAll();
	m_P4Files_Deselect = TRUE;
	OnPositionDepot();
}

void CDepotTreeCtrl::OnFileRename() 
{
	OnIntegrate(FALSE, NULL, FALSE, 0, NULL, TRUE);
}

void CDepotTreeCtrl::OnFileIntegspec() 
{
	OnIntegrate(FALSE, NULL, FALSE, 0, NULL);
}

void CDepotTreeCtrl::OnFileIntegrate() 
{
	OnIntegrate(TRUE, NULL, FALSE, 0, NULL);
}

// Process a request from the Branch View, to integrate an entire branch view
LRESULT CDepotTreeCtrl::OnBranchIntegrate(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR branchName= (LPCTSTR) wParam;
	
	OnIntegrate(TRUE, branchName, TRUE, 0, NULL);
	return 0;
}

// Process a request from the Sumitted Changelist View to integrate a change
LRESULT CDepotTreeCtrl::OnChangelistIntegrate(WPARAM wParam, LPARAM lParam)
{
	INTEGCHG *integChg= (INTEGCHG *) lParam;
	
	OnIntegrate(integChg->useBranch, NULL, FALSE, integChg->changeList, integChg->filelist,
													FALSE, integChg->useBranch);
	return 0;
}

void CDepotTreeCtrl::OnIntegrate(BOOL useBranch, LPCTSTR branchName, BOOL entireView, 
								 int changeList, CStringList *filelist, 
								 BOOL rename/*=FALSE*/, BOOL isChgListInteg/*=FALSE*/) 
{
	if (MainFrame()->IsModlessUp() || MainFrame()->IsPendChgEditInProgress())
		return;

	int branchFlag = useBranch ? ((branchName != NULL && lstrlen(branchName)) 
						? INTEG_USING_BRANCH : INTEG_USING_BRANCH_SPEC) : INTEG_USING_FILE_SPEC;

	if (useBranch && (branchName == NULL || !lstrlen(branchName)))
	{
		m_Save_branchFlag = branchFlag;
		m_Save_useBranch = useBranch;
		m_Save_entireView = entireView;
		m_Save_changeList = changeList;
		m_Save_rename = rename;
		m_Save_isChgListInteg = isChgListInteg;
		// Save the file list
		m_StringListI1.RemoveAll();
		if (filelist)
		{
			for( POSITION pos = filelist->GetHeadPosition(); pos != NULL; )
				m_StringListI1.AddTail(filelist->GetNext( pos ));
		}
		// Display the Branches Browse dialog.
		// If the branches window is empty, a refresh will be initiated.
		::SendMessage(MainFrame()->BranchWnd(), WM_INTEGFETCHOBJECTLIST, 
						(WPARAM)(this->m_hWnd), WM_BROWSECALLBACK1);
	}
	else
		OnIntegrate0(branchFlag, useBranch, branchName, entireView, changeList, filelist, rename);
}

LRESULT CDepotTreeCtrl::OnIntegBranchBrowseCallBack(WPARAM wParam, LPARAM lParam)
{
	CString *str = (CString *)lParam;
	::SendMessage(MainFrame()->BranchWnd(), WM_SELECTTHIS, 0, (LPARAM)str);
	OnIntegrate0(m_Save_branchFlag, m_Save_useBranch, *str, m_Save_entireView, 
				 m_Save_changeList, &m_StringListI1, m_Save_rename, m_Save_isChgListInteg);
	return 0;
}

void CDepotTreeCtrl::OnIntegrate0(int branchFlag, BOOL useBranch, LPCTSTR branchName, 
									BOOL entireView, int changeList, CStringList *filelist, 
									BOOL rename/*=FALSE*/, BOOL isChgListInteg/*=FALSE*/) 
{
	ASSERT( !entireView || useBranch);

	m_IntegWizard = new CIntegFileSpecPage(this);

	if (!m_IntegWizard)
	{
		ASSERT(0);
		AfxMessageBox(IDS_COULD_NOT_CREATE_INTEGRATION_WIZARD, MB_ICONSTOP);
		return;
	}
	MainFrame()->SetModelessUp(TRUE);

	m_IntegWizard->SetIsChgListInteg( isChgListInteg );
	m_IntegWizard->SetIsRename( rename );
	m_IntegWizard->SetIsBranch( useBranch );
	m_IntegWizard->SetBranchFlag( branchFlag );
	m_IntegWizard->SetDepotWnd( m_hWnd );

	///////
	// If this is a branch integ, pass the branch name to the Wizard
	if(useBranch)
		m_IntegWizard->SetBranchName(branchName);

	///////
	// Get a list of my changes, so we can give the dialog a list of possible
	// target change numbers

	m_Changes.RemoveAll();
	::SendMessage(m_changeWnd, WM_GETMYCHANGESLIST, (WPARAM) &m_Changes, 0);
	m_IntegWizard->SetChangesList(&m_Changes);
	
	///////
	// Build the list of my source files or directories
	// target change numbers

	if(changeList)
	{
		if (changeList > 0)
			m_IntegWizard->SetChangeNbr(changeList);
		m_StringList.RemoveAll();
		for( POSITION pos = filelist->GetHeadPosition(); pos != NULL; )
			m_StringList.AddTail(filelist->GetNext( pos ));
	}
	else if(entireView)
	{
		m_StringList.RemoveAll();
		m_StringList.AddHead(_T("//..."));
	}
	else
		AssembleStringList( &m_StringList, TRUE );

	// Make a duplicate of the filelist in case they press Back.
	m_StringListSv.RemoveAll();
	if (!m_StringList.IsEmpty())
	{
		for( POSITION pos = m_StringList.GetHeadPosition(); pos != NULL; )
			m_StringListSv.AddTail(m_StringList.GetNext( pos ));
	}

	if (!m_IntegWizard->SetSpecList(&m_StringList))
	{
		m_IntegWizard->DestroyWindow();	// some error! clean up
		delete m_IntegWizard;
		MainFrame()->SetModelessUp(FALSE);
		m_StringListSv.RemoveAll();
		return;
	}

	// Fire up the dialog to get user's preferences
	if (!m_IntegWizard->Create(IDD_PAGE_INTEGFILESPECS, this))
	{
		m_IntegWizard->DestroyWindow();	// some error! clean up
		delete m_IntegWizard;
		MainFrame()->SetModelessUp(FALSE);
		m_StringListSv.RemoveAll();
	}
}

LRESULT CDepotTreeCtrl::OnIntegrate1(WPARAM wParam, LPARAM lParam)
{
	if ((int)wParam == ID_WIZFINISH)
	{
		// Copy the stringlists for using in the 3 IntegratePreview-Sync-Integrate steps (i-s-i)
		m_StringListI1.RemoveAll();	// will hold source list
		m_StringListI2.RemoveAll();	// will hold target list
		m_StringListI3.RemoveAll();	// will be used to save source list for 3rd step of i-s-i
		m_StringListI4.RemoveAll();	// will be used to save target list for 3rd step of i-s-i

		CStringList *list= m_IntegWizard->GetSourceList();
		POSITION pos;
		for( pos= list->GetHeadPosition(); pos != NULL; )
			m_StringListI1.AddHead( list->GetNext(pos) );

		list= m_IntegWizard->GetTargetList();
		for( pos= list->GetHeadPosition(); pos != NULL; )
			m_StringListI2.AddHead( list->GetNext(pos) );

		// save the parameters for the integrate(s)
		m_integCont3 = CIntegContinue(  m_IntegWizard->GetReference(), 
										m_IntegWizard->GetRevRange(),
										m_IntegWizard->GetCommonPath(),
										m_IntegWizard->IsBranch(), m_IntegWizard->IsReverse(), 
										m_IntegWizard->IsNoCopy(), m_IntegWizard->IsForced(), 
										m_IntegWizard->IsForcedDirect(), 
										m_IntegWizard->IsRename(), m_IntegWizard->IsPreview(),
										m_IntegWizard->GetChangeNum(), 
										m_IntegWizard->IsPermitDelReadd(),
										m_IntegWizard->DelReaddType(),
										m_IntegWizard->IsBaselessMerge(),
										m_IntegWizard->IsIndirectMerge(),
										m_IntegWizard->IsPropagateTypes(),
										m_IntegWizard->IsBaseViaDelReadd(),
										m_IntegWizard->GetBranchFlag(),
										m_IntegWizard->GetBiDirFlag(),
										m_IntegWizard->GetNewChangeNbr() );
		m_IntegWizard->ClrNewChangeNbr();
		// Do they want us to sync the targets (and this is for real - not a preview)?
		if (GET_SERVERLEVEL() >= 13 && !m_IntegWizard->IsRename())	// Do we have support for -h?
		{
			// If we have support for -h, let the server do all the dirty work
			OnIntegrate3(FALSE, !m_IntegWizard->IsAutoSync());
		}
		else if (m_IntegWizard->IsAutoSync() && !m_IntegWizard->IsPreview())
		{
			list= m_IntegWizard->GetSourceList();	// save the source for use during the 2nd integ pass
			for( pos= list->GetHeadPosition(); pos != NULL; )
				m_StringListI3.AddHead( list->GetNext(pos) );

			list= m_IntegWizard->GetTargetList();	// save the target for use during the 2nd integ pass
			for( pos= list->GetHeadPosition(); pos != NULL; )
				m_StringListI4.AddHead( list->GetNext(pos) );

			m_integCont3.m_isPreview = TRUE;	// fire up an integ preview to get the tragets
			OnIntegrate3(TRUE);					// TRUE => run a sync of the targets afterwards
			m_integCont3.m_isPreview = FALSE;	// now set the preview flag back to OFF
		}
		else	// they don't want to sync or this is only a preview
			OnIntegrate3();

		m_IntegWizard->SendMessage(WM_ENABLEDISABLE, 0, FALSE);
		if (m_IntegWizard->IsPreview())
			return 0;	// if it's a preview, we're done.
		
		// Give the user some feedback that we are doing the work
		SET_BUSYCURSOR();
	}
	else
	{
		EnterCriticalSection(&MainFrame()->CriticalSection); 
		if (m_IntegWizard)	// ID_WIZFINISH leaves the dialog up; it's closed later if no error
		{
			m_IntegWizard->DestroyWindow();	 // deletes m_IntegWizard
			m_IntegWizard = 0;
			MainFrame()->SetModelessUp(FALSE);
			if ((int)wParam != ID_WIZBACK)	// Go back, but remember selection
				m_StringListSv.RemoveAll();
		}
		LeaveCriticalSection(&MainFrame()->CriticalSection);
	}
	m_Changes.RemoveAll();

	if ((int)wParam == ID_WIZBACK)	// Go back & let user select branch again
	{
		OnIntegrate(m_Save_useBranch, NULL, m_Save_entireView, 
					m_Save_changeList ? m_Save_changeList : -1, 
					&m_StringListSv, m_Save_rename, m_Save_isChgListInteg);
	}
	else if ((int)wParam != ID_WIZFINISH)
		m_StringListSv.RemoveAll();	// If Finished clicked, don't removed saved list in case integ fails
	return 0;
}

void CDepotTreeCtrl::OnIntegrate2(CStringList *list) // arrive here after the integ preview finishes
{												 //		now do a sync on the targets found
	static	CStringList	svList;		// save the targets from preview here for use in the sync
	POSITION pos;

	svList.RemoveAll();
	m_StringListI1.RemoveAll();		// these are now stale - reload them from lists 3 & 4
	m_StringListI2.RemoveAll();		//	so they can be used in OnIntegrate3() after sync finishes
	if ( list->GetCount() > 0 )		// Did the integ preview actually find any targets?
	{
		for (pos=list->GetHeadPosition(); pos != NULL; )	// copy the integ targets for syncing
			svList.AddHead(list->GetNext(pos));
		for (pos=m_StringListI3.GetHeadPosition(); pos != NULL; )	// reload list 1
			m_StringListI1.AddHead(m_StringListI3.GetNext(pos));
		for (pos=m_StringListI4.GetHeadPosition(); pos != NULL; )	// reload list 2
			m_StringListI2.AddHead(m_StringListI4.GetNext(pos));

		CCmd_Get *pCmd= new CCmd_Get;		// run a sync on the files found by the integ preview
		pCmd->Init( m_hWnd, RUN_ASYNC);
		pCmd->SetRunIntegAfterSync( TRUE );
		if( pCmd->Run( &svList, FALSE ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_FILE_SYNC) );
		else
			delete pCmd;
	}
	else // there were no files in the target list, so there is nothing to do: no sync or integ
	{
		EnterCriticalSection(&MainFrame()->CriticalSection); 
		if (m_IntegWizard)
			m_IntegWizard->SendMessage(WM_ENABLEDISABLE, 0, TRUE);
		LeaveCriticalSection(&MainFrame()->CriticalSection);
	}

	m_StringListI3.RemoveAll();		// these are now no longer needed - free the resources
	m_StringListI4.RemoveAll();
}

BOOL CDepotTreeCtrl::OnIntegrate3(BOOL bRunSyncAftPreview, BOOL bDontSync)
{
	CString txt = LoadStringResource(( m_integCont3.m_isRename ) ? IDS_FILE_RENAME : IDS_FILE_INTEGRATE);

	// Actually run the integrate.  Hang onto the key for possible use
	// in OnP4Integ()

	CCmd_Integrate2 *pCmd= new CCmd_Integrate2;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
	pCmd->SetRunSyncAfterPreview( bRunSyncAftPreview );
	if( pCmd->Run( &m_StringListI1, &m_StringListI2,
					m_integCont3.m_reference, m_integCont3.m_revRange,  m_integCont3.m_commonPath,
					m_integCont3.m_isBranch,  m_integCont3.m_isReverse, m_integCont3.m_isNoCopy, 
					m_integCont3.m_isForced,  m_integCont3.m_isForcedDirect,  
					m_integCont3.m_isRename,  m_integCont3.m_isPreview, 
					m_integCont3.m_changeNum, m_integCont3.m_isPermitDelReadd, 
					m_integCont3.m_DelReaddType,      m_integCont3.m_isBaselessMerge,
					m_integCont3.m_isIndirectMerge,   m_integCont3.m_isPropagateTypes, 
					m_integCont3.m_isBaseViaDelReadd, m_integCont3.m_BranchFlag, 
					bDontSync,                        m_integCont3.m_BiDir,
					m_integCont3.m_NewChangeNbr ) )
	{
		MainFrame()->UpdateStatus(txt);
		return TRUE;
	}
	delete pCmd;
	return FALSE;
}



/*
	_________________________________________________________________

	user clicked on the little plus sign. start our p4 dirs/fstat 
	adventure if expanding; change folder icon always.
	_________________________________________________________________
*/

BOOL CDepotTreeCtrl::ExpandTree( const HTREEITEM hItem )
{
    XTRACE(_T("ExpandTree() already expanded=%d\n"), (GetLParam( hItem ) == FOLDER_ALREADY_EXPANDED));
    if( APP_HALTED() )
        return FALSE;

	// If server is busy and we haven't already expanded this node, ignore the click
    if( SERVER_BUSY() && (GetLParam( hItem ) != FOLDER_ALREADY_EXPANDED) )
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
			return FALSE;
	}

	m_JustExpanded = TRUE;

	// Set the Open Folder image on Folders
	TVITEM tvitem;
	memset(&tvitem, '\0', sizeof(tvitem));
	tvitem.mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE;
	tvitem.hItem = hItem;
	if (TreeView_GetItem(m_hWnd, &tvitem))
	{
		if (tvitem.iImage == CP4ViewImageList::VI_FOLDER
		 || tvitem.iSelectedImage == CP4ViewImageList::VI_FOLDER)
		{
			tvitem.iImage = CP4ViewImageList::VI_OPENFOLDER;
			tvitem.iSelectedImage = CP4ViewImageList::VI_OPENFOLDER;
			TreeView_SetItem(m_hWnd, &tvitem);
		}
	}

	// if user wants to automatically re-expand depot to previous selection
	// upon reconnection, then we have to make sure to do the selection
	if (GET_P4REGPTR()->GetExpandFlag() == 1)
	{
		UnselectAll();
		SetSelectState( hItem, TRUE );
	}

	//		get out if p4 dirs was already called for this subdir
	//		or if the 98.2 server api is not available
	//
	if ( GetLParam( hItem ) == FOLDER_ALREADY_EXPANDED ||
		 !NEW_DEPOT_LISTING || m_RunningUpdate)
	{
		ApplySelectAtts(GetSelectAtts());
		return TRUE;
	}

	// Identify the type of update.  We set full update so that all files
	// under the node, rather than recent ones, will be retrieved
	m_UpdateType = UPDATE_EXPAND;

	//		set the children count to zero - if any subdirs
	//      or files are found, the count will wind up non-zero
	//      after running dirs and fstat
	SetChildCount(hItem, 0);
	
	//		set the member variables we'll be using later:
	//		the htree item for expanding and inserting items
	//		and the string for when we call p4 fstat
	//
	m_LastPathItem = hItem;
	m_LastPath = GetItemPath( m_LastPathItem ) ;
	XTRACE(_T("ExpandTree got m_LastPath= %s\n"), m_LastPath);

	//		call p4 dirs on the path, and don't let it be called again.
	//
	if( RunDirStat( m_LastPath + _T("*") ) )
		SetLParam( m_LastPathItem, FOLDER_ALREADY_EXPANDED );
	else
		SetChildCount( m_LastPathItem, 1 );

	return TRUE;
}

BOOL CDepotTreeCtrl::CollapseTree( const HTREEITEM hItem )
{
	// Set the Closed Folder image on Folders
	TVITEM tvitem;
	memset(&tvitem, '\0', sizeof(tvitem));
	tvitem.mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE;
	tvitem.hItem = hItem;
	if (TreeView_GetItem(m_hWnd, &tvitem))
	{
		if (tvitem.iImage == CP4ViewImageList::VI_OPENFOLDER
		 || tvitem.iSelectedImage == CP4ViewImageList::VI_OPENFOLDER)
		{
			tvitem.iImage = CP4ViewImageList::VI_FOLDER;
			tvitem.iSelectedImage = CP4ViewImageList::VI_FOLDER;
			TreeView_SetItem(m_hWnd, &tvitem);
		}
	}
	return TRUE;
}


/*
	_________________________________________________________________

	call 'p4 dirs' and get a stringarray of the directories under the 
	expanding leaf.
	_________________________________________________________________
*/

BOOL CDepotTreeCtrl::RunDirStat ( const CString &path )
{
	XTRACE(_T("RunDirStat() for path %s\n"), path);
	ASSERT( m_UpdateType == UPDATE_EXPAND );
	
	int	i;
	CStringList *pList= new CStringList;
	CString str = path;
    CString emptyMarker = g_TrulyEmptyDir + _T("/*");
	if ((i = str.Find( emptyMarker )) > -1)
	{
		str = str.Left(i);
		str.Insert(i, _T("/*"));
	}
	pList->AddHead( str );

	CCmd_DirStat *pCmd= new CCmd_DirStat;
	
    // Store our context, so when this command returns we know where we are
    pCmd->SetItemRef(m_LastPathItem);
    pCmd->SetTextRef(m_LastPath);
        
	pCmd->Init( m_hWnd, RUN_ASYNC );
	pCmd->SetAlternateReplyMsg( WM_P4EXPANDSUBDIR );

	BOOL ret = pCmd->Run( pList, GET_P4REGPTR()->ShowEntireDepot() == SDF_DEPOT);
	if( ret )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_UPDATING) );
	else
	{
		MainFrame()->ClearStatus();
		delete pList;
		delete pCmd;
	}
	return ret;
}


/*
	_________________________________________________________________

	get this out of the main line.
	_________________________________________________________________
*/


void CDepotTreeCtrl::AssembleStringList(CStringList *list, 
										BOOL bDepotSyntax4Files/*=FALSE*/,
										BOOL bNoRemoteFiles/*=FALSE*/,
										BOOL bWildOK/*=FALSE*/)
{
	list->RemoveAll();

	HTREEITEM cItem;
	CString itemStr;

	for( int i = GetSelectedCount() - 1; i >= 0; i-- )
	{
		cItem = GetSelectedItem( i );
		itemStr = GetItemPath( cItem );
		if (itemStr.GetAt(1) == _T(':') && !bWildOK)
		{
			if (itemStr.FindOneOf(_T("@#%*")) != -1)
			{
				StrBuf b;
				StrBuf f;
				f << CharFromCString(itemStr);
				StrPtr *p = &f;
				StrOps::WildToStr(*p, b);
				itemStr = CharToCString(b.Value());
			}
		}

		if (bNoRemoteFiles && IsInRemoteDepot(&itemStr))
			continue;

		//		if it's a directory, append the wildcard string
		//
		if( HasChildren( cItem ) ) 
			itemStr += _T("...");
		else if (ITEM_IS_FILE(cItem) && !bWildOK)
		{
			CP4FileStats *fs= m_FSColl.GetStats(GetLParam(cItem));
			CString dPath = fs->GetFullDepotPath();
			if (bDepotSyntax4Files || dPath.Find(_T('%')) != -1)
				itemStr = fs->GetFullDepotPath();
		}
	
		list->AddHead( itemStr );
	}
}

BOOL CDepotTreeCtrl::GetSelectedFiles( CStringList *list )
{
	list->RemoveAll();

	HTREEITEM cItem;

	for( int i = GetSelectedCount() - 1; i >= 0; i-- )
	{
		cItem = GetSelectedItem( i );

		if( ITEM_IS_FILE(cItem) )
		{
			CP4FileStats *fs= m_FSColl.GetStats(GetLParam(cItem));
			if( fs->InClientView() )
				list->AddHead( fs->GetFullClientPath() );
			else
				return FALSE;
		}
	}
	return TRUE;
}

BOOL CDepotTreeCtrl::GetSelectedFStats( CObList *list )
{
	list->RemoveAll();

	HTREEITEM cItem;

	for( int i = GetSelectedCount() - 1; i >= 0; i-- )
	{
		cItem = GetSelectedItem( i );

		if( ITEM_IS_FILE(cItem) )
		{
			CP4FileStats *fs= m_FSColl.GetStats(GetLParam(cItem));
			if( fs->InClientView() )
				list->AddHead( fs );
			else
				return FALSE;
		}
	}
	return TRUE;
}

void CDepotTreeCtrl::OnSetFlyoverMessage(HTREEITEM currentItem)
{
    if( ! GET_P4REGPTR()->ShowClientPath() || !currentItem) 
        return;

    LPARAM lParam= 0;
    lParam= GetLParam(currentItem);
   
    // Bail out if its the depot or a folder
    if( lParam < 0 || lParam >= m_ItemCount )
    {
        MainFrame()->SetMessageText(LoadStringResource(IDS_FOR_HELP_PRESS_F1));
        CMultiSelTreeCtrl::SetItemFocus(NULL);
    }
    else
    {
        if( GET_P4REGPTR()->ShowDepotPathHiLite() )
            CMultiSelTreeCtrl::SetItemFocus(currentItem);
        else
            CMultiSelTreeCtrl::SetItemFocus(NULL);

        CP4FileStats *fs= m_FSColl.GetStats((int) lParam);
    
        if( fs->InClientView() ) // status bar will show local path plus file type explanation
        {
            CString msg = fs->GetFullClientPath();
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
			int		nbrrevs;
			BOOL	unknown = FALSE;

			// convert the GetItemText() string to many flags
			TheApp()->GetFileType(itemStr, baseType, storeType, typeK, typeW, typeX, 
									typeO, typeM, typeL, typeS, nbrrevs, unknown);
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
				type += LoadStringResource(IDS_comma_ONLY_HEAD_REV_STORED);
			if (unknown)
				type += LoadStringResource(IDS_PLUS_UNKNOWN);
			type += ">";
			msg += (type != LoadStringResource(IDS_ONLY_UNKNOWN)) ? type : _T(" ");

			if(fs->GetHaveRev() == 0)
				msg += fs->GetHeadRev() ? LoadStringResource(IDS_DONT_HAVE)
				                        : LoadStringResource(IDS_NOT_IN_DEPOT);
			else 
			{
				if(fs->GetHaveRev() > fs->GetHeadRev() &&
					fs->GetMyOpenAction() == 0 )
					msg += _T("<") + LoadStringResource(IDS_NOT_IN_CLIENT_VIEW) + _T(">");
				else if(fs->GetHaveRev() >= fs->GetHeadRev() || fs->GetMyOpenAction() == F_ADD)  
					msg += LoadStringResource(IDS_HAVE_CURRENT);  // have head rev or doing file recovery
				else
					msg += LoadStringResource(IDS_NOT_LATEST);  // have rev < head
			}


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
                    otherAction.FormatMessage(IDS_OPENFOR_s, fs->GetActionStr( fs->GetOtherOpenAction() ));
					msg += otherAction;
					if (*(fs->GetOtherUsers()))
						msg += CString(_T(" by ")) + fs->GetOtherUsers();
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
}

	
// Support for quick copy of depot path or client path to the clipboard
//

void CDepotTreeCtrl::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetSelectedCount() >= 1);
}

void CDepotTreeCtrl::OnUpdateEditCopyclientpath(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetSelectedCount() >= 1 
		&& (m_SlashChar == _T('\\') || AllInView()));
}

void CDepotTreeCtrl::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
	int i;
	BOOL b = (i = GetSelectedCount()) > 0;
	if (i > 1)
	{
		HTREEITEM parent1 = TreeView_GetParent( m_hWnd, GetSelectedItem(0) );
		HTREEITEM parent2 = TreeView_GetParent( m_hWnd, GetSelectedItem(i-1) );
		b = parent1 == parent2;
	}
	pCmdUI->Enable( b );
}

void CDepotTreeCtrl::OnEditCopyclientpath() 
{
	CString txt;
	for(int i=-1; ++i < GetSelectedCount(); )
	{
		HTREEITEM item= GetSelectedItem(i);
		if (m_SlashChar == _T('/'))
		{
			if( ITEM_IS_FILE(item) )
			{
				CP4FileStats *fs= m_FSColl.GetStats(GetLParam(item));
				if( fs->InClientView() )
				{
					if (i)
						txt += _T("\r\n");
					txt += fs->GetFullClientPath();
				}
			}
			else
			{
				txt.Empty();
				break;
			}
		}
		else
		{
			if (i)
				txt += _T("\r\n");
			txt += GetItemPath(item);
		}
	}
	if (txt.IsEmpty())
		MessageBeep(MB_ICONEXCLAMATION);
	else
		CopyTextToClipboard( txt );
}

void CDepotTreeCtrl::OnEditCopy() 
{
	CString txt;
	for(int i=-1; ++i < GetSelectedCount(); )
	{
		HTREEITEM item= GetSelectedItem(i);
		if (m_SlashChar == _T('\\'))
		{
			if( ITEM_IS_FILE(item) )
			{
				CP4FileStats *fs= m_FSColl.GetStats(GetLParam(item));
				if( fs->InClientView() )
				{
					if (i)
						txt += _T("\r\n");
					txt += fs->GetFullDepotPath();
				}
			}
			else
			{
				txt.Empty();
				break;
			}
		}
		else
		{
			if (i)
				txt += _T("\r\n");
			txt += GetItemPath(item);
		}
	}
	if (txt.IsEmpty())
		MessageBeep(MB_ICONEXCLAMATION);
	else
		CopyTextToClipboard( txt );
}

void CDepotTreeCtrl::OnEditSelectAll() 
{
	HTREEITEM item= GetSelectedItem(0);
	if ( item != NULL )
	{
		UnselectAll();
		SetMultiSelect(TRUE);
		HTREEITEM parent= TreeView_GetParent(m_hWnd, item);
		HTREEITEM child= TreeView_GetChild(m_hWnd, parent);
		while( child != NULL )
		{
			SetSelectState( child, TRUE );
			child= TreeView_GetNextSibling(m_hWnd, child);
		}
		SetMultiSelect(FALSE);
		ShowNbrSelected();
	}
}

void CDepotTreeCtrl::SetToolTipColors(CPoint point)
{
	HTREEITEM item;
	TVHITTESTINFO	htInfo;

	htInfo.pt = point;
	item = HitTest( &htInfo );
	if (item && (htInfo.flags & (TVHT_ONITEMLABEL | TVHT_ONITEMICON)))
	{
		if (IsSelected(item))
		{
			if ((m_ToolState != 1) )
			{
				 m_ToolTip = GetToolTips( );
				 if (m_ToolTip && ::IsWindow(m_ToolTip->m_hWnd))
				 {
					 m_ToolTip->SetTipTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
					 m_ToolTip->SetTipBkColor(GetSysColor(COLOR_HIGHLIGHT));
					 m_ToolState = 1;
				 }
			}
		}
		else
		{
			if (m_ToolState != 0)
			{
				 m_ToolTip = GetToolTips( );
				 if (m_ToolTip && ::IsWindow(m_ToolTip->m_hWnd))
				 {
					 m_ToolTip->SetTipTextColor(GetSysColor(COLOR_INFOTEXT));
					 m_ToolTip->SetTipBkColor(GetSysColor(COLOR_INFOBK));
					 m_ToolState = 0;
				 }
			}
		}
	}
}

void CDepotTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	SetToolTipColors(point);
	CMultiSelTreeCtrl::OnMouseMove(nFlags, point);
}

//	Routine to expand the Depot Treeview down a branch to a given path.
//	The path to expand down is given in the 'path' variable.
//	Since this routine works by calling other routines that work in
//	an async manner, it must be restartable - the 'newPath' variable
//	indicates whether the path is a new one that starts at the top
//	of the Depot tree, or is a continuation and that expansion down
//	the 'path' should continue from the current location in the
//	Depot treeview.
//
void CDepotTreeCtrl::ExpandDepotString(const CString &path, BOOL newPath, 
								   BOOL noExpand /*= FALSE*/, int key /*=0*/, BOOL noErrMsg /*=FALSE*/)
{
	int  i, j;
    TCHAR buf[ LONGPATH + 2 ] = _T(" ");	// will hold tree item names
	CString node2find;		// we will be looking for this node of the treeview
	BOOL	bLast;			// tells whether this is the last element of the path or not
	HTREEITEM svExpandItem;
	TVITEM	tvItem;
    tvItem.pszText = buf+1;         
	tvItem.mask = TVIF_TEXT;

	if (path.GetLength() == 0)	// no path == nothing to do
	{
		m_ExpandDepotContinue = FALSE;
		m_ExpandPath.Empty();
		return;
	}
	// If there are trailing pathnames, 
	//	trim 'em off cuz we won't be able to find 'em
	// also save the path in a class variable 
	//	so other routines in this class can restart us.
	if ((i = path.Find(_T(" //"))) > 0)
		 m_ExpandPath = path.Left(i);
	else m_ExpandPath = path;

	// since a # can't be part of a file or folder name,
	//	trim off everything after a # (includeing the #)
	if ((i = m_ExpandPath.Find(_T('#'))) > 0)
		 m_ExpandPath = m_ExpandPath.Left(i);

	// now check to see if we have a string in the same syntax as the depot tree
	if (newPath)
	{
		if (((m_SlashChar == _T('/'))  && (m_ExpandPath.GetAt(1) == _T(':')))
		 || ((m_SlashChar == _T('\\')) && (m_ExpandPath.GetAt(0) == _T('/'))))
		{
			// we have a mismatch of tree syntax and syntax of the string to find
			// see if fstat can determine its name in the other syntax
			CStringList list;
			BOOL bGotIt = FALSE;
			BOOL bAddSl = FALSE;
			TCHAR tchar = m_SlashChar == _T('/') ? _T('\\') : _T('/');
			if (m_ExpandPath.GetAt(m_ExpandPath.GetLength()-1) == tchar)
				bAddSl = TRUE;
			TrimRightMBCS(m_ExpandPath, _T("*./\\"));
			// quick check for the special case of the client root
			if (m_SlashChar == _T('/') && m_ExpandPath == TheApp()->m_ClientRoot)
			{
				m_ExpandItem = TreeView_GetRoot(m_hWnd);
				m_ExpandDepotContinue = FALSE;
				m_ExpandPath.Empty();
				goto done;
			}
			list.AddHead(m_ExpandPath);
			CCmd_Fstat *pCmd = new CCmd_Fstat;
			pCmd->Init(NULL, RUN_SYNC, key ? HOLD_LOCK : LOSE_LOCK, key);
			if ( pCmd->Run(FALSE, &list, GET_P4REGPTR()->ShowEntireDepot() == SDF_DEPOT ) 
			 && !pCmd->GetError() )
			{
				CObList *list= pCmd->GetFileList();
				if( list->GetCount() > 0 )
				{
					CP4FileStats *pFileStats = (CP4FileStats *)list->GetHead();
					m_ExpandPath = m_SlashChar == _T('/') 
								 ? pFileStats->GetFullDepotPath() : pFileStats->GetFullClientPath();
					bGotIt = !m_ExpandPath.IsEmpty();
					delete pFileStats;
				}
#ifdef _DEBUG
				// More than 1 thing in the list will cause memory to leak!
				if ( list->GetCount() > 1 )
					ASSERT(0);
#endif
			}
			delete pCmd;
			if (!bGotIt)
			{
				// if we couldn't find it using p4 fstat,
				// so now run p4 where on the string and see what we get
				if (m_SlashChar == _T('/'))
				{
					if (m_ExpandPath.FindOneOf(_T("@#%*")) != -1)
					{
						StrBuf b;
						StrBuf f;
						f << CharFromCString(m_ExpandPath);
						StrPtr *p = &f;
						StrOps::WildToStr(*p, b);
						m_ExpandPath = CharToCString(b.Value());
					}
				}
				CCmd_Where *pCmd1 = new CCmd_Where;
				pCmd1->Init(NULL, RUN_SYNC, key ? HOLD_LOCK : LOSE_LOCK, key);
				if ( pCmd1->Run(m_ExpandPath) && !pCmd1->GetError() 
				  && pCmd1->GetDepotFiles()->GetCount() )
				{
					m_ExpandPath = m_SlashChar == _T('/') 
								 ? pCmd1->GetDepotSyntax() : pCmd1->GetLocalSyntax();
					if (bAddSl)
						m_ExpandPath += m_SlashChar;
					bGotIt = TRUE;
				}
				delete pCmd1;
				if (!bGotIt)
					return;
			}
		}
	}

	if (newPath && (m_SlashChar == _T('\\')))
	{
		// if new and is local syntax, skip root and trailing backslash
		i = TheApp()->m_ClientRoot.GetLength();
		if (i >= m_ExpandPath.GetLength())
			i = -1;
	}
	else
	{
		// if new depot and is syntax, gotta skip "//", 
		// otherwise gotta skip a single '/' (or '\')
		// then find the end of the 1st element of the path
		i = FindMBCS(m_ExpandPath, m_SlashChar, newPath ?  2 : 1);
	}
	if (i != -1)	// found a '/', now extract the 1st element
	{
		node2find = m_ExpandPath.Left(i);
		bLast = FALSE;
	}
	else		// if didn't find a '/', this is the last element
	{
		node2find = m_ExpandPath;
		bLast = TRUE;
	}

	if (newPath)
	{
		// start at top of treeview
		m_OrigPath = m_ExpandPath;
		svExpandItem = m_ExpandItem = TreeView_GetRoot(m_hWnd);
	}
	else
	{
		// the leading '/' does not show in the treeview - a space appears instead
		ReplaceMBCS(node2find, m_SlashChar, _T(' '));

		// remeber where we started
		svExpandItem = m_ExpandItem;

		// get the first child
		m_ExpandItem = TreeView_GetChild(m_hWnd, m_ExpandItem);
	}

	do	// look thru all the siblings for a name that equals our 1st element (node2find)
	{
		tvItem.hItem = m_ExpandItem;
	    tvItem.cchTextMax = LONGPATH;

		// get the item so we can compare its name
		if (TreeView_GetItem(m_hWnd, &tvItem))
		{
			// if last element, it may be followed by #rev-number; trim it off
			if (bLast)
			{
				LPTSTR	p;

				if ((p = _tcsrchr( buf, _T('#') )) != NULL)
				{
					do
					{
						*p = _T('\0');
					} while (*--p == _T(' '));
				}
			}
			// is this the tree item we are looking for? if could be " name" or "name"
			if (IS_NOCASE())
			{
				if (!_tcsicmp(buf+1, node2find) || !_tcsicmp(buf, node2find))
					break;
			}
			else
			{
				if (!_tcscmp(buf+1, node2find) || !_tcscmp(buf, node2find))
					break;
			}
		}
	} while ((m_ExpandItem = TreeView_GetNextSibling(m_hWnd, m_ExpandItem)) != NULL);

	if (m_ExpandItem)	// did we find it?
	{
		if (!bLast)		// if it is not the last element of the path
		{
			// if we are doing local syntax and the root is the root of a drive (eg. C:\)
			// we need to leave the \ behind for the next round
			if (newPath && i == 3 && m_SlashChar == _T('\\'))
				i = 2;

			// we will need to be restarted if TreeView_Expand() triggers p4 dirs & fstats
			m_ExpandDepotContinue = TRUE;

			// remove the element that we found above from our class path save variable
			m_ExpandPath = m_ExpandPath.Right(m_ExpandPath.GetLength() - i);

			if (noExpand && ( GetLParam( m_ExpandItem ) != FOLDER_ALREADY_EXPANDED ))
			{	// We were told not to start an expand, so we are done.
				// This shouldn't happen unless the folder was deleted
				//	by someone else and we aren't showing deleted files
				// or we have changed clients, are in client view
				//	and the selected files are not under this client
				m_ExpandDepotContinue = FALSE;
				m_ExpandPath.Empty();
			}
			// expand the node we found - this may fail - which will trigger p4 fdris and fstats
			else if (TreeView_Expand(m_hWnd, m_ExpandItem, TVE_EXPAND))
			{
				// node has already been expanded, so just recurse
				m_ExpandDepotContinue = FALSE;
				if ( GetLParam( m_ExpandItem ) == FOLDER_ALREADY_EXPANDED )
					ExpandDepotString(m_ExpandPath, FALSE, noExpand, key, noErrMsg);
				return;
			}
		}
		else	// it is the last element of the path - we are done
		{
			m_ExpandDepotContinue = FALSE;
			m_ExpandPath.Empty();
			if (m_Add2ExpandItemList)
				m_ExpandItemList.AddTail((CObject *)m_ExpandItem);
			if (TheApp()->m_bFindInChg)
			{
				TheApp()->m_bFindInChg = FALSE;
				if (ITEM_IS_FILE(m_ExpandItem))
				{
					CP4FileStats *fs= m_FSColl.GetStats(GetLParam(m_ExpandItem));
					if (fs->GetMyOpenAction())
						MainFrame()->PositionChgs( fs->GetFullDepotPath(), fs->GetMyOpenAction() );
				}
			}
		}
	}
	else if ((j = m_ExpandPath.Find(_T(' '))) > 0)		// it was not found in the tree; 
	{				// see if there is a space in the string and trim everything after it
		m_ExpandPath = m_ExpandPath.Left(j);
		m_ExpandItem = svExpandItem;
		ExpandDepotString(m_ExpandPath, FALSE, noExpand, key, noErrMsg);
		return;
	}
	else	// it was not found in the tree, highlight last found & we are done
	{
		m_ExpandPath.TrimLeft(m_SlashChar);
		m_ExpandPath.TrimLeft();
		m_ExpandPath.TrimRight();
		if (m_ExpandPath.GetLength())
        {
            CString msg;
            msg.FormatMessage(IDS_COULD_NOT_FIND_s_IN_DEPOT_VIEW, m_OrigPath);
			AddToStatus(msg, noErrMsg ? SV_DEBUG : SV_WARNING);
        }
		m_ExpandItem = svExpandItem;	// so we can highlight the last thing we did find
		m_ExpandDepotContinue = FALSE;
	}
done:
	// we get here if 
	//	1)	we found what we were looking for  or
	//	2)	we cannot find what we are looking for  or
	//	3)	TreeView_Expand() failed and has thus triggered p4 dirs & fstats
	// in all 3 cases, select the last thing we did find.
	UnselectAll();
	SetSelectState( m_ExpandItem, TRUE );
	ASSERT(GetSelectedCount());
	MainFrame()->SetActiveView(DYNAMIC_DOWNCAST(CView,GetParent()), TRUE);	// set focus to Depot pane
}

BOOL CDepotTreeCtrl::SelectExpandItemList()
{
	if (m_ExpandItemList.IsEmpty())
		return FALSE;

	UnselectAll();
	for (POSITION pos = m_ExpandItemList.GetHeadPosition(); pos != NULL; )
	{
		HTREEITEM item = (HTREEITEM)m_ExpandItemList.GetNext(pos);
		SetSelectState(item, TRUE);
	}
	// Make sure selection set is properly displayed
	SetAppearance(FALSE, TRUE, FALSE);
	return TRUE;
}

HTREEITEM CDepotTreeCtrl::FindDepotSibling(const CString &path, HTREEITEM item, BOOL bUp)
{
    TCHAR  buf[ LONGPATH + 2 ] = _T(" ");	// will hold tree item names
	TVITEM tvItem;
    tvItem.pszText = buf+1;         
	tvItem.mask = TVIF_TEXT;
	TCHAR tchar = path.GetAt(0) == _T('/') ? _T('/') : _T('\\');
	int lastSlash;
	CString pathnotconst = path;
	if ((lastSlash = ReverseFindMBCS(pathnotconst, tchar)) != -1)
	{
		// Grab the last element of the given path
		CString node2find = path.Mid(lastSlash+1);
		TrimRightMBCS(node2find, _T("\\/"));

		// look thru all the siblings for a name that equals our last element (node2find)
		while ((item = bUp ? TreeView_GetPrevSibling(m_hWnd, item)
			               : TreeView_GetNextSibling(m_hWnd, item)) != NULL)
		{
			tvItem.hItem = item;
			tvItem.cchTextMax = LONGPATH;

			// get the item so we can compare its name
			if (TreeView_GetItem(m_hWnd, &tvItem))
			{
				// The last element may be followed by #rev-number; trim it off
				LPTSTR	p;

				if ((p = _tcsrchr( buf, _T('#') )) != NULL)
				{
					do
					{
						*p = _T('\0');
					} while (*--p == _T(' '));
				}
				// is this the tree item we are looking for? if could be " name" or "name"
				if (IS_NOCASE())
				{
					if (!_tcsicmp(buf+1, node2find) || !_tcsicmp(buf, node2find))
						return item;
				}
				else
				{
					if (!_tcscmp(buf+1, node2find) || !_tcscmp(buf, node2find))
						return item;
				}
			}
		}
	}
	return NULL;
}

void CDepotTreeCtrl::OnUpdateDiff_sd_se(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && GetSelectedCount());
}

void CDepotTreeCtrl::OnDiff_sd_se() 
{
	m_StringList.RemoveAll();
	for(int i=GetSelectedCount()-1; i>=0; i--)
	{
		HTREEITEM cItem=GetSelectedItem(i);
		CString itemStr= GetItemPath(cItem);
		if( HasChildren( cItem ) )
			itemStr += _T("...");
		else if (ITEM_IS_FILE(cItem))
		{
			CP4FileStats *fs= m_FSColl.GetStats(GetLParam(cItem));
			itemStr = fs->GetFullDepotPath();
		}
		m_StringList.AddHead(itemStr);
	}

	CCmd_Diff *pCmd= new CCmd_Diff;
	pCmd->Init( m_hWnd, RUN_ASYNC);
	if( pCmd->Run( &m_StringList, NULL, 'd' ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_CHECKING_FOR_MISSING) );
	else
		delete pCmd;
}

LRESULT CDepotTreeCtrl::OnP4Diff_sd_se(WPARAM wParam, LPARAM lParam)
{
	int key = 0;
	CString filename;
	POSITION pos;
	CCmd_Diff *pCmd= (CCmd_Diff *) wParam;
	TCHAR sFlag = pCmd->GetSflag();
		
	MainFrame()->DoNotAutoPoll();
	if( !pCmd->GetError() )
	{
		if (pCmd->GetDiffNbrFiles())
		{
			AddToStatus(LoadStringResource(sFlag == _T('d') ? 
                IDS_THERE_ARE_MISSING_FILES : IDS_THERE_ARE_UNOPENED_FILES_THAT_DIFFER),
				SV_WARNING);
			if (sFlag == _T('d'))
				m_StringList.RemoveAll();
			else
				m_StringList2.RemoveAll();
			for(pos = (pCmd->GetList())->GetHeadPosition(); pos != NULL; )
			{
				filename = (pCmd->GetList())->GetNext(pos);
				if (sFlag == _T('d'))
					m_StringList.AddHead(filename);
				else
					m_StringList2.AddHead(filename);
                CString msg;
                msg.FormatMessage(sFlag == _T('d') ? IDS_s_IS_MISSING : IDS_s_IS_DIFFERENT, filename);
				AddToStatus(msg, SV_WARNING);
			}
		}
		else
			AddToStatus(LoadStringResource(sFlag == _T('d') ? 
                IDS_THERE_ARE_NO_MISSING_FILES : IDS_THERE_ARE_NO_UNOPENED_FILES_THAT_DIFFER),
						SV_COMPLETION);
	}
	delete pCmd;
	if (sFlag == _T('d'))
	{
		m_StringList2.RemoveAll();
		for(int i=GetSelectedCount()-1; i>=0; i--)
		{
			HTREEITEM cItem=GetSelectedItem(i);
			CString itemStr= GetItemPath(cItem);
			if( HasChildren( cItem ) )
				itemStr += _T("...");
			else if (ITEM_IS_FILE(cItem))
			{
				CP4FileStats *fs= m_FSColl.GetStats(GetLParam(cItem));
				itemStr = fs->GetFullDepotPath();
			}
			m_StringList2.AddHead(itemStr);
		}

		CCmd_Diff *pCmd= new CCmd_Diff;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		if( pCmd->Run( &m_StringList2, NULL, 'e' ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_CHECKING_FOR_DIFFERENCES) );
		else
			delete pCmd;
	}
	else if (m_StringList.GetCount() || m_StringList2.GetCount())
	{
		MainFrame()->UpdateStatus(LoadStringResource(IDS_ACTION));

		CForceSyncDlg dlg;
		dlg.m_lpCstrListD = &m_StringList;
		dlg.m_lpCstrListC = &m_StringList2;
		dlg.m_Key = key;

		dlg.m_SelChange = LoadStringResource(IDS_DEFAULTCHANGELISTNAME);
		::SendMessage(m_changeWnd, WM_GETMYCHANGESLIST, (WPARAM) &(dlg.m_pChangeList), 0);

		if ((dlg.DoModal() == IDOK) 
		 && (m_StringList.GetCount() || m_StringList2.GetCount()))
		{
			if (dlg.m_Action == 1)
			{
				//	Combine the 2 lists
				//
				for(pos = m_StringList2.GetHeadPosition(); pos != NULL; )
				{
					filename = m_StringList2.GetNext(pos);
					m_StringList.AddHead(filename);
				}
				m_StringList2.RemoveAll();
				//
				//		98.1 changed the command to recopy. before: p4 refresh. after: p4 sync -f
				//
				if ( GET_SERVERLEVEL( ) > 3 )
				{
					// Add a "#have" to each filespec
					POSITION pos= m_StringList.GetHeadPosition();
					while( pos != NULL )
					{
						POSITION oldPos= pos;
						CString filespec= m_StringList.GetNext(pos);
						m_StringList.SetAt(oldPos, filespec+_T("#have"));
					}

					CCmd_Get *pCmd= new CCmd_Get;
					pCmd->Init( m_hWnd, RUN_ASYNC);
					if( pCmd->Run( &m_StringList, FALSE, TRUE ) )
						MainFrame()->UpdateStatus( LoadStringResource(IDS_FILE_SYNC) );
					else
						delete pCmd;
				}
				else
				{
					CCmd_Refresh *pCmd= new CCmd_Refresh;
					pCmd->Init( m_hWnd, RUN_ASYNC);
					if(	pCmd->Run( &m_StringList ) )
						MainFrame()->UpdateStatus( LoadStringResource(IDS_FILE_REFRESH) );
					else
						delete pCmd;
				}
			}
			else	// open for delete and edit
			{
				int selectedChange = dlg.m_SelectedChange;
				if (m_StringList.GetCount())	// Anything to open for delete?
				{
					if( SERVER_BUSY() && !GET_SERVER_LOCK(key) )
					{
						AfxMessageBox(IDS_UNABLE_TO_OPEN_TRY_AGAIN, MB_ICONSTOP);
						MainFrame()->ResumeAutoPoll();
						return 0;
					}
					CCmd_ListOpStat *pCmd= new CCmd_ListOpStat;
					pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
					if (m_StringList2.GetCount())
						pCmd->SetOpenAfterDelete(TRUE, selectedChange);
					if( pCmd->Run( &m_StringList, P4DELETE, selectedChange ) )
						MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_DELETE) );
					else
						delete pCmd;
				}
				else if (m_StringList2.GetCount())	// Anything to open for edit?
				{
					if( SERVER_BUSY() && !GET_SERVER_LOCK(key) )
					{
						AfxMessageBox(IDS_UNABLE_TO_OPEN_TRY_AGAIN, MB_ICONSTOP);
						MainFrame()->ResumeAutoPoll();
						return 0;
					}
					CCmd_ListOpStat *pCmd2= new CCmd_ListOpStat;
					pCmd2->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );

					if( pCmd2->Run( &m_StringList2, P4EDIT, selectedChange ) )
						MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUEST_OPEN_EDIT) );
					else
						delete pCmd2;
				}
				else ASSERT(0);
			}
		}
		else
		{
			m_StringList.RemoveAll();
			m_StringList2.RemoveAll();
			MainFrame()->ClearStatus();
		}
	}
	else MainFrame()->ClearStatus();
	MainFrame()->ResumeAutoPoll();
	return 0;
}

void CDepotTreeCtrl::ClearDepotFilter(BOOL bRunUpdate/*=TRUE*/)
{
	m_FilterDepot=FALSE; 
	m_DepotFilterList.RemoveAll();
	m_DepotFilterPort = m_DepotFilterClient = "";
	MainFrame()->SetDepotCaption( GET_P4REGPTR()->ShowEntireDepot() );
	if (bRunUpdate)
		OnViewUpdate(); 
}

void CDepotTreeCtrl::ConvertDepotFilterList(int key)
{
	CCmd_Fstat *pCmd = new CCmd_Fstat;
	pCmd->Init(NULL, RUN_SYNC, key ? HOLD_LOCK : LOSE_LOCK, key);
	if ( pCmd->Run(FALSE, &m_DepotFilterList, GET_P4REGPTR()->ShowEntireDepot() == SDF_DEPOT ) 
	 && !pCmd->GetError() )
	{
		m_DepotFilterList.RemoveAll();
		CObList *list= pCmd->GetFileList();
		if( list->GetCount() > 0 )
		{
		    for( POSITION pos= list->GetHeadPosition(); pos!= NULL; )
			{
				CP4FileStats *pFileStats = (CP4FileStats *)list->GetNext(pos);
				m_DepotFilterList.AddTail(m_SlashChar == _T('/') 
						 ? pFileStats->GetFullDepotPath() : pFileStats->GetFullClientPath());
				delete pFileStats;
			}
		}
	}
	else
	{
		m_FilterDepot = FALSE;
		m_DepotFilterList.RemoveAll();
		m_DepotFilterPort = m_DepotFilterClient = "";
		MainFrame()->SetDepotCaption( GET_P4REGPTR()->ShowEntireDepot() );
	}
	delete pCmd;
}

LRESULT CDepotTreeCtrl::IsFilteredOnOpen(WPARAM wParam, LPARAM lParam)
{
	int *pInt = (int *)lParam;
	if (m_FilterDepot && (m_DepotFilterType == DFT_ALLOPENED 
					   || m_DepotFilterType == DFT_MYOPENED))
	{
		*pInt = m_DepotFilterType;
	}
	else
		*pInt = 0;
	return *pInt;
}

void CDepotTreeCtrl::LoadDepotFilterList(CStringList *pList)
{
	int i;
	CString str;
	m_FilterDepot=TRUE; 
	for( POSITION pos= pList->GetHeadPosition(); pos != NULL; )
	{
		str = pList->GetNext(pos);
		if ((i = str.Find(_T('#'))) != -1)
			str = str.Left(i);
		m_DepotFilterList.AddHead( str );
	}
	MainFrame()->SetDepotCaption( GET_P4REGPTR()->ShowEntireDepot() );
}

LRESULT CDepotTreeCtrl::OnRedoOpendList(WPARAM wParam, LPARAM lParam)
{
	if (wParam)
		GetOpenedList(wParam == DFT_ALLOPENED ? TRUE : FALSE);
	return 0;
}

void CDepotTreeCtrl::GetOpenedList(BOOL bAll)
{
	CCmd_Opened *pCmd= new CCmd_Opened;
	pCmd->Init( m_hWnd, RUN_ASYNC);
	pCmd->SetItemRef( NULL );
	if( pCmd->Run( bAll, TRUE ) )
	{
		MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_FILE_INFORMATION) );
		EmptyDepotFilter();					// empty out any old filter info
		SetDepotFilterType(bAll ? DFT_ALLOPENED : DFT_MYOPENED);
		m_DepotFilterPort = GET_P4REGPTR()->GetP4Port();
		m_DepotFilterClient = GET_P4REGPTR()->GetP4Client();

	}
	else
		delete pCmd;
}

LRESULT CDepotTreeCtrl::OnP4Opened(WPARAM wParam, LPARAM lParam)
{
	CCmd_Opened *pCmd= (CCmd_Opened *) wParam;

	if(!pCmd->GetError())
	{
		m_StringList.RemoveAll();
		CObList *list = pCmd->GetList( );
		ASSERT_KINDOF( CObList, list );
		for(POSITION pos= list->GetHeadPosition(); pos!=NULL; )
		{
			CP4FileStats *stats = ( CP4FileStats * )list->GetNext( pos );
			ASSERT_KINDOF( CP4FileStats, stats );
			m_StringList.AddHead(stats->GetFullDepotPath( ));
            delete stats;
		}
		if (!m_StringList.IsEmpty())	// if there is anything in the files list,
		{
			EmptyDepotFilter();
			LoadDepotFilterList(&m_StringList);	// load the depot filter list
			OnViewUpdate();						//	and refresh the depot pane
		}
	}
	delete pCmd;
	return 0;
}

void CDepotTreeCtrl::FilterViaList(CString filelist)
{
	if (filelist.IsEmpty())
	{
		ClearDepotFilter();
		return;
	}

	CString line;
	m_StringList.RemoveAll();
	for( int i=0; i<filelist.GetLength(); i++ )
	{
		switch( filelist[i] )
		{
		case _T('\r'):
			break;
		case _T('\n'):
			line.TrimRight();
			if( !line.IsEmpty() )
			{
				m_StringList.AddHead( line );
				line.Empty();
			}
			break;
		default:
			line+= filelist[i];
		}
	}
	if( !line.IsEmpty() )
		m_StringList.AddHead( line );

	if (m_StringList.IsEmpty())
	{
		ClearDepotFilter();
		return;
	}
	EmptyDepotFilter();					// empty out any old filter info
	SetDepotFilterType(DFT_LIST);
	m_Need2Filter = TRUE;
	m_FilterList  = filelist;
	m_DepotFilterPort = GET_P4REGPTR()->GetP4Port();
	m_DepotFilterClient = GET_P4REGPTR()->GetP4Client();
	m_SaveP4Files_FileSpec = m_P4Files_FileSpec;
	GetNextFilesFromFilterList();
}

void CDepotTreeCtrl::GetNextFilesFromFilterList()
{
	if (!m_StringList.IsEmpty())
	{
		CString str = m_StringList.GetTail();
		m_StringList.RemoveTail();
		if ((str.Find(_T('/')) == -1) && (str.Find(_T('\\')) == -1))
		{
			if (GET_P4REGPTR()->ShowEntireDepot() == SDF_DEPOT)
				str = _T("//...") + str;
			else
			{
				CString client = GET_P4REGPTR()->GetP4Client();
				str	= _T("//") + client + _T("/...") + str;
			}
		}
		RunP4Files(str);
	}
	else
	{
		m_P4Files_FileSpec = m_SaveP4Files_FileSpec;
		m_P4Files_List.RemoveAll();		// empty the files list from the find since it is now out-of-date
		m_Need2Filter = FALSE;
		OnViewUpdate();					//	and refresh the depot pane
	}
}

void CDepotTreeCtrl::OnUpdateAddBookmark(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SERVER_BUSY() && GetSelectedCount()==1 && !IsSelected(m_Root) );	
}

void CDepotTreeCtrl::OnAddBookmark() 
{
	if (GetSelectedCount() != 1)
	{
		ASSERT(0);
		return;
	}
	CString depotPath = GetCurrentItemPath();

	if (depotPath.GetAt(0) != _T('/'))
		GetItemDepotSyntax(GetSelectedItem(0), &depotPath);
	AddBookmark(depotPath);
}

LRESULT CDepotTreeCtrl::OnAddBookmarkMsg(WPARAM wParam, LPARAM lParam)
{
	CString depotPath = *((CString *)lParam);
	AddBookmark(depotPath);
	return 0;
}

void CDepotTreeCtrl::AddBookmark(CString &depotPath) 
{
	BOOL bSM[MAX_BOOKMARKS+1];
	CString str[MAX_BOOKMARKS+1];
	CString temp;
	int		i;
	int 	j = 0;
	int		k = MAX_BOOKMARKS+1;
	int		s = MAX_BOOKMARKS+1;
	CBookMarkAdd dlg;

	dlg.SetTitle(LoadStringResource(IDS_EDIT_BOOKMARK));
	dlg.SetLabelText(LoadStringResource(IDS_PATH));
	dlg.SetNewMenuName(depotPath);
	dlg.SetIsSubMenu(FALSE);
	dlg.SetRadioShow(3);
	dlg.SetCanCr8SubMenu(FALSE);
	if ((dlg.DoModal() != IDOK) || (!(dlg.GetNewMenuName()).GetLength()))
		return;

	depotPath = dlg.GetNewMenuName();

	// Get the strings
	for(i = -1; ++i < MAX_BOOKMARKS; )
	{
		str[i] = GET_P4REGPTR()->GetBkMkMenuName(i);
		if (str[i] == depotPath)
		{
			AddToStatus(depotPath + LoadStringResource(IDS_ALREADY_BOOKMARKED), SV_WARNING);
			MessageBeep(0);
			return;
		}
		bSM[i] = GET_P4REGPTR()->GetBkMkIsSubMenu(i);
		if (bSM[i])
		{
			j = i;
			if (s == MAX_BOOKMARKS+1)
				s = i;
		}
		else if (!(str[i].IsEmpty()))
			j = i;
		else if (k == MAX_BOOKMARKS+1)
			k = i;
	}
	if (k == MAX_BOOKMARKS+1)
	{
		MessageBeep(0);
		return;
	}

	// Reload the strings
	BOOL bAdded = FALSE;
	for(i = j = -1; ++j < MAX_BOOKMARKS; )
	{
		if (j == s)
		{
			GET_P4REGPTR()->SetBkMkMenuName(j, depotPath);
			GET_P4REGPTR()->SetBkMkIsSubMenu(j, FALSE);
			bAdded = TRUE;
		}
		else
		{
			GET_P4REGPTR()->SetBkMkMenuName(j, str[++i]);
			GET_P4REGPTR()->SetBkMkIsSubMenu(j, bSM[i]);
		}
	}
	if (!bAdded)
	{
		GET_P4REGPTR()->SetBkMkMenuName(k, depotPath);
		GET_P4REGPTR()->SetBkMkIsSubMenu(k, FALSE);
	}
	MainFrame()->LoadBkMkMenu();
	AddToStatus(depotPath + LoadStringResource(IDS_ADDED_TO_BOOKMARKS));
}

LRESULT CDepotTreeCtrl::OnSetAddFstats(WPARAM wParam, LPARAM lParam)
{
	CObList *list = (CObList *)lParam;
	ASSERT_KINDOF(CObList, list);
	POSITION pos= list->GetHeadPosition();
    InitFindItem();
	while(pos != NULL)
	{
		// Get the filestats info
		CP4FileStats *stats = (CP4FileStats *) list->GetNext(pos);
		ASSERT_KINDOF(CP4FileStats, stats);

		// Find the item
		HTREEITEM item;
		item=FindItem(stats->GetDepotDir(), stats->GetDepotFilename(), FALSE);
		if(item==NULL)
		{
			// It is quite possible for a file to be opened for add but to not be in the tree
			XTRACE(_T("OnSetAddFstats() item not found %s\n"), stats->GetFullDepotPath());
		}
		else
		{
			// Update its properties
			int index=GetLParam(item);
			CP4FileStats *fs=m_FSColl.GetStats(index);
			
			if (!stats->GetOtherUsers()[0])
				fs->SetOpenAction(stats->GetMyOpenAction(), FALSE);
			else
				fs->SetOpenAction(stats->GetOtherOpenAction(), TRUE);
			fs->SetHeadType(stats->GetHeadType());
			fs->SetType(stats->GetType());
			fs->SetOpenChangeNum(stats->GetOpenChangeNum());
			fs->SetOpenAction(stats->GetOtherOpenAction(), TRUE);
			fs->SetOtherOpens(stats->GetOtherOpens());
					
			// Update the image in this window
			SetImage(item, TheApp()->GetFileImageIndex(fs));
		}

		// If this an fstat from 'p4 opened -a' for another client
		// and they have it opened for add, we might also have it opened for add
		// so search m_FstatsAdds and if we find an entry for it update it.
		BOOL b = TRUE;
		if (stats->GetOtherOpenAction() == F_ADD)
		{
			POSITION pos2 = m_FstatsAdds.GetHeadPosition();
			while(pos2 != NULL)
			{
				CP4FileStats *statsAdd = (CP4FileStats *) m_FstatsAdds.GetNext(pos2);
				// We have to check the depot paths
				// because the client path is empty for 'stats'
				if (!_tcscmp(statsAdd->GetFullDepotPath(), stats->GetFullDepotPath()))
				{
					statsAdd->SetOpenAction(F_ADD, FALSE);
					statsAdd->SetOpenAction(F_ADD, TRUE);
					b = FALSE;
					break;
				}
			}
		}
		if (b)
		{
			// Create a copy of the fstat info
			// and add it to the list off fstat's for files opened for add
			CP4FileStats *newStats;
			newStats= new CP4FileStats;
			newStats->Create(stats);
			m_FstatsAdds.AddTail(newStats);
		}

	} // while
	return 0;
}

LRESULT CDepotTreeCtrl::OnGetAddFstats(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)&m_FstatsAdds;
}

void CDepotTreeCtrl::Empty_FstatsAdds()
{
	if (m_FstatsAdds.GetCount())
	{
		POSITION pos= m_FstatsAdds.GetHeadPosition();
		while(pos != NULL)
		{
			// Delete the filestats object
			CP4FileStats *stats = (CP4FileStats *) m_FstatsAdds.GetNext(pos);
			delete stats;
		}
		m_FstatsAdds.RemoveAll();
	}
}

void CDepotTreeCtrl::OnUpdateFileDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( AllAddable() && GetSelectedCount() );	
}

void CDepotTreeCtrl::OnFileDelete()
{
	int selcount;
	if ((selcount = GetSelectedCount()) == 0)
		return;

	CString filename;
	if (GetSelectedCount() == 1)
		filename = GetItemPath(GetLastSelection());
	else
		filename.FormatMessage(IDS_THESE_n_FILES, GetSelectedCount());
	CString txt;
	txt.FormatMessage(IDS_ARE_YOU_SURE_YOU_WANT_TO_DELETE_s, filename );
	if (IDYES == AfxMessageBox(txt, MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2))
	{
		int i, count;
		CDWordArray unselectionSet;
		unselectionSet.SetSize(selcount);
		for( i = -1, count = 0; ++i < selcount; )
		{
			HTREEITEM hItem = GetSelectedItem( i );
			// can't unlink a directory (should never happen, but...)
			if( HasChildren( hItem ) ) 
				continue;
			filename = GetItemPath( hItem );
			int rc;
			if ((rc = _tunlink(filename)) == 0)
			{
				unselectionSet.SetAt(count++, (DWORD)hItem);
				txt.FormatMessage(IDS_s_DELETED, filename);
				TheApp()->StatusAdd( txt );
			}
			else
			{
				txt.FormatMessage(IDS_s_NOT_DELETED_s, filename, 
					errno == EACCES ? LoadStringResource(IDS_BECAUSE_READONLY) : _T(""));
				TheApp()->StatusAdd( txt, SV_WARNING );
			}
		}
		if (selcount > 1)
		{
			txt.FormatMessage(IDS_n_FILES_DELETED, count);
			TheApp()->StatusAdd( txt, SV_COMPLETION );
		}
		if (count)
		{
			for (i = count; i--; )
				SetSelectState((HTREEITEM)unselectionSet.GetAt(i), FALSE);
			OnViewUpdate();
		}
	}
}

void CDepotTreeCtrl::OnUpdateFileAdd(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY()) );	
}

void CDepotTreeCtrl::OnFileAdd()
{
	CString initDir;
	if ( GetSelectedCount() == 1 )
	{
		HTREEITEM item = GetSelectedItem(0);
		if (!ITEM_IS_FILE(item))
		{
			int i;
			CString itemTxt= GetItemPath(item);

			if ( (i = itemTxt.Find(g_TrulyEmptyDir)) != -1 )
				itemTxt = itemTxt.Left(i);

			if (itemTxt.GetAt(1) == _T(':'))
			{
				initDir = itemTxt;
			}
			else
			{
				itemTxt.TrimRight(_T('/'));
				if (itemTxt.Find(_T('/'), 2) == -1)
					initDir = TheApp()->m_ClientRoot;
				else
				{
					// so now run p4 where on the string and see what we get
					CCmd_Where *pCmd1 = new CCmd_Where;
					pCmd1->Init(NULL, RUN_SYNC);
					if ( pCmd1->Run(itemTxt) && !pCmd1->GetError() 
					  && pCmd1->GetDepotFiles()->GetCount() )
					{
						initDir = pCmd1->GetLocalSyntax();
					}
					delete pCmd1;
				}
			}
			initDir.Replace('/', '\\');
		}
	}
	MainFrame()->OnFileAddSetDir(initDir.IsEmpty() ? (LPTSTR)NULL 
							   : initDir.GetBuffer(initDir.GetLength()));
	if (!initDir.IsEmpty())
		 initDir.ReleaseBuffer(-1);
}

void CDepotTreeCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	MainFrame()->WaitAWhileToPoll( );
	CMultiSelTreeCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDepotTreeCtrl::SetCaption( int iShowEntiredepot )
{
	m_caption.Empty();
	switch (iShowEntiredepot)
	{
	case SDF_CLIENT:	// Client View of Depot
		m_caption = LoadStringResource( IDS_CLIENTVIEWONLY );
		break;
	case SDF_DEPOT:		// Entire Depot
		m_caption = LoadStringResource( IDS_ENTIREDEPOT );
		break;
	case SDF_LOCALP4:	// Local Perforce Files
		m_caption = LoadStringResource( IDS_LOCALP4FILES );
		break;
	case SDF_LOCALTREE:	// All Local Files in Client Tree
		m_caption = LoadStringResource( MainFrame()->m_ShowOnlyNotInDepot 
				  ? IDS_FILESNOTINDEPOT : IDS_LOCALCLIENTTREE );
		break;
	}
	if (IsDepotFiltered())
		m_caption += LoadStringResource(IDS_CAPTION_DEPOT_FILTERED);
	if (GET_P4REGPTR()->ShowDeleted() && (iShowEntiredepot <= IDS_ENTIREDEPOT))
	{
		CString del = LoadStringResource(IDS_CAPTION_DEPOT_DELETED);
		if (IsDepotFiltered())
		{
			m_caption.TrimRight(_T(')'));
			del.TrimLeft(_T(" ("));
			del = _T(", ") + del;
		}
		m_caption += del;
	}
	GetView()->SetCaption(  );
}

void CDepotTreeCtrl::OnUpdateViewUpdate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && !MainFrame()->IsModlessUp());
}

void CDepotTreeCtrl::OnUpdateViewFilteredview(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() && !MainFrame()->m_ShowOnlyNotInDepot));
}

void CDepotTreeCtrl::OnViewFilteredview() 
{
	// Note that the filter type is 0 relative in the dlg,
	// but is 1 relative in _dft enum in mainframe.h
	// that because -1 is "not set" for dialogbox radio buttons
	FilterDepotDlg dlg;
	dlg.m_FilterType = GetDepotFilterType()-1;
	dlg.m_FileList   = GetFilterList();
	if (dlg.DoModal() == IDOK)	// get the user's preferences
	{
		switch (dlg.m_FilterType+1)
		{
		case DFT_ALLOPENED:
			GetOpenedList(TRUE);
			break;
		case DFT_MYOPENED:
			GetOpenedList(FALSE);
			break;
		case DFT_LIST:
			FilterViaList(dlg.m_FileList);
			break;
		}
	}
}

void CDepotTreeCtrl::OnUpdateViewClearfilter(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() && IsDepotFiltered()));
}

void CDepotTreeCtrl::OnViewClearfilter() 
{
	ClearDepotFilter();
}

BOOL CDepotTreeCtrl::IsInRemoteDepot(CString *depotfilename)
{
	if (!m_RemoteDepotList.IsEmpty())
	{
		int i;
		CString depotname = *depotfilename;
		if ((i = depotname.Find(_T('/'), 2)) != -1)
			depotname = depotname.Left(i);
		POSITION pos;
		for ( pos= m_RemoteDepotList.GetHeadPosition(); pos != NULL; )
		{
			if (depotname == m_RemoteDepotList.GetNext( pos ))
				return TRUE;
		}
	}
	return FALSE;
}

void CDepotTreeCtrl::OnPerforceOptions()
{
	MainFrame()->OnPerforceOptions(TRUE, FALSE, IDS_PAGE_DEPOT);
}

// Render CF_HDROP format drag and drop data.  Note that this routine is NOT a CDepotTreeCtrl
// routine - it is an override of COleDataSource for m_OLESource.  It calls back to
// CDepotTreeCtrl::RenderFileNames() to render the files names - it just sets up the structure
// that will hold the rendered file names.
BOOL CP4DOleDataSource::OnRenderData( LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium )
{
	int	lgth;

	if ((lgth = m_depotTree->RenderFileNames(NULL)) == 0)
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
	BOOL rc = m_depotTree->RenderFileNames((LPTSTR)((char*)lpdropfiles + lpdropfiles->pFiles)) 
			? TRUE : FALSE;
	GlobalUnlock(lpStgMedium->hGlobal);
	if (!rc)	// if there were no file names rendered, clean up
	{
		GlobalFree(lpStgMedium->hGlobal);
		lpStgMedium->hGlobal = 0;
	}
	return rc;
}

// This rountine provides a list of dropped file names for CF_HDROP format drag and drop
// when the Depot pane is the source of the drag and drop.
// The return value is the length of all the file names plus number of files (this is one less
// than the length of the memory needed to render the file names).  If there is nothing to
// render, the return value is 0.
// Call this routine with a NULL to just obtain the length needed for the buffer; call it
// with a pointer to the addr of a buffer in order to load that buffer with the file names.
int CDepotTreeCtrl::RenderFileNames(LPTSTR p)
{
	static LPTSTR	pFN = 0;	// ptr to buffer to store file names to be rendered
	static DWORD	lFN = 0;	// lgth of buffer at pFN
	static DWORD	uFN = 0;	// amt of buffer at pFN actually in use
	static DWORD	ddCtr = 0;	// counter to validate contents of buffer at pFN
	LPTSTR ptr;

	// If the depot pane is the drop target, don't provide a list of files
	// because files from the depot pane to the depot pane are NOT in CF_HDROP format.
	if ( m_DepotIsDropTarget )
		return 0;

	if (!pFN)
	{
		 pFN = (LPTSTR)::VirtualAlloc(NULL, 4096*sizeof(TCHAR), MEM_COMMIT, PAGE_READWRITE);
		 if (!pFN)
			 return(0);	// out of memory!
		 lFN = 4096;
	}
	else if (ddCtr == m_DragDropCtr)	// is this D&D the same as the one we have stored?
	{
		if (p)
			memcpy(p, pFN, uFN*sizeof(TCHAR));
		return uFN;
	}

	CObList list;
	if (!GetSelectedFStats(&list) || !list.GetCount())
		return(0);

	POSITION pos = list.GetHeadPosition();
	ptr = pFN;
	uFN = 0;
	while(pos != NULL)
	{
		CP4FileStats *fs = (CP4FileStats *)list.GetNext(pos);
		CString clientPath = fs->GetFullClientPath();
		// Check to see if we need to use a temp file rather than the live file
	    if( fs->GetHaveRev() > 0 && !CString(fs->GetFullClientPath()).IsEmpty() 
		 && (!fs->IsMyOpen() || (fs->GetMyOpenAction() == F_INTEGRATE)))
        {
			if ((GET_P4REGPTR()->GetUseTempForView() == 1) 
			 || ((GET_P4REGPTR()->GetUseTempForView() == 2) 
			  && !FileExtUsesLiveFileToView(fs->GetFullClientPath())))
			{
				CString fileName;
				int fileRev= fs->GetHaveRev();
				int i = clientPath.ReverseFind(_T('\\'));
				fileName = (i != -1) ? clientPath.Mid(i+1) : _T("TEMP");
				CString tempPath= GET_P4REGPTR()->GetTempDir();
				DWORD errorCode = 0;
	    		for( int i=0; i< 100; i++)
				{
					clientPath.Format(_T("%s\\ReadOnly-%d-Rev-%d-%s"), tempPath, i, fileRev, fileName);
					int j;
					while ((j = clientPath.Find(':', 2)) != -1)
						clientPath.SetAt(j, '_');
					while ((j = clientPath.FindOneOf(_T("/*?\"<>|"))) != -1)
						clientPath.SetAt(j, '_');
					if( CopyFile(fs->GetFullClientPath(), clientPath, TRUE) )
					{
						// Verify that it is readonly
						SetFileAttributes( clientPath, FILE_ATTRIBUTE_READONLY );
						errorCode = 0;
						break;
					}
					else
					{
						errorCode = GetLastError();
						if ((errorCode == ERROR_FILE_NOT_FOUND) 
						 || (errorCode == ERROR_PATH_NOT_FOUND) 
						 || (errorCode == ERROR_HANDLE_DISK_FULL))
							break;
					}
				}
				if (errorCode == ERROR_HANDLE_DISK_FULL)
				{
					// after 100 tries, we couldn't open the temp file
					CString ErrorTxt;
					ErrorTxt.FormatMessage(IDS_DISKFULL_OPENING_TEMP_FILE_s, clientPath);
					AddToStatus(ErrorTxt, SV_ERROR);
					return(0);
				}
			}
		}
		// clientPath now contains the name of either the live file or the newly created temp file
		if ((ptr + clientPath.GetLength() + 4) >= (pFN + lFN))	// running out of room?
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
	*ptr = _T('\0');
	ddCtr = m_DragDropCtr;
	if (p)
		memcpy(p, pFN, uFN*sizeof(TCHAR));
	return uFN;
}
