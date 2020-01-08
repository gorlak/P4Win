/*
 * Copyright 1997, 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */


// P4Registry.h
// CP4Registry is an extremely primitive class that caches all registry
// information, and potentially could be used to enforce consistency of
// registry values.

#ifndef __P4REGISTRY__
#define __P4REGISTRY__

#include "resource.h"


// Class CP4Registry - handle registry info and updating for P4Win

class CP4Registry
{
public:
	CP4Registry();
	~CP4Registry();

protected:
	// Not in p4win registry tree
	BOOL m_AttemptedRead;
	BOOL m_Virgin;
	BOOL m_NoCliHost;
	CString m_MyID;
	CString m_Hostname;
	
	//	Temporary over-rides to permanent connection params
	//
	CString m_P4Port_Temp;
	CString m_P4User_Temp;
	CString m_P4Client_Temp;
	CString m_P4Password_Temp;
	CString m_P4Charset_Temp;
	BOOL m_bCharsetFromCmdli;

	//	the connection params in the registry. ("permanent")
	//
	CString m_P4Port_Perm;
	CString m_P4User_Perm;
	CString m_P4Client_Perm;
	CString m_P4Password_Perm;
	CString m_P4Charset_Perm;

	// Previous values for client and user
	//
	CString m_P4Client_Prev;
	CString m_P4Client_PrevDef;
	CString m_P4User_Prev;
	CString m_P4User_PrevDef;

	//////////////
	// Options Key
	CString m_EditApp;
	CString m_DiffApp;
	CString m_DiffOptArgs;
	CString m_MergeApp;
	CString m_MergeOptArgs;
	int  m_TabWidth;
	int  m_WhtSpFlag;
	int  m_MrgTabWidth;
	int  m_MrgWhtSpFlag;
	int  m_AnnotateWhtSpace;
	int  m_AnnotateIncInteg;
	int  m_TLVWhtSpace;
	int  m_TLVIncInteg;
	int  m_ResolveWhtSp;
	int  m_UseTempForView;
	CString m_UseTempForExts;
	BOOL m_EditAppIsConsole;
	BOOL m_DiffAppIsConsole;
	BOOL m_DiffAppIsClose;
	BOOL m_DiffAppIsBinary;
	int m_DiffInternal;
	BOOL m_DiffOptArgChk;
	int m_MergeInternal;
	BOOL m_MergeOptArgChk;
	BOOL m_MergeNSF;
	BOOL m_MergeAppIsConsole;
	BOOL m_MergeAppIsClose;
	BOOL m_AlwaysShowFocus;
	BOOL m_AlwaysWarnOnRevert;
	BOOL m_AllowPromptAbove;
	BOOL m_AutoMinEditDlg;
	int  m_DoubleClickOption;
	int  m_OptionStartUpOption;
	int  m_OptionStartUpPage;
	int  m_StatusUpdateInterval;
	int  m_LabelAddRplDefault;
	int  m_Diff2Default1;
	int  m_Diff2Default2;

	int  m_Explorer;
	BOOL m_2Panes;
	CString m_AltExpl;
	CString m_TempDir;
	CString m_ExpandPath;
	CString m_WarnLimit;
	CString m_WarnLimitDiff;
	CString m_WarnLimitOpen;
	CString m_ExtSortMax;
	CString m_MinMultiLineSize;
	CString m_UserFilter;
	CString m_ClientFilter;
	CString m_LabelFilterByOwner;
	BOOL m_FilteredByUser;
	BOOL m_FilteredByClient;
	BOOL m_LabelFilterByOwnerFlag;
	BOOL m_LabelFilterIncBlank;
	BOOL m_DescWrapSw;
	int  m_DescWrap;
	int  m_ExpandFlag;
	int  m_BusyWaitTime;
	int  m_UseLongChglistDesc;
	int  m_SyncDlgFlag;
	CString m_UseOpenForEditExts;
	BOOL m_UseOpenForEdit;
	BOOL m_UseShortRevHistDesc;
	BOOL m_UseShortSubmittedDesc;
	BOOL m_IsNoCopy;
	BOOL m_IsForceInteg;
	BOOL m_IsForceDirect;
	BOOL m_IsDeleteSource;
	BOOL m_IsPermitDelReadd;
	int  m_DelReaddType;
	BOOL m_IsBaselessMerge;
	BOOL m_IsIndirectMerge;
	BOOL m_IsPropagateTypes;
	BOOL m_IsBaseViaDelReadd;
	BOOL m_UseNewChglist;
	BOOL m_UseNewChglist2;
	BOOL m_SyncFirstDefault;
	BOOL m_WarnAlreadyOpened;
	BOOL m_ShowDeleted;
	BOOL m_SortByExtension;
	BOOL m_SortChgFilesByAction;
	BOOL m_SortChgFilesByName;
	BOOL m_SortChgFilesByExt;
	BOOL m_SortChgFilesByResolve;
	BOOL m_ShowToolBar;
	int  m_ShowEntireDepot;
	BOOL m_ShowFileType;
	BOOL m_ShowOpenAction;
	BOOL m_ShowChangeDesc;
	BOOL m_SortChgsByUser;
	BOOL m_ShowCommandTrace;
	BOOL m_ShowConnectSettings;
	BOOL m_ShowConnectPort1st;
    BOOL m_ShowClientPath4Chgs;
    BOOL m_ShowClientPath;
    BOOL m_ShowDepotPathHiLite;
    BOOL m_ShowStatusTime;
    BOOL m_ShowHiddenFilesNotInDepot;
    BOOL m_Use24hourClock;
    BOOL m_PreserveSpecFormat;
    BOOL m_AutoExpandOptions;
    BOOL m_LabelFilesInDialog;
    BOOL m_LabelShowPreviewDetail;
    BOOL m_Diff2InDialog;
    BOOL m_AutoTreeExpand;
    BOOL m_UseNotepad4WarnAndErr;
    BOOL m_Cvt2ValComboToChkBx;
    BOOL m_IconsInMenus;
    BOOL m_ExpandChgLists;
    BOOL m_ReExpandChgs;
	BOOL m_EnableSubChgIntegFilter;
	BOOL m_EnableRevHistShowIntegs;
	BOOL m_EnablePendingChgsOtherClients;
	long m_AddFilterExclude;
	BOOL m_AutoPoll;
	long m_AutoPollTime;
	BOOL m_AutoPollIconic;
	BOOL m_AutoPollJobs;
	BOOL m_LogoutOnExit;
	BOOL m_ClearAndReload;
	BOOL m_ReloadOnUncover;
	long m_ReloadUncoverTime;
	long m_ChglistSyncDef;
	long m_FetchChangeCount;
    BOOL m_FetchAll;
	long m_FetchJobCount;
    BOOL m_FetchAllJobs;
	BOOL m_JobsEnabled;
	long m_FetchHistCount;
    BOOL m_FetchAllHist;
    long m_FetchCompleteHist;
    long m_HistListHeight;
    long m_JobFilterWidth;
	BOOL m_UseDepotInClientView;
	BOOL m_Use256colorIcons;
	BOOL m_DontShowYouHaveChgClientView;
	BOOL m_DontShowYouHaveCr8NewClient;
	BOOL m_DontShowDiscardFormChgs;
	BOOL m_AllowExceptionReporting;
	BOOL m_TryResetingFocus;

	long m_AutoGetOnEdit;

	CString m_FontFace;
	int		m_FontSize;
	BOOL	m_IsFontItalic;
	int		m_FontWeight;
	int		m_FindFilesWidth;
	int		m_P4StartWith;
	BOOL	m_P4BusyCursor;
	BOOL	m_CloseAfterFind;
	BOOL	m_SubmitOnlyChged;
	BOOL	m_RevertUnchged;
	BOOL	m_AutoReopen;
	BOOL	m_FilterPendChgsByMyClient;
	CString	m_FilterPendChgsByPath;
	int		m_LabelDragDropOption;
	int		m_ResolveDefault;
	int		m_Resolve2wayDefault;
	int		m_Resolve3wayDefault;
	BOOL	m_ConvertJobNbr;
	
	//////////////
	// Settings Key
	CString m_AddFileCurDir;
	CString m_AddFileExtFilter;
	CString m_AddFileFilter;
	int m_AddFileFilterIndex;
	int m_DefaultDnDfromExp;
	int m_MultiProcessorSleep;
	int m_MaxStatusLines;
	int m_ShowStatusMsgs;
	int m_ShowTruncTooltip;
	int m_DontThreadDiffs;
	CString m_PendChgExpansion;
	CString m_LastBranch;
	CString m_LastLabel;
	CString m_LastTemplate;
	CString m_LocalCliTemplate;
	BOOL m_LocalCliTemplateSw;
	BOOL m_UseClientSpecSubmitOpts;
	BOOL m_SwapButtonPosition;
	int m_BranchFilteredFlags;		// 1 = by Owner; 0x10 by current User
	CString m_BranchFilterOwner;
	int m_ClientFilteredFlags;		// 1 = by Owner; 2 = by Host; 4 = Description Contains
	CString m_ClientFilterOwner;
	CString m_ClientFilterHost;
	CString m_ClientFilterDesc;

	//////////////
	// Layout Key
	CRect m_WindowPosition;
	BOOL m_Iconic;
	BOOL m_Maximized;
	long m_HSplitterPosition;	
	long m_VSplitterPosition;	

	//////////////
	// Tools Key
	CString m_ToolMenuName[MAX_TOOLS];
	CString m_ToolCommand[MAX_TOOLS];
	CString m_ToolArgs[MAX_TOOLS];
	CString m_ToolInitDir[MAX_TOOLS];
	CString m_ToolPromptText[MAX_TOOLS];
	BOOL m_ToolIsConsole[MAX_TOOLS];
	BOOL m_ToolIsPrompt[MAX_TOOLS];
	BOOL m_ToolIsOutput2Status[MAX_TOOLS];
	BOOL m_ToolIsCloseOnExit[MAX_TOOLS];
	BOOL m_ToolIsShowBrowse[MAX_TOOLS];
	BOOL m_ToolIsRefresh[MAX_TOOLS];
	BOOL m_ToolOnContext[MAX_TOOLS];
	BOOL m_ToolIsSubMenu[MAX_TOOLS];

	//////////////
	// Bookmark Key
	CString m_BkMkMenuName[MAX_BOOKMARKS];
	BOOL m_BkMkIsSubMenu[MAX_BOOKMARKS];

	//////////////
	// Favorites Key
	CString m_FavMenuName[MAX_FAVORITES];
	BOOL m_FavIsSubMenu[MAX_FAVORITES];

	//////////////
	// MRU Tool Arguments
	CString m_MRUToolArg[MAX_MRU_TOOLARGS]; 

	//////////////
	// MRU Find Strings
	CString m_MRUFindStr[MAX_MRU_FINDSTRS]; 

	//////////////
	// MRU Viewers
	CString m_MRUViewer[MAX_MRU_VIEWERS]; 

	//////////////
	// MRU Job Filters
	CString m_MRUFilter[MAX_MRU_FILTERS]; 

	//////////////
	// MRU Submitted Changelist Filters
	CString m_MRUChgFilter[MAX_MRU_FILTERS]; 

	//////////////
	// MRU Ports
	CString m_MRUPort[MAX_MRU_PORTS]; 
	CString m_MRUPortCharset[MAX_MRU_PORTS]; // corresponding charset for each port

	//////////////
	// MRU Charsets
	CString m_MRUCharset[MAX_MRU_CHARSETS]; 

	//////////////
	// Default Charsets
	CString m_DefCharset[MAX_MRU_CHARSETS];

	//////////////
	// MRU PCUs
	CString m_MRUPcu[MAX_MRU_PCUS]; 

	//////////////
	// File Associations
	BOOL m_IgnoreWinAssoc;  
	CStringList m_AssocViewers;  // .extn|appname

	//////////////
	// Diff Associations
	CStringList m_AssocDiffs;  // .extn|appname

	//////////////
	// Merge Associations
	CStringList m_AssocMerges;  // .extn|appname

public:
	inline BOOL Is1stRun() { ASSERT(m_AttemptedRead); return m_Virgin; }
	inline LPCTSTR GetMyID() { ASSERT(m_AttemptedRead); return LPCTSTR(m_MyID); }
	BOOL ReadRegistry();


	//	Set and get either the permanent connection params (the ones
	//	in the registry) or the active ones (the ones currently being used).
	//
	LPCTSTR GetP4Port( BOOL permanent = FALSE );
	LPCTSTR GetP4User( BOOL permanent = FALSE );
	LPCTSTR GetP4Client( BOOL permanent = FALSE );
	LPCTSTR GetP4UserPassword( BOOL permanent = FALSE );
	LPCTSTR GetP4Charset( BOOL permanent = FALSE );
	
    //  Set the temporary over-rides of connection parameters and/or the permanent
	//  registry copies
	BOOL SetP4Port( LPCTSTR port, BOOL temporary, BOOL permanent, BOOL writetoreg );
	BOOL SetP4User( LPCTSTR user, BOOL temporary, BOOL permanent, BOOL writetoreg );
	BOOL SetP4Client( LPCTSTR client, BOOL temporary, BOOL permanent, BOOL writetoreg );
	BOOL SetP4Password( LPCTSTR password, BOOL temporary, BOOL permanent, BOOL writetoreg );
	BOOL SetP4Charset( LPCTSTR charset, BOOL temporary, BOOL permanent, BOOL writetoreg );
	void SetP4CharsetFromCmdli( BOOL b ) {m_bCharsetFromCmdli = b; }

   	///////////////
	// Options Key
	inline LPCTSTR GetEditApp() { ASSERT(m_AttemptedRead); return LPCTSTR(m_EditApp); }
	inline LPCTSTR GetDiffApp() { ASSERT(m_AttemptedRead); return LPCTSTR(m_DiffApp); }
	inline LPCTSTR GetDiffOptArgs() { ASSERT(m_AttemptedRead); return LPCTSTR(m_DiffOptArgs); }
	inline LPCTSTR GetMergeApp() { ASSERT(m_AttemptedRead); return LPCTSTR(m_MergeApp); }
	inline LPCTSTR GetMergeOptArgs() { ASSERT(m_AttemptedRead); return LPCTSTR(m_MergeOptArgs); }
	inline int GetTabWidth() { ASSERT(m_AttemptedRead); return int(m_TabWidth); }
	inline int GetWhtSpFlag() { ASSERT(m_AttemptedRead); return int(m_WhtSpFlag); }
	inline int GetMrgTabWidth() { ASSERT(m_AttemptedRead); return int(m_MrgTabWidth); }
	inline int GetMrgWhtSpFlag() { ASSERT(m_AttemptedRead); return int(m_MrgWhtSpFlag); }
	inline int GetAnnotateWhtSpace() { ASSERT(m_AttemptedRead); return int(m_AnnotateWhtSpace); }
	inline int GetAnnotateIncInteg() { ASSERT(m_AttemptedRead); return int(m_AnnotateIncInteg); }
	inline int GetTLVWhtSpace() { ASSERT(m_AttemptedRead); return int(m_TLVWhtSpace); }
	inline int GetTLVIncInteg() { ASSERT(m_AttemptedRead); return int(m_TLVIncInteg); }
	inline int GetResolveWhtSp() { ASSERT(m_AttemptedRead); return int(m_ResolveWhtSp); }
	inline int GetUseTempForView() { ASSERT(m_AttemptedRead); return m_UseTempForView; }
	inline LPCTSTR GetUseTempForExts() { ASSERT(m_AttemptedRead); return m_UseTempForExts; }
	inline BOOL GetEditAppIsConsole() { ASSERT(m_AttemptedRead); return m_EditAppIsConsole; }
	inline BOOL GetDiffAppIsConsole() { ASSERT(m_AttemptedRead); return m_DiffAppIsConsole; }
	inline BOOL GetDiffAppIsClose() { ASSERT(m_AttemptedRead); return m_DiffAppIsClose; }
	inline BOOL GetDiffAppIsBinary() { ASSERT(m_AttemptedRead); return m_DiffAppIsBinary; }
	inline int GetDiffInternal() { ASSERT(m_AttemptedRead); return m_DiffInternal; }
	inline BOOL GetDiffAppOptArgChk() { ASSERT(m_AttemptedRead); return m_DiffOptArgChk; }
	inline int GetMergeInternal() { ASSERT(m_AttemptedRead); return m_MergeInternal; }
	inline BOOL GetMergeAppOptArgChk() { ASSERT(m_AttemptedRead); return m_MergeOptArgChk; }
	inline BOOL GetMergeNSF() { ASSERT(m_AttemptedRead); return m_MergeNSF; }
	inline BOOL GetMergeAppIsConsole() { ASSERT(m_AttemptedRead); return m_MergeAppIsConsole; }
	inline BOOL GetMergeAppIsClose() { ASSERT(m_AttemptedRead); return m_MergeAppIsClose; }
	inline BOOL AlwaysShowFocus() { ASSERT(m_AttemptedRead); return m_AlwaysShowFocus; }
	inline BOOL AlwaysWarnOnRevert() { ASSERT(m_AttemptedRead); return m_AlwaysWarnOnRevert; }
	inline BOOL AllowPromptAbove() { ASSERT(m_AttemptedRead); return m_AllowPromptAbove; }
	inline BOOL AutoMinEditDlg() { ASSERT(m_AttemptedRead); return m_AutoMinEditDlg; }
	inline int GetDoubleClickOption() { ASSERT(m_AttemptedRead); return m_DoubleClickOption; }
	inline int GetOptionStartUpOption() { ASSERT(m_AttemptedRead); return m_OptionStartUpOption; }
	inline int GetOptionStartUpPage() { ASSERT(m_AttemptedRead); return m_OptionStartUpPage; }
	inline int GetStatusUpdateInterval() { ASSERT(m_AttemptedRead); return m_StatusUpdateInterval; }
	inline int GetLabelAddRplDefault() { ASSERT(m_AttemptedRead); return m_LabelAddRplDefault; }
	inline int GetDiff2Default1() { ASSERT(m_AttemptedRead); return m_Diff2Default1; }
	inline int GetDiff2Default2() { ASSERT(m_AttemptedRead); return m_Diff2Default2; }
	BOOL SetEditApp(LPCTSTR appPath);
	BOOL SetDiffApp(LPCTSTR appPath);
	BOOL SetDiffOptArgs(LPCTSTR optArgs);
	BOOL SetMergeApp(LPCTSTR appPath);
	BOOL SetMergeOptArgs(LPCTSTR optArgs);
	BOOL SetTabWidth(int tabWidth);
	BOOL SetWhtSpFlag(int whtSpFlag);
	BOOL SetMrgTabWidth(int mrgTabWidth);
	BOOL SetMrgWhtSpFlag(int whtSpFlag);
	BOOL SetAnnotateWhtSpace(int whtSpFlag);
	BOOL SetAnnotateIncInteg(int incIntegFlag);
	BOOL SetTLVWhtSpace(int whtSpFlag);
	BOOL SetTLVIncInteg(int incIntegFlag);
	BOOL SetResolveWhtSp(int whtSpFlag);
	BOOL SetUseTempForView(BOOL useTempForView);
	BOOL SetUseTempForExts(LPCTSTR useTempForExts);
	BOOL SetEditAppIsConsole(BOOL isConsole);
	BOOL SetDiffAppIsConsole(BOOL isConsole);
	BOOL SetDiffAppIsClose(BOOL isClose);
	BOOL SetDiffAppIsBinary(BOOL isBinary);
	int SetDiffInternal(BOOL isInternal);
	BOOL SetDiffOptArgChk(BOOL isOptArg);
	int SetMergeInternal(BOOL isInternal);
	BOOL SetMergeOptArgChk(BOOL isOptArg);
	BOOL SetMergeNSF(BOOL bNSF);
	BOOL SetMergeAppIsConsole(BOOL isConsole);
	BOOL SetMergeAppIsClose(BOOL isClose);
	BOOL SetAlwaysShowFocus(BOOL alwaysShowFocus);
	BOOL SetAlwaysWarnOnRevert(BOOL alwaysWarnOnRevert);
	BOOL SetAllowPromptAbove(BOOL allowPromptAbove);
	BOOL SetAutoMinEditDlg(BOOL autoMinEditDlg);
	BOOL SetDoubleClickOption(int dblClick);
	BOOL SetOptionStartUpOption(int startupoption);
	BOOL SetOptionStartUpPage(int startuppage);
	BOOL SetStatusUpdateInterval(int statinterval);
	BOOL SetLabelAddRplDefault(int defaultNbr);
	BOOL SetDiff2Default1(int defaultNbr);
	BOOL SetDiff2Default2(int defaultNbr);
	void SetHostname(CString& hostname);

	inline LPCTSTR GetHostname() { ASSERT(m_AttemptedRead); return LPCTSTR(m_Hostname); }
	inline LPCTSTR GetTempDir() { ASSERT(m_AttemptedRead); return LPCTSTR(m_TempDir); }
	inline int     GetExplorer() { ASSERT(m_AttemptedRead); return m_Explorer; }
	inline BOOL    Get2Panes() { ASSERT(m_AttemptedRead); return m_2Panes; }
	inline LPCTSTR GetAltExpl() { ASSERT(m_AttemptedRead); return LPCTSTR(m_AltExpl); }
	inline LPCTSTR GetExpandPath() { ASSERT(m_AttemptedRead); return LPCTSTR(m_ExpandPath); }
	inline LPCTSTR GetWarnLimit() { ASSERT(m_AttemptedRead); return LPCTSTR(m_WarnLimit); }
	inline LPCTSTR GetWarnLimitDiff() { ASSERT(m_AttemptedRead); return LPCTSTR(m_WarnLimitDiff); }
	inline LPCTSTR GetWarnLimitOpen() { ASSERT(m_AttemptedRead); return LPCTSTR(m_WarnLimitOpen); }
	inline LPCTSTR GetExtSortMax() { ASSERT(m_AttemptedRead); return LPCTSTR(m_ExtSortMax); }
	inline LPCTSTR GetMinMultiLineSize() { ASSERT(m_AttemptedRead); return LPCTSTR(m_MinMultiLineSize); }
	inline LPCTSTR GetUserFilter() { ASSERT(m_AttemptedRead); return LPCTSTR(m_UserFilter); }
	inline LPCTSTR GetClientFilter() { ASSERT(m_AttemptedRead); return LPCTSTR(m_ClientFilter); }
	inline BOOL GetLabelFilterByOwnerFlag() { ASSERT(m_AttemptedRead); return m_LabelFilterByOwnerFlag; }
	inline LPCTSTR GetLabelFilterByOwner() { ASSERT(m_AttemptedRead); return LPCTSTR(m_LabelFilterByOwner); }
	inline BOOL GetFilteredByUser() { ASSERT(m_AttemptedRead); return m_FilteredByUser; }
	inline BOOL GetFilteredByClient() { ASSERT(m_AttemptedRead); return m_FilteredByClient; }
	inline BOOL GetLabelFilterIncBlank() { ASSERT(m_AttemptedRead); return m_LabelFilterIncBlank; }
	inline LPCTSTR GetUseOpenForEditExts() { ASSERT(m_AttemptedRead); return LPCTSTR(m_UseOpenForEditExts); }
	inline BOOL GetUseOpenForEdit() { ASSERT(m_AttemptedRead); return m_UseOpenForEdit; }
	inline BOOL GetUseShortRevHistDesc() { ASSERT(m_AttemptedRead); return m_UseShortRevHistDesc; }
	inline BOOL GetUseShortSubmittedDesc() { ASSERT(m_AttemptedRead); return m_UseShortSubmittedDesc; }
	inline int  GetUseLongChglistDesc() { ASSERT(m_AttemptedRead); return m_UseLongChglistDesc; }
	inline int  BusyWaitTime() { ASSERT(m_AttemptedRead); return m_BusyWaitTime; }
	inline int  GetSyncDlgFlag() { ASSERT(m_AttemptedRead); return m_SyncDlgFlag; }
	inline int  GetDescWrap() { ASSERT(m_AttemptedRead); return m_DescWrap; }
	inline BOOL GetDescWrapSw() { ASSERT(m_AttemptedRead); return m_DescWrapSw; }
	inline BOOL GetExpandFlag() { ASSERT(m_AttemptedRead); return m_ExpandFlag; }
	inline BOOL GetIsNoCopy() { ASSERT(m_AttemptedRead); return m_IsNoCopy; }
	inline BOOL GetIsForceInteg() { ASSERT(m_AttemptedRead); return m_IsForceInteg; }
	inline BOOL GetIsForceDirect() { ASSERT(m_AttemptedRead); return m_IsForceDirect; }
	inline BOOL GetIsDeleteSource() { ASSERT(m_AttemptedRead); return m_IsDeleteSource; }
	inline BOOL GetIsPermitDelReadd() { ASSERT(m_AttemptedRead); return m_IsPermitDelReadd; }
	inline int  GetDelReaddType() { ASSERT(m_AttemptedRead); return m_DelReaddType; }
	inline BOOL GetIsBaselessMerge() { ASSERT(m_AttemptedRead); return m_IsBaselessMerge; }
	inline BOOL GetIsIndirectMerge() { ASSERT(m_AttemptedRead); return m_IsIndirectMerge; }
	inline BOOL GetIsPropagateTypes() { ASSERT(m_AttemptedRead); return m_IsPropagateTypes; }
	inline BOOL GetIsBaseViaDelReadd() { ASSERT(m_AttemptedRead); return m_IsBaseViaDelReadd; }
	inline BOOL GetUseNewChglist() { ASSERT(m_AttemptedRead); return m_UseNewChglist; }
	inline BOOL GetUseNewChglist2() { ASSERT(m_AttemptedRead); return m_UseNewChglist2; }
	inline BOOL GetSyncFirstDefault() { ASSERT(m_AttemptedRead); return m_SyncFirstDefault; }
	inline BOOL GetWarnAlreadyOpened() { ASSERT(m_AttemptedRead); return m_WarnAlreadyOpened; }
	inline BOOL ShowDeleted() { ASSERT(m_AttemptedRead); return m_ShowDeleted; }
	inline BOOL SortByExtension() { ASSERT(m_AttemptedRead); return m_SortByExtension; }
	inline BOOL SortChgFilesByAction() { ASSERT(m_AttemptedRead); return m_SortChgFilesByAction; }
	inline BOOL SortChgFilesByName() { ASSERT(m_AttemptedRead); return m_SortChgFilesByName; }
	inline BOOL SortChgFilesByExt() { ASSERT(m_AttemptedRead); return m_SortChgFilesByExt; }
	inline BOOL SortChgFilesByResolve() { ASSERT(m_AttemptedRead); return m_SortChgFilesByResolve; }
	inline BOOL ShowToolBar() { ASSERT(m_AttemptedRead); return m_ShowToolBar; }
	inline int  ShowEntireDepot() { ASSERT(m_AttemptedRead); return m_ShowEntireDepot; }
	inline BOOL ShowFileType() { ASSERT(m_AttemptedRead); return m_ShowFileType; }
	inline BOOL ShowOpenAction() { ASSERT(m_AttemptedRead); return m_ShowOpenAction; }
	inline BOOL ShowChangeDesc() { ASSERT(m_AttemptedRead); return m_ShowChangeDesc; }
	inline BOOL SortChgsByUser() { ASSERT(m_AttemptedRead); return m_SortChgsByUser; }
	inline BOOL ShowCommandTrace() { ASSERT(m_AttemptedRead); return m_ShowCommandTrace; }
	inline BOOL ShowConnectSettings() { ASSERT(m_AttemptedRead); return m_ShowConnectSettings; }
	inline BOOL ShowConnectPort1st() { ASSERT(m_AttemptedRead); return m_ShowConnectPort1st; }
    inline BOOL ShowClientPath4Chgs() { ASSERT(m_AttemptedRead); return FALSE; }
    inline BOOL ShowClientPath() { ASSERT(m_AttemptedRead); return m_ShowClientPath; }
    inline BOOL ShowDepotPathHiLite() { ASSERT(m_AttemptedRead); return m_ShowDepotPathHiLite; }
    inline BOOL ShowStatusTime() { ASSERT(m_AttemptedRead); return m_ShowStatusTime; }
    inline BOOL ShowHiddenFilesNotInDepot() { ASSERT(m_AttemptedRead); return m_ShowHiddenFilesNotInDepot; }
    inline BOOL Use24hourClock() { ASSERT(m_AttemptedRead); return m_Use24hourClock; }
    inline BOOL PreserveSpecFormat() { ASSERT(m_AttemptedRead); return m_PreserveSpecFormat; }
    inline BOOL AutoExpandOptions() { ASSERT(m_AttemptedRead); return m_AutoExpandOptions; }
    inline BOOL LabelFilesInDialog() { ASSERT(m_AttemptedRead); return m_LabelFilesInDialog; }
    inline BOOL LabelShowPreviewDetail() { ASSERT(m_AttemptedRead); return m_LabelShowPreviewDetail; }
    inline BOOL Diff2InDialog() { ASSERT(m_AttemptedRead); return m_Diff2InDialog; }
    inline BOOL AutoTreeExpand() { ASSERT(m_AttemptedRead); return m_AutoTreeExpand; }
    inline BOOL UseNotepad4WarnAndErr() { ASSERT(m_AttemptedRead); return m_UseNotepad4WarnAndErr; }
    inline BOOL Cvt2ValComboToChkBx() { ASSERT(m_AttemptedRead); return m_Cvt2ValComboToChkBx; }
    inline BOOL IconsInMenus() { ASSERT(m_AttemptedRead); return m_IconsInMenus; }
    inline BOOL ExpandChgLists() { ASSERT(m_AttemptedRead); return m_ExpandChgLists; }
    inline BOOL ReExpandChgs() { ASSERT(m_AttemptedRead); return m_ReExpandChgs; }
    inline BOOL GetEnableSubChgIntegFilter() { ASSERT(m_AttemptedRead); return m_EnableSubChgIntegFilter; }
    inline BOOL GetEnableRevHistShowIntegs() { ASSERT(m_AttemptedRead); return m_EnableRevHistShowIntegs; }
    inline BOOL GetEnablePendingChgsOtherClients() { ASSERT(m_AttemptedRead); return m_EnablePendingChgsOtherClients; }
    inline long GetAddFilterExclude() { ASSERT(m_AttemptedRead); return m_AddFilterExclude; }
	inline BOOL GetAutoPoll() { ASSERT(m_AttemptedRead); return m_AutoPoll; }
	inline long GetAutoPollTime() { ASSERT(m_AttemptedRead); return m_AutoPollTime; }
	inline BOOL GetAutoPollIconic() { ASSERT(m_AttemptedRead); return m_AutoPollIconic; }
	inline BOOL GetAutoPollJobs() { ASSERT(m_AttemptedRead); return m_AutoPollJobs; }
	inline BOOL GetLogoutOnExit() { ASSERT(m_AttemptedRead); return m_LogoutOnExit; }
	inline BOOL GetClearAndReload() { ASSERT(m_AttemptedRead); return m_ClearAndReload; }
	inline BOOL GetReloadOnUncover() { ASSERT(m_AttemptedRead); return m_ReloadOnUncover; }
	inline long GetReloadUncoverTime() { ASSERT(m_AttemptedRead); return m_ReloadUncoverTime; }
	inline long GetChglistSyncDef() { ASSERT(m_AttemptedRead); return m_ChglistSyncDef; }
	inline int  GetFetchChangeCount() { ASSERT(m_AttemptedRead); return m_FetchChangeCount; }
    inline BOOL GetFetchAllChanges() { ASSERT(m_AttemptedRead); return m_FetchAll; }
	inline int  GetFetchJobCount() { ASSERT(m_AttemptedRead); return m_FetchJobCount; }
    inline BOOL GetFetchAllJobs() { ASSERT(m_AttemptedRead); return m_FetchAllJobs; }
    inline BOOL GetJobsEnabled() { ASSERT(m_AttemptedRead); return m_JobsEnabled; }
	inline int  GetFetchHistCount() { ASSERT(m_AttemptedRead); return m_FetchHistCount; }
    inline BOOL GetFetchAllHist() { ASSERT(m_AttemptedRead); return m_FetchAllHist; }
    inline long GetFetchCompleteHist() { ASSERT(m_AttemptedRead); return m_FetchCompleteHist; }
    inline BOOL GetHistListHeight() { ASSERT(m_AttemptedRead); return m_HistListHeight; }
    inline BOOL GetJobFilterWidth() { ASSERT(m_AttemptedRead); return m_JobFilterWidth; }
    inline BOOL UseDepotInClientView() { ASSERT(m_AttemptedRead); return m_UseDepotInClientView; }
    inline BOOL Use256colorIcons() { ASSERT(m_AttemptedRead); return m_Use256colorIcons; }
	inline BOOL DontShowYouHaveChgClientView() { ASSERT(m_AttemptedRead); return m_DontShowYouHaveChgClientView; }
	inline BOOL DontShowYouHaveCr8NewClient() { ASSERT(m_AttemptedRead); return m_DontShowYouHaveCr8NewClient; }
	inline BOOL DontShowDiscardFormChgs() { ASSERT(m_AttemptedRead); return m_DontShowDiscardFormChgs; }
	inline int GetAutoGetOnEdit() { ASSERT(m_AttemptedRead); return m_AutoGetOnEdit; }
    inline BOOL AllowExceptionReporting() { ASSERT(m_AttemptedRead); return m_AllowExceptionReporting; }
    inline BOOL TryResetingFocus() { ASSERT(m_AttemptedRead); return m_TryResetingFocus; }
	BOOL SetTempDir(LPCTSTR tempdir);
	BOOL SetExplorer(int explorersw);
	BOOL Set2Panes(BOOL b2Panes);
	BOOL SetAltExpl(LPCTSTR explorerstring);
	BOOL SetPreviewDefault(BOOL previewDefault);
	BOOL SetIsBaselessMerge(BOOL isBaselessMerge);
	BOOL SetIsIndirectMerge(BOOL isIndirectMerge);
	BOOL SetIsPropagateTypes(BOOL isPropagateTypes);
	BOOL SetIsBaseViaDelReadd(BOOL isBaseViaDelReadd);
	BOOL SetUseNewChglist(BOOL useNewChglist);
	BOOL SetUseNewChglist2(BOOL useNewChglist2);
	BOOL SetIsPermitDelReadd(BOOL isPermitDelReadd);
	BOOL SetDelReaddType(int delReaddType);
	BOOL SetIsDeleteSource(BOOL isDeleteSource);
	BOOL SetIsForceInteg(BOOL isForceInteg);
	BOOL SetIsForceDirect(BOOL isForceDirect);
	BOOL SetIsNoCopy(BOOL isNoCopy);
	BOOL SetSyncFirstDefault(BOOL syncFirstDefault);
	BOOL SetWarnAlreadyOpened(BOOL warnAlreadyOpened);
	BOOL SetExpandFlag(BOOL expandFlag);
	BOOL SetExpandPath(LPCTSTR expandPath);
	BOOL SetShowDeleted(BOOL showDeleted);
	BOOL SetSortByExtension(BOOL sortByExtension);
	BOOL SetSortChgFilesByAction(BOOL sortChgFilesByAction);
	BOOL SetSortChgFilesByName(BOOL sortChgFilesByName);
	BOOL SetSortChgFilesByExt(BOOL sortChgFilesByExt);
	BOOL SetSortChgFilesByResolve(BOOL sortChgFilesByResolve);
	BOOL SetWarnLimit(LPCTSTR warnLimit);
	BOOL SetWarnLimitDiff(LPCTSTR warnLimitDiff);
	BOOL SetWarnLimitOpen(LPCTSTR warnLimitOpen);
	BOOL SetExtSortMax(LPCTSTR extSortMax);
	BOOL SetMinMultiLineSize(LPCTSTR minMultiLineSize);
	BOOL SetUserFilter(LPCTSTR userFilter);
	BOOL SetFilteredByUser(int filteredByUser);
	BOOL SetClientFilter(LPCTSTR clientFilter);
	BOOL SetFilteredByClient(int filteredByClient);
	BOOL SetLabelFilterByOwnerFlag(int labelFilterByOwnerFlag);
	BOOL SetLabelFilterByOwner(LPCTSTR labelFilterByOwner);
	BOOL SetLabelFilterIncBlank(int labelFilterIncBlank);
	BOOL SetUseOpenForEditExts(LPCTSTR useOpenForEditExts);
	BOOL SetUseOpenForEdit(int useOpenForEdit);
	BOOL SetUseShortRevHistDesc(int useShortRevHistDesc);
	BOOL SetUseShortSubmittedDesc(int useShortSubmittedDesc);
	BOOL SetUseLongChglistDesc(int useLongChglistDesc);
	BOOL SetBusyWaitTime(int busyWaitTime);
	BOOL SetSyncDlgFlag(int syncDlgFlag);
	BOOL SetDescWrap(int descWrap);
	BOOL SetDescWrapSw(BOOL descWrapSw);
	BOOL SetShowToolBar(BOOL showToolBar);
	BOOL SetShowEntireDepot(int showEntire);
	BOOL SetShowFileType(BOOL showFileType);
	BOOL SetShowOpenAction(BOOL showOpenAction);
	BOOL SetShowCommandTrace ( BOOL );
	BOOL SetShowChangeDesc(BOOL showChangeDesc);
	BOOL SetSortChgsByUser(BOOL sortChgsByUser);
	BOOL SetShowConnectSettings(BOOL showConnectSettings);
	BOOL SetShowConnectPort1st(BOOL showConnectPort1st);
    BOOL SetShowClientPath4Chgs(BOOL showClientPath4Chgs);
    BOOL SetShowClientPath(BOOL showClientPath);
    BOOL SetShowDepotPathHiLite(BOOL showDepotPathHiLite);
	BOOL SetShowStatusTime(BOOL showStatusTime);
	BOOL SetShowHiddenFilesNotInDepot(BOOL showHiddenFilesNotInDepot);
	BOOL SetUse24hourClock(BOOL use24hourClock);
	BOOL SetPreserveSpecFormat(BOOL preserveSpecFormat);
	BOOL SetAutoExpandOptions(BOOL autoExpandOptions);
	BOOL SetLabelFilesInDialog(BOOL labelFilesInDialog);
	BOOL SetLabelShowPreviewDetail(BOOL labelShowPreviewDetail);
	BOOL SetDiff2InDialog(BOOL diff2InDialog);
	BOOL SetAutoTreeExpand(BOOL autoTreeExpand);
	BOOL SetUseNotepad4WarnAndErr(BOOL useNotepad4WarnAndErr);
	BOOL SetCvt2ValComboToChkBx(BOOL cvt2ValComboToChkBx);
	BOOL SetIconsInMenus(BOOL iconsInMenus);
	BOOL SetExpandChgLists(BOOL expandChgLists);
	BOOL SetReExpandChgs(BOOL reExpandChgs);
	BOOL SetEnableSubChgIntegFilter(BOOL enableSubChgIntegFilter);
	BOOL SetEnableRevHistShowIntegs(BOOL enableRevHistShowIntegs);
	BOOL SetEnablePendingChgsOtherClients(BOOL enablePendingChgsOtherClients);
	BOOL SetAddFilterExclude(long AddFilterExclude);
	BOOL SetAutoPoll(BOOL autoPoll);
	BOOL SetAutoPollTime(long autoPollTime);
	BOOL SetAutoPollIconic(BOOL autoPoll);
	BOOL SetAutoPollJobs(BOOL autoPollJobs);
	BOOL SetLogoutOnExit(BOOL logoutOnExit);
	BOOL SetClearAndReload(BOOL clearAndReload);
	BOOL SetReloadOnUncover(BOOL reloadOnUncover);
	BOOL SetReloadUncoverTime(long reloadUncoverTime);
	BOOL SetChglistSyncDef(long chglistSyncDef);
	BOOL SetFetchChangeCount(long fetchChangeCount);
    BOOL SetFetchAllChanges(BOOL fetchAll);
	BOOL SetFetchJobCount(long fetchJobCount);
    BOOL SetFetchAllJobs(BOOL fetchAllJobs);
	BOOL SetJobsEnabled(BOOL jobsEnabled);
	BOOL SetFetchHistCount(long fetchHistCount);
	BOOL SetFetchAllHist(BOOL fetchAll);
	BOOL SetFetchCompleteHist(long fetchComplete);
	BOOL SetHistListHeight(int histListHeight);
	BOOL SetJobFilterWidth(int jobFilterWidth);
	BOOL SetUseDepotInClientView(int useDepotInClientView);
	BOOL SetUse256colorIcons(int useDepotInClientView);
	BOOL SetDontShowYouHaveChgClientView(int bDontShowYouHaveChgClientView);
	BOOL SetDontShowYouHaveCr8NewClient(int bDontShowYouHaveCr8NewClient);
	BOOL SetDontShowDiscardFormChgs(int bDontShowDiscardFormChgs);
	BOOL SetAllowExceptionReporting(BOOL allowExceptionReporting);
	BOOL SetTryResetingFocus(BOOL tryResetingFocus);

	LPCTSTR GetFontFace() { ASSERT(m_AttemptedRead); return LPCTSTR(m_FontFace); }
	int  GetFontSize() { ASSERT(m_AttemptedRead); return m_FontSize; }
	BOOL GetFontItalic() { ASSERT(m_AttemptedRead); return m_IsFontItalic; }
	int  GetFontWeight() { ASSERT(m_AttemptedRead); return m_FontWeight; }
	BOOL SetFontFace( LPCTSTR fontFace );
	BOOL SetFontSize( int fontSize );
	BOOL SetFontItalic( BOOL isItalic );
	BOOL SetFontWeight( int fontWeight );

	int  GetFindFilesWidth() { ASSERT(m_AttemptedRead); return m_FindFilesWidth; }
	BOOL SetFindFilesWidth( int findFilesWidth );

	int  GetP4StartWith() { ASSERT(m_AttemptedRead); return m_P4StartWith; }
	BOOL SetP4StartWith( int p4StartWith );

	BOOL GetP4BusyCursor() { ASSERT(m_AttemptedRead); return m_P4BusyCursor; }
	BOOL SetP4BusyCursor( BOOL p4BusyCursor );

	BOOL GetCloseAfterFind() { ASSERT(m_AttemptedRead); return m_CloseAfterFind; }
	BOOL SetCloseAfterFind( BOOL CloseAfterFind );

	BOOL GetSubmitOnlyChged() { ASSERT(m_AttemptedRead); return m_SubmitOnlyChged; }
	BOOL SetSubmitOnlyChged( BOOL submitOnlyChged );

	BOOL GetRevertUnchged() { ASSERT(m_AttemptedRead); return m_RevertUnchged; }
	BOOL SetRevertUnchged( BOOL revertUnchged );

	BOOL GetAutoReopen() { ASSERT(m_AttemptedRead); return m_AutoReopen; }
	BOOL SetAutoReopen( BOOL autoReopen );

	BOOL FilterPendChgsByMyClient() { ASSERT(m_AttemptedRead); return m_FilterPendChgsByMyClient; }
	BOOL SetFilterPendChgsByMyClient( BOOL filterPendChgsByMyClient );

	LPCTSTR FilterPendChgsByPath() { ASSERT(m_AttemptedRead); return m_FilterPendChgsByPath; }
	BOOL SetFilterPendChgsByPath( LPCTSTR filterPendChgsByPath );

	int  GetLabelDragDropOption() { ASSERT(m_AttemptedRead); return m_LabelDragDropOption; }
	BOOL SetLabelDragDropOption( int labelDragDropOption );

	int  GetResolveDefault() { ASSERT(m_AttemptedRead); return m_ResolveDefault; }
	BOOL SetResolveDefault( int resolveDefault );

	int  GetResolve2wayDefault() { ASSERT(m_AttemptedRead); return m_Resolve2wayDefault; }
	BOOL SetResolve2wayDefault( int resolve2wayDefault );

	int  GetResolve3wayDefault() { ASSERT(m_AttemptedRead); return m_Resolve3wayDefault; }
	BOOL SetResolve3wayDefault( int resolve3wayDefault );

	int  GetConvertJobNbr() { ASSERT(m_AttemptedRead); return m_ConvertJobNbr; }
	BOOL SetConvertJobNbr( int convertJobNbr );

	///////////////
	// Settings Key
	inline LPCTSTR GetAddFileCurDir() { ASSERT(m_AttemptedRead); return LPCTSTR(m_AddFileCurDir); }
	inline LPCTSTR GetAddFileExtFilter() { ASSERT(m_AttemptedRead); return LPCTSTR(m_AddFileExtFilter); }
	inline LPCTSTR GetAddFileFilter() { ASSERT(m_AttemptedRead); return LPCTSTR(m_AddFileFilter); }
	inline int GetAddFileFilterIndex() { ASSERT(m_AttemptedRead); return m_AddFileFilterIndex; }
	inline int GetDefaultDnDfromExp() { ASSERT(m_AttemptedRead); return m_DefaultDnDfromExp; }
	inline int GetMultiProcessorSleep() { ASSERT(m_AttemptedRead); return m_MultiProcessorSleep; }
	inline int GetMaxStatusLines() { ASSERT(m_AttemptedRead); return m_MaxStatusLines; }
	inline BOOL GetShowStatusMsgs() { ASSERT(m_AttemptedRead); return m_ShowStatusMsgs; }
	inline BOOL GetShowTruncTooltip() { ASSERT(m_AttemptedRead); return m_ShowTruncTooltip; }
	inline BOOL GetDontThreadDiffs() { ASSERT(m_AttemptedRead); return m_DontThreadDiffs; }
	inline LPCTSTR GetPendChgExpansion() { ASSERT(m_AttemptedRead); return LPCTSTR(m_PendChgExpansion); }
	inline LPCTSTR GetLastBranch() { ASSERT(m_AttemptedRead); return LPCTSTR(m_LastBranch); }
	inline LPCTSTR GetLastLabel() { ASSERT(m_AttemptedRead); return LPCTSTR(m_LastLabel); }
	inline LPCTSTR GetLastTemplate() { ASSERT(m_AttemptedRead); return LPCTSTR(m_LastTemplate); }
	inline LPCTSTR GetLocalCliTemplate() { ASSERT(m_AttemptedRead); return LPCTSTR(m_LocalCliTemplate); }
	inline BOOL LocalCliTemplateSw() { ASSERT(m_AttemptedRead); return m_LocalCliTemplateSw; }
	inline BOOL UseClientSpecSubmitOpts() { ASSERT(m_AttemptedRead); return m_UseClientSpecSubmitOpts; }
	inline BOOL SwapButtonPosition() { ASSERT(m_AttemptedRead); return m_SwapButtonPosition; }
	inline int GetBranchFilteredFlags() { ASSERT(m_AttemptedRead); return m_BranchFilteredFlags; }
	inline LPCTSTR GetBranchFilterOwner() { ASSERT(m_AttemptedRead); return LPCTSTR(m_BranchFilterOwner); }
	inline int GetClientFilteredFlags() { ASSERT(m_AttemptedRead); return m_ClientFilteredFlags; }
	inline LPCTSTR GetClientFilterOwner() { ASSERT(m_AttemptedRead); return LPCTSTR(m_ClientFilterOwner); }
	inline LPCTSTR GetClientFilterHost() { ASSERT(m_AttemptedRead); return LPCTSTR(m_ClientFilterHost); }
	inline LPCTSTR GetClientFilterDesc() { ASSERT(m_AttemptedRead); return LPCTSTR(m_ClientFilterDesc); }
	BOOL SetAddFileCurDir(LPCTSTR dir);
	BOOL SetAddFileExtFilter(LPCTSTR exts);
	BOOL SetAddFileFilter(LPCTSTR filter);
	BOOL SetAddFileFilterIndex(int index);
	BOOL SetDefaultDnDfromExp(int index);
	BOOL SetMultiProcessorSleep(int index);
	BOOL SetMaxStatusLines(int maxStatusLines);
	BOOL SetShowStatusMsgs(BOOL showStatusMsgs);
	BOOL SetShowTruncTooltip(BOOL showTruncTooltip);
	BOOL SetDontThreadDiffs(BOOL dontThreadDiffs);
	BOOL SetPendChgExpansion(LPCTSTR pendChgExpansion);
	BOOL SetLastBranch(LPCTSTR lastBranch);
	BOOL SetLastLabel(LPCTSTR lastLabel);
	BOOL SetLastTemplate(LPCTSTR lastTemplate);
	BOOL SetLocalCliTemplate(LPCTSTR localCliTemplate);
	BOOL SetLocalCliTemplateSw(BOOL localCliTemplateSw);
	BOOL SetUseClientSpecSubmitOpts(BOOL useClientSpecSubmitOpts);
	BOOL SetSwapButtonPosition(BOOL swapButtonPosition);
	BOOL SetBranchFilteredFlags(int branchFilteredFlags);
	BOOL SetBranchFilterOwner(LPCTSTR branchFilterOwner);
	BOOL SetClientFilteredFlags(int clientFilteredFlags);
	BOOL SetClientFilterOwner(LPCTSTR clientFilterOwner);
	BOOL SetClientFilterHost(LPCTSTR clientFilterHost);
	BOOL SetClientFilterDesc(LPCTSTR clientFilterDesc);
	
	///////////////
	// Layout Key
	inline CRect GetWindowPosition() { ASSERT(m_AttemptedRead); return m_WindowPosition; }
	inline BOOL GetWindowIconic() { ASSERT(m_AttemptedRead); return (UINT) m_Iconic; }
	inline BOOL GetWindowMaximized() { ASSERT(m_AttemptedRead); return (UINT) m_Maximized; }
	inline int GetHSplitterPosition() { ASSERT(m_AttemptedRead); return m_HSplitterPosition; }	
	inline int GetVSplitterPosition() { ASSERT(m_AttemptedRead); return m_VSplitterPosition; }	
	BOOL SetWindowPosition(CRect position);
	BOOL SetWindowIconic(BOOL iconic);
	BOOL SetWindowMaximized(BOOL maximized);
	BOOL SetHSplitterPosition(int position);
	BOOL SetVSplitterPosition(int position);


	///////////////
	// Tools Key
	CString CP4Registry::GetToolMenuName(int index);
	CString CP4Registry::GetToolCommand(int index);
	CString CP4Registry::GetToolArgs(int index);
	CString CP4Registry::GetToolInitDir(int index);
	CString CP4Registry::GetToolPromptText(int index);
	BOOL CP4Registry::GetToolIsConsole(int index);
	BOOL CP4Registry::GetToolIsPrompt(int index);
	BOOL CP4Registry::GetToolIsOutput2Status(int index);
	BOOL CP4Registry::GetToolIsCloseOnExit(int index);
	BOOL CP4Registry::GetToolIsShowBrowse(int index);
	BOOL CP4Registry::GetToolIsRefresh(int index);
	BOOL CP4Registry::GetToolOnContext(int index);
	BOOL CP4Registry::GetToolIsSubMenu(int index);
	BOOL CP4Registry::SetToolMenuName(int index, CString menuName);
	BOOL CP4Registry::SetToolCommand(int index, CString command);
	BOOL CP4Registry::SetToolArgs(int index, CString args);
	BOOL CP4Registry::SetToolInitDir(int index, CString initDir);
	BOOL CP4Registry::SetToolPromptText(int index, CString initDir);
	BOOL CP4Registry::SetToolIsConsole(int index, BOOL isConsole);
	BOOL CP4Registry::SetToolIsPrompt(int index, BOOL isPrompt);
	BOOL CP4Registry::SetToolIsOutput2Status(int index, BOOL isOutput2Status);
	BOOL CP4Registry::SetToolIsCloseOnExit(int index, BOOL isCloseOnExit);
	BOOL CP4Registry::SetToolIsShowBrowse(int index, BOOL isShowBrowse);
	BOOL CP4Registry::SetToolIsRefresh(int index, BOOL isRefresh);
	BOOL CP4Registry::SetToolOnContext(int index, BOOL onContext);
	BOOL CP4Registry::SetToolIsSubMenu(int index, BOOL isSubMenu);


	///////////////
	// Bookmark Key
	CString CP4Registry::GetBkMkMenuName(int index);
	BOOL CP4Registry::GetBkMkIsSubMenu(int index);
	BOOL CP4Registry::SetBkMkMenuName(int index, CString menuName);
	BOOL CP4Registry::SetBkMkIsSubMenu(int index, BOOL isSubMenu);


	///////////////
	// Favorites Key
	CString CP4Registry::GetFavMenuName(int index);
	BOOL CP4Registry::GetFavIsSubMenu(int index);
	BOOL CP4Registry::SetFavMenuName(int index, CString menuName);
	BOOL CP4Registry::SetFavIsSubMenu(int index, BOOL isSubMenu);


	///////////////
	// MRU Tool Arguments
	CString GetMRUToolArg(int index);
	void AddMRUToolArg( LPCTSTR toolArg );

	///////////////
	// MRU Find Strings
	CString GetMRUFindStr(int index);
	void AddMRUFindStr( LPCTSTR findStr );

	///////////////
	// MRU Viewers
	CString GetMRUViewer(int index);
	CString GetMRUViewerName(int index);
	void AddMRUViewer( LPCTSTR viewerPath );
	void RmvMRUViewer( LPCTSTR viewerPath );

	///////////////
	// MRU Job Filters
	CString GetMRUFilter(int index);
	void AddMRUFilter( LPCTSTR filterStr );
	void RmvMRUFilter( LPCTSTR filterStr );

	///////////////
	// MRU Submitted Changelist Filters
	CString GetMRUChgFilter(int index);
	void AddMRUChgFilter( LPCTSTR filterStr );
	void RmvMRUChgFilter( LPCTSTR filterStr );

	///////////////
	// MRU Ports
	CString GetMRUPort(int index);
	void AddMRUPort( LPCTSTR portStr, LPCTSTR charsetStr );

	///////////////
	// MRU Charsets
	CString GetMRUCharset(int index);
	void AddMRUCharset( LPCTSTR charsetStr );

	///////////////
	// MRU Pcus
	CString GetMRUPcu(int index);
	void AddMRUPcu( LPCTSTR pcuStr );
	void RmvMRUPcu( LPCTSTR pcuStr );
	BOOL AddMRUPcuPath( LPCTSTR path );

	///////////////
	// File Associations
	inline BOOL GetIgnoreWinAssoc() { ASSERT(m_AttemptedRead); return m_IgnoreWinAssoc; }  
	BOOL SetIgnoreWinAssoc(BOOL ignore);
	BOOL SetAssociation(LPCTSTR extension, LPCTSTR application);
	BOOL ClearAssociation(LPCTSTR extension);
	CString GetAssociatedApp(LPCTSTR extension);
	CString GetAssociatedAppName(LPCTSTR extension);
	CStringList *GetAssociationList() { ASSERT(m_AttemptedRead); return &m_AssocViewers; }	
	BOOL SetDiffAssociation(LPCTSTR extension, LPCTSTR application);
	BOOL ClearDiffAssociation(LPCTSTR extension);
	CString GetAssociatedDiff(LPCTSTR extension);
	CString GetAssociatedDiffName(LPCTSTR extension);
	CStringList *GetDiffAssocList() { ASSERT(m_AttemptedRead); return &m_AssocDiffs; }	
	BOOL SetMergeAssociation(LPCTSTR extension, LPCTSTR application);
	BOOL ClearMergeAssociation(LPCTSTR extension);
	CString GetAssociatedMerge(LPCTSTR extension);
	CString GetAssociatedMergeName(LPCTSTR extension);
	CStringList *GetMergeAssocList() { ASSERT(m_AttemptedRead); return &m_AssocMerges; }	
		
protected:
	BOOL GetRegKey(CString& str, LPCTSTR section, LPCTSTR entry, LPCTSTR envVarName, LPCTSTR defaultValue);
	BOOL GetRegKey(BOOL *num, LPCTSTR section, LPCTSTR entry, BOOL defaultValue);
	BOOL GetRegKey(long *num, LPCTSTR section, LPCTSTR entry, long defaultValue);
	BOOL SetRegKey( CString& str, LPCTSTR section, LPCTSTR entry);
	BOOL DelRegKey( LPCTSTR section, LPCTSTR entry);
	void WriteVirginRegistry();

public:
	void ReadAssociationList();
	void ReadDiffAssocList();
	void ReadMergeAssocList();
	CString GetPasswordfromReg( );

protected:
	POSITION FindAssociation(LPCTSTR extension);
	POSITION FindDiffAssociation(LPCTSTR extension);
	POSITION FindMergeAssociation(LPCTSTR extension);
	void SetMyID(); 
	void RegWriteFailed(LONG rc);
};

#endif //__P4REGISTRY__
