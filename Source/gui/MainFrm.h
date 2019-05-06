//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "BranchView.h"
#include "ClientView.h"
#include "DepotView.h"
#include "DeltaView.h"
#include "JobView.h"
#include "OldChgView.h"
#include "LabelView.h"
#include "UserView.h"
#include "StatusView.h"
#include "FlatSplitter.h"
#include "ZimbabweSplitter.h"
#include "P4Menu.h"

#define	MISC_TIMER	 97
#define SORT_TIMER	 98
#define UPDATE_TIMER 99

// Depot Filter Types
enum _dft
{
	DFT_NOTFILTERED,// Depot is Not Filtered
	DFT_ALLOPENED,	// Filtered to show all opened files
	DFT_MYOPENED,	// Filtered to show my opened files
	DFT_LIST,		// Filtered according to m_FilterFileList
	DFT_FIND		// Filtered according to Find list
};

// Helper App Message types
enum _ham
{
	HAM_ADDHANDLE,	// LPARAM is window handle of helper app
	HAM_RMVHANDLE	// LPARAM is window handle to be removed from list
};

// Message used for view updates when file open status changes
#define WM_UPDATEOPEN (WM_USER+400)
#define WM_INITTREE (WM_USER+401)
#define WM_GETDRAGTOCHANGENUM (WM_USER+402)
#define WM_SETUNRESOLVED (WM_USER+406)

// Messages to interogate for selected list in depot wnd
#define WM_GETSELCOUNT (WM_USER+408)
#define WM_GETSELLIST (WM_USER+409)

// Messages from chg wndo to depot wndo to initiate an edit
#define WM_FILEEDITTXT (WM_USER+410)
#define WM_FILEEDITBIN (WM_USER+411)

#define WM_JOBDELETED (WM_USER+413)

// Message to fetch a list of my changes
#define WM_GETMYCHANGESLIST (WM_USER+415)
#define WM_REQUESTBRANCHLIST (WM_USER+416)

// Message to notify that new branchlist is available
#define WM_NEWBRANCHESLIST (WM_USER+417)

// Message to request that depot view perform integ of entire branch
#define WM_BRANCHINTEG (WM_USER+418)

// Message to pass a list of 'P4 revert' results
#define WM_REVERTLIST (WM_USER+419)

// Messages to query the job view for contents
#define WM_QUERYJOBS (WM_USER+420)
#define WM_QUERYJOBSPEC (WM_USER+421)
#define WM_QUERYJOBFIELDS (WM_USER+422)
#define WM_FETCHJOBS (WM_USER+423)

// Message to indicate bad client
#define WM_CLIENTERROR (WM_USER+424)

// Message to edit jobs filter
#define	WM_JOB_FILTER (WM_USER+425)

// Message to indicate who received a m_CF_DEPOT drop
#define	WM_DROPTARGET (WM_USER+426)

// Messages from chg wndo to depot wndo to initiate a browse
#define WM_FILEBROWSETXT (WM_USER+427)
#define WM_FILEBROWSEBIN (WM_USER+428)

// Message to request that depot view perform integ from changelist
#define WM_CHANGELISTINTEG (WM_USER+429)

// Message to request that depot view run assoc viewer on head rev of a given file
#define WM_VIEWHEAD (WM_USER+430)

// Message to request m_IsDepotFiltered setting from depot view
#define	WM_ISFILTEREDONOPEN	(WM_USER+431)

// Message to inform the depot view to rebuild its open file filter list
#define	WM_REDOOPENEDFILTER (WM_USER+432)

// Message to inform the label view to popup a drag and drop menu
#define	WM_LABELDROPMENU (WM_USER+433)

// Message to tell a listview to find a string in the listview
#define WM_FINDPATTERN (WM_USER+434)

// Message to pass the fstats of files opened for add to the Depotview
#define WM_SETADDFSTATS (WM_USER+435)

// Message to get the fstats of files opened for add from the Depotview
#define WM_GETADDFSTATS (WM_USER+436)

// Messages to query the job view for column names and current selection
#define WM_QUERYJOBCOLS (WM_USER+437)
#define WM_QUERYJOBSELECTION (WM_USER+438)

// Message to set the focus in a dialog box
#define WM_GOTODLGCTRL (WM_USER+439)

// Message to indicate client and user have changed
#define WM_NEWCLIENT (WM_USER+440)
#define WM_NEWUSER   (WM_USER+441)

// Messages to SubChg & Label panes to update clear filter menu item
#define WM_SUBCHGOUFC	(WM_USER+442)
#define WM_LABELOUFC	(WM_USER+443)

// Message to ModifyStyle of window(s)
#define	WM_MODIFYSTYLE	(WM_USER+444)

// Messages to Request the Dialog box listing all the item in a list control
// and to return the result from the Dialog box
#define	WM_FETCHOBJECTLIST	(WM_USER+445)
#define	WM_GETOBJECTLIST	(WM_USER+446)
#define	WM_BROWSECALLBACK1	(WM_USER+447)
#define	WM_BROWSECALLBACK2	(WM_USER+448)
#define	WM_BROWSECALLBACK3	(WM_USER+449)
#define	WM_BROWSECALLBACK4	(WM_USER+450)
#define	WM_WIZFETCHOBJECTLIST	(WM_USER+451)
#define	WM_WIZGETOBJECTLIST		(WM_USER+452)
#define	WM_INTEGFETCHOBJECTLIST	(WM_USER+453)
#define	WM_INTEGGETOBJECTLIST	(WM_USER+454)
#define	WM_SELECTTHIS		(WM_USER+455)

// Message to notify modeless dialogs to cleanup
#define	WM_QUITTING			(WM_USER+456)

// Message posted when modeless dialog is destroyed
#define	WM_P4DLGDESTROY		(WM_USER+457)

// Message to request that the User set a password
#define	WM_USERPSWDDLG		(WM_USER+458)

// Message to request that the User set a password
#define	WM_PERFORCE_OPTIONS	(WM_USER+459)

// Messages to trigger the action of a Browse button being pressed
#define	WM_BROWSECLIENTS	(WM_USER+460)
#define	WM_BROWSEUSERS		(WM_USER+461)

// Message to clear a list control so it updates itself.
#define	WM_CLEARLIST		(WM_USER+462)

// Message to Enable/disable dialog buttons (lParam is TRUE or FALSE)
#define	WM_ENABLEDISABLE	(WM_USER+463)

// Message to set the name of a file viewer - addr in lParam
#define	WM_SETVIEWER		(WM_USER+464)

// Message to Righathand Pane to have it activate its modeless edit dialogs
#define	WM_ACTIVATEMODELESS	(WM_USER+465)

// Message to Depot pane with lParam contain CSTring ptr to path to add
#define	WM_ADDBOOKMARK		(WM_USER+466)

// Message to Post that when received forces the focus to lParam window
#define	WM_FORCEFOCUS		(WM_USER+467)

// Message from help app
#define	WM_HELPERAPP		(WM_USER+0x1C00)


#define UPDATE_FAILED   1
#define UPDATE_SUCCESS  2


/////////////////////////////////////////////////////////////////////////////
// COleEditDropTarget

class CTBDropTarget : public COleDropTarget
{
// Construction
public:
    CTBDropTarget();

// Implementation
public:
    virtual ~CTBDropTarget();  
    
   //
   // These members MUST be overridden for an OLE drop target
   // See DRAG and DROP section of OLE classes reference
   //
   DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD 
                                                dwKeyState, CPoint point );
   DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD 
                                               dwKeyState, CPoint point );
   void OnDragLeave(CWnd* pWnd);               
   
   BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT 
                                          dropEffect, CPoint point );    
#ifdef USE_NATIVE
   void SetHWnd(HWND hwnd) { m_hWnd = hwnd;};
#endif

};


class CDeltaView;
class CDepotView;

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	OSVERSIONINFO m_osVer;
	BOOL m_IconsInMenus;
	BOOL m_ShowOnlyNotInDepot;
	BOOL m_InPopUpMenu;
	int m_NbrDisplays;
	CString m_ReviewListClass;
	CRITICAL_SECTION CriticalSection; 

protected:
	CZimbabweSplitter m_wndHSplitter;
	CFlatSplitter m_wndVSplitter;
		
	CP4 *m_pP4;
	BOOL m_FirstActivation;
	BOOL m_IsActive;
	HWND m_hWndLastActive;	// last active dialog window when loose focus
	BOOL m_GotInput;
	int  m_ClientGet;	// 1==got client; 0==cancel; -1==Back
	int  m_P4QTree;		// 0==none, 1==p4v, 2==p4tree.exe, 3==P4QTree.exe
	int  m_P4TLV;		// 0==none, 1==p4v
	DWORD m_P4Vver;		// p4v's version eg 20042
	CObList m_hWndHelperApp;

	CStatusView *m_pStatusView;
	CDepotView *m_pDepotView;
	CDeltaView *m_pDeltaView;
	CLabelView *m_pLabelView;
	CClientView *m_pClientView;
	CJobView *m_pJobView;
	CBranchView *m_pBranchView;
	CUserView *m_pUserView;
	COldChgView *m_pOldChgView;
	CView *m_pRightView;

	CFindReplaceDialog *m_pFRDlg;
	CString m_FindWhatStr;
	int m_FindWhatFlags;
	int m_FindStatusFlags;
	
	HMODULE m_USER32dll;
	int m_currentTab;
	int m_Need2ExpandDepot;
	int m_Need2Poll4Jobs;
	StatusView m_StatusBarWarnLevel;
	int m_StatusUpdateInterval;
	BOOL m_Need2RefreshOldChgs;
	BOOL m_bStatusBarWarnMsg;
	BOOL m_FullRefreshRequired;
	UINT m_CF_FILENAME;
	UINT m_CF_FILENAMEW;
	CStringList m_StringList;
	CStringList m_StringList2;
	int  m_SelectedChange;
	BOOL m_Quitting;

	// Switch to indicate what to do after running P4 INFO
	int m_GetCliRootAndContinue;
	enum WindowType
	{
		P4INFO_P4INFO,
		P4INFO_ADDFILE,
		P4INFO_DOSBOX
	};

	// For use in timer update
	UINT_PTR m_Timer;
	long m_LastUpdateTime;
    BOOL m_LastUpdateResult;
	BOOL m_ClientError;
	int  m_DoNotAutoPollCtr;

	// Update on uncover timers
	DWORD m_DeltaUpdateTime;
	DWORD m_LabelUpdateTime;
	DWORD m_BranchUpdateTime;
	DWORD m_UserUpdateTime;
	DWORD m_ClientUpdateTime;
	DWORD m_JobUpdateTime;
	DWORD m_OldChgUpdateTime;

	// For modeless dialogboxes
	BOOL m_ModlessUp;

	// Printing structures
	HANDLE m_hDevNames;
	HANDLE m_hDevMode;
	RECT   m_rtMinMargin;   
	RECT   m_rtMargin;
	BOOL   m_bMetric;

	// Submenus for Tools, Bookmark & Favorites menus
	CMenu m_ToolsSubMenu[MAX_TOOLS_SUBMENUS];
	CMenu m_ContextToolsSubMenu[MAX_TOOLS_SUBMENUS];
	CMenu m_BkMkSubMenu[MAX_BOOKMARK_SUBMENUS];
	CMenu m_FavSubMenu[MAX_FAVORITE_SUBMENUS];

	// Toolbar drag & drop data
	BOOL m_bNoRefresh;
	int  m_DragEnterTabNbr;
	DWORD m_DragEnterTime;
	DROPEFFECT m_CurDropEffect;

	//	Modless dialogs
	CObList m_DlgWndList;

public:

	CTBDropTarget *m_pDropTgt;

// Operations
public:
	void ViewBranches( );
	void ViewLabels( );
	void ViewClients( );
	void ViewUsers( );
	void ViewJobs( );
	void UpdateCaption(BOOL updatePCU = TRUE);
	void SetLastUpdateTime(BOOL updateResult);
	void SetGotUserInput( ) { m_GotInput = TRUE; }
	void ClearLastUpdateTime(); 
	void OnCmdPromptPublic();
	int  HaveTLV() { return m_P4TLV; }
	int  HaveP4QTree() { return m_P4QTree; }
	DWORD GetP4Vver() { return m_P4Vver; }
	BOOL IsQuitting() { return m_Quitting; }
	HWND OldChgsWnd();
	HWND ClientWnd();
	HWND BranchWnd();
	HWND LabelWnd();
	HWND UserWnd();
	HWND JobWnd();

	// Status logging functions
	void UpdateStatus(LPCTSTR, BOOL forceRedraw=FALSE);	// status bar rightmost pane
    void ClearStatus(BOOL forceRedraw=FALSE) { UpdateStatus(_T("")); }	// status bar rightmost pane
	void SetStatusBarLevel(StatusView level);						// set status bar warning level
	void AddToStatusLog( LPCTSTR txt, StatusView level = SV_MSG, bool showDialog = false );		// add to status view log
	void AddToStatusLog( CStringArray *pArray, StatusView level = SV_MSG, bool showDialog = false );	// add to status view log
	void ClearStatusLog();									// clear status view log
	void SetMessageText(LPCTSTR lpszText);

	CString GetFindWhatStr() { return m_FindWhatStr; }
	int GetFindWhatFlags() { return m_FindWhatFlags; }

	void SetFullRefresh(BOOL full) { m_FullRefreshRequired= full; }
	BOOL IsFullRefreshRequired() {return m_FullRefreshRequired; }
	BOOL UpdateRightView();

	// Support for NT3.51 file dialog
	void Expand83FileNames(CStringList *files, char *buf);
	void Expand83Path(CString &path);
	char *GetLongName(LPCTSTR shortpath);

	// Get the pending chglist pane
	CDeltaView *GetDeltaView() { return m_pDeltaView; }

	// Get Drag and Drop item
	CString GetDragFromJob();
	CString GetDragFromClient();
	CString GetDragFromUser();

	// Filtering functions
	BOOL IsClientFilteredOut(CP4Client *client, CString *user=NULL, CString *curcli=0, CString *defcli=0);
	BOOL IsBranchFilteredOut(CP4Branch *branch, CString *user=NULL);

// Overrides
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);

// Implementation
protected:
	void SetWindowReferences();

public:
	DWORD GetExeVersion( TCHAR *exeName );
	int FindDollarArg(CString commandLine, int offset);
	CString ReplaceDollarArg(CString commandLine, int i, LPCTSTR str);
	int FindPercentArg(CString commandLine, int offset);
	void AddToolsToContextMenu(CP4Menu *popmenu);
	void LoadToolsMenu();
	void LoadBkMkMenu();
	void LoadFavMenu();
	void SwitchPanes(CView * pView, BOOL bShift);
	void ShowPerforceInfo();
	void OnPerforceOptions(BOOL b, BOOL portWasChanged = FALSE, int contextPage=0, int intialPage=0);
	int GetRightSplitter( );
	void SetRightSplitter(int tabNumber);
	void SetModelessUp(BOOL b) { m_ModlessUp = b; }
	BOOL IsModlessUp() { return m_ModlessUp; }
	void FinishedGettingChgs(BOOL bNeed2RefreshOldChgs);
	void ExpandDepotIfNeedBe();
	CString GetCurrentItemPath();
	BOOL PrintString(CString &string, CString caption);
	void PageSetup();
	HWND GetLeftHandWnd() { return m_pDepotView->m_hWnd; }
	HWND GetRightHandWnd() { return m_pRightView->m_hWnd; }
	HWND GetVSplitterWnd() { return m_wndVSplitter.m_hWnd; }
	HWND GetHSplitterWnd() { return m_wndHSplitter.m_hWnd; }
	HWND GetDepotWnd() { return m_pDepotView->GetTreeCtrl().m_hWnd; }
	HWND GetBranchesWnd() { return m_pBranchView->GetListCtrl().m_hWnd; }
	CClientView * GetClientView() { return m_pClientView; }
	CUserView * GetUserView() { return m_pUserView; }
	BOOL ClientSpecSwitch(CString switchTo, BOOL bAlways = FALSE, BOOL portWasChangedAlso = FALSE);
	int  CreateNewChangeList(int key, CString *description=NULL, BOOL autoOK=FALSE);
	void SetDepotCaption( BOOL);
	void AssembleDepotStringList(CStringList *list,BOOL b) { m_pDepotView->GetTreeCtrl().AssembleStringList(list,b); }
	BOOL IsFileInList(CString *filename, CStringList *list);
	void OnFileAddSetDir(LPTSTR lpInitDir = NULL);
	LPVOID P4GetEnvironmentStrings();
	BOOL SetMenuIcon(CCmdUI* pCmdUI, BOOL bEnable);
	void SaveToolBarBitmap(int iconnbr, UINT id);

	void GetClients(CStringArray *list);
	void GetUsers(CStringArray *list);
	int GetClientColNamesAndCount(CStringArray &cols);
	int GetUserColNamesAndCount(CStringArray &cols);

	void EditJobSpec(CString *jobname);
	void OnJobConfigure() { ::PostMessage(m_pJobView->m_hWnd, WM_COMMAND, ID_JOB_CONFIGURE, 0); }

	// Public function to start updates
	void UpdateDepotandChangeViews(BOOL redrill, int key=0);
	void ExpandDepotString(const CString &path, BOOL newPath);
	void DoNotAutoPoll() { m_DoNotAutoPollCtr++; };
	void ResumeAutoPoll();
	void WaitAWhileToPoll();
	long PositionChgs(const CString &path, BOOL lookInMine, BOOL lookInOthers=TRUE, BOOL addToSelectionSet=FALSE);

	// Right pane update timer functions
	void SetDeltaUpdateTime(DWORD time)  { m_DeltaUpdateTime = time; }
	void SetLabelUpdateTime(DWORD time)  { m_LabelUpdateTime = time; }
	void SetBranchUpdateTime(DWORD time) { m_BranchUpdateTime = time; }
	void SetUserUpdateTime(DWORD time)   { m_UserUpdateTime = time; }
	void SetClientUpdateTime(DWORD time) { m_ClientUpdateTime = time; }
	void SetJobUpdateTime(DWORD time)    { m_JobUpdateTime = time; }
	void SetOldChgUpdateTime(DWORD time) { m_OldChgUpdateTime = time; }

	// Toolbar Drag & Drop functions
	DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	void OnDragLeave(CWnd* pWnd);
	int CvtPointToTabNbr(int x, int y);

	// Functions for selecting (multiple) files in the depot
	void SetAdd2ExpandItemList(BOOL b) { m_pDepotView->GetTreeCtrl().SetAdd2ExpandItemList(b); }
	BOOL IsExpandDepotContinuing() { return m_pDepotView->GetTreeCtrl().IsExpandDepotContinuing(); }
	BOOL PumpMessages( ) { return m_pDeltaView->GetTreeCtrl().PumpMessages( ); }
	BOOL SelectExpandItemList() { return m_pDepotView->GetTreeCtrl().SelectExpandItemList(); }
	INT_PTR  GetExpandItemListCount() { return m_pDepotView->GetTreeCtrl().GetExpandItemListCount(); }

	// a wrapper to make OnToolTipText public
	BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult ) 
		{ return CFrameWnd::OnToolTipText(id, pNMHDR, pResult); }

	// Support for modeless dialogs
	void SetModelessWnd(CDialog *pDlg);
	void SignalAllModelessDlgs(UINT msg);
	BOOL IsPendChgEditInProgress() { return m_pDeltaView->GetTreeCtrl().IsEditInProgress(); }

	void loadMRUPcuMenuItems(CMenu *pMenu);

	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	// Bitmaps to use as icons on menu
	// and the menu ID associated with each icon
	// plus the count of bitmaps in use
	CBitmap		m_MenuBitmap[32];
	CBitmap		m_MenuBitDis[32];
	UINT		m_MenuIDbm[32];
	int			m_MenuBmpCtr;
	BOOL		m_HasDisabled;

// Generated message map functions
public:
	afx_msg void OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGettingStartedWithP4win();
	afx_msg void OnViewSubmitted();
protected:
	DECLARE_MESSAGE_MAP()
	void OnCmdPromptContinue();
	int GetOpenedFiles( CStringList *list );
	int InsertSelectedObjectIntoCmdline(CString &cmdLine, int offset);
	int InsertAllSelectedFilesIntoCmdline(CString &cmdLine, int offset, 
				BOOL bUseOpened, BOOL bDepotSyntax);
	void ExecOnceForEachFile(int offset, CString &cmdLine, TCHAR sw,
				LPCTSTR lpInitDirectory, DWORD dwCreationFlags, LPSTARTUPINFO lpStartupInfo, 
				LPPROCESS_INFORMATION lpProcessInformation, BOOL bNoCommand, BOOL bRefresh,
				BOOL isOutput2Status);
	CMenu * GetBookmarkMenu();
	CMenu * GetFavoriteMenu();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateStatus(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPerforceOptions();
	afx_msg void OnUpdateP4Options(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnPerforceInfo();
	afx_msg void OnUpdatePerforceInfo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewSwitch(CCmdUI* pCmdUI);
	afx_msg void OnViewBranches();
	afx_msg void OnViewChanges();
	afx_msg void OnViewClients();
	afx_msg void OnViewJobs();
	afx_msg void OnViewLabels();
	afx_msg void OnViewUsers();
	afx_msg void OnUpdateViewDepot(CCmdUI* pCmdUI);
	afx_msg void OnViewDepot(UINT nID);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnUpdateJobNew(CCmdUI* pCmdUI);
    afx_msg void OnUpdateBranchNew(CCmdUI* pCmdUI);
    afx_msg void OnUpdateLabelNew(CCmdUI* pCmdUI);
    afx_msg void OnJobNew();
    afx_msg void OnBranchNew();
    afx_msg void OnLabelNew();
	afx_msg void OnUpdateClientEditmy(CCmdUI* pCmdUI);
	afx_msg void OnClientEditmy();
	afx_msg void OnUpdateUserEditmy(CCmdUI* pCmdUI);
	afx_msg void OnUserEditmy();
	afx_msg void OnSysColorChange();
	afx_msg void OnUpdateFileAdd(CCmdUI* pCmdUI);
	afx_msg void OnFileAdd();
	afx_msg void OnUpdateClientspecNew(CCmdUI* pCmdUI);
	afx_msg void OnClientspecNew();
	afx_msg void OnUpdateSubmittedActivate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUserNew(CCmdUI* pCmdUI);
	afx_msg void OnUserNew();
	afx_msg void OnUpdateUserPassword(CCmdUI* pCmdUI);
	afx_msg void OnUserPassword();
	afx_msg void OnUpdateMruPcu(CCmdUI* pCmdUI);
	afx_msg void OnMruPcu0();
	afx_msg void OnMruPcu1();
	afx_msg void OnMruPcu2();
	afx_msg void OnMruPcu3();
	afx_msg void OnMruPcu4();
	afx_msg void OnMruPcu5();
	afx_msg void OnMruPcu6();
	afx_msg void OnMruPcu7();
	afx_msg void OnMruPcu8();
	afx_msg void OnMruPcu9();
	afx_msg void OnUpdateMruPcuHdr(CCmdUI* pCmdUI);
	afx_msg void OnShowDeletedFiles();
	afx_msg void OnUpdateShowDeletedFiles(CCmdUI* pCmdUI);
	afx_msg void OnSortByExtension();
	afx_msg void OnUpdateSortByExtension(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewWarnAndErrs(CCmdUI* pCmdUI);
	afx_msg void OnViewWarnAndErrs() ;
	afx_msg void OnUpdatePositionToPattern(CCmdUI* pCmdUI);
	afx_msg void OnPositionToPattern();
	afx_msg void OnUpdatePositionToPrev(CCmdUI* pCmdUI);
	afx_msg void OnPositionToPrev();
	afx_msg void OnUpdatePositionToNext(CCmdUI* pCmdUI);
	afx_msg void OnPositionToNext();
	afx_msg void OnUpdateWinExplore(CCmdUI* pCmdUI);
	afx_msg void OnWinExplore();
	afx_msg void OnUpdateCmdPrompt(CCmdUI* pCmdUI);
	afx_msg void OnCmdPrompt();
	afx_msg void OnCustomizeTools();
	afx_msg void OnUpdateToolsImport(CCmdUI* pCmdUI);
	afx_msg void OnToolsImport();
	afx_msg void OnUpdateToolsExport(CCmdUI* pCmdUI);
	afx_msg void OnToolsExport();
	afx_msg void OnPageSetup();
	afx_msg void OnUpdateMakeDefaultPcu(CCmdUI* pCmdUI);
	afx_msg void OnMakeDefaultPcu();
	afx_msg void OnMenuSelect( UINT nItemID, UINT nFlags, HMENU hSysMenu );
	afx_msg void OnUpdateViewChgsByUser(CCmdUI* pCmdUI);
	afx_msg void OnViewChgsByUser();
	afx_msg void OnUpdateViewChgsByClient(CCmdUI* pCmdUI);
	afx_msg void OnViewChgsByClient();
	afx_msg void OnUpdateDescribeChg(CCmdUI* pCmdUI);
	afx_msg void OnDescribeChg();
	afx_msg void OnUpdateDescribeJob(CCmdUI* pCmdUI);
	afx_msg void OnDescribeJob();
	afx_msg void OnUpdateBookmarks(CCmdUI* pCmdUI);
	afx_msg void OnBookmarks();
	afx_msg void OnUpdateOrganizeBookmarks(CCmdUI* pCmdUI);
	afx_msg void OnOrganizeBookmarks();
	afx_msg void OnUpdateFavorites(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddFavorite(CCmdUI* pCmdUI);
	afx_msg void OnAddFavorite();
	afx_msg void OnUpdateLogout(CCmdUI* pCmdUI);
	afx_msg void OnLogout();
	afx_msg void OnUpdateOrganizeFavorites(CCmdUI* pCmdUI);
	afx_msg void OnOrganizeFavorites();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg LRESULT OnFindReplace(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateMenuWithIcon(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewUpdate(CCmdUI* pCmdUI);
	afx_msg void OnViewUpdate();
	afx_msg void OnUpdateCancelCommand(CCmdUI* pCmdUI);
	afx_msg void OnCancelCommand();
	afx_msg void OnCancelButton();
	afx_msg void OnUpdateCloseAllModelessDlgs(CCmdUI* pCmdUI);
	afx_msg void CloseAllModelessDlgs();

	void ImportTools(CString *filename, CString *errorTxt);
	void OnUpdateTool(CCmdUI* pCmdUI);
	void OnTool(UINT nID);
	void OnUpdateBkMk(CCmdUI* pCmdUI);
	void OnBkMk(UINT nID);
	void OnUpdateFav(CCmdUI* pCmdUI);
	void OnFav(UINT nID);

	LRESULT OnP4InitInfo(WPARAM wParam, LPARAM lParam);
	LRESULT OnStatusAdd(WPARAM wParam, LPARAM lParam);
	LRESULT OnStatusAddArray(WPARAM wParam, LPARAM lParam);
	LRESULT OnStatusClear(WPARAM wParam, LPARAM lParam);
	LRESULT OnExternalActivation(WPARAM wParam, LPARAM lParam);
	LRESULT OnExternalSendcmd(WPARAM wParam, LPARAM lParam);
	LRESULT OnExternalRplycmd(WPARAM wParam, LPARAM lParam);
	LRESULT OnClientError(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4ListOp(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4FileInformation(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Diff(WPARAM wParam, LPARAM lParam);
	LRESULT OnNewClient(WPARAM wParam, LPARAM lParam);
	LRESULT OnNewUser(WPARAM wParam, LPARAM lParam);
	LRESULT OnUserPasswordDlg( WPARAM wParam, LPARAM lParam );
	LRESULT OnBrowseClientsCallBack(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4DialogDestroy(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4HelperApp(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4OptionsDlg(WPARAM wParam, LPARAM lParam);
	LRESULT OnActivateModeless(WPARAM wParam, LPARAM lParam);

private:
	void EditConnectionParam( CCmdUI* pCmdUI, const CString & );
	BOOL m_ShowOptionsDlg;
	void OnMruPcu(int nbr);
	void OnNew();
	void OnFileRevisionhistory();
	LPTSTR GetNextToolToken(LPTSTR pStr, CString &token);
};

static UINT WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);


/////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK ChildSetRedraw(HWND hwnd, LPARAM lParam);

/////////////////////////////////////////////////////////////////////////////

extern CWnd *MainFrameCWnd;

inline CMainFrame * MainFrame()
{
    CMainFrame * mainWnd = (CMainFrame *)AfxGetMainWnd();
	ASSERT((mainWnd && (mainWnd)->IsKindOf(RUNTIME_CLASS(CMainFrame))) || !mainWnd);
    return mainWnd;
}
