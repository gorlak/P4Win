//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// DepotTreeCtrl.h : header file
//

#ifndef __DEPOTCTRL
#define __DEPOTCTRL

#include "P4FileStats.h"
#include "P4StatColl.h"
#include "MSTreeCtrl.h"
#include "customgetdlg.h"
#include "IntegContinue.h"	// Added by ClassView
#include "integdlg\FileSpecPage.h"
#include "P4PaneView.h"
#include "Diff2Output.h"

#define EDIT_FINDNEWVIEWER 1000
#define EDIT_CHOOSEVIEWER   100
#define EDIT_ASSOCVIEWER     10

////////////////////////////////////////////////
// WINDOW RELOAD/UPDATE CONSIDERATIONS
//
// There are two entry points for initiating some sort of
// update - ExpandTree() and OnViewUpdate().  The expand
// operation is only valid for 98.2 or newer servers, but
// OnViewUpdate() can be called for both new and old servers.
// 
// All update code will examine m_UpdateType before proceeding.
// Its values are:
//	UPDATE_NONE:		
//		there should be no update going on at this time
//	UPDATE_FULL:	
//		With pre-98.2 servers, we initialize and refresh by running fstat on the whole 
//		friggin depot to cache its contents. With 98.2 servers, the initial update just 
//		loads depot folders
//	UPDATE_EXPAND:
//		with 98.2 servers, when a folder or depot is clicked, we run
//		p4 dirs and p4 fstat just for that directory level prior to
//		expanding the tree node
//  UPDATE_REDRILL:
//      with 98.2 servers, if the port or client hasnt changed, refresh
//      information in all explored depot folders and re-expand folders
//      as required

#define UPDATE_NONE			0  // There is no update under way
#define UPDATE_FULL			1  // Reloading all
#define UPDATE_EXPAND		2  // A single node is expanding
#define UPDATE_REDRILL		3  // Doing a refresh of all explored folders

#define REDRILL         TRUE
#define NO_REDRILL      FALSE

class CP4;

/////////////////////////////////////////////////////////////////////////////
// CDepotTreeCtrl

class CDepotTreeCtrl;
class CCmd_ListOpStat;

class CP4DOleDataSource : public COleDataSource
{
	CDepotTreeCtrl * m_depotTree;
public:
	CP4DOleDataSource()
		: m_depotTree(0)
	{}
	void SetTreeCtrl(CDepotTreeCtrl *pDepotTree)
	{
		m_depotTree = pDepotTree;
	}
	BOOL OnRenderData( LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium );
};

class CDepotTreeCtrl : public CMultiSelTreeCtrl
{
private:
	//////////////
	// Tracking info for window updates.  
	CString     m_LastPath;         // last folder we inserted an fstat file under	
    CString     m_LastChildlessPath;// last childless folder we searched to
	HTREEITEM   m_LastPathItem;     // The tree node of that folder

	// Note that folders are not counted
	long m_ItemCount;       // number of files in tree
	long m_DepotCount;      // number of depots in tree

	// Type of window update
	int m_UpdateType;
	BOOL m_RunningUpdate;
    BOOL m_Redrill;
	BOOL m_ClearedChangeWnd;

	// The root item of the entire tree
	HTREEITEM m_Root;

	// The Custom Sync Dialog Box
	CCustomGetDlg *m_CustomGetDlg;

	// The Integ File Spec Dialox Box
	CIntegFileSpecPage *m_IntegWizard;

protected:
public:
	CDepotTreeCtrl();           // protected constructor used by dynamic creation
protected:
	DECLARE_DYNCREATE(CDepotTreeCtrl)

	BOOL ExpandTree( const HTREEITEM hItem );
	BOOL CollapseTree( const HTREEITEM hItem );
	
	// Additional file info is stored in an array class attached to tree nodes w/ LParam
	// There is one row per file
	CP4StatColl m_FSColl;


	// A list of depot filespecs sent to CP4 for all multiple file type commands
	CStringList m_StringList;
	CStringList m_StringList2;
	CStringList m_SelectionList;

	// A list of depot filespecs sent to CP4 for integrates
	CStringList m_StringListI1;
	CStringList m_StringListI2;
	CStringList m_StringListI3;
	CStringList m_StringListI4;
	CStringList m_StringListSv;

	// A list of change numbers an integration can be done under
	CStringList m_Changes;

	// Data save area when doing an interg using branch
	int	 m_Save_branchFlag;
	BOOL m_Save_useBranch;
	BOOL m_Save_entireView;
	int  m_Save_changeList;
	BOOL m_Save_rename;
	BOOL m_Save_isChgListInteg;

    // A list of nodes that are currently expanded, to support UPDATE_REDRILL_982API
    CStringList m_ExpandedNodeList;
    CString m_FirstVisibleNodeText;

	// A list of depot files currently selected - saved during refreshes
	CStringList m_SavedSelectionSet;

	// OLE drag-drop source widget
	CP4DOleDataSource m_OLESource; 
	BOOL m_DepotIsDropTarget;
	DWORD m_DragDropCtr;

	// Internal clipboard formats
	UINT m_CF_DELTA;
	UINT m_CF_DEPOT;

	// Drag info stored in OnDragEnter
	DROPEFFECT  m_DropEffect;
	UINT        m_DragDataFormat;   // clipboard data format (also indicates source)
	UINT        m_SelectOnDrag;

	// Context for p4 edit in response to a drop
	long m_OpenUnderChangeNumber;

	// Depot lists
    CStringList m_LocalDepotList;
    CStringList m_RemoteDepotList;

	// Utility functions for counting leafs
	int GetLeafCount(HTREEITEM item);
	int LeafSelectedCount();

	// Context menu tests for selection set
	BOOL AnyHaveChildren();
	BOOL HasChildren(HTREEITEM currentItem);
	BOOL AnyEditable();
	BOOL AnyAddable();
	BOOL AllAddable();
	BOOL AnyOpenedForInteg();
	BOOL AnyLockable();
	BOOL AnyRecoverable();
	BOOL AnyUnlockable();
	BOOL AnyOpened();
	BOOL AnyDeleted();
	BOOL AnyOtherOpened();
	BOOL AnyRemoveable();
	BOOL AnyInView();
	BOOL AnyNotCurrent();
	BOOL AnyInRemoteDepot();
	BOOL AllInView();
	BOOL AllNotInDepot();
	BOOL IsDeleted(HTREEITEM item);
	BOOL IsOpened(HTREEITEM item);
	BOOL IsInView();
	BOOL AutoEditPossible(BOOL *mustWaitForServer);
	CString m_Viewer;
	BOOL m_Editing;
	CString m_ViewFilePath;
	BOOL m_ViewFileIsText;
	HTREEITEM m_ViewItem;
	CIntegContinue m_integCont3;
	CPoint m_ContextPoint;
	BOOL m_InContextMenu;
	BOOL m_SkipSyncDialog;
	CDiff2Output *m_Diff2dlg;

	// Expand Depot String data
	CString m_ExpandPath;
	CString m_OrigPath;
	HTREEITEM m_ExpandItem;
	CObList m_ExpandItemList;
	BOOL m_Add2ExpandItemList;
	BOOL m_ExpandDepotContinue;
	BOOL m_JustExpanded;
	BOOL m_Need2Filter;
	CString m_SaveP4Files_FileSpec;
	CString m_P4Files_FileSpec;
	CStringList m_P4Files_List;
	BOOL m_P4Files_Deselect;
	void *m_pCmdFiles;
	TCHAR m_SlashChar;

	int m_DropTargetFlag;
	CPoint m_DropTargetPt;

	BOOL m_DiffSd;
	BOOL m_DiffSe;

	// Depot Filter data
	BOOL m_FilterDepot;
	int m_DepotFilterType;
	CStringList m_DepotFilterList;	// list of depot syntax files to filter to
	CString m_FilterList;			// list of ambigeous filenames to filter on - 1 per line
	CString m_DepotFilterPort;		// depot filter was created for this port
	CString m_DepotFilterClient;	// depot filter was created for this client

	// Fields to speed up idle processing
	int  m_FlgHaveChildren;
	int  m_FlgLockable;
	int  m_FlgUnlockable;
	int  m_FlgNotCurrent;
	int  m_FlgAllNotInDepot;
	int  m_FlgAllInView;
	int  m_FlgInView;
	int  m_FlgEditable;
	int  m_FlgAddable;
	int  m_FlgAllAddable;
	int  m_FlgOpened;
	BOOL m_AnyHaveChildren;
	BOOL m_AnyLockable;
	BOOL m_AnyUnlockable;
	BOOL m_AnyNotCurrent;
	BOOL m_AllNotInDepot;
	BOOL m_AllInView;
	BOOL m_AnyInView;
	BOOL m_AnyEditable;
	BOOL m_AnyAddable;
	BOOL m_AllAddable;
	BOOL m_AnyOpened;

	// Fstats from 'p4 opened' for files opened for add.
	CObList m_FstatsAdds;

// Operations
protected:
	inline CP4winApp *TheApp() { return (CP4winApp *) AfxGetApp(); }

public:
	void SetCaption( int iShowEntiredepot );
	void AssembleStringList(CStringList *list,BOOL bDepotSyntax4Files=FALSE,BOOL bNoRemoteFiles=FALSE,BOOL bWildOK=FALSE);
	BOOL GetSelectedFiles( CStringList *list );
	BOOL GetSelectedFStats( CObList *list );

	int  RenderFileNames(LPTSTR  p);
	BOOL IsDepotFiltered() { return m_FilterDepot; }
	void EmptyDepotFilter() { m_DepotFilterList.RemoveAll(); }
	void ClearDepotFilter(BOOL bRunUpdate = TRUE);
	void ConvertDepotFilterList(int key);
	void LoadDepotFilterList(CStringList *pList);
	int  GetDepotFilterType() { return m_DepotFilterType; }
	void SetDepotFilterType(int i) { m_DepotFilterType = i; }
	CString GetDepotFilterPort() { return m_DepotFilterPort; }
	void SetDepotFilterPort(CString port) { m_DepotFilterPort = port; }
	CString GetDepotFilterClient() { return m_DepotFilterClient; }
	void SetDepotFilterClient(CString client) { m_DepotFilterClient = client; }
	BOOL IsExpandDepotContinuing() { return m_ExpandDepotContinue; }
	void GetOpenedList(BOOL bAll);
	void FilterViaList(CString filelist);
	CString GetFilterList() { return m_FilterList; }
	void GetNextFilesFromFilterList();
	BOOL IsInRemoteDepot(CString *depotfilename);

	CString GetCurrentItemPath();
	CString GetItemDepotSyntax(HTREEITEM item, CString *localStr=NULL);
	void RunP4Files(CString str);
	void Clear();
	void Empty_FstatsAdds();

	void Call_OnContextMenu(CWnd* pWnd, CPoint point) { OnContextMenu(pWnd, point); }

	// Call this with TRUE to have ExpandDepotString() start keeping a list of found items
	// Call this with FALSE to stop ExpandDepotString() from adding to the list
	void SetAdd2ExpandItemList(BOOL b) { m_Add2ExpandItemList = b; if (b) m_ExpandItemList.RemoveAll(); }
	int  GetExpandItemListCount() { return m_ExpandItemList.GetCount(); }
	BOOL SelectExpandItemList();

	//		called by mainframe to initiate updates of depot view
	//
	void OnViewUpdate( BOOL redrill, int key=0 );
		
	//		Expand an entire path. TRUE=new path in path; FALSE continue expanding old path
	//
	void ExpandDepotString(const CString &path,BOOL newPath,BOOL noExpand=FALSE,int key=0,BOOL noErrMsg=FALSE);

	//		Given a tree item, search it siblings for the last element of the given path
	//
	HTREEITEM FindDepotSibling(const CString &path, HTREEITEM itemStart, BOOL bUp = FALSE);

// Overrides
public:
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);

// Implementation
protected:
	BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point); 
	// Message handlers to receive info from server
	LRESULT OnP4Error(WPARAM wParam, LPARAM lParam);
    LRESULT OnP4Depots(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4ExpandTree(WPARAM wParam, LPARAM lParam);
    LRESULT OnP4DirStat(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4FStat(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4PrepBrowse(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4PrepEdit(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4ListOp(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4History(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndHistory(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Get(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4GetWhatIf(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Integ(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Add(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Recover(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Refresh(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4UpdateOpen(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Diff(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Diff_sd_se(WPARAM wParam, LPARAM lParam);
	LRESULT InsertFromFstat(CP4FileStats *stats);
	LRESULT OnP4FileInformation(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndFileInformation(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Files( WPARAM wParam, LPARAM lParam );
	LRESULT OnDropTarget( WPARAM wParam, LPARAM lParam );
	LRESULT OnSetAddFstats(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetAddFstats(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Diff2(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndDiff2( WPARAM wParam, LPARAM lParam );
	LRESULT OnP4EndDiff2Output( WPARAM wParam, LPARAM lParam );
	BOOL    InsertDir( CString &path );

	void FindParentDirectory(CString path, int& commonLength);

	void ProcessStatListResults(WPARAM wParam);
	void ProcessGetListResults(UINT command, CStringList *list);
	BOOL OnP4RevertFile(CCmd_ListOpStat *pCmd);
	void LabelDropMenu(BOOL bLabelSelected);
	
	// Message handler to process request from Branch View to integrate an entire branch
	LRESULT OnBranchIntegrate(WPARAM wParam, LPARAM lParam);

	// Message handler to process request from Sumitted Changelist View to integrate a change
	LRESULT OnChangelistIntegrate(WPARAM wParam, LPARAM lParam);

	// A few functions to make tree item manipulation a little easier
	HTREEITEM Insert(LPCTSTR text, int imageIndex, LPARAM lparam, HTREEITEM hParent);
	void DeleteLeaf(HTREEITEM item);
	CString GetItemPath(HTREEITEM item);
	CString GetItemName(HTREEITEM curr_item);
	CString GetItemRev(HTREEITEM curr_item);
	CString GetItemHeadRev(HTREEITEM curr_item);
	CString GetItemType(HTREEITEM curr_item);
	HTREEITEM VerifySubdir(CString path, HTREEITEM startItem);
	HTREEITEM CheckItem(CString lookingfor, HTREEITEM item, BOOL useRevNum, int revNum);
	HTREEITEM FindItem(CString path, CString fnamerev, BOOL useRevision);
    void InitFindItem() { m_LastChildlessPath=_T("////"); }
	HTREEITEM FindFolder(LPCTSTR depotName);
	
	void RunCStat( int key );
	BOOL IsDepot( HTREEITEM hItem );

    // CMSTreeView virt func to set status messages on mouse flyover
    void OnSetFlyoverMessage(HTREEITEM currentItem);

	// CMSTreeView virt func to attempt drag drop during left mouse button down
	BOOL TryDragDrop(HTREEITEM currentItem);

	// CMSTreeView virt func to handle a left dbl clk after the Item is determined
	void OnLButtonDblClk(HTREEITEM currentItem);

	// Initiate a refresh of the changes window.  Called from OnP4Depots or OnP4FStat
	//
	void StartChangeWndUpdate(int key);

    // Support for UPDATE_REDRILL_982API
    void RecordTreeExploration();
    void RecordFolderExploration(HTREEITEM parentItem, LPCTSTR path);
    void RunRedrill( int key );

	// FileGet used by OnFileGet(), OnFileGetWhatIf(), OnFileRemove()
	void FileGet(BOOL whatIf, BOOL force, BOOL removeFiles, LPCTSTR qualifier=_T(""));

	// Used if Depot is to be sorted by extension
	HTREEITEM SortItemByExtension(LPCTSTR text, HTREEITEM hParent);

public:
	virtual ~CDepotTreeCtrl() { Empty_FstatsAdds(); }
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
public:
	afx_msg void OnViewUpdate();
protected:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUpdateFileLock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileUnlock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpendelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpenedit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileRevert(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileGet(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGetwhatif(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileRemove(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileTimeLapse(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileAnnotate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileRevisionhistory(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileRevisiontree(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileDiffhead(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileDiff2(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePositionDepot(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePositionDepotNext(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePositionChgs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowDeletedFiles(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWinExplore(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCmdPrompt(CCmdUI* pCmdUI);
	afx_msg void OnFileLock();
	afx_msg void OnFileUnlock();
	afx_msg void OnFileOpenedit();
	afx_msg void OnFileOpendelete();
	afx_msg void OnFileRevert();
	afx_msg void OnFileGet();
	afx_msg void OnFileGetwhatif();
	afx_msg void OnFileRemove();
	afx_msg void OnFileTimeLapseView();
	afx_msg void OnFileAnnotate();
	afx_msg void OnFileAnnotateAll();
	afx_msg void OnFileAnnotateChg();
	afx_msg void OnFileAnnotateChgAll();
	afx_msg void OnFileRevisionhistory();
	afx_msg void OnFileRevisionTree();
	afx_msg void OnPositionDepot();
	afx_msg void OnPositionDepotNext();
	afx_msg void OnPositionDepotPrev();
	afx_msg void OnPositionChgs();
	afx_msg void OnShowDeletedFiles();
	afx_msg void OnWinExplore();
	afx_msg void OnCmdPrompt();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateFileGetcustom(CCmdUI* pCmdUI);
	afx_msg void OnFileGetcustom();
	afx_msg void OnUpdateFileRefresh(CCmdUI* pCmdUI);
	afx_msg void OnFileRefresh();
	afx_msg void CDepotTreeCtrl::OnUpdateFileForceToHead(CCmdUI* pCmdUI);
	afx_msg void OnFileForceToHead();
	afx_msg void OnUpdateFileSubmit(CCmdUI* pCmdUI);
	afx_msg void OnFileSubmit();
	afx_msg void OnFileDiffhead();
	afx_msg void OnFileDiff2();
	afx_msg void OnUpdateFileInformation(CCmdUI* pCmdUI);
	afx_msg void OnFileInformation();
	afx_msg void OnUpdateFiledropEdit(CCmdUI* pCmdUI);
	afx_msg void OnFiledropEdit();
	afx_msg void OnUpdateFiledropDelete(CCmdUI* pCmdUI);
	afx_msg void OnFiledropDelete();
	afx_msg void OnUpdateFiledropCancel(CCmdUI* pCmdUI);
	afx_msg void OnFiledropCancel();
	afx_msg void OnUpdateFileAutoedit(CCmdUI* pCmdUI);
	afx_msg void OnFileAutoedit();
	afx_msg void OnFileQuickedit();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnUpdateFileAutobrowse(CCmdUI* pCmdUI);
	afx_msg void OnFileAutobrowse();
	afx_msg void OnFileQuickbrowse();
	afx_msg void OnUpdateFileRename(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileIntegrate(CCmdUI* pCmdUI);
	afx_msg void OnFileIntegrate();
	afx_msg void OnFileIntegspec();
	afx_msg void OnFileRename();
	afx_msg void OnUpdateFileRecover(CCmdUI* pCmdUI);
	afx_msg void OnFileRecover();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopyclientpath(CCmdUI* pCmdUI);
	afx_msg void OnEditCopyclientpath();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDiff_sd_se();
	afx_msg void OnUpdateDiff_sd_se(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddToClientView(CCmdUI* pCmdUI);
	afx_msg void OnAddToClientView();
	afx_msg void OnUpdateAddReviews(CCmdUI* pCmdUI);
	afx_msg void OnAddReviews();
	afx_msg void OnUpdateFilterJobview(CCmdUI* pCmdUI);
	afx_msg void OnFilterJobview();
	afx_msg void OnFilterJobviewInteg();
	afx_msg void OnUpdateFindFileUnderFolder(CCmdUI* pCmdUI);
	afx_msg void OnFindFileUnderFolder();
	afx_msg void OnUpdateRemoveViewer(CCmdUI* pCmdUI);
	afx_msg void OnRemoveViewer();
	afx_msg void OnUpdateAddBookmark(CCmdUI* pCmdUI);
	afx_msg void OnAddBookmark();
	afx_msg void OnUpdateLabelSync(CCmdUI* pCmdUI);
	afx_msg void OnLabelSync();
	afx_msg void OnUpdateLabelDeleteFiles(CCmdUI* pCmdUI);
	afx_msg void OnLabelDeleteFiles();
	afx_msg void OnUpdateLabelSyncClient(CCmdUI* pCmdUI);
	afx_msg void OnLabelSyncClient();
	afx_msg void OnUpdateFileDelete(CCmdUI* pCmdUI);
	afx_msg void OnFileDelete();
	afx_msg void OnUpdateFileAdd(CCmdUI* pCmdUI);
	afx_msg void OnFileAdd();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnUpdateViewUpdate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFilteredview(CCmdUI* pCmdUI);
	afx_msg void OnViewFilteredview();
	afx_msg void OnUpdateViewClearfilter(CCmdUI* pCmdUI);
	afx_msg void OnViewClearfilter();
	afx_msg void OnUpdateFilterSetview(CCmdUI* pCmdUI);
	afx_msg void OnFilterSetview();
	afx_msg void OnUpdateFilterClearview(CCmdUI* pCmdUI);
	afx_msg void OnFilterClearview();
	afx_msg void OnUpdateLabelFilterSetview(CCmdUI* pCmdUI);
	afx_msg void OnLabelFilterSetview();
	afx_msg void OnUpdateLabelFilterSetviewRev(CCmdUI* pCmdUI);
	afx_msg void OnLabelFilterSetviewRev();
	afx_msg void OnUpdateLabelFilterClearview(CCmdUI* pCmdUI);
	afx_msg void OnLabelFilterClearview();
	afx_msg void OnPerforceOptions();
	// Keep the OnContextMenu out of the AFX_MSG set, since MFC right click in a TreeView is busted. 
	void OnFileMRUBrowse(UINT  nID);
	void OnFileMRUEditor(UINT  nID);
	void RunMRUViewer(UINT nID);
	void OnFileNewBrowser();
	void OnFileNewEditor();
	void FindAndRunNewViewer();
	void RunAssocViewer();
	void ChooseAndRunViewer();
	void PrepareForViewer();
	void RunViewer();
	void UpdateFileAnnotate(CCmdUI* pCmdUI, BOOL bUnicodeOK=FALSE);
	void FileAnnotate(BOOL bAll, BOOL bChg=FALSE);
	void OnIntegrate(BOOL useBranch,LPCTSTR branchName,BOOL entireView,int changeList,CStringList *filelist,BOOL rename=FALSE,BOOL isChgListInteg=FALSE);
	void OnIntegrate0(int branchFlag, BOOL useBranch,LPCTSTR branchName,BOOL entireView,int changeList,CStringList *filelist,BOOL rename=FALSE,BOOL isChgListInteg=FALSE);
	LRESULT OnIntegrate1(WPARAM wParam, LPARAM lParam);
	void OnIntegrate2(CStringList *list);
	BOOL OnIntegrate3(BOOL bRunSyncAftPreview = FALSE, BOOL bDontSync = FALSE);
	void AddBookmark(CString &depotPath);

	LRESULT OnEditFileTxt(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditFileBin(WPARAM wParam, LPARAM lParam);
	LRESULT OnEditFile(WPARAM wParam, LPARAM lParam);
	LRESULT OnBrowseFileTxt(WPARAM wParam, LPARAM lParam);
	LRESULT OnBrowseFileBin(WPARAM wParam, LPARAM lParam);
	LRESULT OnBrowseFile(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetSelectedCount(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetSelectedList(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Opened(WPARAM wParam, LPARAM lParam);
	LRESULT OnViewHead(WPARAM wParam, LPARAM lParam);
	LRESULT OnRedoOpendList(WPARAM wParam, LPARAM lParam);
	LRESULT OnDoGetCustom(WPARAM wParam, LPARAM lParam);
	LRESULT IsFilteredOnOpen(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4FileRevert(WPARAM wParam, LPARAM lParam);
	LRESULT OnAddBookmarkMsg(WPARAM wParam, LPARAM lParam);
	LRESULT OnIntegBranchBrowseCallBack(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetViewer( WPARAM wParam, LPARAM lParam );
	LRESULT OnEndPositionDepot( WPARAM wParam, LPARAM lParam );

	void OnContextMenu(CWnd*, CPoint point);
	void SetToolTipColors(CPoint point);
	BOOL AllOpenedForAdd();
	BOOL extUsesOpen(CString extension);
	DECLARE_MESSAGE_MAP()

private:
	BOOL RunDirStat( const CString &path );
	CString GetFileSpec( );
	void LockAndUnlock( int which ) ;
	BOOL FindFileInDepotFilter(LPCTSTR depotPath);
	BOOL FindDirInDepotFilter(LPCTSTR path);
	BOOL FileExtUsesLiveFileToView(LPCTSTR path);
	void OnFileRecoverProceed();
};

/////////////////////////////////////////////////////////////////////////////
#endif //DEPOTTREECTRL
