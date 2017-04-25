//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// DeltaTreeCtrl.h : header file
//
#ifndef __DELTATREECTRL
#define __DELTATREECTRL


#include "MSTreeCtrl.h"
#include "P4filestats.h"

#define WM_DROPFILE

class CDeltaTreeCtrl;

class CP4OleDataSource : public COleDataSource
{
	CDeltaTreeCtrl * m_deltaTree;
public:
	CP4OleDataSource()
		: m_deltaTree(0)
	{}
	void SetTreeCtrl(CDeltaTreeCtrl *pDeltaTree)
	{
		m_deltaTree = pDeltaTree;
	}
	BOOL OnRenderData( LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium );
};


class CDeltaTreeCtrl : public CMultiSelTreeCtrl
{
protected:
public:
	CDeltaTreeCtrl();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDeltaTreeCtrl)

// Attributes
protected:
	// The root items in the Changes Tree
	HTREEITEM m_MyRoot, m_OthersRoot, m_MyDefault;

	// Where to post messages for changes that affect DepotView.  
	// This is set in CMainFrame::OnCreateCLientby calling SetDepotWnd()
	
	// Number of items retrieved w/ "P4 opened", a smaller number than CTreeCtrl::GetCount,
	// since change roots, changes and jobs are also in the tree
	unsigned int m_ItemCount;        
 
	// A list of depot filespecs for all multiple file type commands
	CStringList m_StringList;
	CStringList m_StringList2;
		
	BOOL m_DoRevert;

	// Store tree state.  This includes a list of all expanded items, and the topmost
    // change group, change and file that were visible.  If the topmost visible item
    // was a job, we will not attempt to scroll that job back to the top, but will scroll 
    // the preceding item to the top of the view
	BOOL m_MyRootExpanded;
	BOOL m_OthersRootExpanded;
	CStringList	m_ExpandedItems;
	CStringList	m_SelectedItems;
    int m_FirstVisibleNodeGroup;
    CString m_FirstVisibleNodeChange;
    CString m_FirstVisibleNodeFile;

	// Collect previous execution tree state expansion here
	BOOL m_RedoExpansion;
	CString m_PrevExpansion;
    
	//////////////////////
	// OLE drag drop support

	// OLE drag-drop source widget
	CP4OleDataSource m_OLESource; 
	BOOL m_DeltaIsDropTarget;
	DWORD m_LastDragDropTime;
	DWORD m_DragDropCtr;

	// Internal clipboard formats
	UINT m_CF_DELTA;
	UINT m_CF_DEPOT;
	UINT m_CF_JOB;

	// Drag info stored in OnDragEnter
	UINT m_DragDataFormat;   // clipboard data format (also indicates source)
	HTREEITEM m_DragLastOver;  // item drag was last over
	HTREEITEM m_DragLastHighlite;  // item last highlighted for receive
	HTREEITEM m_DragFromChange;
	HTREEITEM m_DragToChange;
	long m_DragToChangeNum;
	CPoint m_DragToPoint;
	DROPEFFECT m_DropEffect;
	BOOL m_Need2Refresh;


	// Dropped data from another change or from Exploder
	CStringList m_DroppedFileList;
	CStringList m_DroppedJobList;

	///////////////////////
	// Context info for change edit/submit
	BOOL m_NewChangeSpec;
	CStringList m_SelectionList;
	CStringList m_FileList;
	CStringList m_JobList;
	long m_EditChangeNum;
	BOOL m_ChangeIsSelected;
	BOOL m_EnableChangeSubmit;
	BOOL m_SubmitOnlyChged;
	BOOL m_SubmitOnlySeled;
	HTREEITEM m_EditChange;
	CString m_SpecText;

	// Used to keep track of single item pending operations
	// Currently only used by OnFileResolve()
	HTREEITEM m_ActiveItem;

	// Used to keep track of client path of selected item 
	// when doing a reopen for edit followed by an edit
	CString m_ClientPath;
	LPARAM m_SavelParam;
	BOOL m_Need2Edit;
	UINT m_Msg2Send;

	// Used to keep track of depot syntax path we are seeking
	// in the Other Clients' changelists
	CString m_PositionTo;

	// Interactive resolve data
	BOOL m_ReResolve;
	BOOL m_TextualMerge;
	CObList m_ResolveList;
	BOOL m_ForcedResolve;
	BOOL m_bRunMerge;

	// Data for modless edit and submit dialogs
	BOOL m_EditInProgress;
	CWnd *m_EditInProgressWnd;

	// Rename data
	int m_NewChgNbr;
	CString m_NewDesc;

	// Used by OnUpdateUI's and OnContextMenu()
	CPoint m_ContextPoint;
	BOOL m_InContextMenu;
	BOOL IsMyPendingChange(HTREEITEM currentItem);
	BOOL IsMyPendingChangeFile(HTREEITEM currentItem);
	BOOL IsSelectionInSubmittableChange();
	BOOL IsSelectionSubmittableChange();
	BOOL IsMyPendingChangeItem(HTREEITEM currentItem);
	BOOL IsOpenedForInteg(HTREEITEM currentItem);
	BOOL IsOpenedForBranch(HTREEITEM currentItem);
	BOOL IsSelectionInMyNumberedChange();

	BOOL AnyMyPendingChangeFiles();
	BOOL AnyJobs();
	BOOL AnyFilesInChange( HTREEITEM changeItem );
	BOOL AnyMyUnLocked();
	BOOL AnyMyLock();
	BOOL AnyBinaryFiles(BOOL bAnyResolvable=FALSE);
    BOOL AnyUnresolvedFiles();
	BOOL AnyMyFilesUnresolved();
	BOOL AnyMyInteg();
	BOOL AnyMyBranch();

	CString GetClientFromChange();
	CString GetUserFromChange();

private:
	void LockOrUnlock( int which );

// Operations
protected:
	inline CP4winApp *TheApp() { return (CP4winApp *) AfxGetApp(); }
	BOOL ExpandTree( const HTREEITEM item );
	BOOL ExpandOthersRoot();
	BOOL m_ExpandingOthersRoot;

	void ClearUnresolvedFlags( );
	void RunChangeEdit(int key);

	// Sync a list of filename to the head rev
	void SyncList2Head(CStringList *pSyncList, int key);

	// Add a job fixes
	void AddJobFixes(CStringList *jobnames, LPCTSTR jobstatusvalue);

	// Can have only 1 edit spec dialog open at a time - this gives error msg
	void CantDoItRightNow(int type);

public:
	// TRUE => List contains Exactly all the files that have not changed
	BOOL m_FileListDefinitive;

	void Clear();
	BOOL PumpMessages( );
	int  RenderFileNames(LPTSTR  p);
    BOOL AnyResolvedFiles(BOOL bList=FALSE);
	BOOL AnyMyFilesResolved(BOOL bList=FALSE);
	BOOL AnyUnresolvedFilesInChg(HTREEITEM chgitem);
	BOOL AnyResolvedFilesInChg(HTREEITEM chgitem);
	BOOL AssembleStringList(CStringList* list = NULL, BOOL includeAdds = TRUE);
	CStringList* GetStringList() { return &m_StringList; }
	int CreateNewChangeList(int key, CString *description=NULL, BOOL autoOK=FALSE);
	BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll, BOOL *bScrolled);
	BOOL IsEditInProgress() { return m_EditInProgress; }

// Overrides
public:
		// Drag and Drop functions delegated by CDeltaView
	DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	void OnDragLeave();
	DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
protected:

	/////////////////
	// handlers for info returned by server
	LRESULT OnP4Add(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4AutoResolve(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Merge2(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Merge3(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Resolve(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4UnResolved(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Resolved(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Change(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Ostat(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4ChangeSpec(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndSpecEdit(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4ListOp(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4SyncAndEdit(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Diff(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4UpdateOpen(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4UpdateRevert(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4SetUnresolved(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4ChangeDel(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4JobDel(WPARAM wParam, LPARAM lParam);
    LRESULT OnP4JobList(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Fixes(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Fix(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Describe(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndDescribe(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4JobDescribe(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndJobDescribe(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4ChangeDescribe(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndChgDescribe(WPARAM wParam, LPARAM lParam);
	LRESULT OnGotMoveLists(WPARAM wParam, LPARAM lParam);
	LRESULT OnInitTree(WPARAM wParam, LPARAM lParam);
	LRESULT OnOLEAddFiles(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Revert(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4FileRevert(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4FileInformation(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndFileInformation(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4DiffChangeEdit(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4TheirFindInDepot(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4TheirHistory(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4TheirProperties(WPARAM wParam, LPARAM lParam);
	LRESULT OnActivateModeless(WPARAM wParam, LPARAM lParam);
	LRESULT CallOnUpdateFilterClearview(WPARAM wParam, LPARAM lParam);

	BOOL OnP4RevertFile(CStringList *list, BOOL notifyDepotWnd=TRUE, BOOL errs=FALSE, 
						BOOL revertUnchgAfter=FALSE,int key=0, BOOL redoOpenedFilter=FALSE);
	void OnP4Reopen(CStringList *list);
	void OnP4EditFile(CStringList *list);
	void OnFixes(HTREEITEM changeItem, CObList *fixes);
	BOOL OnUpdateJob(CCmdUI* pCmdUI, int msgnbr);

	// other info request message handlers
	LRESULT OnGetDragToChangeNum(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetMyChangesList(WPARAM wParam, LPARAM lParam);

	void ChangeEdit(long chgnum= -1, HTREEITEM chgItem= 0);

	UINT GetItemState(HTREEITEM curr_item);
	void SetUnexpanded(HTREEITEM curr_item);
	HTREEITEM Insert(LPCTSTR text, int imageIndex, LPARAM lParam, HTREEITEM hParent, BOOL sort);
    HTREEITEM InsertChange(CP4FileStats *stats, BOOL searchFirst=TRUE);
	BOOL HasChildren(HTREEITEM curr_item);
	HTREEITEM GetLastChild(HTREEITEM currentItem);
	long GetSelectedChangeNumber();
	long GetChangeNumber(HTREEITEM item);
	HTREEITEM FindChange(long changeNum);
	HTREEITEM FindMyOpenFile(LPCTSTR fileName, HTREEITEM lastfound=NULL);
	HTREEITEM FindItemByText(LPCTSTR text);
	HTREEITEM FindFix(long changeNum, LPCTSTR jobName);
	void SetCorrectChglistImage(HTREEITEM item);

    // CMSTreeView virt func to set status messages on mouse flyover
    void OnSetFlyoverMessage(HTREEITEM currentItem);

	// CMSTreeView virt func to handle a left dbl clk after the Item is determined
	void OnLButtonDblClk(HTREEITEM currentItem);

	// Store/recall expanded state of tree nodes
	void UpdateTreeState(BOOL saveTreeState);
	
	void InitList();
	void DeleteLParams(HTREEITEM root);
	void DeleteItem(HTREEITEM item);

	BOOL DoRevert(CStringList *list, int key=0, BOOL bUnChg=FALSE);
	void ReopenAs(LPCTSTR newtype);

public:
	// Public add files funtion - used for CFileDialog adds from menu
	void AddFileList(int changeNum, CStringList *list, BOOL bDropped=FALSE);
	void GetMyChangesList(CStringList *changeList);
	void ExpandEmptyChglists();

	BOOL GetSelectedFiles( CStringList *list );
	int GetItemLevel(HTREEITEM currentItem, BOOL *underMyRoot);
	BOOL IsAFile(HTREEITEM curr_item);

	BOOL AnyInDefault();
	BOOL AnyNbredChg();

	BOOL IsAMemeberOfFileList(CString &fileName);
	BOOL IsAMemeberOfSelectionList(CString &fileName);

	// Implementation
public:
	virtual ~CDeltaTreeCtrl();
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnUpdateChgEdspec(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChgDel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChgNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChgRevorig(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChgSubmit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileDiffhead(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileRevert(CCmdUI* pCmdUI);
	afx_msg void OnChangeDel();
	afx_msg void OnChangeEdspec();
	afx_msg void OnChangeNew();
	afx_msg void OnChangeRevorig();
	afx_msg void OnChangeSubmit();
	afx_msg void OnFileDiff();
	afx_msg void OnFileRevert();
	afx_msg void OnChgListRevert();
	afx_msg void OnUpdateFileAutoresolve(CCmdUI* pCmdUI);
	afx_msg void OnFileAutoresolve();
	afx_msg void OnUpdateFileResolve(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTheirFile(CCmdUI* pCmdUI);
	afx_msg void OnFileResolve();
	afx_msg void OnFileMerge();
	afx_msg void OnTheirFindInDepot();
	afx_msg void OnTheirHistory();
	afx_msg void OnTheirProperties();
	afx_msg void OnDestroy();
	afx_msg void OnUpdateJobDescribe(CCmdUI* pCmdUI);
	afx_msg void OnJobDescribe();
	afx_msg void OnUpdateJobEditspec(CCmdUI* pCmdUI);
	afx_msg void OnJobEditspec();
	afx_msg void OnUpdateRemovefix(CCmdUI* pCmdUI);
	afx_msg void OnRemovefix();
	afx_msg void OnUpdateAddjobfix(CCmdUI* pCmdUI);
	afx_msg void OnAddjobfix();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnUpdateFileAutoedit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileAutobrowse(CCmdUI* pCmdUI);
	afx_msg void OnFileAutoedit();
	afx_msg void OnFileQuickedit();
	afx_msg void OnFileQuickbrowse();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnUpdateFileLock(CCmdUI* pCmdUI);
	afx_msg void OnFileLock();
	afx_msg void OnUpdateFileUnlock(CCmdUI* pCmdUI);
	afx_msg void OnFileUnlock();
	afx_msg void OnUpdateFileGet(CCmdUI* pCmdUI);
	afx_msg void OnFileGetWhatIf();
	afx_msg void OnFileGet();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopyclientpath(CCmdUI* pCmdUI);
	afx_msg void OnEditCopyclientpath();
	afx_msg void OnUpdateFileRevisionhistory(CCmdUI* pCmdUI);
	afx_msg void OnFileRevisionhistory();
	afx_msg void OnFileRevisionTree();
	afx_msg void OnUpdateFileAnnotate(CCmdUI* pCmdUI);
	afx_msg void OnFileTimeLapseView();
	afx_msg void OnFileAnnotate();
	afx_msg void OnFileAnnotateAll();
	afx_msg void OnFileAnnotateChg();
	afx_msg void OnFileAnnotateChgAll();
	afx_msg void OnUpdateFileInformation(CCmdUI* pCmdUI);
	afx_msg void OnFileInformation();
	afx_msg void OnUpdateWinExplore(CCmdUI* pCmdUI);
	afx_msg void OnWinExplore();
	afx_msg void OnUpdateCmdPrompt(CCmdUI* pCmdUI);
	afx_msg void OnCmdPrompt();
	afx_msg void OnUpdateChangeDescribe(CCmdUI* pCmdUI);
	afx_msg void OnChangeDescribe();
	afx_msg void OnUpdateFileOpenedit(CCmdUI* pCmdUI);
	afx_msg void OnFileOpenedit();
	afx_msg void OnUpdateFiletype(CCmdUI* pCmdUI);
	afx_msg void OnFiletype();
	afx_msg void OnUpdateMoveFiles(CCmdUI* pCmdUI);
	afx_msg void OnMoveFiles();
	afx_msg void OnUpdatePositionDepot(CCmdUI* pCmdUI);
	afx_msg void OnPositionDepot();
	afx_msg void OnUpdatePositionToPattern(CCmdUI* pCmdUI);
	afx_msg void OnPositionToPattern();
	afx_msg void OnUpdateFileSchedule(CCmdUI* pCmdUI);
	afx_msg void OnSortChgFilesByName();
	afx_msg void OnUpdateSortChgFilesByName(CCmdUI* pCmdUI);
	afx_msg void OnSortChgFilesByExt();
	afx_msg void OnUpdateSortChgFilesByExt(CCmdUI* pCmdUI);
	afx_msg void OnSortChgFilesByAction();
	afx_msg void OnUpdateSortChgFilesByAction(CCmdUI* pCmdUI);
	afx_msg void OnSortChgFilesByResolve();
	afx_msg void OnUpdateSortChgFilesByResolve(CCmdUI* pCmdUI);
	afx_msg void OnSortChgsByUser();
	afx_msg void OnUpdateSortChgsByUser(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUserSwitchtouser(CCmdUI* pCmdUI);
	afx_msg void OnUserSwitchtouser();
	afx_msg void OnUpdateClientspecSwitch(CCmdUI* pCmdUI);
	afx_msg void OnClientspecSwitch();
	afx_msg void OnUpdateRemoveViewer(CCmdUI* pCmdUI);
	afx_msg void OnRemoveViewer();
	afx_msg void OnUpdatePositionOtherChgs(CCmdUI* pCmdUI);
	afx_msg void OnPositionOtherChgs();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnUpdateViewUpdate(CCmdUI* pCmdUI);
	afx_msg void OnViewUpdate();
	afx_msg void OnPerforceOptions();
	afx_msg void OnUpdateSelectChanged(CCmdUI* pCmdUI);
	afx_msg void OnSelectChanged();
	afx_msg void OnSelectUnchanged();
	afx_msg void OnUpdateFilterSetview(CCmdUI* pCmdUI);
	afx_msg void OnFilterSetview();
	afx_msg void OnUpdateFilterClearview(CCmdUI* pCmdUI);
	afx_msg void OnFilterClearview();
	afx_msg void OnUpdateAddBookmark(CCmdUI* pCmdUI);
	afx_msg void OnAddBookmark();
	void OnFileMRUEditor(UINT  nID);
	void OnFileMRUBrowser(UINT  nID);
	void OnFileNewEditor();
	void OnFileNewBrowser();
	void CallOnChangeNew();
	void EditFile(int lparam, BOOL editing);
	void ResolveItem(HTREEITEM item);
	void SelectAllFilesInChange(HTREEITEM changeitem, int resolveFlag=0);
	long PositionChgs(const CString &path, BOOL lookInMine, BOOL lookInOthers=TRUE, BOOL addToSelectionSet=FALSE);
	long PositionToFileInChg(const CString &path, HTREEITEM start, HTREEITEM root, BOOL afterExpand=FALSE, BOOL addToSelectionSet=FALSE);
	void SaveExpansion();
	DECLARE_MESSAGE_MAP()

	BOOL IsEditableFile();
	BOOL GetClientPath(HTREEITEM item, CString& clientPath);

	BOOL OKToAddSelection( HTREEITEM currentItem );
	BOOL TryDragDrop( HTREEITEM currentItem );

	void OnFileMergeResolve(BOOL bRunMerge); 
	BOOL SelectChgUnchg(BOOL bChged, int *totfiles=NULL);

	void FileAnnotate(BOOL bAll, BOOL bChg=FALSE);

	void FileGet(BOOL whatIf);
};

#endif // __DELTATREECTRL
/////////////////////////////////////////////////////////////////////////////
