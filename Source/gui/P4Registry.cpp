/*
 * Copyright 1997, 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */


// P4Registry.cpp

#include "stdafx.h"
#include "P4Win.h"
#include "MainFrm.h"
#include "p4registry.h"
#include "guiclientuser.h"
#include "StringUtil.h"
#include "GuiClient.h"
#include "ExceptionHandler.h"

#define	NONE				_T("none")

#define TabWidth			_T("TabWidth")
#define WhtSpFlag			_T("WhtSpFlag")
#define MrgTabWidth			_T("MrgTabWidth")
#define MrgWhtSpFlag		_T("MrgWhtSpFlag")
#define AnnotateWhtSpace	_T("AnnotateWhtSpace")
#define AnnotateIncInteg	_T("AnnotateIncInteg")
#define TLVWhtSpace			_T("TLVWhtSpace")
#define TLVIncInteg			_T("TLVIncInteg")
#define ResolveWhtSp		_T("ResolveWhtSp")
#define UseTempForView		_T("UseTempForView")
#define UseTempForExts		_T("UseTempForExts")
#define EditAppIsConsole	_T("EditAppIsConsole")
#define DiffAppIsConsole	_T("DiffAppIsConsole")
#define DiffAppIsClose		_T("DiffAppIsClose")
#define DiffAppIsBinary		_T("DiffAppIsBinary")
#define DiffInternal		_T("DiffAppInternal")
#define DiffOptArgs			_T("DiffOptArgs")
#define DiffOptArgChk		_T("DiffOptArgChk")
#define MergeInternal		_T("MergeAppInternal")
#define MergeOptArgs		_T("MergeOptArgs")
#define MergeOptArgChk		_T("MergeOptArgChk")
#define MergeNSF			_T("MergeAppNSF")
#define MergeAppIsConsole	_T("MergeAppIsConsole")
#define MergeAppIsClose		_T("MergeAppIsClose")
#define AlwaysShowFocus		_T("AlwaysShowFocus")
#define AlwaysWarnOnRevert	_T("AlwaysWarnOnRevert")
#define AllowPromptAbove	_T("AllowPromptAbove")
#define AutoMinEditDlg		_T("AutoMinEditDlg")
#define DoubleClickOption	_T("DoubleClickOption")
#define OptionStartUpOption	_T("OptionStartUpOption")
#define OptionStartUpPage	_T("OptionStartUpPage")
#define StatusUpdateInterval _T("StatusUpdate10thsSec")
#define LabelAddRplDefault  _T("LabelAddRplDefault")
#define Diff2Default1       _T("Diff2Default1")
#define Diff2Default2       _T("Diff2Default2")
#define TempFiles			_T("TempFiles")
#define Explorer			_T("Explorer")
#define Use2Panes			_T("Use2Panes")
#define AltExplorer			_T("AltExplorer")
#define	PreviewDefault		_T("PreviewDefault")
#define	IsBaselessMerge		_T("IntegDefIsBaselessMerge")
#define	IsIndirectMerge		_T("IntegDefIsIndirectMerge")
#define	IsPropagateTypes	_T("IntegDefIsPropagateTypes")
#define	IsBaseViaDelReadd	_T("IntegDefIsBaseViaDelReadd")
#define	UseNewChglist		_T("IntegDefUseNewChglist")
#define	UseNewChglist2		_T("RenameDefUseNewChglist")
#define	IsPermitDelReadd	_T("IntegDefIsPermitDelReadd")
#define	DelReaddType		_T("IntegDefDelReaddType")
#define	IsDeleteSource		_T("IntegDefIsDeleteSource")
#define	IsForceInteg		_T("IntegDefIsForceInteg")
#define	IsForceDirect		_T("IntegDefIsForceDirect")
#define	IsNoCopy			_T("IntegDefIsNoCopy")
#define	SyncFirstDefault	_T("SyncFirstDefault")
#define	WarnAlreadyOpened	_T("WarnAlreadyOpened")
#define	ExpandFlag			_T("ExpandFlag")
#define	ExpandPath			_T("ExpandPath")
#define	WarnLimit			_T("WarnLimit")
#define	WarnLimitDiff		_T("WarnLimitDiff")
#define	WarnLimitOpen		_T("WarnLimitOpen")
#define	ExtSortMax			_T("ExtSortMax")
#define	MinMultiLineSize	_T("MinMultiLineSize")
#define	UserFilter			_T("UserFilter")
#define	FilteredByUser		_T("FilteredByUser")
#define	ClientFilter		_T("ClientFilter")
#define	FilteredByClient	_T("FilteredByClient")
#define	LabelFilterByOwnerFlag	_T("LabelFilterByOwnerFlag")
#define	LabelFilterByOwner	_T("LabelFilterByOwner")
#define	LabelFilterIncBlank	_T("LabelFilterIncBlank")
#define	UseOpenForEditExts	_T("UseOpenForEditExts")
#define	UseOpenForEdit		_T("UseOpenForEdit")
#define	UseShortRevHistDesc	_T("UseShortRevHistDesc")
#define	UseShortSubmittedDesc _T("UseShortSubmittedDesc")
#define	UseLongChglistDesc	_T("UseLongChglistDesc")
#define	BusyWaitTime		_T("BusyWaitTime")
#define	SyncDlgFlag			_T("SyncDlgFlag")
#define	DescWrap			_T("DescWrap")
#define	DescWrapSw			_T("DescWrapSw")
#define ShowDeleted			_T("ShowDeleted")
#define SortByExtension		_T("SortByExtension")
#define SortChgFilesByAction _T("SortChgFilesByAction")
#define SortChgFilesByName	 _T("SortChgFilesByName")
#define SortChgFilesByExt	 _T("SortChgFilesByExt")
#define SortChgFilesByResolve _T("SortChgFilesByResolve")
#define ShowToolBar			_T("ShowToolBar")
#define ShowEntireDepot		_T("ShowEntireDepot")
#define ShowFileTypes		_T("ShowFileTypes")
#define ShowOpenActions		_T("ShowOpenActions")
#define ShowChangeDescs		_T("ShowChangeDescs")
#define SortChgsByUser		_T("ShowChgsByUser")
#define ShowCommandTrace	_T("ShowCommandTrace")
#define ShowConnectSettings _T("ShowConnectionSettings")
#define ShowConnectPort1st  _T("ShowConnectionPort1st")
#define ShowClientPath4Chgs _T("ShowClientPath4Chgs")
#define ShowClientPath      _T("ShowClientPath")
#define ShowDepotPathHiLite _T("ShowDepotPathHiLite")
#define ShowStatusTime		_T("ShowStatusTime")
#define ShowHiddenFilesNotInDepot _T("ShowHiddenFilesNotInDepot")
#define Use24hourClock		_T("Use24hourClock")
#define PreserveSpecFormat	_T("PreserveSpecFormat")
#define AutoExpandOptions	_T("AutoExpandOptions")
#define LabelFilesInDialog	_T("LabelFilesInDialog")
#define LabelShowPreviewDetail	_T("LabelShowPreviewDetail")
#define Diff2InDialog			_T("Diff2InDialog")
#define AutoTreeExpand			_T("AutoTreeExpand")
#define UseNotepad4WarnAndErr	_T("UseNotepad4WarnAndErr")
#define Cvt2ValComboToChkBx	_T("Cvt2ValComboToChkBx")
#define IconsInMenus		_T("IconsInMenus")
#define ExpandChgLists		_T("ExpandChgLists")
#define ReExpandChgs		_T("ReExpandChgs")
#define EnableSubChgIntegFilter	_T("EnableSubChgIntegFilter")
#define EnableRevHistShowIntegs	_T("EnableRevHistShowIntegs")
#define EnablePendingChgsOtherClients _T("EnablePendingChgsOtherClients")
#define AddFilterExclude	_T("AddFilterExclude")
#define AutoPollServer		_T("AutoPollServer")
#define AutoPollTime		_T("AutoPollTime")
#define AutoPollIconic		_T("AutoPollIconic")
#define AutoPollJobs		_T("AutoPollJobs")
#define LogoutOnExit		_T("LogoutOnExit")
#define	ClearAndReload		_T("ClearAndReload")
#define	ReloadOnUncover		_T("ReloadOnUncover")
#define ReloadUncoverTime	_T("ReloadUncoverTime")
#define ChglistSyncDef		_T("ChglistSyncDef")
#define FetchChangeCount	_T("FetchChangeCount")
#define FetchAllChanges     _T("FetchAllChanges")
#define FetchJobCount		_T("FetchJobCount")
#define FetchAllJobs		_T("FetchAllJobs")
#define FetchHistCount		_T("FetchHistCount")
#define FetchAllHist		_T("FetchAllHist")
#define FetchCompleteHist	_T("FetchCompleteHist")
#define HistListHeight		_T("HistListHeight")
#define JobFilterWidth		_T("JobFilterWidth")
#define IgnoreWinAssoc      _T("IgnoreWinAssoc")
#define AddFileCurDir		_T("AddFileCurDir")
#define AddFileExtFilter	_T("AddFileExtFilter")
#define AddFileFilter		_T("AddFileFilter")
#define AddFileFilterIndex	_T("AddFileFilterIndex")
#define DefaultDnDfromExp	_T("DefaultDnDfromExp")
#define MultiProcessorSleep	_T("MultiProcessorSleep")
#define MaxStatusLines		_T("MaxStatusLines")
#define ShowStatusMsgs		_T("ShowStatusMsgs")
#define ShowTruncTooltip	_T("ShowTruncTooltip")
#define DontThreadDiffs		_T("DontThreadDiffs")
#define PendChgExpansion	_T("PendChgExpansion")
#define LastBranch			_T("LastBranch")
#define LastLabel			_T("LastLabel")
#define LastTemplate		_T("LastTemplate")
#define LocalCliTemplate	_T("LocalCliTemplate")
#define LocalCliTemplateSw	_T("LocalCliTemplateSw")
#define UseClientSpecSubmitOpts	_T("UseClientSpecSubmitOpts")
#define SwapButtonPosition	_T("SwapButtonPosition")
#define BranchFilteredFlags	_T("BranchFilteredFlags")
#define BranchFilterOwner	_T("BranchFilterOwner")
#define ClientFilteredFlags	_T("ClientFilteredFlags")
#define ClientFilterOwner	_T("ClientFilterOwner")
#define ClientFilterHost	_T("ClientFilterHost")
#define ClientFilterDesc	_T("ClientFilterDesc")
#define WindowPosition		_T("WindowPosition")
#define WindowIconic		_T("WindowIconic")
#define WindowRestoreMaximized	_T("WindowRestoreMaximized")
#define HorizontalSplitter	_T("HorizontalSplitter")
#define VerticalSplitter	_T("VerticalSplitter")
#define RecentFindStrList	_T("Recent Find String List")
#define RecentToolArgList	_T("Recent Tool Arg List")
#define RecentViewerList	_T("Recent Viewer List")
#define RecentFilterList	_T("Recent Filter List")
#define RecentChgFilterList	_T("Recent Chg Filter List")
#define RecentPortList		_T("Recent Port List")
#define RecentCharsetList	_T("Recent Charset List")
#define RecentPcuList		_T("Recent Port-Client-User List")
#define DlgFontFace			_T("DialogFontFace")
#define DlgFontSize			_T("DialogFontSize")
#define DlgFontItalic		_T("DialogFontItalic")
#define DlgFontWeight		_T("DialogFontWeight")
#define	FindFilesWidth		_T("FindFilesWidth")
#define	P4StartWith			_T("P4StartWith")
#define	P4BusyCursor		_T("P4BusyCursor")
#define	CloseAfterFind		_T("CloseAfterFind")
#define	SubmitOnlyChged		_T("SubmitOnlyChged")
#define	RevertUnchged		_T("RevertUnchged")
#define	AutoReopen			_T("AutoReopen")
#define	LabelDragDropOption	_T("LabelDragDropOption")
#define	ResolveDefault		_T("ResolveDefault")
#define	Resolve2wayDefault	_T("Resolve2wayDefault")
#define	Resolve3wayDefault	_T("Resolve3wayDefault")
#define	ConvertJobNbr		_T("ConvertJobNbr")
#define Use256colorIcons	_T("Use256colorIcons")
#define UseDepotInClientView		_T("UseDepotInClientView")
#define DontShowYouHaveChgClientView _T("DontShowYouHaveChgClientView")
#define DontShowYouHaveCr8NewClient  _T("DontShowYouHaveCr8NewClient")
#define DontShowDiscardFormChgs		_T("DontShowDiscardFormChgs")
#define AllowExceptionReporting		_T("AllowExceptionReporting")
#define TryResetingFocus			_T("TryResetingFocus")
#define	FilterPendChgsByMyClient	_T("FilterPendChgsByMyClient")
#define	FilterPendChgsByPath		_T("FilterPendChgsByPath")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const DWORD SIZE_REG_VALUE = 255;


CP4Registry::CP4Registry()
{
	m_AttemptedRead=FALSE;
	m_Virgin = m_NoCliHost = m_bCharsetFromCmdli = FALSE;
	m_DefCharset[0] = _T("shiftjis");
	m_DefCharset[1] = _T("winansi");
	m_DefCharset[2] = _T("eucjp");
	m_DefCharset[3] = _T("iso8859-1");
	m_DefCharset[4] = _T("utf8");
	m_DefCharset[5] = _T("iso8859-15");
	m_DefCharset[6] = _T("macosroman");
	m_DefCharset[7] = _T("iso8859-5");
	m_DefCharset[8] = _T("cp1251");
	m_DefCharset[9] = _T("koi8-r");
	m_DefCharset[10] = _T("utf16le-bom");
	m_DefCharset[11] = _T("utf8-bom");
	m_DefCharset[12] = _T("");
	m_DefCharset[13] = _T("");
	m_DefCharset[14] = _T("");
	m_DefCharset[15] = _T("");
}

CP4Registry::~CP4Registry()
{
	
}


/*
	_________________________________________________________________

	Attempt to read all possible registry entries.  
	If any are not found, write defaults to the registry and to member 
	vars.  If P4Port, P4User or P4Client are not read successfully, 
	return FALSE
	_________________________________________________________________
*/

BOOL CP4Registry::ReadRegistry()
{
	//	The user, client and port will get set the first time that a
    //  command is run (will be MaxChange) 
    //
    //  The __Reg version of each value is what the P4 api will find,
    //  whether that info actually comes from the registry or from a
    //  function like gethostname().  The other version of each value
    //  is the value the user is actually running with, and can be
    //  set via command line args or clicking in UserView or ClientView
	//
	
	m_P4Client_Perm = m_P4Client_Temp = _T("");
	m_P4User_Perm = m_P4User_Temp = _T("");
	m_P4Port_Perm = m_P4Port_Temp =_T("");
	m_P4Password_Perm = m_P4Password_Temp = _T("");
	m_P4Charset_Perm = m_P4Charset_Temp =_T("");
	m_P4Client_Prev = m_P4User_Prev = m_P4Client_PrevDef = m_P4User_PrevDef = _T("@");

	m_AttemptedRead=TRUE;
	GetP4Client(TRUE);	// These 2 will get the values from the registry
	GetP4User(TRUE);
	m_AttemptedRead=FALSE;
	SetMyID();

	if(!GetRegKey( m_EditApp, _T("Options"), _T("EditApp"), _T("EDITOR"), _T("notepad") ))
		SetEditApp( m_EditApp );

	if(!GetRegKey( &m_TabWidth, _T("Options"), TabWidth, 4 ))
		SetTabWidth( m_TabWidth );

	if(!GetRegKey( &m_WhtSpFlag, _T("Options"), WhtSpFlag, 0 ))
		SetWhtSpFlag( m_WhtSpFlag );

	if(!GetRegKey( m_DiffApp, _T("Options"), _T("DiffApp"), _T("DIFF"), _T("windiff") ))
	{
		SetDiffApp( m_DiffApp );
		// if this key is not set, then we ca assume a virgin installation
		m_Virgin = TRUE;
		WriteVirginRegistry();
	}

	if(!GetRegKey( m_DiffOptArgs, _T("Options"), DiffOptArgs, _T("DIFFOPTARGS"), _T("%1 %2") ))
		SetDiffOptArgs( m_DiffOptArgs );

	m_AttemptedRead=TRUE;
	if(!GetRegKey( &m_MrgTabWidth, _T("Options"), MrgTabWidth, GetTabWidth() ))
		SetMrgTabWidth( m_MrgTabWidth );
	m_AttemptedRead=FALSE;

	if(!GetRegKey( &m_MrgWhtSpFlag, _T("Options"), MrgWhtSpFlag, 0 ))
	{
		SetMrgWhtSpFlag( m_MrgWhtSpFlag );
		SetResolveWhtSp( m_MrgWhtSpFlag );
	}

	if(!GetRegKey( m_MergeApp, _T("Options"), _T("MergeApp"), _T("MERGE"), _T("") ))
		SetMergeApp( m_MergeApp );

	if(!GetRegKey( m_MergeOptArgs, _T("Options"), MergeOptArgs, _T("MERGEOPTARGS"), _T("%1 %2 %3 %4") ))
		SetMergeOptArgs( m_MergeOptArgs );

	if(!GetRegKey( &m_AnnotateWhtSpace, _T("Options"), AnnotateWhtSpace, 0 ))
		SetAnnotateWhtSpace( m_AnnotateWhtSpace );

	if(!GetRegKey( &m_AnnotateIncInteg, _T("Options"), AnnotateIncInteg, 0 ))
		SetAnnotateIncInteg( m_AnnotateIncInteg );

	if(!GetRegKey( &m_TLVWhtSpace, _T("Options"), TLVWhtSpace, 0 ))
		SetTLVWhtSpace( m_TLVWhtSpace );

	if(!GetRegKey( &m_TLVIncInteg, _T("Options"), TLVIncInteg, 0 ))
		SetTLVIncInteg( m_TLVIncInteg );

	if(!GetRegKey( &m_ResolveWhtSp, _T("Options"), ResolveWhtSp, 0 ))
		SetResolveWhtSp( m_ResolveWhtSp );

	if(!GetRegKey( &m_UseTempForView, _T("Options"), UseTempForView, 1 ))
		SetUseTempForView( m_UseTempForView );

	if(!GetRegKey( m_UseTempForExts, _T("Options"), UseTempForExts, _T("USETEMPFOREXTS"), _T("htm,html,exe") ))
		SetUseTempForExts( m_UseTempForExts );

	if(!GetRegKey( &m_EditAppIsConsole, _T("Options"), EditAppIsConsole, FALSE ))
		SetEditAppIsConsole( m_EditAppIsConsole );

    if(!GetRegKey( &m_IgnoreWinAssoc, _T("Options"), IgnoreWinAssoc, FALSE ))
		SetIgnoreWinAssoc( m_IgnoreWinAssoc );

	if(!GetRegKey( &m_DiffAppIsConsole, _T("Options"), DiffAppIsConsole, FALSE ))
		SetDiffAppIsConsole( m_DiffAppIsConsole );

	if(!GetRegKey( &m_DiffAppIsClose, _T("Options"), DiffAppIsClose, FALSE ))
		SetDiffAppIsClose( m_DiffAppIsClose );

	if(!GetRegKey( &m_DiffAppIsBinary, _T("Options"), DiffAppIsBinary, FALSE ))
		SetDiffAppIsBinary( m_DiffAppIsBinary );

	if(!GetRegKey( &m_DiffOptArgChk, _T("Options"), DiffOptArgChk, FALSE ))
		SetDiffOptArgChk( m_DiffOptArgChk );

	if(!GetRegKey( &m_DiffInternal, _T("Options"), DiffInternal, TRUE ))
		SetDiffInternal( m_DiffInternal );

	if(!GetRegKey( &m_MergeInternal, _T("Options"), MergeInternal, 1 ))
		SetMergeInternal( m_MergeInternal );

	if(!GetRegKey( &m_MergeOptArgChk, _T("Options"), MergeOptArgChk, FALSE ))
		SetMergeOptArgChk( m_MergeOptArgChk );

	if(!GetRegKey( &m_MergeNSF, _T("Options"), MergeNSF, FALSE ))
		SetMergeNSF( m_MergeNSF );

	if(!GetRegKey( &m_MergeAppIsConsole, _T("Options"), MergeAppIsConsole, FALSE ))
		SetMergeAppIsConsole( m_MergeAppIsConsole );

	if(!GetRegKey( &m_MergeAppIsClose, _T("Options"), MergeAppIsClose, FALSE ))
		SetMergeAppIsClose( m_MergeAppIsClose );

	if(!GetRegKey( &m_AlwaysShowFocus, _T("Options"), AlwaysShowFocus, TRUE ))
		SetAlwaysShowFocus( m_AlwaysShowFocus );

	if(!GetRegKey( &m_AlwaysWarnOnRevert, _T("Options"), AlwaysWarnOnRevert, FALSE ))
		SetAlwaysWarnOnRevert( m_AlwaysWarnOnRevert );

	if(!GetRegKey( &m_AllowPromptAbove, _T("Options"), AllowPromptAbove, FALSE ))
		SetAllowPromptAbove( m_AllowPromptAbove );

	if(!GetRegKey( &m_AutoMinEditDlg, _T("Options"), AutoMinEditDlg, FALSE ))
		SetAutoMinEditDlg( m_AutoMinEditDlg );

	if(!GetRegKey( &m_DoubleClickOption, _T("Options"), DoubleClickOption, 0 ))
		SetDoubleClickOption( m_DoubleClickOption );

	if(!GetRegKey( &m_OptionStartUpOption, _T("Options"), OptionStartUpOption, 2 ))
		SetOptionStartUpOption( m_OptionStartUpOption );

	if(!GetRegKey( &m_OptionStartUpPage, _T("Options"), OptionStartUpPage, 0 ))
		SetOptionStartUpPage( m_OptionStartUpPage );

	if(!GetRegKey( &m_StatusUpdateInterval, _T("Options"), StatusUpdateInterval, 0 ))
		SetStatusUpdateInterval( m_StatusUpdateInterval );

	if(!GetRegKey( &m_LabelAddRplDefault, _T("Options"), LabelAddRplDefault, 0 ))
		SetLabelAddRplDefault( m_LabelAddRplDefault );

	if(!GetRegKey( &m_Diff2Default1, _T("Options"), Diff2Default1, 0 ))
		SetDiff2Default1( m_Diff2Default1 );

	if(!GetRegKey( &m_Diff2Default2, _T("Options"), Diff2Default2, 0 ))
		SetDiff2Default2( m_Diff2Default2 );

	// Always set temp dir, to make sure trailing slashes are removed
	TCHAR EnvStr[MAX_PATH+1];
	if (::GetTempPath(sizeof(EnvStr)/sizeof(TCHAR), EnvStr))
		lstrcat(EnvStr, _T("p4win"));
	else
		lstrcpy(EnvStr, _T("C:\\P4WinTemp"));
	GetRegKey( m_TempDir, _T("Options"), TempFiles, NULL, EnvStr );
	m_TempDir.TrimRight();
	SetTempDir( m_TempDir );

	if(!GetRegKey( &m_Explorer, _T("Options"), Explorer, 0 ))
		SetExplorer( m_Explorer );

	if(!GetRegKey( &m_2Panes, _T("Options"), Use2Panes, 1 ))
		Set2Panes( m_2Panes );

	if(!GetRegKey( m_AltExpl, _T("Options"), AltExplorer, NULL, _T("Explorer.exe /e,") ))
		SetAltExpl( m_AltExpl );

	if(!GetRegKey( &m_ShowDeleted, _T("Options"), ShowDeleted, FALSE ))
		SetShowDeleted( m_ShowDeleted );

	if(!GetRegKey( &m_SortByExtension, _T("Options"), SortByExtension, FALSE ))
		SetSortByExtension( m_SortByExtension );

	if(!GetRegKey( &m_SortChgFilesByAction, _T("Options"), SortChgFilesByAction, FALSE ))
		SetSortChgFilesByAction( m_SortChgFilesByAction );

	if(!GetRegKey( &m_SortChgFilesByName, _T("Options"), SortChgFilesByName, FALSE ))
		SetSortChgFilesByName( m_SortChgFilesByName );

	if(!GetRegKey( &m_SortChgFilesByExt, _T("Options"), SortChgFilesByExt, FALSE ))
		SetSortChgFilesByExt( m_SortChgFilesByExt );

	if(!GetRegKey( &m_SortChgFilesByResolve, _T("Options"), SortChgFilesByResolve, FALSE ))
		SetSortChgFilesByResolve( m_SortChgFilesByResolve );

	if(!GetRegKey( &m_IsNoCopy, _T("Options"), IsNoCopy, FALSE ))
		SetIsNoCopy( m_IsNoCopy );

	if(!GetRegKey( &m_IsForceInteg, _T("Options"), IsForceInteg, FALSE ))
		SetIsForceInteg( m_IsForceInteg );

	if(!GetRegKey( &m_IsForceDirect, _T("Options"), IsForceDirect, FALSE ))
		SetIsForceDirect( m_IsForceDirect );

	if(!GetRegKey( &m_IsDeleteSource, _T("Options"), IsDeleteSource, FALSE ))
		SetIsDeleteSource( m_IsDeleteSource );

	if(!GetRegKey( &m_IsPermitDelReadd, _T("Options"), IsPermitDelReadd, FALSE ))
		SetIsPermitDelReadd( m_IsPermitDelReadd );

	if(!GetRegKey( &m_DelReaddType, _T("Options"), DelReaddType, 2 ))
		SetDelReaddType( m_DelReaddType );

	if(!GetRegKey( &m_IsPropagateTypes, _T("Options"), IsPropagateTypes, FALSE ))
		SetIsPropagateTypes( m_IsPropagateTypes );

	if(!GetRegKey( &m_IsBaseViaDelReadd, _T("Options"), IsBaseViaDelReadd, FALSE ))
		SetIsBaseViaDelReadd( m_IsBaseViaDelReadd );

	if(!GetRegKey( &m_UseNewChglist, _T("Options"), UseNewChglist, FALSE ))
		SetUseNewChglist( m_UseNewChglist );

	if(!GetRegKey( &m_UseNewChglist2, _T("Options"), UseNewChglist2, TRUE ))
		SetUseNewChglist2( m_UseNewChglist2 );

	if(!GetRegKey( &m_IsBaselessMerge, _T("Options"), IsBaselessMerge, FALSE ))
		SetIsBaselessMerge( m_IsBaselessMerge );

	// Keep this disabled!
	// if(!GetRegKey( &m_IsIndirectMerge, _T("Options"), IsIndirectMerge, FALSE ))
	/* SetIsIndirectMerge(*/ m_IsIndirectMerge = FALSE /*)*/;

	if(!GetRegKey( &m_SyncFirstDefault, _T("Options"), SyncFirstDefault, TRUE ))
		SetSyncFirstDefault( m_SyncFirstDefault );

	if(!GetRegKey( &m_WarnAlreadyOpened, _T("Options"), WarnAlreadyOpened, FALSE ))
		SetWarnAlreadyOpened( m_WarnAlreadyOpened );

	if(!GetRegKey( &m_ExpandFlag, _T("Options"), ExpandFlag, 0 ))
		SetExpandFlag( m_ExpandFlag );

	if(!GetRegKey( m_ExpandPath, _T("Options"), ExpandPath, NULL, _T("") ))
		SetExpandPath( m_ExpandPath );

	if(!GetRegKey( m_WarnLimit, _T("Options"), WarnLimit, NULL, _T("5000") ))
		SetWarnLimit( m_WarnLimit );

	if(!GetRegKey( m_WarnLimitDiff, _T("Options"), WarnLimitDiff, NULL, _T("12") ))
		SetWarnLimitDiff( m_WarnLimitDiff );

	if(!GetRegKey( m_WarnLimitOpen, _T("Options"), WarnLimitOpen, NULL, _T("50") ))
		SetWarnLimitOpen( m_WarnLimitOpen );

	if(!GetRegKey( m_ExtSortMax, _T("Options"), ExtSortMax, NULL, _T("1000") ))
		SetExtSortMax( m_ExtSortMax );

	if(!GetRegKey( m_MinMultiLineSize, _T("Options"), MinMultiLineSize, NULL, _T("3") ))
		SetMinMultiLineSize( m_MinMultiLineSize );

	if(!GetRegKey( m_UserFilter, _T("Options"), UserFilter, NULL, _T("") ))
		SetUserFilter( m_UserFilter );

	if(!GetRegKey( &m_FilteredByUser, _T("Options"), FilteredByUser, 0 ))
		SetFilteredByUser( m_FilteredByUser );

	if(!GetRegKey( m_ClientFilter, _T("Options"), ClientFilter, NULL, _T("") ))
		SetClientFilter( m_ClientFilter );

	if(!GetRegKey( &m_FilteredByClient, _T("Options"), FilteredByClient, 0 ))
		SetFilteredByClient( m_FilteredByClient );

	if(!GetRegKey( m_LabelFilterByOwner, _T("Options"), LabelFilterByOwner, NULL, _T("") ))
		SetLabelFilterByOwner( m_LabelFilterByOwner );

	if(!GetRegKey( &m_LabelFilterByOwnerFlag, _T("Options"), LabelFilterByOwnerFlag, 
													m_LabelFilterByOwner.IsEmpty() ? 0 : 1 ))
		SetLabelFilterByOwnerFlag( m_LabelFilterByOwnerFlag );

	if(!GetRegKey( &m_LabelFilterIncBlank, _T("Options"), LabelFilterIncBlank, 0 ))
		SetLabelFilterIncBlank( m_LabelFilterIncBlank );

	if(!GetRegKey( m_UseOpenForEditExts, _T("Options"), UseOpenForEditExts, NULL, 
					_T("c,cc,cpp,cxx,h,hpp,hxx,rc,ico,def,dsp,dsw,vbs,vcproj,ncb,sln,ilk") ))
		SetUseOpenForEditExts( m_UseOpenForEditExts );

	if(!GetRegKey( &m_UseOpenForEdit, _T("Options"), UseOpenForEdit, 0 ))
		SetUseOpenForEdit( m_UseOpenForEdit );

	if(!GetRegKey( &m_UseShortRevHistDesc, _T("Options"), UseShortRevHistDesc, 0 ))
		SetUseShortRevHistDesc( m_UseShortRevHistDesc );

	if(!GetRegKey( &m_UseShortSubmittedDesc, _T("Options"), UseShortSubmittedDesc, 0 ))
		SetUseShortSubmittedDesc( m_UseShortSubmittedDesc );

	if(!GetRegKey( &m_UseLongChglistDesc, _T("Options"), UseLongChglistDesc, 31 ))
		SetUseLongChglistDesc( m_UseLongChglistDesc );

	if(!GetRegKey( &m_BusyWaitTime, _T("Options"), BusyWaitTime, 250 ))
		SetBusyWaitTime( m_BusyWaitTime );

	if(!GetRegKey( &m_SyncDlgFlag, _T("Options"), SyncDlgFlag, -1 ))
		SetSyncDlgFlag( m_SyncDlgFlag );

	if(!GetRegKey( &m_DescWrap, _T("Options"), DescWrap, 72 ))
		SetDescWrap( m_DescWrap );

	if(!GetRegKey( &m_DescWrapSw, _T("Options"), DescWrapSw, FALSE ))
		SetDescWrapSw( m_DescWrapSw );

	if(!GetRegKey( &m_ShowToolBar, _T("Options"), ShowToolBar, TRUE ))
		SetShowToolBar( m_ShowToolBar );

	if(!GetRegKey( &m_ShowEntireDepot, _T("Options"), ShowEntireDepot, 0 ))
		SetShowEntireDepot( m_ShowEntireDepot );

	if(!GetRegKey( &m_ShowFileType, _T("Options"), ShowFileTypes, TRUE ))
		SetShowFileType( m_ShowFileType );

	if(!GetRegKey( &m_ShowOpenAction, _T("Options"), ShowOpenActions, TRUE ))
		SetShowOpenAction( m_ShowOpenAction );

	if(!GetRegKey( &m_ShowChangeDesc, _T("Options"), ShowChangeDescs, TRUE ))
		SetShowChangeDesc( m_ShowChangeDesc );

	if(!GetRegKey( &m_SortChgsByUser, _T("Options"), SortChgsByUser, FALSE ))
		SetSortChgsByUser( m_SortChgsByUser );

	if(!GetRegKey( &m_ShowCommandTrace, _T("Options"), ShowCommandTrace, FALSE ))
		SetShowCommandTrace( m_ShowCommandTrace );

	if(!GetRegKey( &m_ShowConnectSettings, _T("Options"), ShowConnectSettings, TRUE ))
		SetShowConnectSettings( m_ShowConnectSettings );

	if(!GetRegKey( &m_ShowConnectPort1st, _T("Options"), ShowConnectPort1st, FALSE ))
		SetShowConnectPort1st( m_ShowConnectPort1st );

	// This has been disabled since it causes too muc server overhead
	SetShowClientPath4Chgs( m_ShowClientPath4Chgs = FALSE );

    if(!GetRegKey( &m_ShowClientPath, _T("Options"), ShowClientPath, TRUE ))
		SetShowClientPath( m_ShowClientPath );

    if(!GetRegKey( &m_ShowDepotPathHiLite, _T("Options"), ShowDepotPathHiLite, FALSE ))
		SetShowDepotPathHiLite( m_ShowDepotPathHiLite );

    if(!GetRegKey( &m_ShowStatusTime, _T("Options"), ShowStatusTime, FALSE ))
		SetShowStatusTime( m_ShowStatusTime );

    if(!GetRegKey( &m_ShowHiddenFilesNotInDepot, _T("Options"), ShowHiddenFilesNotInDepot, FALSE ))
		SetShowHiddenFilesNotInDepot( m_ShowHiddenFilesNotInDepot );

	TCHAR cSetting[3];
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITIME, cSetting, 3);
    if(!GetRegKey( &m_Use24hourClock, _T("Options"), Use24hourClock, cSetting[0]==_T('1') ))
		SetUse24hourClock( m_Use24hourClock );

    if(!GetRegKey( &m_PreserveSpecFormat, _T("Options"), PreserveSpecFormat, FALSE ))
		SetPreserveSpecFormat( m_PreserveSpecFormat );

    if(!GetRegKey( &m_AutoExpandOptions, _T("Options"), AutoExpandOptions, FALSE ))
		SetAutoExpandOptions( m_AutoExpandOptions );

    if(!GetRegKey( &m_LabelFilesInDialog, _T("Options"), LabelFilesInDialog, TRUE ))
		SetLabelFilesInDialog( m_LabelFilesInDialog );

    if(!GetRegKey( &m_LabelShowPreviewDetail, _T("Options"), LabelShowPreviewDetail, FALSE ))
		SetLabelShowPreviewDetail( m_LabelShowPreviewDetail );

    if(!GetRegKey( &m_Diff2InDialog, _T("Options"), Diff2InDialog, TRUE ))
		SetDiff2InDialog( m_Diff2InDialog );

    if(!GetRegKey( &m_AutoTreeExpand, _T("Options"), AutoTreeExpand, FALSE ))
		SetAutoTreeExpand( m_AutoTreeExpand );

    if(!GetRegKey( &m_UseNotepad4WarnAndErr, _T("Options"), UseNotepad4WarnAndErr, FALSE ))
		SetUseNotepad4WarnAndErr( m_UseNotepad4WarnAndErr );

    if(!GetRegKey( &m_Cvt2ValComboToChkBx, _T("Options"), Cvt2ValComboToChkBx, FALSE ))
		SetCvt2ValComboToChkBx( m_Cvt2ValComboToChkBx );

    if(!GetRegKey( &m_IconsInMenus, _T("Options"), IconsInMenus, TRUE ))
		SetIconsInMenus( m_IconsInMenus );

    if(!GetRegKey( &m_ExpandChgLists, _T("Options"), ExpandChgLists, TRUE ))
		SetExpandChgLists( m_ExpandChgLists );

    if(!GetRegKey( &m_ReExpandChgs, _T("Options"), ReExpandChgs, FALSE ))
		SetReExpandChgs( m_ReExpandChgs );

    if(!GetRegKey( &m_EnableSubChgIntegFilter, _T("Options"), EnableSubChgIntegFilter, TRUE ))
		SetEnableSubChgIntegFilter( m_EnableSubChgIntegFilter );

    if(!GetRegKey( &m_EnableRevHistShowIntegs, _T("Options"), EnableRevHistShowIntegs, TRUE ))
		SetEnableRevHistShowIntegs( m_EnableRevHistShowIntegs );

    if(!GetRegKey( &m_EnablePendingChgsOtherClients, _T("Options"), EnablePendingChgsOtherClients, TRUE ))
		SetEnablePendingChgsOtherClients( m_EnablePendingChgsOtherClients );

    if(!GetRegKey( &m_AddFilterExclude, _T("Options"), AddFilterExclude, -1 ))
		SetAddFilterExclude( m_AddFilterExclude );

	if(!GetRegKey( &m_AutoPoll, _T("Options"), AutoPollServer, TRUE ))
		SetAutoPoll( m_AutoPoll );

	if(!GetRegKey( &m_AutoPollTime, _T("Options"), AutoPollTime, 10 ))
		SetAutoPollTime( m_AutoPollTime );

	if(!GetRegKey( &m_AutoPollIconic, _T("Options"), AutoPollIconic, FALSE ))
		SetAutoPollIconic( m_AutoPollIconic );

	if(!GetRegKey( &m_AutoPollJobs, _T("Options"), AutoPollJobs, FALSE ))
		SetAutoPollIconic( m_AutoPollIconic );

	if(!GetRegKey( &m_LogoutOnExit, _T("Options"), LogoutOnExit, FALSE ))
		SetAutoPollIconic( m_AutoPollIconic );

	if(!GetRegKey( &m_ClearAndReload, _T("Options"), ClearAndReload, TRUE ))
		SetClearAndReload( m_ClearAndReload );

	if(!GetRegKey( &m_ReloadOnUncover, _T("Options"), ReloadOnUncover, FALSE ))
		SetReloadOnUncover( m_ReloadOnUncover );

	if(!GetRegKey( &m_ReloadUncoverTime, _T("Options"), ReloadUncoverTime, 1 ))
		SetReloadUncoverTime( m_ReloadUncoverTime );

	if(!GetRegKey( &m_ChglistSyncDef, _T("Options"), ChglistSyncDef, 0 ))
		SetChglistSyncDef( m_ChglistSyncDef );

	if(!GetRegKey( &m_FetchChangeCount, _T("Options"), FetchChangeCount, 100 ))
		SetFetchChangeCount( m_FetchChangeCount );

    if(!GetRegKey( &m_FetchAll, _T("Options"), FetchAllChanges, FALSE ))
		SetFetchAllChanges( m_FetchAll );

	if(!GetRegKey( &m_FetchJobCount, _T("Options"), FetchJobCount, 100 ))
		SetFetchJobCount( m_FetchJobCount );

    if(!GetRegKey( &m_FetchAllJobs, _T("Options"), FetchAllJobs, FALSE ))
		SetFetchAllJobs( m_FetchAllJobs );

	if(!GetRegKey( &m_FetchHistCount, _T("Options"), FetchHistCount, 100 ))
		SetFetchHistCount( m_FetchHistCount );

    if(!GetRegKey( &m_FetchAllHist, _T("Options"), FetchAllHist, TRUE ))
		SetFetchAllHist( m_FetchAllHist );

    if(!GetRegKey( &m_FetchCompleteHist, _T("Options"), FetchCompleteHist, 0 ))
		SetFetchCompleteHist( m_FetchCompleteHist );

    if(!GetRegKey( &m_HistListHeight, _T("Options"), HistListHeight, 0 ))
		SetHistListHeight( m_HistListHeight );

    if(!GetRegKey( &m_JobFilterWidth, _T("Options"), JobFilterWidth, 0 ))
		SetJobFilterWidth( m_JobFilterWidth );

    if(!GetRegKey( &m_UseDepotInClientView, _T("Options"), UseDepotInClientView, TRUE ))
		SetUseDepotInClientView( m_UseDepotInClientView );

    if(!GetRegKey( &m_Use256colorIcons, _T("Options"), Use256colorIcons, FALSE ))
		SetUse256colorIcons( m_Use256colorIcons );

    if(!GetRegKey( &m_DontShowYouHaveChgClientView, _T("Options"), DontShowYouHaveChgClientView, FALSE ))
		SetDontShowYouHaveChgClientView( m_DontShowYouHaveChgClientView );

    if(!GetRegKey( &m_DontShowYouHaveCr8NewClient, _T("Options"), DontShowYouHaveCr8NewClient, FALSE ))
		SetDontShowYouHaveCr8NewClient( m_DontShowYouHaveCr8NewClient );

    if(!GetRegKey( &m_DontShowDiscardFormChgs, _T("Options"), DontShowDiscardFormChgs, FALSE ))
		SetDontShowDiscardFormChgs( m_DontShowDiscardFormChgs );

	if(!GetRegKey( m_FontFace, _T("Options"), DlgFontFace, _T(""), LoadStringResource(IDS_DEFAULTFONTNAME) ))
		SetFontFace( m_FontFace );

	if(!GetRegKey( &m_FontSize, _T("Options"), DlgFontSize, _tstoi(LoadStringResource(IDS_DEFAULTFONTSIZE)) ))
		SetFontSize( m_FontSize );

	if(!GetRegKey( &m_IsFontItalic, _T("Options"), DlgFontItalic, FALSE ))
		SetFontItalic( m_IsFontItalic );

	if(!GetRegKey( &m_FontWeight, _T("Options"), DlgFontWeight, FW_NORMAL ))
		SetFontWeight( m_FontWeight );

	if(!GetRegKey( &m_FindFilesWidth, _T("Options"), FindFilesWidth, 0 ))
		SetFindFilesWidth( m_FindFilesWidth );

	if(!GetRegKey( &m_P4StartWith, _T("Options"), P4StartWith, 1 ))
		SetP4StartWith( m_P4StartWith );
	
	if(!GetRegKey( &m_P4BusyCursor, _T("Options"), P4BusyCursor, TRUE ))
		SetP4BusyCursor( m_P4BusyCursor );
	
	if(!GetRegKey( &m_CloseAfterFind, _T("Options"), CloseAfterFind, TRUE ))
		SetCloseAfterFind( m_CloseAfterFind );
	
	if(!GetRegKey( &m_SubmitOnlyChged, _T("Options"), SubmitOnlyChged, FALSE ))
		SetSubmitOnlyChged( m_SubmitOnlyChged );

	if(!GetRegKey( &m_RevertUnchged, _T("Options"), RevertUnchged, FALSE ))
		SetRevertUnchged( m_RevertUnchged );

	if(!GetRegKey( &m_AutoReopen, _T("Options"), AutoReopen, FALSE ))
		SetAutoReopen( m_AutoReopen );

	if(!GetRegKey( &m_FilterPendChgsByMyClient, _T("Options"), FilterPendChgsByMyClient, FALSE ))
		SetFilterPendChgsByMyClient( m_FilterPendChgsByMyClient );

	if(!GetRegKey( m_FilterPendChgsByPath, _T("Options"), FilterPendChgsByPath, _T(""), _T("") ))
		SetFilterPendChgsByPath( m_FilterPendChgsByPath );

	if(!GetRegKey( &m_LabelDragDropOption, _T("Options"), LabelDragDropOption, LDD_MENU ))
		SetLabelDragDropOption( m_LabelDragDropOption );
	
	if(!GetRegKey( &m_ResolveDefault, _T("Options"), ResolveDefault, 3 ))
		SetResolveDefault( m_ResolveDefault );
	
	if(!GetRegKey( &m_Resolve2wayDefault, _T("Options"), Resolve2wayDefault, 0 ))
		SetResolve2wayDefault( m_Resolve2wayDefault );
	
	if(!GetRegKey( &m_Resolve3wayDefault, _T("Options"), Resolve3wayDefault, 2 ))
		SetResolve3wayDefault( m_Resolve3wayDefault );
	
	if(!GetRegKey( &m_ConvertJobNbr, _T("Options"), ConvertJobNbr, TRUE ))
		SetConvertJobNbr( m_ConvertJobNbr );
	
	if(!GetRegKey( &m_AllowExceptionReporting, _T("Options"), AllowExceptionReporting, FALSE ))
		SetAllowExceptionReporting( m_AllowExceptionReporting );
	else
		EnableErrorRecording(m_AllowExceptionReporting == TRUE);

	if(!GetRegKey( &m_TryResetingFocus, _T("Options"), TryResetingFocus, FALSE ))
		SetTryResetingFocus( m_TryResetingFocus );

	/////////////
	// Settings Key
	if(!GetRegKey( m_AddFileCurDir, _T("Settings"), AddFileCurDir, NULL, _T("") ))
		SetAddFileCurDir( m_AddFileCurDir );

	if(!GetRegKey( m_AddFileExtFilter, _T("Settings"), AddFileExtFilter, NULL, _T("") ))
		SetAddFileCurDir( m_AddFileExtFilter );

	if(!GetRegKey( m_AddFileFilter, _T("Settings"), AddFileFilter, NULL, _T("") ))
		SetAddFileFilter( m_AddFileFilter );

	if(!GetRegKey( &m_AddFileFilterIndex, _T("Settings"), AddFileFilterIndex, 0 ))
		SetAddFileFilterIndex( m_AddFileFilterIndex );

	if(!GetRegKey( &m_DefaultDnDfromExp, _T("Settings"), DefaultDnDfromExp, 2 ))
		SetDefaultDnDfromExp( m_DefaultDnDfromExp );

	if(!GetRegKey( &m_MultiProcessorSleep, _T("Settings"), MultiProcessorSleep, 0 ))
		SetMultiProcessorSleep( m_MultiProcessorSleep );

	if(!GetRegKey( &m_MaxStatusLines, _T("Settings"), MaxStatusLines, 5000 ))
		SetMaxStatusLines( m_MaxStatusLines );

	if(!GetRegKey( &m_ShowStatusMsgs, _T("Settings"), ShowStatusMsgs, TRUE ))
		SetShowStatusMsgs( m_ShowStatusMsgs );

	if(!GetRegKey( &m_ShowTruncTooltip, _T("Settings"), ShowTruncTooltip, TRUE ))
		SetShowTruncTooltip( m_ShowTruncTooltip );

	if(!GetRegKey( &m_DontThreadDiffs, _T("Settings"), DontThreadDiffs, FALSE ))
		SetDontThreadDiffs( m_DontThreadDiffs );

	if(!GetRegKey( m_PendChgExpansion, _T("Settings"), PendChgExpansion, NULL, _T("0") ))
		SetPendChgExpansion( m_PendChgExpansion );

	if(!GetRegKey( m_LastBranch, _T("Settings"), LastBranch, NULL, _T("") ))
		SetLastBranch( m_LastBranch );

	if(!GetRegKey( m_LastLabel, _T("Settings"), LastLabel, NULL, _T("") ))
		SetLastLabel( m_LastLabel );

	if(!GetRegKey( m_LastTemplate, _T("Settings"), LastTemplate, NULL, _T("") ))
		SetLastTemplate( m_LastTemplate );

	if(!GetRegKey( m_LocalCliTemplate, _T("Settings"), LocalCliTemplate, NULL, _T("") ))
		SetLocalCliTemplate( m_LocalCliTemplate );

	if(!GetRegKey( &m_LocalCliTemplateSw, _T("Settings"), LocalCliTemplateSw, FALSE ))
		SetLocalCliTemplateSw( m_LocalCliTemplateSw );

	if(!GetRegKey( &m_UseClientSpecSubmitOpts, _T("Settings"), UseClientSpecSubmitOpts, FALSE ))
		SetUseClientSpecSubmitOpts( m_UseClientSpecSubmitOpts );

	if(!GetRegKey( &m_SwapButtonPosition, _T("Settings"), SwapButtonPosition, FALSE ))
		SetSwapButtonPosition( m_SwapButtonPosition );

	if(!GetRegKey( &m_BranchFilteredFlags, _T("Settings"), BranchFilteredFlags, 0 ))
		SetBranchFilteredFlags( m_BranchFilteredFlags );

	if(!GetRegKey( m_BranchFilterOwner, _T("Settings"), BranchFilterOwner, NULL, _T("") ))
		SetBranchFilterOwner( m_BranchFilterOwner );

	if(!GetRegKey( &m_ClientFilteredFlags, _T("Settings"), ClientFilteredFlags, 0 ))
		SetClientFilteredFlags( m_ClientFilteredFlags );

	if(!GetRegKey( m_ClientFilterOwner, _T("Settings"), ClientFilterOwner, NULL, _T("") ))
		SetClientFilterOwner( m_ClientFilterOwner );

	if(!GetRegKey( m_ClientFilterHost, _T("Settings"), ClientFilterHost, NULL, _T("") ))
	{
		SetClientFilterHost( m_ClientFilterHost );
		m_NoCliHost = TRUE;
	}

	if(!GetRegKey( m_ClientFilterDesc, _T("Settings"), ClientFilterDesc, NULL, _T("") ))
		SetClientFilterDesc( m_ClientFilterDesc );

	/////////////
	// Layout Key
	
	// Any error reading or parsing the frame position leave position as 0,0,0,0
	CString temp;
	if(!GetRegKey( temp, _T("Layout"), WindowPosition, NULL, _T("0,0,0,0") ))
		SetWindowPosition(CRect(0,0,0,0));
	int left= GetPositiveNumber(temp);
	int top= GetPositiveNumber(temp);
	int width= GetPositiveNumber(temp);
	int height= GetPositiveNumber(temp);
	m_WindowPosition= CRect(left, top, left+width, top+height);
	
	if(!GetRegKey( &m_Iconic, _T("Layout"), WindowIconic, FALSE ))
		SetWindowIconic( m_Iconic );

	if(!GetRegKey( &m_Maximized, _T("Layout"), WindowRestoreMaximized, FALSE ))
		SetWindowMaximized( m_Maximized );
	
	if(!GetRegKey( &m_HSplitterPosition, _T("Layout"), HorizontalSplitter, 0 ))
		SetHSplitterPosition( m_HSplitterPosition );

	if(!GetRegKey( &m_VSplitterPosition, _T("Layout"), VerticalSplitter, 0 ))
		SetVSplitterPosition( m_VSplitterPosition );

	
	/////////////
	// Bookmarks Key
	
	CString keyName;

	int i;
	for(i=0; i < MAX_BOOKMARKS; i++)
	{
		keyName.Format(_T("MenuName%d"), i);
		GetRegKey( m_BkMkMenuName[i], _T("Bookmarks"), keyName, _T(""), _T("") );
	}

	for(i=0; i < MAX_BOOKMARKS; i++)
	{
		keyName.Format(_T("IsSubMenu%d"), i);
		GetRegKey( &m_BkMkIsSubMenu[i], _T("Bookmarks"), keyName, FALSE );
	}

	
	/////////////
	// Favorites Key
	
	for(i=0; i < MAX_FAVORITES; i++)
	{
		keyName.Format(_T("MenuName%d"), i);
		GetRegKey( m_FavMenuName[i], _T("Favorites"), keyName, _T(""), _T("") );
	}

	for(i=0; i < MAX_FAVORITES; i++)
	{
		keyName.Format(_T("IsSubMenu%d"), i);
		GetRegKey( &m_FavIsSubMenu[i], _T("Favorites"), keyName, FALSE );
	}

	
	/////////////
	// Tools Key
	
	for(i=0; i < MAX_TOOLS; i++)
	{
		keyName.Format(_T("MenuName%d"), i);
		GetRegKey( m_ToolMenuName[i], _T("Tools"), keyName, _T(""), _T("") );
	}

	for(i=0; i < MAX_TOOLS; i++)
	{
		keyName.Format(_T("Command%d"), i);
		GetRegKey( m_ToolCommand[i], _T("Tools"), keyName, _T(""), _T("") );
	}

	for(i=0; i < MAX_TOOLS; i++)
	{
		keyName.Format(_T("Arguments%d"), i);
		GetRegKey( m_ToolArgs[i], _T("Tools"), keyName, _T(""), _T("") );
	}

	for(i=0; i < MAX_TOOLS; i++)
	{
		keyName.Format(_T("InitialDirectory%d"), i);
		GetRegKey( m_ToolInitDir[i], _T("Tools"), keyName, _T(""), _T("") );
	}

	for(i=0; i < MAX_TOOLS; i++)
	{
		keyName.Format(_T("PromptText%d"), i);
		GetRegKey( m_ToolPromptText[i], _T("Tools"), keyName, _T(""), _T("") );
	}

	for(i=0; i < MAX_TOOLS; i++)
	{
		keyName.Format(_T("IsConsole%d"), i);
		GetRegKey( &m_ToolIsConsole[i], _T("Tools"), keyName, FALSE );
	}

	for(i=0; i < MAX_TOOLS; i++)
	{
		keyName.Format(_T("IsPrompt%d"), i);
		GetRegKey( &m_ToolIsPrompt[i], _T("Tools"), keyName, FALSE );
	}

	for(i=0; i < MAX_TOOLS; i++)
	{
		keyName.Format(_T("IsOutput2Status%d"), i);
		GetRegKey( &m_ToolIsOutput2Status[i], _T("Tools"), keyName, FALSE );
	}

	for(i=0; i < MAX_TOOLS; i++)
	{
		keyName.Format(_T("IsCloseOnExit%d"), i);
		GetRegKey( &m_ToolIsCloseOnExit[i], _T("Tools"), keyName, FALSE );
	}

	for(i=0; i < MAX_TOOLS; i++)
	{
		keyName.Format(_T("IsShowBrowse%d"), i);
		GetRegKey( &m_ToolIsShowBrowse[i], _T("Tools"), keyName, FALSE );
	}

	for(i=0; i < MAX_TOOLS; i++)
	{
		keyName.Format(_T("IsRefresh%d"), i);
		GetRegKey( &m_ToolIsRefresh[i], _T("Tools"), keyName, FALSE );
	}

	for(i=0; i < MAX_TOOLS; i++)
	{
		keyName.Format(_T("OnContext%d"), i);
		GetRegKey( &m_ToolOnContext[i], _T("Tools"), keyName, FALSE );
	}

	for(i=0; i < MAX_TOOLS; i++)
	{
		keyName.Format(_T("IsSubMenu%d"), i);
		GetRegKey( &m_ToolIsSubMenu[i], _T("Tools"), keyName, FALSE );
	}

	
	/////////////
	// MRU Tool Arguments
	
	for(i=0; i < MAX_MRU_TOOLARGS; i++)
	{
		keyName.Format(_T("Arg%d"), i);
		GetRegKey( m_MRUToolArg[i], RecentToolArgList, keyName, _T(""), _T("") );
	}

	/////////////
	// MRU Find Strings
	
	for(i=0; i < MAX_MRU_FINDSTRS; i++)
	{
		keyName.Format(_T("FindStr%d"), i);
		GetRegKey( m_MRUFindStr[i], RecentFindStrList, keyName, _T(""), _T("") );
	}

	/////////////
	// MRU Viewers
	
	for(i=0; i < MAX_MRU_VIEWERS; i++)
	{
		keyName.Format(_T("File%d"), i);
		GetRegKey( m_MRUViewer[i], RecentViewerList, keyName, _T(""), _T("") );
	}

	/////////////
	// MRU Job Filters
	
	for(i=0; i < MAX_MRU_FILTERS; i++)
	{
		keyName.Format(_T("Filter%d"), i);
		GetRegKey( m_MRUFilter[i], RecentFilterList, keyName, _T(""), _T("") );
	}

	/////////////
	// MRU Submitted Changelist Filters
	
	for(i=0; i < MAX_MRU_FILTERS; i++)
	{
		keyName.Format(_T("Filter%d"), i);
		GetRegKey( m_MRUChgFilter[i], RecentChgFilterList, keyName, _T(""), _T("") );
	}

	/////////////
	// MRU Ports & Associated Charsets
	
	for(i=0; i < MAX_MRU_PORTS; i++)
	{
		keyName.Format(_T("Port%d"), i);
		GetRegKey( m_MRUPort[i], RecentPortList, keyName, _T(""), _T("") );
		keyName.Format(_T("Charset%d"), i);
		GetRegKey( m_MRUPortCharset[i], RecentPortList, keyName, _T(""), _T("") );
	}

	/////////////
	// MRU Charsets
	
	for(i=0; i < MAX_MRU_CHARSETS; i++)
	{
		keyName.Format(_T("Charset%d"), i);
		GetRegKey( m_MRUCharset[i], RecentCharsetList, keyName, _T(""), m_DefCharset[i] );
	}

	/////////////
	// MRU PCUs
	
	for(i=0; i < MAX_MRU_PCUS; i++)
	{
		keyName.Format(_T("PCU%d"), i);
		GetRegKey( m_MRUPcu[i], RecentPcuList, keyName, _T(""), _T("") );
	}
	if (m_P4StartWith == 0)
	{
		int j;
		CString mru = m_MRUPcu[0];
		if ((j = mru.Find(_T('@'))) > 0)
			mru = mru.Left(j);
		// find the separators (spaces)
		int firstsep = mru.Find(_T(' '));
		int secondsep= mru.Find(_T(' '), firstsep+1);

		// set the 3 port, client and user edit controls
		if (firstsep > 0 && secondsep > 0)
		{
			m_P4Port_Temp   = mru.Left(firstsep);
			m_P4Client_Temp = mru.Mid(firstsep+1, secondsep - firstsep - 1);
			m_P4User_Temp   = mru.Right(mru.GetLength() - secondsep - 1);
		}
		SetMyID();
	}

	/////////////
	// File Associations
	ReadAssociationList();

	/////////////
	// File Associations
	ReadDiffAssocList();
	ReadMergeAssocList();

	m_AttemptedRead=TRUE;
	return(TRUE);
}


/*
	_________________________________________________________________
*/

CString CP4Registry::GetPasswordfromReg( )
{
	HKEY hKey = NULL;
	CString sKey = _T("Software\\Perforce\\environment");
	static TCHAR szTag[ ] = _T("P4PASSWD");

	CString sPassword;
	TCHAR szValue[ SIZE_REG_VALUE + 1 ];
	DWORD cbData = sizeof(szValue);
	DWORD type = 0;

	if ( RegOpenKeyEx( HKEY_CURRENT_USER, sKey, 0, KEY_QUERY_VALUE, &hKey ) != ERROR_SUCCESS )
		goto outreg;

	if ( RegQueryValueEx( hKey, szTag, NULL, &type
				, ( LPBYTE ) szValue, &cbData ) != ERROR_SUCCESS )
		goto outreg;

	//		end value with a NULL since RegQueryValueEx can return any datatype
	//
	szValue[ cbData / sizeof(TCHAR) ] = _T('\0'); 
	sPassword = szValue;

outreg:
	if ( hKey )
		RegCloseKey( hKey );

	//		in case some nimrod uses env variable instead ...
	//
	if ( sPassword.IsEmpty() )
	{
		GetEnvironmentVariable( szTag, szValue, SIZE_REG_VALUE );
		sPassword = szValue;
	}
	return sPassword;
}


/*
	_________________________________________________________________
*/

void CP4Registry::ReadAssociationList()
{
	// Make sure the list is empty
	m_AssocViewers.RemoveAll();

	// Rummage for entries, which have names like 'cpp' and data fields that are appnames
	HKEY key;

	long success= RegOpenKeyEx( HKEY_CURRENT_USER, 
								_T("Software\\Perforce\\P4Win\\Associations"),
								0,
								KEY_READ,
								&key);

	if(success == ERROR_SUCCESS)
	{
		int i;
		for(i=0; success == ERROR_SUCCESS; i++)
		{
			TCHAR valueName[LONGPATH];
			DWORD valueNameLen= LONGPATH;
			TCHAR buf[LONGPATH];
			DWORD bufLen= LONGPATH;
			DWORD valueType;

			success= RegEnumValue(	key, i, valueName, &valueNameLen, NULL,
									&valueType, (LPBYTE)buf, &bufLen);

			if(success == ERROR_SUCCESS && valueType == REG_SZ && bufLen)
			{
				lstrcat(valueName, _T("|"));
				lstrcat(valueName, buf);
				m_AssocViewers.AddHead(valueName);
			}
					
		}
		RegCloseKey(key);
	}
}

void CP4Registry::ReadDiffAssocList()
{
	// Make sure the list is empty
	m_AssocDiffs.RemoveAll();

	// Rummage for entries, which have names like 'cpp' and data fields that are appnames
	HKEY key;

	long success= RegOpenKeyEx( HKEY_CURRENT_USER, 
								_T("Software\\Perforce\\P4Win\\DiffAssocs"),
								0,
								KEY_READ,
								&key);

	if(success == ERROR_SUCCESS)
	{
		int i;
		for(i=0; success == ERROR_SUCCESS; i++)
		{
			TCHAR valueName[LONGPATH];
			DWORD valueNameLen= LONGPATH;
			TCHAR buf[LONGPATH];
			DWORD bufLen= LONGPATH;
			DWORD valueType;

			success= RegEnumValue(	key, i, valueName, &valueNameLen, NULL,
									&valueType, (LPBYTE)buf, &bufLen);

			if(success == ERROR_SUCCESS && valueType == REG_SZ && bufLen)
			{
				lstrcat(valueName, _T("|"));
				lstrcat(valueName, buf);
				m_AssocDiffs.AddHead(valueName);
			}
					
		}
		RegCloseKey(key);
	}
}

void CP4Registry::ReadMergeAssocList()
{
	// Make sure the list is empty
	m_AssocMerges.RemoveAll();

	// Rummage for entries, which have names like 'cpp' and data fields that are appnames
	HKEY key;

	long success= RegOpenKeyEx( HKEY_CURRENT_USER, 
								_T("Software\\Perforce\\P4Win\\MergeAssocs"),
								0,
								KEY_READ,
								&key);

	if(success == ERROR_SUCCESS)
	{
		int i;
		for(i=0; success == ERROR_SUCCESS; i++)
		{
			TCHAR valueName[LONGPATH];
			DWORD valueNameLen= LONGPATH;
			TCHAR buf[LONGPATH];
			DWORD bufLen= LONGPATH;
			DWORD valueType;

			success= RegEnumValue(	key, i, valueName, &valueNameLen, NULL,
									&valueType, (LPBYTE)buf, &bufLen);

			if(success == ERROR_SUCCESS && valueType == REG_SZ && bufLen)
			{
				lstrcat(valueName, _T("|"));
				lstrcat(valueName, buf);
				m_AssocMerges.AddHead(valueName);
			}
					
		}
		RegCloseKey(key);
	}
}

BOOL CP4Registry::GetRegKey(CString& str, LPCTSTR section, LPCTSTR entry, LPCTSTR envVarName, LPCTSTR defaultValue)
{
	ASSERT(defaultValue != NULL);
	ASSERT(section != NULL);
	ASSERT(entry != NULL);

	BOOL foundIt=TRUE;

	TCHAR buf[256];
	int buflen=255;

	// First search the registry
	str=AfxGetApp()->GetProfileString(section, entry);

	if(str.GetLength()==0)
	{
		foundIt=FALSE;
		// Its not in registry, so check the environment variable
		if(envVarName != NULL && GetEnvironmentVariable(envVarName, buf, buflen)>0)
			str=buf;
		else
		{
			// if no environment variable, use the default
			str=defaultValue;
		}
	}
	
	return foundIt;
}
	
BOOL CP4Registry::GetRegKey(BOOL *num, LPCTSTR section, LPCTSTR entry, BOOL defaultValue)
{
	long lNum;
	BOOL success=GetRegKey(&lNum, section, entry, (long) defaultValue);
	*num=(BOOL) lNum;
	return success;
}

BOOL CP4Registry::GetRegKey(long *num, LPCTSTR section, LPCTSTR entry, long defaultValue)
{
	ASSERT(section[0] != _T('\0'));  
	ASSERT(entry[0] != _T('\0'));

	BOOL foundIt=TRUE;

	// First search the registry
	CString str=AfxGetApp()->GetProfileString(section, entry);
	if(str.GetLength()==0)
	{
		foundIt=FALSE;
		*num=defaultValue;
	}
	else
	{
		*num=_ttol(str); 
		if(*num >= 10000000 || *num < 0)  // Assume some 8-ball was hand-editing registry
		{
			foundIt=FALSE;
			*num = 100;
		}
	}

	return foundIt;
}

BOOL CP4Registry::SetRegKey( CString& str, LPCTSTR section, LPCTSTR entry)
{
	if(AfxGetApp()->WriteProfileString(section, entry, str))
		return TRUE;
	else
		return FALSE;
}

BOOL CP4Registry::DelRegKey( LPCTSTR section, LPCTSTR entry)
{
	if(AfxGetApp()->WriteProfileString(section, entry, NULL))
		return TRUE;
	else
		return FALSE;
}

LPCTSTR CP4Registry::GetP4Port( BOOL permanent /*=FALSE*/ ) 
{ 
    ASSERT( m_AttemptedRead ); 

	if ( permanent )
        return m_P4Port_Perm; 
    else if( m_P4Port_Temp.GetLength() )
		return m_P4Port_Temp;
	else
        return m_P4Port_Perm; 
}

LPCTSTR CP4Registry::GetP4User( BOOL permanent /*=FALSE*/ ) 
{ 
    ASSERT( m_AttemptedRead ); 

	if ( permanent )
	{
		CString defUser = CGuiClient::GetUserStr();
		if (m_P4User_Perm.Compare(defUser))
		{
			if(!m_P4User_Temp.GetLength())
				m_P4User_Temp = m_P4User_Perm;
			m_P4User_Perm = defUser;
		}
		return m_P4User_Perm;
	}
    else if( m_P4User_Temp.GetLength() )
		return m_P4User_Temp;
	else
        return m_P4User_Perm; 
}

LPCTSTR CP4Registry::GetP4Client( BOOL permanent /*=FALSE*/ ) 
{ 
    ASSERT( m_AttemptedRead ); 

	if ( permanent ) 
	{
		CString defclient = CGuiClient::GetClientStr();
		if (m_P4Client_Perm.Compare(defclient))
		{
			if(!m_P4Client_Temp.GetLength())
				m_P4Client_Temp = m_P4Client_Perm;
			m_P4Client_Perm = defclient;
		}
		return m_P4Client_Perm;
	}
	else if( m_P4Client_Temp.GetLength() )
		return m_P4Client_Temp;
    else
        return m_P4Client_Perm; 
}

LPCTSTR CP4Registry::GetP4UserPassword( BOOL permanent /*=FALSE*/ ) 
{ 
    ASSERT( m_AttemptedRead ); 

	if ( permanent )
        return m_P4Password_Perm; 
	else if( m_P4Password_Temp.GetLength() )
		return m_P4Password_Temp;
    else
        return m_P4Password_Perm; 
}
	
LPCTSTR CP4Registry::GetP4Charset( BOOL permanent /*=FALSE*/ ) 
{ 
    ASSERT( m_AttemptedRead ); 

	if ( permanent )
		return m_P4Charset_Perm == NONE ? _T("") : m_P4Charset_Perm;
    else if( m_P4Charset_Temp.GetLength() )
		return m_P4Charset_Temp == NONE ? _T("") : m_P4Charset_Temp;
	else
        return m_P4Charset_Perm == NONE ? _T("") : m_P4Charset_Perm;
}

BOOL CP4Registry::SetP4Port(LPCTSTR port, BOOL temporary, BOOL permanent, BOOL writetoreg)
{
    bool success = true;

	if (lstrcmp(GetP4Port(), port))
	{
		CMainFrame * mainWnd = (CMainFrame *)AfxGetMainWnd();
		if (mainWnd)
			::SendMessage(mainWnd->m_hWnd, WM_COMMAND, ID_VIEW_CLOSEDIALOGS, 0);
	}

	if(permanent)
	{
		// Use this to keep a copy of the input
		// since it might get reset by m_P4Port_Temp.Empty();
		CString portIn = port;

		if (writetoreg)
            success = CGuiClient::DefinePort(port);
		
		// If the permanent value changed and we do not as-yet have an over-ride,
		// turn the old value into a temporary over-ride.  This avoids problems
		// if a user is tinkering with the registry while the app is running
		if( !temporary && !m_P4Port_Perm.IsEmpty() && m_P4Port_Temp.IsEmpty())
			m_P4Port_Temp= m_P4Port_Perm;
		else if( temporary || ((m_P4Port_Temp.Compare(port) == 0)
							&& (m_P4Port_Perm.Compare(port) == 0)) )
			m_P4Port_Temp.Empty();

		if (writetoreg || m_P4Port_Perm.IsEmpty())
			m_P4Port_Perm = portIn;
	}
	else
	{
		// Only store m_P4Port_Temp if it is different than the permanent registry
		// setting for P4PORT
		if( m_P4Port_Perm.Compare( port ) == 0 )
			m_P4Port_Temp.Empty();
		else
			m_P4Port_Temp= port;
	}

	if (success && !m_bCharsetFromCmdli && (!permanent || !m_P4Port_Temp.GetLength()) )
	{
		int i;
		for(i=0; i < MAX_MRU_PORTS; i++)
		{
			m_MRUPort[i].TrimRight();
			if(m_MRUPort[i].GetLength() > 0 && m_MRUPort[i].Compare(port) == 0)
			{
				SetP4Charset(m_MRUPortCharset[i], TRUE, FALSE, FALSE);
				break;
			}
		}
		AddMRUPort( port, GetP4Charset() );
	}
	else
		m_bCharsetFromCmdli = FALSE;

	return success;
}

BOOL CP4Registry::SetP4User(LPCTSTR user, BOOL temporary, BOOL permanent, BOOL writetoreg)
{
	// Use this to keep a copy of the input
	// since it might get reset by m_P4User_Temp.Empty();
	CString userIn = user;
    BOOL success = TRUE;

	if(permanent)
	{
		if (writetoreg)
		{
			if (!m_P4User_Perm.IsEmpty())
				m_P4User_PrevDef = m_P4User_Perm;
			// save the old value in the registry in case of a failure
			// due to having a P4CONFIG or SET value that overrides
			HKEY	hKey = NULL;
			CString sKey = _T("Software\\Perforce\\environment\\");
			DWORD   type = REG_SZ;
			DWORD	disposition;
			TCHAR   saveRegUser[260];
			LONG	rc;
			if ( (rc = RegCreateKeyEx( HKEY_CURRENT_USER, sKey,
										0, NULL,
										REG_OPTION_NON_VOLATILE,
										KEY_READ|KEY_WRITE, NULL,
										&hKey, &disposition )) == ERROR_SUCCESS )
			{
				disposition = sizeof(saveRegUser);
				if ((rc = RegQueryValueEx( hKey, _T("P4USER"), NULL, &type,
						(LPBYTE)saveRegUser, &disposition)) != ERROR_SUCCESS )
					saveRegUser[0] = _T('\0');
			}
			if (rc != ERROR_SUCCESS )
				RegWriteFailed(rc);

			// use the api to write it to the registry
            success = CGuiClient::DefineUser(userIn);
			if (success)
			{
				// make sure the value written is now the value that will be used
				CString defUser = CGuiClient::GetUserStr();
				if (userIn != defUser)
				{
					// If we get here, there is a P4CONFIG
					//  or a Enviroment setting overriding.
					// So set this 'real' value as the permanent value,
					// then because of possible side effects,
					// set the value passed in as the temporary value.
					SetP4User(defUser, temporary, TRUE, FALSE);
					if (temporary)
						SetP4User(userIn, TRUE, FALSE, FALSE);
					// Now because we messed up the registry with the call
					// CGuiClient::DefineUser(userIn), we have to restore
					// the original registry value from above (saveRegUser).
					RegSetValueEx(hKey, _T("P4USER"), NULL,
								REG_SZ, (LPBYTE)saveRegUser, 
								lstrlen(saveRegUser)*sizeof(TCHAR)+1);
					RegCloseKey( hKey );
					// Finally indicate failure.
					return FALSE;
				}
			}
			RegCloseKey( hKey );
		}

		// If the permanent value changed and we do not as-yet have an over-ride,
		// turn the old value into a temporary over-ride.  This avoids problems
		// if a user is tinkering with the registry while the app is running
		if( !temporary && !m_P4User_Perm.IsEmpty() && m_P4User_Temp.IsEmpty())
			m_P4User_Temp= m_P4User_Perm;
		else if( temporary || ((m_P4User_Temp.Compare(userIn) == 0)
							&& (m_P4User_Perm.Compare(userIn) == 0)) )
			m_P4User_Temp.Empty();

		if (writetoreg || m_P4User_Perm.IsEmpty())
			m_P4User_Perm= userIn;
	}
	else
	{
		// Only store m_P4User_Temp if it is different than the permanent registry
		// setting for P4USER
		if( m_P4User_Perm.Compare( userIn ) == 0 )
			m_P4User_Temp.Empty();
		else
			m_P4User_Temp= userIn;
	}

	if (m_P4User_Prev.IsEmpty())
		m_P4User_Prev = _T("@");
	if (m_P4User_Prev != userIn)
	{
		CMainFrame * mainWnd = (CMainFrame *)AfxGetMainWnd();
		if (mainWnd)
			::PostMessage(mainWnd->m_hWnd, WM_NEWUSER, 0, 0);
		m_P4User_Prev = userIn;
		SetP4Password( _T(""), TRUE, FALSE, FALSE );
	}
	if (m_P4User_PrevDef.IsEmpty())
		m_P4User_PrevDef = _T("@");
	if (writetoreg && m_P4User_PrevDef != userIn)
	{
		CMainFrame * mainWnd = (CMainFrame *)AfxGetMainWnd();
		if (mainWnd)
			::PostMessage(mainWnd->m_hWnd, WM_NEWUSER, 1, 0);
		m_P4User_PrevDef = userIn;
	}

	SetMyID();
	return success;
}

BOOL CP4Registry::SetP4Client(LPCTSTR client, BOOL temporary, BOOL permanent, BOOL writetoreg)
{
	// Use this to keep a copy of the input
	// since it might get reset by m_P4Client_Temp.Empty();
	CString clientIn = client;
    BOOL success = TRUE;

	if( permanent )
	{
		if (writetoreg)
		{
			if (!m_P4Client_Perm.IsEmpty())
				m_P4Client_PrevDef = m_P4Client_Perm;
			// save the old value in the registry in case of a failure
			// due to having a P4CONFIG or SET value that overrides
			HKEY	hKey = NULL;
			CString sKey = _T("Software\\Perforce\\environment\\");
			DWORD   type = REG_SZ;
			DWORD	disposition;
			TCHAR   saveRegClient[260];
			LONG	rc;
			if ( (rc = RegCreateKeyEx( HKEY_CURRENT_USER, sKey,
										0, NULL,
										REG_OPTION_NON_VOLATILE,
										KEY_READ|KEY_WRITE, NULL,
										&hKey, &disposition )) == ERROR_SUCCESS )
			{
				disposition = sizeof(saveRegClient);
				if ((rc = RegQueryValueEx( hKey, _T("P4CLIENT"), NULL, &type,
						(LPBYTE)saveRegClient, &disposition)) != ERROR_SUCCESS )
					saveRegClient[0] = _T('\0');
			}
			if (rc != ERROR_SUCCESS )
				RegWriteFailed(rc);

			// use the api to write it to the registry
            success = CGuiClient::DefineClient(clientIn);
			if (success)
			{
				// make sure the value written is now the value that will be used
				CString defClient = CGuiClient::GetClientStr();
				if (clientIn != defClient)
				{
					// If we get here, there is a P4CONFIG
					//  or a Enviroment setting overriding.
					// So set this 'real' value as the permanent value,
					// then because of possible side effects,
					// set the value passed in as the temporary value.
					SetP4Client(defClient, temporary, TRUE, FALSE);
					if (temporary)
						SetP4Client(clientIn, TRUE, FALSE, FALSE);
					// Now because we messed up the registry with the call
					// CGuiClient::DefineClient(clientIn), we have to restore
					// the original registry value from above (saveRegClient).
					RegSetValueEx(hKey, _T("P4CLIENT"), NULL,
								REG_SZ, (LPBYTE)saveRegClient, 
								lstrlen(saveRegClient)*sizeof(TCHAR)+1);
					RegCloseKey( hKey );
					// Finally indicate failure.
					return FALSE;
				}
			}
			RegCloseKey( hKey );
		}

		// If the permanent value changed and we do not as-yet have an over-ride,
		// turn the old value into a temporary over-ride.  This avoids problems
		// if a user is tinkering with the registry while the app is running
		if( !temporary && !m_P4Client_Perm.IsEmpty() && m_P4Client_Temp.IsEmpty())
			m_P4Client_Temp= m_P4Client_Perm;
		else if( temporary || ((m_P4Client_Temp.Compare(clientIn) == 0)
							&& (m_P4Client_Perm.Compare(clientIn) == 0)) )
			m_P4Client_Temp.Empty();

		if (writetoreg || m_P4Client_Perm.IsEmpty())
			m_P4Client_Perm = clientIn;
	}
	else
	{
		// Only store m_P4Client_Temp if it is different than the permanent registry
		// setting for P4CLIENT
		if( m_P4Client_Perm.Compare( clientIn ) == 0 )
			m_P4Client_Temp.Empty();
		else
			m_P4Client_Temp= clientIn;
	}
	
	if (m_P4Client_Prev.IsEmpty())
		m_P4Client_Prev = _T("@");
	if (m_P4Client_Prev != clientIn)
	{
		CMainFrame * mainWnd = (CMainFrame *)AfxGetMainWnd();
		if (mainWnd)
			::PostMessage(mainWnd->m_hWnd, WM_NEWCLIENT, 0, 0);
		m_P4Client_Prev = clientIn;
	}
	if (m_P4Client_PrevDef.IsEmpty())
		m_P4Client_PrevDef = _T("@");
	if (writetoreg && m_P4Client_PrevDef != clientIn)
	{
		CMainFrame * mainWnd = (CMainFrame *)AfxGetMainWnd();
		if (mainWnd)
			::PostMessage(mainWnd->m_hWnd, WM_NEWCLIENT, 1, 0);
		m_P4Client_PrevDef = clientIn;
	}
	
	SetMyID();
    return success;
}


BOOL CP4Registry::SetP4Password(LPCTSTR password, BOOL temporary, BOOL permanent, BOOL writetoreg)
{
    bool success = true;

	if(permanent)
	{
		// Use this to keep a copy of the input
		// since it might get reset by m_P4Password_Temp.Empty();
		CString passwordIn = password;

		if (writetoreg)
            success = CGuiClient::DefinePassword(password);

		// If the permanent value changed and we do not as-yet have an over-ride,
		// turn the old value into a temporary over-ride.  This avoids problems
		// if a user is tinkering with the registry while the app is running
		if( !temporary && !m_P4Password_Perm.IsEmpty() && m_P4Password_Temp.IsEmpty())
			m_P4Password_Temp= m_P4Password_Perm;
		else if( temporary || ((m_P4Password_Temp.Compare(password) == 0)
							&& (m_P4Password_Perm.Compare(password) == 0)) )
			m_P4Password_Temp.Empty();

		if (writetoreg || m_P4Password_Perm.IsEmpty())
			m_P4Password_Perm = passwordIn;
	}
	else
	{
		// Only store m_P4Password_Temp if it is different than the permanent registry
		// setting for P4PASSWORD
		if( m_P4Password_Perm.Compare( password ) == 0 )
			m_P4Password_Temp.Empty();
		else
			m_P4Password_Temp= password;
	}

	return success;
}


BOOL CP4Registry::SetP4Charset(LPCTSTR charset, BOOL temporary, BOOL permanent, BOOL writetoreg)
{
    bool success = true;

	m_bCharsetFromCmdli = FALSE;

	if(permanent)
	{
		if (writetoreg)
            success = CGuiClient::DefineCharset(charset);

		if (!*charset)
			charset = NONE;

		// Use this to keep a copy of the input
		// since it might get reset by m_P4Charset_Temp.Empty();
		CString charsetIn = charset;

		// If the permanent value changed and we do not as-yet have an over-ride,
		// turn the old value into a temporary over-ride.  This avoids problems
		// if a user is tinkering with the registry while the app is running
		if( !temporary && !m_P4Charset_Perm.IsEmpty() && m_P4Charset_Temp.IsEmpty())
			m_P4Charset_Temp= m_P4Charset_Perm;
		else if( temporary || ((m_P4Charset_Temp.Compare(charset) == 0)
							&& (m_P4Charset_Perm.Compare(charset) == 0)) )
			m_P4Charset_Temp.Empty();

		if (writetoreg || m_P4Charset_Perm.IsEmpty())
			m_P4Charset_Perm = charsetIn;
	}
	else
	{
		if (!*charset)
			charset = NONE;
		// Only store m_P4Charset_Temp if it is different than the permanent registry
		// setting for P4CHARSET
		if( m_P4Charset_Perm.Compare( charset ) == 0 )
			m_P4Charset_Temp.Empty();
		else
			m_P4Charset_Temp= charset;
	}

	AddMRUPort( GetP4Port(), charset );

	return success;
}


void CP4Registry::SetMyID() 
{ 
    if(m_P4User_Temp.GetLength())
        m_MyID= m_P4User_Temp + _T("@"); 
    else
        m_MyID= m_P4User_Perm + _T("@"); 

    if(m_P4Client_Temp.GetLength())
        m_MyID+= m_P4Client_Temp; 
    else
        m_MyID+= m_P4Client_Perm; 
}

void CP4Registry::SetHostname(CString& hostname)
{
	m_Hostname = hostname;
	if (m_NoCliHost && m_ClientFilterHost.IsEmpty())
	{
		SetClientFilterHost(hostname);
		m_NoCliHost = FALSE;
	}
}


///////////////////////////////////////////////////////////////
// Options Keys
///////////////////////////////////////////////////////////////

BOOL CP4Registry::SetEditApp(LPCTSTR appPath)
{
	m_EditApp=appPath;
	return SetRegKey( m_EditApp, _T("Options"), _T("EditApp") );
}

BOOL CP4Registry::SetTabWidth(int tabWidth)
{
	CString str;
	str.Format(_T("%d"), tabWidth);
	m_TabWidth=tabWidth;
	return SetRegKey( str, _T("Options"), _T("TabWidth") );
}

BOOL CP4Registry::SetWhtSpFlag(int whtSpFlag)
{
	CString str;
	str.Format(_T("%d"), whtSpFlag);
	m_WhtSpFlag=whtSpFlag;
	return SetRegKey( str, _T("Options"), _T("WhtSpFlag") );
}

BOOL CP4Registry::SetDiffApp(LPCTSTR appPath)
{
	m_DiffApp=appPath;
	return SetRegKey( m_DiffApp, _T("Options"), _T("DiffApp") );
}

BOOL CP4Registry::SetDiffOptArgs(LPCTSTR optAprg)
{
	m_DiffOptArgs=optAprg;
	return SetRegKey( m_DiffOptArgs, _T("Options"), _T("DiffOptArgs") );
}

BOOL CP4Registry::SetMrgTabWidth(int mrgTabWidth)
{
	CString str;
	str.Format(_T("%d"), mrgTabWidth);
	m_MrgTabWidth=mrgTabWidth;
	return SetRegKey( str, _T("Options"), _T("MrgTabWidth") );
}

BOOL CP4Registry::SetMrgWhtSpFlag(int whtSpFlag)
{
	CString str;
	str.Format(_T("%d"), whtSpFlag);
	m_MrgWhtSpFlag=whtSpFlag;
	return SetRegKey( str, _T("Options"), _T("MrgWhtSpFlag") );
}

BOOL CP4Registry::SetMergeApp(LPCTSTR appPath)
{
	m_MergeApp=appPath;
	return SetRegKey( m_MergeApp, _T("Options"), _T("MergeApp") );
}

BOOL CP4Registry::SetMergeOptArgs(LPCTSTR optAprg)
{
	m_MergeOptArgs=optAprg;
	return SetRegKey( m_MergeOptArgs, _T("Options"), _T("MergeOptArgs") );
}

BOOL CP4Registry::SetAnnotateWhtSpace(int whtSpFlag)
{
	CString str;
	str.Format(_T("%d"), whtSpFlag);
	m_AnnotateWhtSpace=whtSpFlag;
	return SetRegKey( str, _T("Options"), _T("AnnotateWhtSpace") );
}

BOOL CP4Registry::SetAnnotateIncInteg(int incIntegFlag)
{
	CString str;
	str.Format(_T("%d"), incIntegFlag);
	m_AnnotateIncInteg=incIntegFlag;
	return SetRegKey( str, _T("Options"), _T("AnnotateIncInteg") );
}

BOOL CP4Registry::SetTLVWhtSpace(int whtSpFlag)
{
	CString str;
	str.Format(_T("%d"), whtSpFlag);
	m_TLVWhtSpace=whtSpFlag;
	return SetRegKey( str, _T("Options"), _T("TLVWhtSpace") );
}

BOOL CP4Registry::SetResolveWhtSp(int whtSpFlag)
{
	CString str;
	str.Format(_T("%d"), whtSpFlag);
	m_ResolveWhtSp=whtSpFlag;
	return SetRegKey( str, _T("Options"), _T("ResolveWhtSp") );
}

BOOL CP4Registry::SetTLVIncInteg(int incIntegFlag)
{
	CString str;
	str.Format(_T("%d"), incIntegFlag);
	m_TLVIncInteg=incIntegFlag;
	return SetRegKey( str, _T("Options"), _T("TLVIncInteg") );
}

BOOL CP4Registry::SetUseTempForView(int useTempForView)
{
	CString str;
	str.Format(_T("%ld"), (long) useTempForView);
	m_UseTempForView=useTempForView;
	return SetRegKey( str, _T("Options"), UseTempForView );
}

BOOL CP4Registry::SetUseTempForExts(LPCTSTR useTempForExts)
{
	m_UseTempForExts=useTempForExts;
	return SetRegKey( m_UseTempForExts, _T("Options"), UseTempForExts );
}

BOOL CP4Registry::SetEditAppIsConsole(BOOL isConsole)
{
	CString str;
	str.Format(_T("%ld"), (long) isConsole);
	m_EditAppIsConsole=isConsole;
	return SetRegKey( str, _T("Options"), EditAppIsConsole );
}

BOOL CP4Registry::SetDiffAppIsConsole(BOOL isConsole)
{
	CString str;
	str.Format(_T("%ld"), (long) isConsole);
	m_DiffAppIsConsole=isConsole;
	return SetRegKey( str, _T("Options"), DiffAppIsConsole );
}

BOOL CP4Registry::SetDiffAppIsClose(BOOL isClose)
{
	CString str;
	str.Format(_T("%ld"), (long) isClose);
	m_DiffAppIsClose=isClose;
	return SetRegKey( str, _T("Options"), DiffAppIsClose );
}

BOOL CP4Registry::SetDiffAppIsBinary(BOOL isBinary)
{
	CString str;
	str.Format(_T("%ld"), (long) isBinary);
	m_DiffAppIsBinary=isBinary;
	return SetRegKey( str, _T("Options"), DiffAppIsBinary );
}

int CP4Registry::SetDiffInternal(BOOL isInternal)
{
	CString str;
	str.Format(_T("%ld"), (long) isInternal);
	m_DiffInternal=isInternal;
	return SetRegKey( str, _T("Options"), DiffInternal );
}

BOOL CP4Registry::SetDiffOptArgChk(BOOL isOptArg)
{
	CString str;
	str.Format(_T("%ld"), (long) isOptArg);
	m_DiffOptArgChk=isOptArg;
	return SetRegKey( str, _T("Options"), DiffOptArgChk );
}

int CP4Registry::SetMergeInternal(int isInternal)
{
	CString str;
	str.Format(_T("%ld"), (long) isInternal);
	m_MergeInternal=isInternal;
	return SetRegKey( str, _T("Options"), MergeInternal );
}

BOOL CP4Registry::SetMergeOptArgChk(BOOL isOptArg)
{
	CString str;
	str.Format(_T("%ld"), (long) isOptArg);
	m_MergeOptArgChk=isOptArg;
	return SetRegKey( str, _T("Options"), MergeOptArgChk );
}

BOOL CP4Registry::SetMergeNSF(BOOL bNSF)
{
	CString str;
	str.Format(_T("%ld"), (long) bNSF);
	m_MergeNSF=bNSF;
	return SetRegKey( str, _T("Options"), MergeNSF );
}

BOOL CP4Registry::SetMergeAppIsConsole(BOOL isConsole)
{
	CString str;
	str.Format(_T("%ld"), (long) isConsole);
	m_MergeAppIsConsole=isConsole;
	return SetRegKey( str, _T("Options"), MergeAppIsConsole );
}

BOOL CP4Registry::SetMergeAppIsClose(BOOL isClose)
{
	CString str;
	str.Format(_T("%ld"), (long) isClose);
	m_MergeAppIsClose=isClose;
	return SetRegKey( str, _T("Options"), MergeAppIsClose );
}

BOOL CP4Registry::SetAlwaysShowFocus(BOOL alwaysShowFocus)
{
	CString str;
	str.Format(_T("%ld"), (long) alwaysShowFocus);
	m_AlwaysShowFocus=alwaysShowFocus;
	return SetRegKey( str, _T("Options"), AlwaysShowFocus );
}

BOOL CP4Registry::SetAlwaysWarnOnRevert(BOOL alwaysWarnOnRevert)
{
	CString str;
	str.Format(_T("%ld"), (long) alwaysWarnOnRevert);
	m_AlwaysWarnOnRevert=alwaysWarnOnRevert;
	return SetRegKey( str, _T("Options"), AlwaysWarnOnRevert );
}

BOOL CP4Registry::SetAllowPromptAbove(BOOL allowPromptAbove)
{
	CString str;
	str.Format(_T("%ld"), (long) allowPromptAbove);
	m_AllowPromptAbove=allowPromptAbove;
	return SetRegKey( str, _T("Options"), AllowPromptAbove );
}

BOOL CP4Registry::SetAutoMinEditDlg(BOOL autoMinEditDlg)
{
	CString str;
	str.Format(_T("%ld"), (long) autoMinEditDlg);
	m_AutoMinEditDlg=autoMinEditDlg;
	return SetRegKey( str, _T("Options"), AutoMinEditDlg );
}

BOOL CP4Registry::SetDoubleClickOption(int dblClick)
{
	CString str;
	str.Format(_T("%ld"), (long) dblClick);
	m_DoubleClickOption=dblClick;
	return SetRegKey( str, _T("Options"), DoubleClickOption );
}

BOOL CP4Registry::SetOptionStartUpOption(int startupoption)
{
	CString str;
	str.Format(_T("%ld"), (long) startupoption);
	m_OptionStartUpOption=startupoption;
	return SetRegKey( str, _T("Options"), OptionStartUpOption );
}

BOOL CP4Registry::SetOptionStartUpPage(int startuppage)
{
	CString str;
	str.Format(_T("%ld"), (long) startuppage);
	m_OptionStartUpPage=startuppage;
	return SetRegKey( str, _T("Options"), OptionStartUpPage );
}

BOOL CP4Registry::SetStatusUpdateInterval(int statinterval)
{
	CString str;
	str.Format(_T("%ld"), (long) statinterval);
	m_StatusUpdateInterval=statinterval;
	return SetRegKey( str, _T("Options"), StatusUpdateInterval );
}

BOOL CP4Registry::SetLabelAddRplDefault(int defaultNbr)
{
	CString str;
	str.Format(_T("%ld"), (long) defaultNbr);
	m_LabelAddRplDefault=defaultNbr;
	return SetRegKey( str, _T("Options"), LabelAddRplDefault );
}

BOOL CP4Registry::SetDiff2Default1(int defaultNbr)
{
	CString str;
	str.Format(_T("%ld"), (long) defaultNbr);
	m_Diff2Default1=defaultNbr;
	return SetRegKey( str, _T("Options"), Diff2Default1 );
}

BOOL CP4Registry::SetDiff2Default2(int defaultNbr)
{
	CString str;
	str.Format(_T("%ld"), (long) defaultNbr);
	m_Diff2Default2=defaultNbr;
	return SetRegKey( str, _T("Options"), Diff2Default2 );
}

BOOL CP4Registry::SetTempDir(LPCTSTR tempdir)
{
	CString temp(tempdir);
	// Get rid of trailing slash
	if(temp.GetLength() > 1 && temp[temp.GetLength()-1] == _T('\\'))
		temp= temp.Left(temp.GetLength()-1);

	m_TempDir=temp;
	return SetRegKey( m_TempDir, _T("Options"), TempFiles );
}

BOOL CP4Registry::SetExplorer(int explorersw)
{
	CString str;
	str.Format(_T("%ld"), (long) explorersw);
	m_Explorer=explorersw;
	return SetRegKey( str, _T("Options"), Explorer );
}

BOOL CP4Registry::Set2Panes(BOOL b2Panes)
{
	CString str;
	str.Format(_T("%ld"), (long) b2Panes);
	m_2Panes=b2Panes;
	return SetRegKey( str, _T("Options"), Use2Panes );
}

BOOL CP4Registry::SetAltExpl(LPCTSTR explorerstring)
{
	CString temp(explorerstring);
	// Get rid of trailing slash
	if(temp.GetLength() > 1 && temp[temp.GetLength()-1] == _T('\\'))
		temp= temp.Left(temp.GetLength()-1);

	m_AltExpl=temp;
	return SetRegKey( m_AltExpl, _T("Options"), AltExplorer );
}

BOOL CP4Registry::SetIsNoCopy(BOOL isNoCopy)
{
	CString str;
	str.Format(_T("%ld"), (long) isNoCopy);
	m_IsNoCopy=isNoCopy;
	return SetRegKey( str, _T("Options"), IsNoCopy );
}

BOOL CP4Registry::SetIsForceInteg(BOOL isForceInteg)
{
	CString str;
	str.Format(_T("%ld"), (long) isForceInteg);
	m_IsForceInteg=isForceInteg;
	return SetRegKey( str, _T("Options"), IsForceInteg );
}

BOOL CP4Registry::SetIsForceDirect(BOOL isForceDirect)
{
	CString str;
	str.Format(_T("%ld"), (long) isForceDirect);
	m_IsForceDirect=isForceDirect;
	return SetRegKey( str, _T("Options"), IsForceDirect );
}

BOOL CP4Registry::SetIsDeleteSource(BOOL isDeleteSource)
{
	CString str;
	str.Format(_T("%ld"), (long) isDeleteSource);
	m_IsDeleteSource=isDeleteSource;
	return SetRegKey( str, _T("Options"), IsDeleteSource );
}

BOOL CP4Registry::SetIsPermitDelReadd(BOOL isPermitDelReadd)
{
	CString str;
	str.Format(_T("%ld"), (long) isPermitDelReadd);
	m_IsPermitDelReadd=isPermitDelReadd;
	return SetRegKey( str, _T("Options"), IsPermitDelReadd );
}

BOOL CP4Registry::SetDelReaddType(int delReaddType)
{
	CString str;
	str.Format(_T("%ld"), (long) delReaddType);
	m_DelReaddType=delReaddType;
	return SetRegKey( str, _T("Options"), DelReaddType );
}

BOOL CP4Registry::SetIsPropagateTypes(BOOL isPropagateTypes)
{
	CString str;
	str.Format(_T("%ld"), (long) isPropagateTypes);
	m_IsPropagateTypes=isPropagateTypes;
	return SetRegKey( str, _T("Options"), IsPropagateTypes );
}

BOOL CP4Registry::SetIsBaseViaDelReadd(BOOL isBaseViaDelReadd)
{
	CString str;
	str.Format(_T("%ld"), (long) isBaseViaDelReadd);
	m_IsBaseViaDelReadd=isBaseViaDelReadd;
	return SetRegKey( str, _T("Options"), IsBaseViaDelReadd );
}

BOOL CP4Registry::SetUseNewChglist(BOOL useNewChglist)
{
	CString str;
	str.Format(_T("%ld"), (long) useNewChglist);
	m_UseNewChglist=useNewChglist;
	return SetRegKey( str, _T("Options"), UseNewChglist );
}

BOOL CP4Registry::SetUseNewChglist2(BOOL useNewChglist2)
{
	CString str;
	str.Format(_T("%ld"), (long) useNewChglist2);
	m_UseNewChglist2=useNewChglist2;
	return SetRegKey( str, _T("Options"), UseNewChglist2 );
}

BOOL CP4Registry::SetIsBaselessMerge(BOOL isBaselessMerge)
{
	CString str;
	str.Format(_T("%ld"), (long) isBaselessMerge);
	m_IsBaselessMerge=isBaselessMerge;
	return SetRegKey( str, _T("Options"), IsBaselessMerge );
}

BOOL CP4Registry::SetIsIndirectMerge(BOOL isIndirectMerge)
{
	CString str;
	str.Format(_T("%ld"), (long) isIndirectMerge);
	m_IsIndirectMerge=isIndirectMerge;
	return SetRegKey( str, _T("Options"), IsIndirectMerge );
}

BOOL CP4Registry::SetSyncFirstDefault(BOOL syncFirstDefault)
{
	CString str;
	str.Format(_T("%ld"), (long) syncFirstDefault);
	m_SyncFirstDefault=syncFirstDefault;
	return SetRegKey( str, _T("Options"), SyncFirstDefault );
}

BOOL CP4Registry::SetWarnAlreadyOpened(BOOL warnAlreadyOpened)
{
	CString str;
	str.Format(_T("%ld"), (long) warnAlreadyOpened);
	m_WarnAlreadyOpened=warnAlreadyOpened;
	return SetRegKey( str, _T("Options"), WarnAlreadyOpened );
}

BOOL CP4Registry::SetExpandFlag(BOOL expandFlag)
{
	CString str;
	str.Format(_T("%ld"), (long) expandFlag);
	m_ExpandFlag=expandFlag;
	return SetRegKey( str, _T("Options"), ExpandFlag );
}

BOOL CP4Registry::SetExpandPath(LPCTSTR expandPath)
{
	CString str(expandPath);
	m_ExpandPath=str;
	return SetRegKey( str, _T("Options"), ExpandPath );
}

BOOL CP4Registry::SetShowDeleted(BOOL showDeleted)
{
	CString str;
	str.Format(_T("%ld"), (long) showDeleted);
	m_ShowDeleted=showDeleted;
	return SetRegKey( str, _T("Options"), ShowDeleted );
}

BOOL CP4Registry::SetSortByExtension(BOOL sortByExtension)
{
	CString str;
	str.Format(_T("%ld"), (long) sortByExtension);
	m_SortByExtension=sortByExtension;
	return SetRegKey( str, _T("Options"), SortByExtension );
}

BOOL CP4Registry::SetSortChgFilesByAction(BOOL sortChgFilesByAction)
{
	CString str;
	str.Format(_T("%ld"), (long) sortChgFilesByAction);
	m_SortChgFilesByAction=sortChgFilesByAction;
	return SetRegKey( str, _T("Options"), SortChgFilesByAction );
}

BOOL CP4Registry::SetSortChgFilesByName(BOOL sortChgFilesByName)
{
	CString str;
	str.Format(_T("%ld"), (long) sortChgFilesByName);
	m_SortChgFilesByName=sortChgFilesByName;
	return SetRegKey( str, _T("Options"), SortChgFilesByName );
}

BOOL CP4Registry::SetSortChgFilesByExt(BOOL sortChgFilesByExt)
{
	CString str;
	str.Format(_T("%ld"), (long) sortChgFilesByExt);
	m_SortChgFilesByExt=sortChgFilesByExt;
	return SetRegKey( str, _T("Options"), SortChgFilesByExt );
}

BOOL CP4Registry::SetSortChgFilesByResolve(BOOL sortChgFilesByResolve)
{
	CString str;
	str.Format(_T("%ld"), (long) sortChgFilesByResolve);
	m_SortChgFilesByResolve=sortChgFilesByResolve;
	return SetRegKey( str, _T("Options"), SortChgFilesByResolve );
}

BOOL CP4Registry::SetWarnLimit(LPCTSTR warnLimit)
{
	CString temp(warnLimit);
	m_WarnLimit=temp;
	return SetRegKey( m_WarnLimit, _T("Options"), WarnLimit );
}

BOOL CP4Registry::SetWarnLimitDiff(LPCTSTR warnLimitDiff)
{
	CString temp(warnLimitDiff);
	m_WarnLimitDiff=temp;
	return SetRegKey( m_WarnLimitDiff, _T("Options"), WarnLimitDiff );
}

BOOL CP4Registry::SetWarnLimitOpen(LPCTSTR warnLimitOpen)
{
	CString temp(warnLimitOpen);
	m_WarnLimitOpen=temp;
	return SetRegKey( m_WarnLimitOpen, _T("Options"), WarnLimitOpen );
}

BOOL CP4Registry::SetExtSortMax(LPCTSTR extSortMax)
{
	CString temp(extSortMax);
	m_ExtSortMax=temp;
	return SetRegKey( m_ExtSortMax, _T("Options"), ExtSortMax );
}

BOOL CP4Registry::SetMinMultiLineSize(LPCTSTR minMultiLineSize)
{
	CString temp(minMultiLineSize);
	m_MinMultiLineSize=temp;
	return SetRegKey( m_MinMultiLineSize, _T("Options"), MinMultiLineSize );
}

BOOL CP4Registry::SetUserFilter(LPCTSTR userFilter)
{
	CString temp(userFilter);
	m_UserFilter=temp;
	return SetRegKey( m_UserFilter, _T("Options"), UserFilter );
}

BOOL CP4Registry::SetFilteredByUser(int filteredByUser)
{
	CString str;
	str.Format(_T("%ld"), (long) filteredByUser);
	m_FilteredByUser=filteredByUser;
	return SetRegKey( str, _T("Options"), FilteredByUser );
}

BOOL CP4Registry::SetClientFilter(LPCTSTR clientFilter)
{
	CString temp(clientFilter);
	m_ClientFilter=temp;
	return SetRegKey( m_ClientFilter, _T("Options"), ClientFilter );
}

BOOL CP4Registry::SetFilteredByClient(int filteredByClient)
{
	CString str;
	str.Format(_T("%ld"), (long) filteredByClient);
	m_FilteredByClient=filteredByClient;
	return SetRegKey( str, _T("Options"), FilteredByClient );
}

BOOL CP4Registry::SetLabelFilterByOwnerFlag(int labelFilterByOwnerFlag)
{
	CString str;
	str.Format(_T("%ld"), (long) labelFilterByOwnerFlag);
	m_LabelFilterByOwnerFlag=labelFilterByOwnerFlag;
	return SetRegKey( str, _T("Options"), LabelFilterByOwnerFlag );
}

BOOL CP4Registry::SetLabelFilterByOwner(LPCTSTR labelFilterByOwner)
{
	CString temp(labelFilterByOwner);
	m_LabelFilterByOwner=temp;
	return SetRegKey( m_LabelFilterByOwner, _T("Options"), LabelFilterByOwner );
}

BOOL CP4Registry::SetLabelFilterIncBlank(int labelFilterIncBlank)
{
	CString str;
	str.Format(_T("%ld"), (long) labelFilterIncBlank);
	m_LabelFilterIncBlank=labelFilterIncBlank;
	return SetRegKey( str, _T("Options"), LabelFilterIncBlank );
}

BOOL CP4Registry::SetUseOpenForEditExts(LPCTSTR useOpenForEditExts)
{
	CString temp(useOpenForEditExts);
	m_UseOpenForEditExts=temp;
	return SetRegKey( m_UseOpenForEditExts, _T("Options"), UseOpenForEditExts );
}

BOOL CP4Registry::SetUseOpenForEdit(int useOpenForEdit)
{
	CString str;
	str.Format(_T("%ld"), (long) useOpenForEdit);
	m_UseOpenForEdit=useOpenForEdit;
	return SetRegKey( str, _T("Options"), UseOpenForEdit );
}

BOOL CP4Registry::SetUseShortRevHistDesc(int useShortRevHistDesc)
{
	CString str;
	str.Format(_T("%ld"), (long) useShortRevHistDesc);
	m_UseShortRevHistDesc=useShortRevHistDesc;
	return SetRegKey( str, _T("Options"), UseShortRevHistDesc );
}

BOOL CP4Registry::SetUseShortSubmittedDesc(int useShortSubmittedDesc)
{
	CString str;
	str.Format(_T("%ld"), (long) useShortSubmittedDesc);
	m_UseShortSubmittedDesc=useShortSubmittedDesc;
	return SetRegKey( str, _T("Options"), UseShortSubmittedDesc );
}

BOOL CP4Registry::SetUseLongChglistDesc(int useLongChglistDesc)
{
	CString str;
	str.Format(_T("%ld"), (long) useLongChglistDesc);
	m_UseLongChglistDesc=useLongChglistDesc;
	return SetRegKey( str, _T("Options"), UseLongChglistDesc );
}

BOOL CP4Registry::SetBusyWaitTime(int busyWaitTime)
{
	CString str;
	str.Format(_T("%ld"), (long) busyWaitTime);
	m_BusyWaitTime=busyWaitTime;
	return SetRegKey( str, _T("Options"), BusyWaitTime );
}

BOOL CP4Registry::SetSyncDlgFlag(int syncDlgFlag)
{
	CString str;
	str.Format(_T("%ld"), (long) syncDlgFlag);
	m_SyncDlgFlag=syncDlgFlag;
	return SetRegKey( str, _T("Options"), SyncDlgFlag );
}

BOOL CP4Registry::SetDescWrap(int descWrap)
{
	CString str;
	str.Format(_T("%ld"), (long) descWrap);
	m_DescWrap=descWrap;
	return SetRegKey( str, _T("Options"), DescWrap );
}

BOOL CP4Registry::SetDescWrapSw(BOOL descWrapSw)
{
	CString str;
	str.Format(_T("%ld"), (long) descWrapSw);
	m_DescWrapSw=descWrapSw;
	return SetRegKey( str, _T("Options"), DescWrapSw );
}

BOOL CP4Registry::SetShowToolBar(BOOL showToolBar)
{
	CString str;
	str.Format(_T("%ld"), (long) showToolBar);
	m_ShowToolBar=showToolBar;
	return SetRegKey( str, _T("Options"), ShowToolBar );
}

BOOL CP4Registry::SetShowEntireDepot(int showEntire)
{
	CString str;
	str.Format(_T("%ld"), (long) showEntire);
	m_ShowEntireDepot=showEntire;
	return SetRegKey( str, _T("Options"), ShowEntireDepot );
}

BOOL CP4Registry::SetShowFileType(BOOL showFileType)
{
	CString str;
	str.Format(_T("%ld"), (long) showFileType);
	m_ShowFileType=showFileType;
	return SetRegKey( str, _T("Options"), ShowFileTypes );
}

BOOL CP4Registry::SetShowOpenAction(BOOL showOpenAction)
{
	CString str;
	str.Format(_T("%ld"), (long) showOpenAction);
	m_ShowOpenAction=showOpenAction;
	return SetRegKey( str, _T("Options"), ShowOpenActions );
}

BOOL CP4Registry::SetShowChangeDesc(BOOL showChangeDesc)
{
	CString str;
	str.Format(_T("%ld"), (long) showChangeDesc);
	m_ShowChangeDesc=showChangeDesc;
	return SetRegKey( str, _T("Options"), ShowChangeDescs );
}

BOOL CP4Registry::SetSortChgsByUser(BOOL sortChgsByUser)
{
	CString str;
	str.Format(_T("%ld"), (long) sortChgsByUser);
	m_SortChgsByUser=sortChgsByUser;
	return SetRegKey( str, _T("Options"), SortChgsByUser );
}

BOOL CP4Registry::SetShowCommandTrace( BOOL show )
{
	CString str;
	str.Format(_T("%ld"), (long) show );
	m_ShowCommandTrace = show;
	return SetRegKey( str, _T("Options"), ShowCommandTrace );
}

BOOL CP4Registry::SetShowConnectSettings(BOOL showConnectSettings)
{
	CString str;
	str.Format(_T("%ld"), (long) showConnectSettings);
	m_ShowConnectSettings=showConnectSettings;
	return SetRegKey( str, _T("Options"), ShowConnectSettings );
}

BOOL CP4Registry::SetShowConnectPort1st(BOOL showConnectPort1st)
{
	CString str;
	str.Format(_T("%ld"), (long) showConnectPort1st);
	m_ShowConnectPort1st=showConnectPort1st;
	return SetRegKey( str, _T("Options"), ShowConnectPort1st );
}

BOOL CP4Registry::SetShowClientPath4Chgs(BOOL showClientPath4Chgs)
{
	CString str;
	str.Format(_T("%ld"), (long) showClientPath4Chgs);
	m_ShowClientPath4Chgs= showClientPath4Chgs;
	return SetRegKey( str, _T("Options"), ShowClientPath4Chgs );
}

BOOL CP4Registry::SetShowClientPath(BOOL showClientPath)
{
	CString str;
	str.Format(_T("%ld"), (long) showClientPath);
	m_ShowClientPath= showClientPath;
	return SetRegKey( str, _T("Options"), ShowClientPath );
}

BOOL CP4Registry::SetShowDepotPathHiLite(BOOL showDepotPathHiLite)
{
	CString str;
	str.Format(_T("%ld"), (long) showDepotPathHiLite);
	m_ShowDepotPathHiLite= showDepotPathHiLite;
	return SetRegKey( str, _T("Options"), ShowDepotPathHiLite);
}

BOOL CP4Registry::SetShowStatusTime(BOOL showStatusTime)
{
	CString str;
	str.Format(_T("%ld"), (long) showStatusTime);
	m_ShowStatusTime= showStatusTime;
	return SetRegKey( str, _T("Options"), ShowStatusTime);
}

BOOL CP4Registry::SetShowHiddenFilesNotInDepot(BOOL showHiddenFilesNotInDepot)
{
	CString str;
	str.Format(_T("%ld"), (long) showHiddenFilesNotInDepot);
	m_ShowHiddenFilesNotInDepot= showHiddenFilesNotInDepot;
	return SetRegKey( str, _T("Options"), ShowHiddenFilesNotInDepot);
}

BOOL CP4Registry::SetUse24hourClock(BOOL use24hourClock)
{
	CString str;
	str.Format(_T("%ld"), (long) use24hourClock);
	m_Use24hourClock= use24hourClock;
	return SetRegKey( str, _T("Options"), Use24hourClock);
}

BOOL CP4Registry::SetPreserveSpecFormat(BOOL preserveSpecFormat)
{
	CString str;
	str.Format(_T("%ld"), (long) preserveSpecFormat);
	m_PreserveSpecFormat= preserveSpecFormat;
	return SetRegKey( str, _T("Options"), PreserveSpecFormat);
}

BOOL CP4Registry::SetAutoExpandOptions(BOOL autoExpandOptions)
{
	CString str;
	str.Format(_T("%ld"), (long) autoExpandOptions);
	m_AutoExpandOptions= autoExpandOptions;
	return SetRegKey( str, _T("Options"), AutoExpandOptions);
}

BOOL CP4Registry::SetLabelFilesInDialog(BOOL labelFilesInDialog)
{
	CString str;
	str.Format(_T("%ld"), (long) labelFilesInDialog);
	m_LabelFilesInDialog= labelFilesInDialog;
	return SetRegKey( str, _T("Options"), LabelFilesInDialog);
}

BOOL CP4Registry::SetLabelShowPreviewDetail(BOOL labelShowPreviewDetail)
{
	CString str;
	str.Format(_T("%ld"), (long) labelShowPreviewDetail);
	m_LabelShowPreviewDetail= labelShowPreviewDetail;
	return SetRegKey( str, _T("Options"), LabelShowPreviewDetail);
}

BOOL CP4Registry::SetDiff2InDialog(BOOL diff2InDialog)
{
	CString str;
	str.Format(_T("%ld"), (long) diff2InDialog);
	m_Diff2InDialog= diff2InDialog;
	return SetRegKey( str, _T("Options"), Diff2InDialog);
}

BOOL CP4Registry::SetAutoTreeExpand(BOOL autoTreeExpand)
{
	CString str;
	str.Format(_T("%ld"), (long) autoTreeExpand);
	m_AutoTreeExpand= autoTreeExpand;
	return SetRegKey( str, _T("Options"), AutoTreeExpand);
}

BOOL CP4Registry::SetUseNotepad4WarnAndErr(BOOL useNotepad4WarnAndErr)
{
	CString str;
	str.Format(_T("%ld"), (long) useNotepad4WarnAndErr);
	m_UseNotepad4WarnAndErr= useNotepad4WarnAndErr;
	return SetRegKey( str, _T("Options"), UseNotepad4WarnAndErr);
}

BOOL CP4Registry::SetCvt2ValComboToChkBx(BOOL cvt2ValComboToChkBx)
{
	CString str;
	str.Format(_T("%ld"), (long) cvt2ValComboToChkBx);
	m_Cvt2ValComboToChkBx= cvt2ValComboToChkBx;
	return SetRegKey( str, _T("Options"), Cvt2ValComboToChkBx);
}

BOOL CP4Registry::SetIconsInMenus(BOOL iconsInMenus)
{
	CString str;
	str.Format(_T("%ld"), (long) iconsInMenus);
	m_IconsInMenus= iconsInMenus;
	return SetRegKey( str, _T("Options"), IconsInMenus);
}

BOOL CP4Registry::SetExpandChgLists(BOOL expandChgLists)
{
	CString str;
	str.Format(_T("%ld"), (long) expandChgLists);
	m_ExpandChgLists= expandChgLists;
	return SetRegKey( str, _T("Options"), ExpandChgLists);
}

BOOL CP4Registry::SetReExpandChgs(BOOL reExpandChgs)
{
	CString str;
	str.Format(_T("%ld"), (long) reExpandChgs);
	m_ReExpandChgs= reExpandChgs;
	return SetRegKey( str, _T("Options"), ReExpandChgs);
}

BOOL CP4Registry::SetEnableSubChgIntegFilter(BOOL enableSubChgIntegFilter)
{
	CString str;
	str.Format(_T("%ld"), (long) enableSubChgIntegFilter);
	m_EnableSubChgIntegFilter= enableSubChgIntegFilter;
	return SetRegKey( str, _T("Options"), EnableSubChgIntegFilter);
}

BOOL CP4Registry::SetEnableRevHistShowIntegs(BOOL enableRevHistShowIntegs)
{
	CString str;
	str.Format(_T("%ld"), (long) enableRevHistShowIntegs);
	m_EnableRevHistShowIntegs= enableRevHistShowIntegs;
	return SetRegKey( str, _T("Options"), EnableRevHistShowIntegs);
}

BOOL CP4Registry::SetEnablePendingChgsOtherClients(BOOL enablePendingChgsOtherClients)
{
	CString str;
	str.Format(_T("%ld"), (long) enablePendingChgsOtherClients);
	m_EnablePendingChgsOtherClients= enablePendingChgsOtherClients;
	return SetRegKey( str, _T("Options"), EnablePendingChgsOtherClients);
}

BOOL CP4Registry::SetAddFilterExclude(long addFilterExclude)
{
	CString str;
	str.Format(_T("%ld"), addFilterExclude);
	m_AddFilterExclude= addFilterExclude;
	return SetRegKey( str, _T("Options"), AddFilterExclude);
}

BOOL CP4Registry::SetAutoPoll(BOOL autoPoll)
{
	CString str;
	str.Format(_T("%ld"), (long) autoPoll);
	m_AutoPoll=autoPoll;
	return SetRegKey( str, _T("Options"), AutoPollServer );
}

BOOL CP4Registry::SetAutoPollTime(long autoPollTime)
{
	ASSERT( autoPollTime >= 0);

	CString str;
	str.Format(_T("%ld"), (long) autoPollTime);
	m_AutoPollTime=autoPollTime;
	return SetRegKey( str, _T("Options"), AutoPollTime );
}

BOOL CP4Registry::SetAutoPollIconic(BOOL autoPollIconic)
{
	CString str;
	str.Format(_T("%ld"), (long) autoPollIconic);
	m_AutoPollIconic=autoPollIconic;
	return SetRegKey( str, _T("Options"), AutoPollIconic );
}

BOOL CP4Registry::SetAutoPollJobs(BOOL autoPollJobs)
{
	CString str;
	str.Format(_T("%ld"), (long) autoPollJobs);
	m_AutoPollJobs=autoPollJobs;
	return SetRegKey( str, _T("Options"), AutoPollJobs );
}

BOOL CP4Registry::SetLogoutOnExit(BOOL logoutOnExit)
{
	CString str;
	str.Format(_T("%ld"), (long) logoutOnExit);
	m_LogoutOnExit=logoutOnExit;
	return SetRegKey( str, _T("Options"), LogoutOnExit );
}

BOOL CP4Registry::SetClearAndReload(BOOL clearAndReload)
{
	CString str;
	str.Format(_T("%ld"), (long) clearAndReload);
	m_ClearAndReload=clearAndReload;
	return SetRegKey( str, _T("Options"), ClearAndReload );
}

BOOL CP4Registry::SetReloadOnUncover(BOOL reloadOnUncover)
{
	CString str;
	str.Format(_T("%ld"), (long) reloadOnUncover);
	m_ReloadOnUncover=reloadOnUncover;
	return SetRegKey( str, _T("Options"), ReloadOnUncover );
}

BOOL CP4Registry::SetReloadUncoverTime(long reloadUncoverTime)
{
	ASSERT( reloadUncoverTime >= 0);

	CString str;
	str.Format(_T("%ld"), (long) reloadUncoverTime);
	m_ReloadUncoverTime=reloadUncoverTime;
	return SetRegKey( str, _T("Options"), ReloadUncoverTime );
}

BOOL CP4Registry::SetChglistSyncDef(long chglistSyncDef)
{
	CString str;
	str.Format(_T("%ld"), (long) chglistSyncDef);
	m_ChglistSyncDef=chglistSyncDef;
	return SetRegKey( str, _T("Options"), ChglistSyncDef );
}

BOOL CP4Registry::SetFetchChangeCount(long fetchChangeCount)
{
	ASSERT( fetchChangeCount >= 0);

	CString str;
	str.Format(_T("%ld"), (long) fetchChangeCount);
	m_FetchChangeCount=fetchChangeCount;
	return SetRegKey( str, _T("Options"), FetchChangeCount );
}

BOOL CP4Registry::SetFetchAllChanges(BOOL fetchAll)
{
	CString str;
	str.Format(_T("%ld"), (long) fetchAll);
	m_FetchAll=fetchAll;
	return SetRegKey( str, _T("Options"), FetchAllChanges );
}

BOOL CP4Registry::SetFetchJobCount(long fetchJobCount)
{
	ASSERT( fetchJobCount >= 0);

	CString str;
	str.Format(_T("%ld"), (long) fetchJobCount);
	m_FetchJobCount=fetchJobCount;
	return SetRegKey( str, _T("Options"), FetchJobCount );
}

BOOL CP4Registry::SetFetchAllJobs(BOOL fetchAll)
{
	CString str;
	str.Format(_T("%ld"), (long) fetchAll);
	m_FetchAllJobs=fetchAll;
	return SetRegKey( str, _T("Options"), FetchAllJobs );
}

BOOL CP4Registry::SetFetchHistCount(long fetchHistCount)
{
	ASSERT( fetchHistCount >= 0);

	CString str;
	str.Format(_T("%ld"), (long) fetchHistCount);
	m_FetchHistCount=fetchHistCount;
	return SetRegKey( str, _T("Options"), FetchHistCount );
}

BOOL CP4Registry::SetFetchAllHist(BOOL fetchAll)
{
	CString str;
	str.Format(_T("%ld"), (long) fetchAll);
	m_FetchAllHist=fetchAll;
	return SetRegKey( str, _T("Options"), FetchAllHist );
}

BOOL CP4Registry::SetFetchCompleteHist(long fetchComplete)
{
	CString str;
	str.Format(_T("%ld"), (long) fetchComplete);
	m_FetchCompleteHist=fetchComplete;
	return SetRegKey( str, _T("Options"), FetchCompleteHist );
}

BOOL CP4Registry::SetHistListHeight(int histListHeight)
{
	CString str;
	str.Format(_T("%ld"), (long) histListHeight);
	m_HistListHeight=histListHeight;
	return SetRegKey( str, _T("Options"), HistListHeight );
}

BOOL CP4Registry::SetJobFilterWidth(int jobFilterWidth)
{
	CString str;
	str.Format(_T("%ld"), (long) jobFilterWidth);
	m_JobFilterWidth=jobFilterWidth;
	return SetRegKey( str, _T("Options"), JobFilterWidth );
}

BOOL CP4Registry::SetUseDepotInClientView(BOOL useDepotInClientView)
{
	CString str;
	str.Format(_T("%ld"), (long) useDepotInClientView);
	m_UseDepotInClientView=useDepotInClientView;
	return SetRegKey( str, _T("Options"), UseDepotInClientView );
}

BOOL CP4Registry::SetUse256colorIcons(BOOL use256colorIcons)
{
	CString str;
	str.Format(_T("%ld"), (long) use256colorIcons);
	m_Use256colorIcons=use256colorIcons;
	return SetRegKey( str, _T("Options"), Use256colorIcons );
}

BOOL CP4Registry::SetDontShowYouHaveChgClientView(BOOL bDontShowYouHaveChgClientView)
{
	CString str;
	str.Format(_T("%ld"), (long) bDontShowYouHaveChgClientView);
	m_DontShowYouHaveChgClientView=bDontShowYouHaveChgClientView;
	return SetRegKey( str, _T("Options"), DontShowYouHaveChgClientView );
}

BOOL CP4Registry::SetDontShowYouHaveCr8NewClient(BOOL bDontShowYouHaveCr8NewClient)
{
	CString str;
	str.Format(_T("%ld"), (long) bDontShowYouHaveCr8NewClient);
	m_DontShowYouHaveCr8NewClient=bDontShowYouHaveCr8NewClient;
	return SetRegKey( str, _T("Options"), DontShowYouHaveCr8NewClient );
}

BOOL CP4Registry::SetDontShowDiscardFormChgs(BOOL bDontShowDiscardFormChgs)
{
	CString str;
	str.Format(_T("%ld"), (long) bDontShowDiscardFormChgs);
	m_DontShowDiscardFormChgs=bDontShowDiscardFormChgs;
	return SetRegKey( str, _T("Options"), DontShowDiscardFormChgs );
}

BOOL CP4Registry::SetFontFace( LPCTSTR fontFace )
{
	m_FontFace= fontFace;
	return SetRegKey( m_FontFace, _T("Options"), DlgFontFace );
}

BOOL CP4Registry::SetFontSize( int fontSize )
{
	ASSERT( fontSize >= 0);

	CString str;
	str.Format(_T("%ld"), (long) fontSize);
	m_FontSize= fontSize;
	return SetRegKey( str, _T("Options"), DlgFontSize );
}

BOOL CP4Registry::SetFontItalic( BOOL isItalic )
{
	CString str;
	str.Format(_T("%ld"), (long) isItalic);
	m_IsFontItalic= isItalic;
	return SetRegKey( str, _T("Options"), DlgFontItalic );
}

BOOL CP4Registry::SetFontWeight( int fontWeight )
{
	ASSERT( fontWeight >= 0);

	CString str;
	str.Format(_T("%ld"), (long) fontWeight);
	m_FontWeight= fontWeight;
	return SetRegKey( str, _T("Options"), DlgFontWeight );
}

BOOL CP4Registry::SetFindFilesWidth( int findFilesWidth )
{
	CString str;
	str.Format(_T("%ld"), (long) findFilesWidth);
	m_FindFilesWidth= findFilesWidth;
	return SetRegKey( str, _T("Options"), FindFilesWidth );
}

BOOL CP4Registry::SetP4StartWith( int p4StartWith )
{
	CString str;
	str.Format(_T("%ld"), (long) p4StartWith);
	m_P4StartWith= p4StartWith;
	return SetRegKey( str, _T("Options"), P4StartWith );
}

BOOL CP4Registry::SetP4BusyCursor( int p4BusyCursor )
{
	CString str;
	str.Format(_T("%ld"), (long) p4BusyCursor);
	m_P4BusyCursor= p4BusyCursor;
	return SetRegKey( str, _T("Options"), P4BusyCursor );
}

BOOL CP4Registry::SetCloseAfterFind( int closeAfterFind )
{
	CString str;
	str.Format(_T("%ld"), (long) closeAfterFind);
	m_CloseAfterFind= closeAfterFind;
	return SetRegKey( str, _T("Options"), CloseAfterFind );
}

BOOL CP4Registry::SetSubmitOnlyChged( BOOL submitOnlyChged )
{
	CString str;
	str.Format(_T("%ld"), (long) submitOnlyChged);
	m_SubmitOnlyChged= submitOnlyChged;
	return SetRegKey( str, _T("Options"), SubmitOnlyChged );
}

BOOL CP4Registry::SetRevertUnchged( BOOL revertUnchged )
{
	CString str;
	str.Format(_T("%ld"), (long) revertUnchged);
	m_RevertUnchged= revertUnchged;
	return SetRegKey( str, _T("Options"), RevertUnchged );
}

BOOL CP4Registry::SetAutoReopen( BOOL autoReopen )
{
	CString str;
	str.Format(_T("%ld"), (long) autoReopen);
	m_AutoReopen= autoReopen;
	return SetRegKey( str, _T("Options"), AutoReopen );
}

BOOL CP4Registry::SetFilterPendChgsByMyClient( BOOL filterPendChgsByMyClient )
{
	CString str;
	str.Format(_T("%ld"), (long) filterPendChgsByMyClient);
	m_FilterPendChgsByMyClient= filterPendChgsByMyClient;
	return SetRegKey( str, _T("Options"), FilterPendChgsByMyClient );
}

BOOL CP4Registry::SetFilterPendChgsByPath( LPCTSTR filterPendChgsByPath )
{
	m_FilterPendChgsByPath= filterPendChgsByPath;
	return SetRegKey( m_FilterPendChgsByPath, _T("Options"), FilterPendChgsByPath );
}

BOOL CP4Registry::SetLabelDragDropOption( int labelDragDropOption )
{
	CString str;
	str.Format(_T("%ld"), (long) labelDragDropOption);
	m_LabelDragDropOption= labelDragDropOption;
	return SetRegKey( str, _T("Options"), LabelDragDropOption );
}

BOOL CP4Registry::SetResolveDefault( int resolveDefault )
{
	CString str;
	str.Format(_T("%ld"), (long) resolveDefault);
	m_ResolveDefault= resolveDefault;
	return SetRegKey( str, _T("Options"), ResolveDefault );
}

BOOL CP4Registry::SetResolve2wayDefault( int resolve2wayDefault )
{
	CString str;
	str.Format(_T("%ld"), (long) resolve2wayDefault);
	m_Resolve2wayDefault= resolve2wayDefault;
	return SetRegKey( str, _T("Options"), Resolve2wayDefault );
}

BOOL CP4Registry::SetResolve3wayDefault( int resolve3wayDefault )
{
	CString str;
	str.Format(_T("%ld"), (long) resolve3wayDefault);
	m_Resolve3wayDefault= resolve3wayDefault;
	return SetRegKey( str, _T("Options"), Resolve3wayDefault );
}

BOOL CP4Registry::SetConvertJobNbr( BOOL convertJobNbr )
{
	CString str;
	str.Format(_T("%ld"), (long) convertJobNbr);
	m_ConvertJobNbr= convertJobNbr;
	return SetRegKey( str, _T("Options"), ConvertJobNbr );
}

BOOL CP4Registry::SetAllowExceptionReporting(BOOL allowExceptionReporting)
{
	CString str;
	str.Format(_T("%ld"), (long) allowExceptionReporting);
	m_AllowExceptionReporting= allowExceptionReporting;
	EnableErrorRecording(allowExceptionReporting == TRUE);
	return SetRegKey( str, _T("Options"), AllowExceptionReporting);
}

BOOL CP4Registry::SetTryResetingFocus(BOOL tryResetingFocus)
{
	CString str;
	str.Format(_T("%ld"), (long) tryResetingFocus);
	m_TryResetingFocus= tryResetingFocus;
	EnableErrorRecording(tryResetingFocus == TRUE);
	return SetRegKey( str, _T("Options"), TryResetingFocus);
}
///////////////////////////////////////////////////////////////
// Settings Key
///////////////////////////////////////////////////////////////

BOOL CP4Registry::SetAddFileCurDir(LPCTSTR dir)
{
	m_AddFileCurDir=dir;
	return SetRegKey( m_AddFileCurDir, _T("Settings"), AddFileCurDir );
}

BOOL CP4Registry::SetAddFileExtFilter(LPCTSTR exts)
{
	m_AddFileExtFilter=exts;
	return SetRegKey( m_AddFileExtFilter, _T("Settings"), AddFileExtFilter );
}

BOOL CP4Registry::SetAddFileFilter(LPCTSTR filter)
{
	m_AddFileFilter=filter;
	return SetRegKey( m_AddFileFilter, _T("Settings"), AddFileFilter );
}

BOOL CP4Registry::SetAddFileFilterIndex(int index)
{
	CString str;
	str.Format(_T("%ld"), (long) index);
	m_AddFileFilterIndex= index;
	return SetRegKey( str, _T("Settings"), AddFileFilterIndex );
}

BOOL CP4Registry::SetDefaultDnDfromExp(int index)
{
	CString str;
	str.Format(_T("%ld"), (long) index);
	m_DefaultDnDfromExp= index;
	return SetRegKey( str, _T("Settings"), DefaultDnDfromExp );
}

BOOL CP4Registry::SetMultiProcessorSleep(int index)
{
	ASSERT( index >= 0);

	CString str;
	str.Format(_T("%ld"), (long) index);
	m_MultiProcessorSleep= index;
	return SetRegKey( str, _T("Settings"), MultiProcessorSleep );
}

BOOL CP4Registry::SetMaxStatusLines(int maxStatusLines)
{
	if (maxStatusLines < 1000)
		maxStatusLines = 1000;
	CString str;
	str.Format(_T("%ld"), (long) maxStatusLines);
	m_MaxStatusLines= maxStatusLines;
	return SetRegKey( str, _T("Settings"), MaxStatusLines );
}

BOOL CP4Registry::SetShowStatusMsgs(BOOL showStatusMsgs)
{
	CString str;
	str.Format(_T("%ld"), (long) showStatusMsgs);
	m_ShowStatusMsgs= showStatusMsgs;
	return SetRegKey( str, _T("Settings"), ShowStatusMsgs );
}

BOOL CP4Registry::SetShowTruncTooltip(BOOL showTruncTooltip)
{
	CString str;
	str.Format(_T("%ld"), (long) showTruncTooltip);
	m_ShowTruncTooltip= showTruncTooltip;
	return SetRegKey( str, _T("Settings"), ShowTruncTooltip );
}

BOOL CP4Registry::SetDontThreadDiffs(BOOL dontThreadDiffs)
{
	CString str;
	str.Format(_T("%ld"), (long) dontThreadDiffs);
	m_DontThreadDiffs= dontThreadDiffs;
	return SetRegKey( str, _T("Settings"), DontThreadDiffs );
}

BOOL CP4Registry::SetPendChgExpansion(LPCTSTR pendChgExpansion)
{
	m_PendChgExpansion=pendChgExpansion;
	return SetRegKey( m_PendChgExpansion, _T("Settings"), PendChgExpansion );
}

BOOL CP4Registry::SetLastBranch(LPCTSTR lastBranch)
{
	m_LastBranch=lastBranch;
	return SetRegKey( m_LastBranch, _T("Settings"), LastBranch );
}

BOOL CP4Registry::SetLastLabel(LPCTSTR lastLabel)
{
	m_LastLabel=lastLabel;
	return SetRegKey( m_LastLabel, _T("Settings"), LastLabel );
}

BOOL CP4Registry::SetLastTemplate(LPCTSTR lastTemplate)
{
	m_LastTemplate=lastTemplate;
	return SetRegKey( m_LastTemplate, _T("Settings"), LastTemplate );
}

BOOL CP4Registry::SetLocalCliTemplate(LPCTSTR localCliTemplate)
{
	m_LocalCliTemplate=localCliTemplate;
	return SetRegKey( m_LocalCliTemplate, _T("Settings"), LocalCliTemplate );
}

BOOL CP4Registry::SetLocalCliTemplateSw(BOOL localCliTemplateSw)
{
	CString str;
	str.Format(_T("%ld"), (long) localCliTemplateSw);
	m_LocalCliTemplateSw=localCliTemplateSw;
	return SetRegKey( str, _T("Settings"), LocalCliTemplateSw );
}

BOOL CP4Registry::SetUseClientSpecSubmitOpts(BOOL useClientSpecSubmitOpts)
{
	CString str;
	str.Format(_T("%ld"), (long) useClientSpecSubmitOpts);
	m_UseClientSpecSubmitOpts=useClientSpecSubmitOpts;
	return SetRegKey( str, _T("Settings"), UseClientSpecSubmitOpts );
}

BOOL CP4Registry::SetSwapButtonPosition(BOOL swapButtonPosition)
{
	CString str;
	str.Format(_T("%ld"), (long) swapButtonPosition);
	m_SwapButtonPosition=swapButtonPosition;
	return SetRegKey( str, _T("Settings"), SwapButtonPosition );
}

BOOL CP4Registry::SetBranchFilteredFlags(int branchFilteredFlags)
{
	CString str;
	str.Format(_T("%ld"), (long) branchFilteredFlags);
	m_BranchFilteredFlags= branchFilteredFlags;
	return SetRegKey( str, _T("Settings"), BranchFilteredFlags );
}

BOOL CP4Registry::SetBranchFilterOwner(LPCTSTR branchFilterOwner)
{
	m_BranchFilterOwner=branchFilterOwner;
	return SetRegKey( m_BranchFilterOwner, _T("Settings"), BranchFilterOwner );
}

BOOL CP4Registry::SetClientFilteredFlags(int clientFilteredFlags)
{
	CString str;
	str.Format(_T("%ld"), (long) clientFilteredFlags);
	m_ClientFilteredFlags= clientFilteredFlags;
	return SetRegKey( str, _T("Settings"), ClientFilteredFlags );
}

BOOL CP4Registry::SetClientFilterOwner(LPCTSTR clientFilterOwner)
{
	m_ClientFilterOwner=clientFilterOwner;
	return SetRegKey( m_ClientFilterOwner, _T("Settings"), ClientFilterOwner );
}

BOOL CP4Registry::SetClientFilterHost(LPCTSTR clientFilterHost)
{
	m_ClientFilterHost=clientFilterHost;
	return SetRegKey( m_ClientFilterHost, _T("Settings"), ClientFilterHost );
}

BOOL CP4Registry::SetClientFilterDesc(LPCTSTR clientFilterDesc)
{
	m_ClientFilterDesc=clientFilterDesc;
	return SetRegKey( m_ClientFilterDesc, _T("Settings"), ClientFilterDesc );
}

///////////////////////////////////////////////////////////////
// Layout Key
///////////////////////////////////////////////////////////////

BOOL CP4Registry::SetWindowPosition(CRect rect)
{
	m_WindowPosition=rect;

	CString str;
	str.Format(_T("%d,%d,%d,%d"), rect.TopLeft().x, rect.TopLeft().y, 
							rect.Width(), rect.Height());
	return SetRegKey( str, _T("Layout"), WindowPosition );
}

BOOL CP4Registry::SetWindowIconic(BOOL iconic)
{
	CString str;
	str.Format(_T("%ld"), (long) iconic);
	m_Iconic=iconic;
	return SetRegKey( str, _T("Layout"), WindowIconic );
}

BOOL CP4Registry::SetWindowMaximized(BOOL maximized)
{
	CString str;
	str.Format(_T("%ld"), (long) maximized);
	m_Maximized=maximized;
	return SetRegKey( str, _T("Layout"), WindowRestoreMaximized );
}

BOOL CP4Registry::SetHSplitterPosition(int position)
{
	ASSERT(position>=0);
	
	CString str;
	str.Format(_T("%ld"), (long) position);
	m_HSplitterPosition=position;
	return SetRegKey( str, _T("Layout"), HorizontalSplitter );

}


BOOL CP4Registry::SetVSplitterPosition(int position)
{
	ASSERT(position>=0);
	
	CString str;
	str.Format(_T("%ld"), (long) position);
	m_VSplitterPosition=position;
	return SetRegKey( str, _T("Layout"), VerticalSplitter );

}


///////////////////////////////////////////////////////////////
// Tools Key
///////////////////////////////////////////////////////////////

CString CP4Registry::GetToolMenuName(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_TOOLS);
	
	return CString( m_ToolMenuName[index] );
}

BOOL CP4Registry::SetToolMenuName(int index, CString menuName)
{
	ASSERT(index>=0 && index < MAX_TOOLS);
	
	CString str;
	str.Format(_T("MenuName%d"), index);
	m_ToolMenuName[index] = menuName;
	if (!menuName.GetLength())
		return DelRegKey( _T("Tools"), str );
	else
		return SetRegKey( menuName, _T("Tools"), str );
}

CString CP4Registry::GetToolCommand(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_TOOLS);
	
	return CString( m_ToolCommand[index] );
}

BOOL CP4Registry::SetToolCommand(int index, CString command)
{
	ASSERT(index>=0 && index < MAX_TOOLS);
	
	CString str;
	str.Format(_T("Command%d"), index);
	m_ToolCommand[index] = command;
	if (!m_ToolMenuName[index].GetLength())
		return DelRegKey( _T("Tools"), str );
	else
		return SetRegKey( command, _T("Tools"), str );
}

CString CP4Registry::GetToolArgs(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_TOOLS);
	
	return CString( m_ToolArgs[index] );
}

BOOL CP4Registry::SetToolArgs(int index, CString args)
{
	ASSERT(index>=0 && index < MAX_TOOLS);
	
	CString str;
	str.Format(_T("Arguments%d"), index);
	m_ToolArgs[index] = args;
	if (!m_ToolMenuName[index].GetLength())
		return DelRegKey( _T("Tools"), str );
	else
		return SetRegKey( args, _T("Tools"), str );
}

CString CP4Registry::GetToolInitDir(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_TOOLS);
	
	return CString( m_ToolInitDir[index] );
}

CString CP4Registry::GetToolPromptText(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_TOOLS);
	
	return CString( m_ToolPromptText[index] );
}

BOOL CP4Registry::SetToolInitDir(int index, CString initDir)
{
	ASSERT(index>=0 && index < MAX_TOOLS);
	
	CString str;
	str.Format(_T("InitialDirectory%d"), index);
	m_ToolInitDir[index] = initDir;
	if (!m_ToolMenuName[index].GetLength())
		return DelRegKey( _T("Tools"), str );
	else
		return SetRegKey( initDir, _T("Tools"), str );
}

BOOL CP4Registry::SetToolPromptText(int index, CString promptText)
{
	ASSERT(index>=0 && index < MAX_TOOLS);
	
	CString str;
	str.Format(_T("PromptText%d"), index);
	m_ToolPromptText[index] = promptText;
	if (!m_ToolMenuName[index].GetLength())
		return DelRegKey( _T("Tools"), str );
	else
		return SetRegKey( promptText, _T("Tools"), str );
}

BOOL CP4Registry::GetToolIsConsole(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_TOOLS);
	
	return BOOL( m_ToolIsConsole[index] );
}

BOOL CP4Registry::SetToolIsConsole(int index, BOOL isConsole)
{
	ASSERT(index>=0 && index < MAX_TOOLS);
	
	CString str;
	str.Format(_T("IsConsole%d"), index);
	CString value;
	value.Format(_T("%ld"), (long) isConsole);
	m_ToolIsConsole[index] = isConsole;
	if (!m_ToolMenuName[index].GetLength())
		return DelRegKey( _T("Tools"), str );
	else
		return SetRegKey( value, _T("Tools"), str );
}

BOOL CP4Registry::GetToolIsPrompt(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_TOOLS);
	
	return BOOL( m_ToolIsPrompt[index] );
}

BOOL CP4Registry::SetToolIsPrompt(int index, BOOL isPrompt)
{
	ASSERT(index>=0 && index < MAX_TOOLS);
	
	CString str;
	str.Format(_T("IsPrompt%d"), index);
	CString value;
	value.Format(_T("%ld"), (long) isPrompt);
	m_ToolIsPrompt[index] = isPrompt;
	if (!m_ToolMenuName[index].GetLength())
		return DelRegKey( _T("Tools"), str );
	else
		return SetRegKey( value, _T("Tools"), str );
}

BOOL CP4Registry::GetToolIsOutput2Status(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_TOOLS);
	
	return BOOL( m_ToolIsOutput2Status[index] );
}

BOOL CP4Registry::GetToolIsCloseOnExit(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_TOOLS);
	
	return BOOL( m_ToolIsCloseOnExit[index] );
}

BOOL CP4Registry::SetToolIsOutput2Status(int index, BOOL isOutput2Status)
{
	ASSERT(index>=0 && index < MAX_TOOLS);
	
	CString str;
	str.Format(_T("IsOutput2Status%d"), index);
	CString value;
	value.Format(_T("%ld"), (long) isOutput2Status);
	m_ToolIsOutput2Status[index] = isOutput2Status;
	if (!m_ToolMenuName[index].GetLength())
		return DelRegKey( _T("Tools"), str );
	else
		return SetRegKey( value, _T("Tools"), str );
}

BOOL CP4Registry::SetToolIsCloseOnExit(int index, BOOL isCloseOnExit)
{
	ASSERT(index>=0 && index < MAX_TOOLS);
	
	CString str;
	str.Format(_T("IsCloseOnExit%d"), index);
	CString value;
	value.Format(_T("%ld"), (long) isCloseOnExit);
	m_ToolIsCloseOnExit[index] = isCloseOnExit;
	if (!m_ToolMenuName[index].GetLength())
		return DelRegKey( _T("Tools"), str );
	else
		return SetRegKey( value, _T("Tools"), str );
}

BOOL CP4Registry::GetToolIsShowBrowse(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_TOOLS);
	
	return BOOL( m_ToolIsShowBrowse[index] );
}

BOOL CP4Registry::SetToolIsShowBrowse(int index, BOOL isShowBrowse)
{
	ASSERT(index>=0 && index < MAX_TOOLS);
	
	CString str;
	str.Format(_T("IsShowBrowse%d"), index);
	CString value;
	value.Format(_T("%ld"), (long) isShowBrowse);
	m_ToolIsShowBrowse[index] = isShowBrowse;
	if (!m_ToolMenuName[index].GetLength())
		return DelRegKey( _T("Tools"), str );
	else
		return SetRegKey( value, _T("Tools"), str );
}

BOOL CP4Registry::GetToolIsRefresh(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_TOOLS);
	
	return BOOL( m_ToolIsRefresh[index] );
}

BOOL CP4Registry::SetToolIsRefresh(int index, BOOL isRefresh)
{
	ASSERT(index>=0 && index < MAX_TOOLS);
	
	CString str;
	str.Format(_T("IsRefresh%d"), index);
	CString value;
	value.Format(_T("%ld"), (long) isRefresh);
	m_ToolIsRefresh[index] = isRefresh;
	if (!m_ToolMenuName[index].GetLength())
		return DelRegKey( _T("Tools"), str );
	else
		return SetRegKey( value, _T("Tools"), str );
}

BOOL CP4Registry::GetToolOnContext(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_TOOLS);
	
	return BOOL( m_ToolOnContext[index] );
}

BOOL CP4Registry::SetToolOnContext(int index, BOOL onContext)
{
	ASSERT(index>=0 && index < MAX_TOOLS);
	
	CString str;
	str.Format(_T("OnContext%d"), index);
	CString value;
	value.Format(_T("%ld"), (long) onContext);
	m_ToolOnContext[index] = onContext;
	if (!m_ToolMenuName[index].GetLength())
		return DelRegKey( _T("Tools"), str );
	else
		return SetRegKey( value, _T("Tools"), str );
}

BOOL CP4Registry::GetToolIsSubMenu(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_TOOLS);
	
	return BOOL( m_ToolIsSubMenu[index] );
}

BOOL CP4Registry::SetToolIsSubMenu(int index, BOOL isSubMenu)
{
	ASSERT(index>=0 && index < MAX_TOOLS);
	
	CString str;
	str.Format(_T("IsSubMenu%d"), index);
	CString value;
	value.Format(_T("%ld"), (long) isSubMenu);
	m_ToolIsSubMenu[index] = isSubMenu;
	if (!m_ToolMenuName[index].GetLength())
		return DelRegKey( _T("Tools"), str );
	else
		return SetRegKey( value, _T("Tools"), str );
}


///////////////////////////////////////////////////////////////
// Bookmarks Key
///////////////////////////////////////////////////////////////

CString CP4Registry::GetBkMkMenuName(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_BOOKMARKS);
	
	return CString( m_BkMkMenuName[index] );
}

BOOL CP4Registry::SetBkMkMenuName(int index, CString menuName)
{
	ASSERT(index>=0 && index < MAX_BOOKMARKS);
	
	CString str;
	str.Format(_T("MenuName%d"), index);
	m_BkMkMenuName[index] = menuName;
	if (!menuName.GetLength())
		return DelRegKey( _T("Bookmarks"), str );
	else
		return SetRegKey( menuName, _T("Bookmarks"), str );
}

BOOL CP4Registry::GetBkMkIsSubMenu(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_BOOKMARKS);
	
	return BOOL( m_BkMkIsSubMenu[index] );
}

BOOL CP4Registry::SetBkMkIsSubMenu(int index, BOOL isSubMenu)
{
	ASSERT(index>=0 && index < MAX_BOOKMARKS);
	
	CString str;
	str.Format(_T("IsSubMenu%d"), index);
	CString value;
	value.Format(_T("%ld"), (long) isSubMenu);
	m_BkMkIsSubMenu[index] = isSubMenu;
	if (!m_BkMkMenuName[index].GetLength())
		return DelRegKey( _T("Bookmarks"), str );
	else
		return SetRegKey( value, _T("Bookmarks"), str );
}


///////////////////////////////////////////////////////////////
// Favorites Key
///////////////////////////////////////////////////////////////

CString CP4Registry::GetFavMenuName(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_FAVORITES);
	
	return CString( m_FavMenuName[index] );
}

BOOL CP4Registry::SetFavMenuName(int index, CString menuName)
{
	ASSERT(index>=0 && index < MAX_FAVORITES);
	
	CString str;
	str.Format(_T("MenuName%d"), index);
	m_FavMenuName[index] = menuName;
	if (!menuName.GetLength())
		return DelRegKey( _T("Favorites"), str );
	else
		return SetRegKey( menuName, _T("Favorites"), str );
}

BOOL CP4Registry::GetFavIsSubMenu(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_FAVORITES);
	
	return BOOL( m_FavIsSubMenu[index] );
}

BOOL CP4Registry::SetFavIsSubMenu(int index, BOOL isSubMenu)
{
	ASSERT(index>=0 && index < MAX_FAVORITES);
	
	CString str;
	str.Format(_T("IsSubMenu%d"), index);
	CString value;
	value.Format(_T("%ld"), (long) isSubMenu);
	m_FavIsSubMenu[index] = isSubMenu;
	if (!m_FavMenuName[index].GetLength())
		return DelRegKey( _T("Favorites"), str );
	else
		return SetRegKey( value, _T("Favorites"), str );
}


///////////////////////////////////////////////////////////////
// MRU Tool Arguments
///////////////////////////////////////////////////////////////
	
CString CP4Registry::GetMRUToolArg(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_MRU_TOOLARGS);
	
	return CString( m_MRUToolArg[index] );
}

void CP4Registry::AddMRUToolArg( LPCTSTR toolArg )
{
	ASSERT(m_AttemptedRead);
	if( lstrlen(toolArg) == 0)
		return;

	CString temp[MAX_MRU_TOOLARGS+1];
	CString txt;

	// Put toolArg at the top of a temporary list of MRU args, 
	// and add all non-blank, non-duplicates from existing MRU list

	int numEntries=0;
	temp[numEntries]=toolArg;
	numEntries++;
	
	int i;
	for(i=0; i < MAX_MRU_TOOLARGS; i++)
	{
		m_MRUToolArg[i].TrimRight();
		if(m_MRUToolArg[i].GetLength() > 0 && m_MRUToolArg[i].Compare(toolArg) != 0)
		{
			temp[numEntries]= m_MRUToolArg[i];
			numEntries++;
		}
	}

	// Then copy temp list over MRU list,
	for(i=0; i < MAX_MRU_TOOLARGS; i++)
	{
		if(i < numEntries)
			m_MRUToolArg[i]= temp[i];
		else
			m_MRUToolArg[i].Empty();
	}

	// And write the new MRU Tool Arguments
	CString keyName;
	for(i=0; i < MAX_MRU_TOOLARGS; i++)
	{
		keyName.Format(_T("Arg%d"), i);
		SetRegKey( m_MRUToolArg[i], RecentToolArgList, keyName);			
	}
}


///////////////////////////////////////////////////////////////
// MRU Find Strings
///////////////////////////////////////////////////////////////
	
CString CP4Registry::GetMRUFindStr(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_MRU_FINDSTRS);
	
	return CString( m_MRUFindStr[index] );
}

void CP4Registry::AddMRUFindStr( LPCTSTR findStr )
{
	ASSERT(m_AttemptedRead);
	if( lstrlen(findStr) == 0)
		return;

	CString temp[MAX_MRU_FINDSTRS+1];
	CString txt;

	// Put findStr at the top of a temporary list of MRU args, 
	// and add all non-blank, non-duplicates from existing MRU list

	int numEntries=0;
	temp[numEntries]=findStr;
	numEntries++;
		
	int i;
	for(i=0; i < MAX_MRU_FINDSTRS; i++)
	{
		m_MRUFindStr[i].TrimRight();
		if(m_MRUFindStr[i].GetLength() > 0 && m_MRUFindStr[i].Compare(findStr) != 0)
		{
			temp[numEntries]= m_MRUFindStr[i];
			numEntries++;
		}
	}

	// Then copy temp list over MRU list,
	for(i=0; i < MAX_MRU_FINDSTRS; i++)
	{
		if(i < numEntries)
			m_MRUFindStr[i]= temp[i];
		else
			m_MRUFindStr[i]= "";
	}

	// And write the new MRU Find String
	CString keyName;
	for(i=0; i < MAX_MRU_FINDSTRS; i++)
	{
		keyName.Format(_T("FindStr%d"), i);
		SetRegKey( m_MRUFindStr[i], RecentFindStrList, keyName);			
	}
}


///////////////////////////////////////////////////////////////
// MRU viewers
///////////////////////////////////////////////////////////////
	
CString CP4Registry::GetMRUViewer(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_MRU_VIEWERS);
	
	return CString( m_MRUViewer[index] );
}

CString CP4Registry::GetMRUViewerName(int index)
{
	ASSERT(m_AttemptedRead);
	ASSERT(index >=0 && index < MAX_MRU_VIEWERS);

	if(m_MRUViewer[index].GetLength() ==0 )
		return CString("");
	else
	{
		int lastSlash=m_MRUViewer[index].ReverseFind(_T('\\'));
		if(lastSlash != -1)
			return CString(m_MRUViewer[index].Mid(lastSlash+1));
		else
			return CString(m_MRUViewer[index]);
	}
}

void CP4Registry::AddMRUViewer( LPCTSTR viewerPath )
{
	ASSERT(m_AttemptedRead);
	if( lstrlen(viewerPath) == 0)
	{
		ASSERT(0);
		return;
	}
	

	CString temp[MAX_MRU_VIEWERS+1];
	CString txt;

	// Put viewerPath at the top of a temporary list of MRU viewers, 
	// and add all non-blank, non-duplicates from existing MRU list

	int numEntries=0;
	temp[numEntries]=viewerPath;
	numEntries++;
		
	int i;
	for(i=0; i < MAX_MRU_VIEWERS; i++)
	{
		m_MRUViewer[i].TrimRight();
		if(m_MRUViewer[i].GetLength() > 0 && m_MRUViewer[i].Compare(viewerPath) != 0)
		{
			temp[numEntries]= m_MRUViewer[i];
			numEntries++;
		}
	}

	// Then copy temp list over MRU list,
	for(i=0; i < MAX_MRU_VIEWERS; i++)
	{
		if(i < numEntries)
			m_MRUViewer[i]= temp[i];
		else
			m_MRUViewer[i].Empty();
	}

	// And write the new MRU Viewers
	CString keyName;
	for(i=0; i < MAX_MRU_VIEWERS; i++)
	{
		keyName.Format(_T("File%d"), i);
		SetRegKey( m_MRUViewer[i], RecentViewerList, keyName);			
	}
}

void CP4Registry::RmvMRUViewer( LPCTSTR viewerPath )
{
	int	i, j;
	CString	nuthin;

	ASSERT(m_AttemptedRead);

	CString keyName;
	for(i=j=0; i < MAX_MRU_VIEWERS; i++)
	{
		if (m_MRUViewer[i].Compare(viewerPath))
		{
			keyName.Format(_T("File%d"), j++);
			SetRegKey( m_MRUViewer[i], RecentViewerList, keyName );
		}
		else m_MRUViewer[i].Empty();
	}
	// Write empty MRU Viewers
	while (j < MAX_MRU_VIEWERS)
	{
		keyName.Format(_T("File%d"), j++);
		SetRegKey( nuthin, RecentViewerList, keyName );
	}
}


///////////////////////////////////////////////////////////////
// MRU job filters
///////////////////////////////////////////////////////////////
	
CString CP4Registry::GetMRUFilter(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_MRU_FILTERS);
	
	return CString( m_MRUFilter[index] );
}

void CP4Registry::AddMRUFilter( LPCTSTR filterStr )
{
	ASSERT(m_AttemptedRead);
	if( lstrlen(filterStr) == 0)
	{
		ASSERT(0);
		return;
	}
	

	CString temp[MAX_MRU_FILTERS+1];
	CString txt;

	// Put filterStr at the top of a temporary list of MRU filters, 
	// and add all non-blank, non-duplicates from existing MRU list

	int numEntries=0;
	temp[numEntries]=filterStr;
	numEntries++;
		
	int i;
	for(i=0; i < MAX_MRU_FILTERS; i++)
	{
		m_MRUFilter[i].TrimRight();
		if(m_MRUFilter[i].GetLength() > 0 && m_MRUFilter[i].Compare(filterStr) != 0)
		{
			temp[numEntries]= m_MRUFilter[i];
			numEntries++;
		}
	}

	// Then copy temp list over MRU list,
	for(i=0; i < MAX_MRU_FILTERS; i++)
	{
		if(i < numEntries)
			m_MRUFilter[i]= temp[i];
		else
			m_MRUFilter[i].Empty();
	}

	// And write the new MRU Filters
	CString keyName;
	for(i=0; i < MAX_MRU_FILTERS; i++)
	{
		keyName.Format(_T("Filter%d"), i);
		SetRegKey( m_MRUFilter[i], RecentFilterList, keyName);			
	}
}

void CP4Registry::RmvMRUFilter( LPCTSTR filterStr )
{
	int	i, j;
	CString	nuthin;

	ASSERT(m_AttemptedRead);

	CString keyName;
	for(i=j=0; i < MAX_MRU_FILTERS; i++)
	{
		if (m_MRUFilter[i].Compare(filterStr))
		{
			keyName.Format(_T("Filter%d"), j++);
			SetRegKey( m_MRUFilter[i], RecentFilterList, keyName );
		}
		else m_MRUFilter[i].Empty();
	}
	// Write empty MRU Filters
	while (j < MAX_MRU_FILTERS)
	{
		keyName.Format(_T("Filter%d"), j++);
		SetRegKey( nuthin, RecentFilterList, keyName );
	}
}


///////////////////////////////////////////////////////////////
// MRU submitted changelist filters
///////////////////////////////////////////////////////////////
	
CString CP4Registry::GetMRUChgFilter(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_MRUCHG_FILTERS);
	
	return CString( m_MRUChgFilter[index] );
}

void CP4Registry::AddMRUChgFilter( LPCTSTR filterStr )
{
	ASSERT(m_AttemptedRead);
	if( lstrlen(filterStr) == 0)
	{
		ASSERT(0);
		return;
	}
	
	CString temp[MAX_MRUCHG_FILTERS+1];
	CString txt=filterStr;
	txt.TrimRight();

	// Put filterStr at the top of a temporary list of MRU chg filters, 
	// and add all non-blank, non-duplicates from existing MRU list

	int numEntries=0;
	temp[numEntries]=txt;
	numEntries++;
		
	int i;
	for(i=0; i < MAX_MRUCHG_FILTERS; i++)
	{
		m_MRUChgFilter[i].TrimRight();
		if(m_MRUChgFilter[i].GetLength() > 0 && m_MRUChgFilter[i].Compare(txt) != 0)
		{
			temp[numEntries]= m_MRUChgFilter[i];
			numEntries++;
		}
	}

	// Then copy temp list over MRU list,
	for(i=0; i < MAX_MRUCHG_FILTERS; i++)
	{
		if(i < numEntries)
			m_MRUChgFilter[i]= temp[i];
		else
			m_MRUChgFilter[i].Empty();
	}

	// And write the new MRU Filters
	CString keyName;
	for(i=0; i < MAX_MRUCHG_FILTERS; i++)
	{
		keyName.Format(_T("Filter%d"), i);
		SetRegKey( m_MRUChgFilter[i], RecentChgFilterList, keyName);			
	}
}

void CP4Registry::RmvMRUChgFilter( LPCTSTR filterStr )
{
	int	i, j;
	CString	nuthin;

	ASSERT(m_AttemptedRead);

	CString keyName;
	for(i=j=0; i < MAX_MRUCHG_FILTERS; i++)
	{
		if (m_MRUChgFilter[i].Compare(filterStr))
		{
			keyName.Format(_T("Filter%d"), j++);
			SetRegKey( m_MRUChgFilter[i], RecentChgFilterList, keyName );
		}
		else m_MRUChgFilter[i].Empty();
	}
	// Write empty MRU Filters
	while (j < MAX_MRUCHG_FILTERS)
	{
		keyName.Format(_T("Filter%d"), j++);
		SetRegKey( nuthin, RecentChgFilterList, keyName );
	}
}


///////////////////////////////////////////////////////////////
// MRU Ports
///////////////////////////////////////////////////////////////
	
CString CP4Registry::GetMRUPort(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_MRU_PORTS);
	
	return CString( m_MRUPort[index] );
}

void CP4Registry::AddMRUPort( LPCTSTR portStr, LPCTSTR charsetStr )
{
	ASSERT(m_AttemptedRead);
	if( lstrlen(portStr) == 0)
	{
		ASSERT(0);
		return;
	}

	CString temp[MAX_MRU_PORTS+1];
	CString temp2[MAX_MRU_PORTS+1];
	CString txt;

	// Put portStr at the top of a temporary list of MRU ports, 
	// and add all non-blank, non-duplicates from existing MRU list

	int numEntries=0;
	temp[0]=portStr;
	temp2[0]=charsetStr;
	if (temp[0].Find('@') != -1)
		return;		// ports with @ in them cause problems - don't add 'em to the mru list
	numEntries++;
		
	int i;
	for(i=0; i < MAX_MRU_PORTS; i++)
	{
		m_MRUPort[i].TrimRight();
		if(m_MRUPort[i].GetLength() > 0 && m_MRUPort[i].Compare(portStr) != 0)
		{
			temp[numEntries]= m_MRUPort[i];
			temp2[numEntries]= m_MRUPortCharset[i];
			numEntries++;
		}
	}

	// Then copy temp list over MRU list,
	for(i=0; i < MAX_MRU_PORTS; i++)
	{
		if(i < numEntries)
		{
			m_MRUPort[i]= temp[i];
			m_MRUPortCharset[i]= temp2[i];
		}
		else
		{
			m_MRUPort[i].Empty();
			m_MRUPortCharset[i].Empty();
		}
	}

	// And write the new MRU Ports and Charsets
	CString keyName;
	for(i=0; i < MAX_MRU_PORTS; i++)
	{
		keyName.Format(_T("Port%d"), i);
		SetRegKey( m_MRUPort[i], RecentPortList, keyName);			
		keyName.Format(_T("Charset%d"), i);
		SetRegKey( m_MRUPortCharset[i], RecentPortList, keyName);			
	}
}


///////////////////////////////////////////////////////////////
// MRU Charsets
///////////////////////////////////////////////////////////////
	
CString CP4Registry::GetMRUCharset(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_MRU_CHARSETS);
	
	return CString( m_MRUCharset[index] );
}

void CP4Registry::AddMRUCharset( LPCTSTR charsetStr )
{
	ASSERT(m_AttemptedRead);
	if( lstrlen(charsetStr) == 0)
		return;

	CString temp[MAX_MRU_CHARSETS+1];
	CString txt;

	// Put charsetStr at the top of a temporary list of MRU Charsets, 
	// and add all non-blank, non-duplicates from existing MRU list

	int numEntries=0;
	temp[0]=charsetStr;
	if (temp[0].Find('@') != -1)
		return;		// Charsets with @ in them cause problems - don't add 'em to the mru list
	numEntries++;
		
	int i;
	for(i=0; i < MAX_MRU_CHARSETS; i++)
	{
		m_MRUCharset[i].TrimRight();
		if(m_MRUCharset[i].GetLength() > 0 && m_MRUCharset[i].Compare(charsetStr) != 0)
		{
			temp[numEntries]= m_MRUCharset[i];
			numEntries++;
		}
	}

	// Then copy temp list over MRU list,
	for(i=0; i < MAX_MRU_CHARSETS; i++)
	{
		if(i < numEntries)
			m_MRUCharset[i]= temp[i];
		else
			m_MRUCharset[i].Empty();
	}

	// And write the new MRU Charsets
	CString keyName;
	for(i=0; i < MAX_MRU_CHARSETS; i++)
	{
		keyName.Format(_T("Charset%d"), i);
		SetRegKey( m_MRUCharset[i], RecentCharsetList, keyName);			
	}
}


///////////////////////////////////////////////////////////////
// MRU Port-Client-User combinations
///////////////////////////////////////////////////////////////
	
CString CP4Registry::GetMRUPcu(int index) 
{ 
	ASSERT(m_AttemptedRead); 
	ASSERT(index >=0 && index < MAX_MRU_PCUS);
	
	return CString( m_MRUPcu[index] );
}

void CP4Registry::AddMRUPcu( LPCTSTR pcuStr )
{
	ASSERT(m_AttemptedRead);
	if( lstrlen(pcuStr) == 0)
	{
		ASSERT(0);
		return;
	}
	
	GET_P4REGPTR()->SetExpandPath(_T(""));
	CString temp[MAX_MRU_PCUS+1];
	CString txt;

	// Put pcuStr at the top of a temporary list of MRU pcus, 
	// and add all non-blank, non-duplicates from existing MRU list

	int j;
	int numEntries=0;
	temp[0]=pcuStr;
	temp[0].TrimRight();
	if (temp[0].Find('@') != -1)
		return;		// PCUs with @ in them cause problems - don't add 'em to the mru list
	numEntries++;

	CString str;
	BOOL b = FALSE;
	int i;
	for(i=-1; ++i < MAX_MRU_PCUS; )
	{
		m_MRUPcu[i].TrimRight();
		if(m_MRUPcu[i].GetLength() > 0)
		{
			str = m_MRUPcu[i];
			if ((j = str.Find(_T('@'))) > 0)
			{
				str = str.Left(j);
				str.TrimRight();
			}
			if (str.Compare(pcuStr) != 0)
			{
				temp[numEntries]= m_MRUPcu[i];
				numEntries++;
			}
			else
			{
				temp[0] = m_MRUPcu[i];
				if ((j = temp[0].Find(_T('@'))) != -1)
				{
					GET_P4REGPTR()->SetExpandPath(temp[0].Right(temp[0].GetLength() - j - 1));
					b = TRUE;	// we found an expand-path
				}
			}
		}
	}
	if (!b) // do we need to search the Favorites for an expand-path?
	{
		for(i=-1; ++i < MAX_FAVORITES; )
		{
			if (m_FavMenuName[i].GetLength() > 0)
			{
				str = m_FavMenuName[i];
				if ((j = str.Find(_T('@'))) > 0)
				{
					str = str.Left(j);
					str.TrimRight();
				}
				if (str.Compare(pcuStr) == 0)
				{
					if (j > 0)
					{
						str = m_FavMenuName[i].Mid(j+1);
						if ((j = str.Find(_T('#'))) != -1)
							str = str.Left(j);
						GET_P4REGPTR()->SetExpandPath(str);
					}
					break;
				}
			}
		}
	}

	// Then copy temp list over MRU list,
	for(i=-1; ++i < MAX_MRU_PCUS; )
	{
		if(i < numEntries)
			m_MRUPcu[i]= temp[i];
		else
			m_MRUPcu[i].Empty();
	}

	// And write the new MRU Pcus
	CString keyName;
	for(i=0; i < MAX_MRU_PCUS; i++)
	{
		keyName.Format(_T("Pcu%d"), i);
		SetRegKey( m_MRUPcu[i], RecentPcuList, keyName );
	}
}

void CP4Registry::RmvMRUPcu( LPCTSTR pcuStr )
{
	int	i, j, k;
	CString	nuthin;

	ASSERT(m_AttemptedRead);

	CString keyName;
	for(i=j=0; i < MAX_MRU_PCUS; i++)
	{
		CString str = m_MRUPcu[i];
		if ((k = str.Find(_T('@'))) > 0)
		{
			str = str.Left(k);
			str.TrimRight();
		}
		if (str.Compare(pcuStr))
		{
			keyName.Format(_T("Pcu%d"), j++);
			SetRegKey( m_MRUPcu[i], RecentPcuList, keyName );
		}
		else m_MRUPcu[i].Empty();
	}
	// Write empty MRU Pcus
	while (j < MAX_MRU_PCUS)
	{
		keyName.Format(_T("Pcu%d"), j++);
		SetRegKey( nuthin, RecentPcuList, keyName );
	}
}

BOOL CP4Registry::AddMRUPcuPath( LPCTSTR path )
{
	int j;

	ASSERT(m_AttemptedRead);
	CString str;
	CString pcu0 = m_MRUPcu[0];
	if ((j = pcu0.Find(_T('@'))) > 0)
	{
		pcu0 = pcu0.Left(j);
		pcu0.TrimRight();
	}
	if (pcu0.IsEmpty())
		return FALSE;
	CString newpath = path;
	if (!newpath.IsEmpty())
		m_MRUPcu[0] = pcu0 + _T('@') + newpath;

	CString desc;
	int i;
	for(i=-1; ++i < MAX_FAVORITES; )
	{
		if (m_FavMenuName[i].GetLength() > 0)
		{
			str = m_FavMenuName[i];
			if ((j = str.Find(_T('#'))) > 0)
			{
				desc = str.Mid(j+1);
				str = str.Left(j);
				str.TrimRight();
			}
			else desc.Empty();
			if ((j = str.Find(_T('@'))) > 0)
			{
				str = str.Left(j);
				str.TrimRight();
			}
			if (str.Compare(pcu0) == 0)
			{
				str = m_FavMenuName[i].Mid(j+1);
				m_FavMenuName[i] = pcu0;
				if (!newpath.IsEmpty())
					m_FavMenuName[i] += _T('@') + newpath;
				if (!desc.IsEmpty())
					m_FavMenuName[i] += _T('#') + desc;
				str.Format(_T("MenuName%d"), i);
				SetRegKey( m_FavMenuName[i], _T("Favorites"), str );
				break;
			}
		}
	}

	return SetRegKey( m_MRUPcu[0], RecentPcuList, LPCTSTR(_T("Pcu0")) );
}

///////////////////////////////////////////////////////////////
// File Associations
///////////////////////////////////////////////////////////////

BOOL CP4Registry::SetIgnoreWinAssoc(BOOL ignore)
{
	CString str;
	str.Format(_T("%ld"), (long) ignore);
	m_IgnoreWinAssoc=ignore;
	return SetRegKey( str, _T("Options"), IgnoreWinAssoc );
}

BOOL CP4Registry::SetAssociation(LPCTSTR extension, LPCTSTR application)
{
	ASSERT(lstrlen(extension) > 0);

	CString newValue= extension;
	newValue+=_T("|");
	newValue+=application;

	/////////
	// Find the extension in the cache list and update the list
	POSITION pos=FindAssociation(extension);
	
	if(pos != NULL)
	{
		if(lstrlen(application)==0)
		{
			// delete from list
			m_AssocViewers.RemoveAt(pos);
		}
		else
		{
			// update on list
			m_AssocViewers.GetAt(pos)=newValue;
		}
	}
	else if(lstrlen(application))
	{
		// add it to the list
		m_AssocViewers.AddHead(newValue);
	}
		
	/////////
	// Find the value in the registry and update the registry
	HKEY key;
	DWORD disposition;
	long success= RegCreateKeyEx( HKEY_CURRENT_USER, 
								_T("Software\\Perforce\\P4Win\\Associations"),
								0,
								NULL,
								REG_OPTION_NON_VOLATILE,
								KEY_WRITE,
								NULL,
								&key,
							    &disposition );
	
	if(success == ERROR_SUCCESS)
	{
		// Always delete the existing key
		success=RegDeleteValue( key, extension );

		if((success == ERROR_SUCCESS || success == ERROR_FILE_NOT_FOUND) && lstrlen(application) > 0)
		{
			// Add a new key if required
			success= RegSetValueEx(key, extension, 0,
								REG_SZ, (LPBYTE) application, 
								lstrlen(application) * sizeof(TCHAR) +1);
		}	
		RegCloseKey(key);
	}

	return (success == ERROR_SUCCESS);
}

BOOL CP4Registry::ClearAssociation(LPCTSTR extension)
{
	return SetAssociation(extension, _T(""));
}


CString CP4Registry::GetAssociatedApp(LPCTSTR extension)
{
	ASSERT(lstrlen(extension) > 0);
	
	CString path;
	POSITION pos=FindAssociation(extension);
	if(pos == NULL)
		path=_T("");
	else
	{
		path= m_AssocViewers.GetAt(pos);

		int bar= path.Find(_T("|"));
		if(bar == -1)
			ASSERT(0);
		else
			path=path.Mid(bar+1);
	}

	return CString(path);
}

CString CP4Registry::GetAssociatedAppName(LPCTSTR extension)
{
	CString path= GetAssociatedApp(extension);

	int lastSlash=path.ReverseFind(_T('\\'));
	if(lastSlash != -1)
		return CString(path.Mid(lastSlash+1));
	else
		return CString(path);
}

POSITION CP4Registry::FindAssociation(LPCTSTR extension)
{
	int len=lstrlen(extension);
	POSITION pos= m_AssocViewers.GetHeadPosition();
	POSITION lastpos;

	while(pos != NULL)
	{
		lastpos=pos;
		CString str= m_AssocViewers.GetNext(pos);
		if(_tcsncicmp(str.GetBuffer(len), extension, len) == 0 &&
			str[len]==_T('|') )
		{
			// make sure we return the position pointing at the correct entry
			pos=lastpos;
			break;
		}
	}
	return pos;
}

BOOL CP4Registry::SetDiffAssociation(LPCTSTR extension, LPCTSTR application)
{
	ASSERT(lstrlen(extension) > 0);

	CString newValue= extension;
	newValue+=_T("|");
	newValue+=application;

	/////////
	// Find the extension in the cache list and update the list
	POSITION pos=FindDiffAssociation(extension);
	
	if(pos != NULL)
	{
		if(lstrlen(application)==0)
		{
			// delete from list
			m_AssocDiffs.RemoveAt(pos);
		}
		else
		{
			// update on list
			m_AssocDiffs.GetAt(pos)=newValue;
		}
	}
	else if(lstrlen(application))
	{
		// add it to the list
		m_AssocDiffs.AddHead(newValue);
	}
		
	/////////
	// Find the value in the registry and update the registry
	HKEY key;
	DWORD disposition;
	long success= RegCreateKeyEx( HKEY_CURRENT_USER, 
								_T("Software\\Perforce\\P4Win\\DiffAssocs"),
								0,
								NULL,
								REG_OPTION_NON_VOLATILE,
								KEY_WRITE,
								NULL,
								&key,
							    &disposition );
	
	if(success == ERROR_SUCCESS)
	{
		// Always delete the existing key
		success=RegDeleteValue( key, extension );

		if((success == ERROR_SUCCESS || success == ERROR_FILE_NOT_FOUND) && lstrlen(application) > 0)
		{
			// Add a new key if required
			success= RegSetValueEx(key, extension, 0,
								REG_SZ, (LPBYTE) application, 
								lstrlen(application)*sizeof(TCHAR)+1);
		}	
		RegCloseKey(key);
	}

	return (success == ERROR_SUCCESS);
}

BOOL CP4Registry::ClearDiffAssociation(LPCTSTR extension)
{
	return SetDiffAssociation(extension, _T(""));
}


CString CP4Registry::GetAssociatedDiff(LPCTSTR extension)
{
	ASSERT(lstrlen(extension) > 0);
	
	CString path;
	POSITION pos=FindDiffAssociation(extension);
	if(pos == NULL)
		path=_T("");
	else
	{
		path= m_AssocDiffs.GetAt(pos);

		int bar= path.Find(_T("|"));
		if(bar == -1)
			ASSERT(0);
		else
			path=path.Mid(bar+1);
	}

	return CString(path);
}

CString CP4Registry::GetAssociatedDiffName(LPCTSTR extension)
{
	CString path= GetAssociatedDiff(extension);

	int lastSlash=path.ReverseFind(_T('\\'));
	if(lastSlash != -1)
		return CString(path.Mid(lastSlash+1));
	else
		return CString(path);
}

POSITION CP4Registry::FindDiffAssociation(LPCTSTR extension)
{
	int len=lstrlen(extension);
	POSITION pos= m_AssocDiffs.GetHeadPosition();
	POSITION lastpos;

	while(pos != NULL)
	{
		lastpos=pos;
		CString str= m_AssocDiffs.GetNext(pos);
		if(_tcsncicmp(str.GetBuffer(len), extension, len) == 0 &&
			str[len]==_T('|') )
		{
			// make sure we return the position pointing at the correct entry
			pos=lastpos;
			break;
		}
	}
	return pos;
}

BOOL CP4Registry::SetMergeAssociation(LPCTSTR extension, LPCTSTR application)
{
	ASSERT(lstrlen(extension) > 0);

	CString newValue= extension;
	newValue+=_T("|");
	newValue+=application;

	/////////
	// Find the extension in the cache list and update the list
	POSITION pos=FindMergeAssociation(extension);
	
	if(pos != NULL)
	{
		if(lstrlen(application)==0)
		{
			// delete from list
			m_AssocMerges.RemoveAt(pos);
		}
		else
		{
			// update on list
			m_AssocMerges.GetAt(pos)=newValue;
		}
	}
	else if(lstrlen(application))
	{
		// add it to the list
		m_AssocMerges.AddHead(newValue);
	}
		
	/////////
	// Find the value in the registry and update the registry
	HKEY key;
	DWORD disposition;
	long success= RegCreateKeyEx( HKEY_CURRENT_USER, 
								_T("Software\\Perforce\\P4Win\\MergeAssocs"),
								0,
								NULL,
								REG_OPTION_NON_VOLATILE,
								KEY_WRITE,
								NULL,
								&key,
							    &disposition );
	
	if(success == ERROR_SUCCESS)
	{
		// Always delete the existing key
		success=RegDeleteValue( key, extension );

		if((success == ERROR_SUCCESS || success == ERROR_FILE_NOT_FOUND) && lstrlen(application) > 0)
		{
			// Add a new key if required
			success= RegSetValueEx(key, extension, 0,
								REG_SZ, (LPBYTE) application, 
								lstrlen(application)*sizeof(TCHAR)+1);
		}	
		RegCloseKey(key);
	}

	return (success == ERROR_SUCCESS);
}

BOOL CP4Registry::ClearMergeAssociation(LPCTSTR extension)
{
	return SetMergeAssociation(extension, _T(""));
}


CString CP4Registry::GetAssociatedMerge(LPCTSTR extension)
{
	ASSERT(lstrlen(extension) > 0);
	
	CString path;
	POSITION pos=FindMergeAssociation(extension);
	if(pos == NULL)
		path=_T("");
	else
	{
		path= m_AssocMerges.GetAt(pos);

		int bar= path.Find(_T("|"));
		if(bar == -1)
			ASSERT(0);
		else
			path=path.Mid(bar+1);
	}

	return CString(path);
}

CString CP4Registry::GetAssociatedMergeName(LPCTSTR extension)
{
	CString path= GetAssociatedMerge(extension);

	int lastSlash=path.ReverseFind(_T('\\'));
	if(lastSlash != -1)
		return CString(path.Mid(lastSlash+1));
	else
		return CString(path);
}

POSITION CP4Registry::FindMergeAssociation(LPCTSTR extension)
{
	int len=lstrlen(extension);
	POSITION pos= m_AssocMerges.GetHeadPosition();
	POSITION lastpos;

	while(pos != NULL)
	{
		lastpos=pos;
		CString str= m_AssocMerges.GetNext(pos);
		if(_tcsncicmp(str.GetBuffer(len), extension, len) == 0 &&
			str[len]==_T('|') )
		{
			// make sure we return the position pointing at the correct entry
			pos=lastpos;
			break;
		}
	}
	return pos;
}

void CP4Registry::WriteVirginRegistry()
{
	HKEY key;
	DWORD disposition;
	long success= RegCreateKeyEx( HKEY_CURRENT_USER, 
								_T("AppEvents\\EventLabels\\PerforceCompleted"),
								0,
								NULL,
								REG_OPTION_NON_VOLATILE,
								KEY_WRITE,
								NULL,
								&key,
							    &disposition );
	if(success == ERROR_SUCCESS)
	{
		if((success == ERROR_SUCCESS || success == ERROR_FILE_NOT_FOUND))
		{
			// Add a new value
			success= RegSetValueEx(key, _T(""), 0,
								REG_SZ, (LPBYTE) _T("Perforce: Task Completed"), 
								lstrlen(_T("Perforce: Task Completed"))*sizeof(TCHAR)+1);
		}	
		RegCloseKey(key);
	}
	if (success != ERROR_SUCCESS )
		RegWriteFailed(success);

	success= RegCreateKeyEx( HKEY_CURRENT_USER, 
								_T("AppEvents\\EventLabels\\PerforceWarning"),
								0,
								NULL,
								REG_OPTION_NON_VOLATILE,
								KEY_WRITE,
								NULL,
								&key,
							    &disposition );
	if(success == ERROR_SUCCESS)
	{
		if((success == ERROR_SUCCESS || success == ERROR_FILE_NOT_FOUND))
		{
			// Add a new value
			success= RegSetValueEx(key, _T(""), 0,
								REG_SZ, (LPBYTE) _T("Perforce: Warning"), 
								lstrlen(_T("Perforce: Warning"))*sizeof(TCHAR)+1);
		}	
		RegCloseKey(key);
	}

	success= RegCreateKeyEx( HKEY_CURRENT_USER, 
								_T("AppEvents\\EventLabels\\PerforceError"),
								0,
								NULL,
								REG_OPTION_NON_VOLATILE,
								KEY_WRITE,
								NULL,
								&key,
							    &disposition );
	if(success == ERROR_SUCCESS)
	{
		if((success == ERROR_SUCCESS || success == ERROR_FILE_NOT_FOUND))
		{
			// Add a new value
			success= RegSetValueEx(key, _T(""), 0,
								REG_SZ, (LPBYTE) _T("Perforce: Error"), 
								lstrlen(_T("Perforce: Error"))*sizeof(TCHAR)+1);
		}	
		RegCloseKey(key);
	}

	success= RegCreateKeyEx( HKEY_CURRENT_USER, 
								_T("AppEvents\\Schemes\\Apps\\.Default\\PerforceCompleted"),
								0,
								NULL,
								REG_OPTION_NON_VOLATILE,
								KEY_WRITE,
								NULL,
								&key,
							    &disposition );
	if(success == ERROR_SUCCESS)
		RegCloseKey(key);

	success= RegCreateKeyEx( HKEY_CURRENT_USER, 
								_T("AppEvents\\Schemes\\Apps\\.Default\\PerforceWarning"),
								0,
								NULL,
								REG_OPTION_NON_VOLATILE,
								KEY_WRITE,
								NULL,
								&key,
							    &disposition );
	if(success == ERROR_SUCCESS)
		RegCloseKey(key);

	success= RegCreateKeyEx( HKEY_CURRENT_USER, 
								_T("AppEvents\\Schemes\\Apps\\.Default\\PerforceError"),
								0,
								NULL,
								REG_OPTION_NON_VOLATILE,
								KEY_WRITE,
								NULL,
								&key,
							    &disposition );
	if(success == ERROR_SUCCESS)
		RegCloseKey(key);
}

void CP4Registry::RegWriteFailed(LONG rc)
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		rc,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);
	CString msg;
	msg.FormatMessage(IDS_FAILEDREGWRITE, (TCHAR *)lpMsgBuf);
	MainFrame()->AddToStatusLog( msg, SV_WARNING );
}
