//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "cderr.h"
// #define TRACE_HERE
#include "P4win.h"

#include "MainFrm.h"
#include "Document.h"
#include "ClientView.h"
#include "ClientWizSheet.h"
#include "cmd_diff.h"
#include "cmd_get.h"
#include "cmd_fstat.h"
#include "cmd_history.h"
#include "cmd_info.h"
#include "cmd_listopstat.h"
#include "Cmd_Logout.h"
#include "Cmd_Opened.h"
#include "cmd_refresh.h"
#include "cmd_where.h"
#include "FileInfoDlg.h"
#include "FirstTimeDlg.h"
#include "P4Registry.h"
#include "GuiClientUser.h"
#include "ToolsDlg.h"
#include "ToolsArgs.h"
#include "Bookmarks.h"
#include "Favorites.h"
#include "FavEditDlg.h"
#include "WindowsVersion.h"
#include "MsgBox.h"
#include "ImageList.h"
#include "OptionsTreeDlg.h"
#include "SpecDescDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Registered windows message used to reactivate p4win from a new instance
// Need a module global, because ON_REGISTERED_MESSAGE can't eat this ptr
UINT wmActivate;
UINT wmSendcmd;
UINT wmRplycmd;

// Get the ctrl ID for a non-existant pane (x,2)  (IDFromRowCol() would assert)
//#define MAKEID(x) (m_wndHRSplitter.IdFromRowCol(0,0)+16*x +1)
#define MAKEID(x) (m_wndVSplitter.IdFromRowCol(0,1)+16*x +1)

/////////////////////////////////////////////////////////////////////////////
// CTBDropTarget

CTBDropTarget::CTBDropTarget() {}

CTBDropTarget::~CTBDropTarget() {}  

//
// OnDragEnter is called by OLE dll's when drag cursor enters
// a window that is REGISTERed with the OLE dll's
// Pass the cal to MainFrame to deal with it
DROPEFFECT CTBDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* 
                           pDataObject, DWORD dwKeyState, CPoint point )
{
	return MainFrame()->OnDragEnter(pWnd, pDataObject, dwKeyState, point);
} 

//
// OnDragLeave is called by OLE dll's when drag cursor leaves
// a window that is REGISTERed with the OLE dll's
// Pass the cal to MainFrame to deal with it
void CTBDropTarget::OnDragLeave(CWnd* pWnd)
{
	MainFrame()->OnDragLeave(pWnd);
    // Call base class implementation
    COleDropTarget:: OnDragLeave(pWnd);
}
 
// 
// OnDragOver is called by OLE dll's when cursor is dragged over 
// a window that is REGISTERed with the OLE dll's
// Pass the cal to MainFrame to deal with it
DROPEFFECT CTBDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* 
           pDataObject, DWORD dwKeyState, CPoint point )
{
	return MainFrame()->OnDragOver(pWnd, pDataObject, dwKeyState, point);
}

// 
// OnDrop is called by OLE dll's when item is dropped in a window
// that is REGISTERed with the OLE dll's
// Pass the cal to MainFrame to deal with it
BOOL CTBDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, 
                 DROPEFFECT dropEffect, CPoint point )
{ 
    return MainFrame()->OnDrop(pWnd, pDataObject, dropEffect, point );
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_FILE_GET, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPENEDIT, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPENDELETE, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_FILE_REVERT, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOCK, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_FILE_UNLOCK, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_FILE_REVISIONHISTORY, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_FILE_DIFFHEAD, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_FILE_QUICKBROWSE, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_FILE_QUICKEDIT, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILTEREDVIEW, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CLEARFILTER, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_SUBMIT, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SETVIEW, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_FILTER_CLEARVIEW, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_LABELFILTER_SETVIEW, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_LABELFILTER_SETVIEWREV, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_LABELFILTER_CLEARVIEW, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_FILTERBYOWNER, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_CLEAROWNERFILTER, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_JOB_CONFIGURE, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_JOB_SETFILTER, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_JOB_REMOVEFILTER, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_FILTERCLIENTS, OnUpdateMenuWithIcon)
	ON_UPDATE_COMMAND_UI(ID_CLEARCLIENTFILTER, OnUpdateMenuWithIcon)
	ON_COMMAND(ID_VIEW_UPDATE, OnViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UPDATE, OnUpdateViewUpdate)
	ON_COMMAND(ID_FILE_CANCEL, OnCancelCommand)
	ON_UPDATE_COMMAND_UI(ID_FILE_CANCEL, OnUpdateCancelCommand)
	ON_COMMAND(ID_CANCEL_BUTTON, OnCancelButton)
	ON_UPDATE_COMMAND_UI(ID_CANCEL_BUTTON, OnUpdateCancelCommand)
	ON_UPDATE_COMMAND_UI(ID_CURRENTTASK, OnUpdateStatus)
	ON_WM_SIZE()
	ON_COMMAND(ID_PERFORCE_OPTIONS, OnPerforceOptions)
	ON_UPDATE_COMMAND_UI(ID_PERFORCE_OPTIONS, OnUpdateP4Options)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_PERFORCE_INFO, OnPerforceInfo)
	ON_UPDATE_COMMAND_UI(ID_PERFORCE_INFO, OnUpdatePerforceInfo)
	ON_UPDATE_COMMAND_UI(ID_BRANCH_ACTIVATE, OnUpdateViewSwitch)
	ON_COMMAND(ID_BRANCH_ACTIVATE, OnViewBranches)
	ON_COMMAND(ID_CHANGELIST_ACTIVATE, OnViewChanges)
	ON_COMMAND(ID_CLIENT_ACTIVATE, OnViewClients)
	ON_COMMAND(ID_JOB_ACTIVATE, OnViewJobs)
	ON_COMMAND(ID_LABEL_ACTIVATE, OnViewLabels)
	ON_COMMAND(ID_SUBMITTED_ACTIVATE, OnViewSubmitted)
	ON_COMMAND(ID_USER_ACTIVATE, OnViewUsers)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_CLIENTVIEW, ID_VIEW_ENTIREVIEW, OnUpdateViewDepot)
	ON_COMMAND_RANGE(ID_VIEW_CLIENTVIEW, ID_VIEW_ENTIREVIEW, OnViewDepot)
	ON_WM_TIMER()
   	ON_UPDATE_COMMAND_UI(ID_JOB_NEW, OnUpdateJobNew)
    ON_UPDATE_COMMAND_UI(ID_BRANCH_NEW, OnUpdateBranchNew)
    ON_UPDATE_COMMAND_UI(ID_LABEL_NEW, OnUpdateLabelNew)
  	ON_COMMAND(ID_JOB_NEW, OnJobNew)
  	ON_COMMAND(ID_BRANCH_NEW, OnBranchNew)
  	ON_COMMAND(ID_LABEL_NEW, OnLabelNew)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_EDITMY, OnUpdateClientEditmy)
	ON_COMMAND(ID_CLIENT_EDITMY, OnClientEditmy)
	ON_UPDATE_COMMAND_UI(ID_USER_EDITMY, OnUpdateUserEditmy)
	ON_COMMAND(ID_USER_EDITMY, OnUserEditmy)
	ON_WM_SYSCOLORCHANGE()
	ON_UPDATE_COMMAND_UI(ID_FILE_ADD, OnUpdateFileAdd)
	ON_COMMAND(ID_FILE_ADD, OnFileAdd)
	ON_UPDATE_COMMAND_UI(ID_CLIENTSPEC_NEW, OnUpdateClientspecNew)
	ON_COMMAND(ID_CLIENTSPEC_NEW, OnClientspecNew)
	ON_UPDATE_COMMAND_UI(ID_SUBMITTED_ACTIVATE, OnUpdateSubmittedActivate)
	ON_UPDATE_COMMAND_UI(ID_USER_CREATENEWUSER, OnUpdateUserNew)
	ON_COMMAND(ID_USER_CREATENEWUSER, OnUserNew)
	ON_UPDATE_COMMAND_UI(ID_USER_PASSWORD, OnUpdateUserPassword)
	ON_COMMAND(ID_USER_PASSWORD, OnUserPassword)
	ON_UPDATE_COMMAND_UI(ID_MRU_PCU0, OnUpdateMruPcu)
	ON_COMMAND(ID_MRU_PCU0, OnMruPcu0)
	ON_COMMAND(ID_MRU_PCU1, OnMruPcu1)
	ON_COMMAND(ID_MRU_PCU2, OnMruPcu2)
	ON_COMMAND(ID_MRU_PCU3, OnMruPcu3)
	ON_COMMAND(ID_MRU_PCU4, OnMruPcu4)
	ON_COMMAND(ID_MRU_PCU5, OnMruPcu5)
	ON_COMMAND(ID_MRU_PCU6, OnMruPcu6)
	ON_COMMAND(ID_MRU_PCU7, OnMruPcu7)
	ON_COMMAND(ID_MRU_PCU8, OnMruPcu8)
	ON_COMMAND(ID_MRU_PCU9, OnMruPcu9)
	ON_UPDATE_COMMAND_UI(ID_MRU_PCU_HDR, OnUpdateMruPcuHdr)
	ON_COMMAND(ID_SHOWDELETEDFILES, OnShowDeletedFiles)
	ON_UPDATE_COMMAND_UI(ID_SHOWDELETEDFILES, OnUpdateShowDeletedFiles)
	ON_COMMAND(ID_SORTBYEXTENSION, OnSortByExtension)
	ON_UPDATE_COMMAND_UI(ID_SORTBYEXTENSION, OnUpdateSortByExtension)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WARNANDERR, OnUpdateViewWarnAndErrs)
	ON_COMMAND(ID_VIEW_WARNANDERR, OnViewWarnAndErrs)
	ON_UPDATE_COMMAND_UI(ID_POSITIONTOPATTERN, OnUpdatePositionToPattern)
	ON_COMMAND(ID_POSITIONTOPATTERN, OnPositionToPattern)
	ON_UPDATE_COMMAND_UI(ID_FINDNEXT, OnUpdatePositionToNext)
	ON_COMMAND(ID_FINDNEXT, OnPositionToNext)
	ON_UPDATE_COMMAND_UI(ID_FINDPREV, OnUpdatePositionToPrev)
	ON_COMMAND(ID_FINDPREV, OnPositionToPrev)
	ON_UPDATE_COMMAND_UI(ID_WINEXPLORE, OnUpdateWinExplore)
	ON_COMMAND(ID_WINEXPLORE, OnWinExplore)
	ON_UPDATE_COMMAND_UI(ID_CMDPROMPT, OnUpdateCmdPrompt)
	ON_COMMAND(ID_CMDPROMPT, OnCmdPrompt)
	ON_COMMAND(ID_CUSTOMIZETOOLS, OnCustomizeTools)
	ON_UPDATE_COMMAND_UI(ID_IMPORT, OnUpdateToolsImport)
	ON_COMMAND(ID_IMPORT, OnToolsImport)
	ON_UPDATE_COMMAND_UI(ID_EXPORT, OnUpdateToolsExport)
	ON_COMMAND(ID_EXPORT, OnToolsExport)
	ON_COMMAND(ID_PAGE_SETUP, OnPageSetup)
	ON_UPDATE_COMMAND_UI(ID_MAKE_DEFAULT_PCU, OnUpdateMakeDefaultPcu)
	ON_COMMAND(ID_MAKE_DEFAULT_PCU, OnMakeDefaultPcu)
	ON_WM_MENUSELECT()
	ON_UPDATE_COMMAND_UI(ID_VIEW_CLOSEDIALOGS, OnUpdateCloseAllModelessDlgs)
	ON_COMMAND(ID_VIEW_CLOSEDIALOGS, CloseAllModelessDlgs)
	ON_UPDATE_COMMAND_UI(ID_CHGS_BY_USER, OnUpdateViewChgsByUser)
	ON_COMMAND(ID_CHGS_BY_USER, OnViewChgsByUser)
	ON_UPDATE_COMMAND_UI(ID_CHGS_BY_CLIENT, OnUpdateViewChgsByClient)
	ON_COMMAND(ID_CHGS_BY_CLIENT, OnViewChgsByClient)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_DESCRIBE_2, OnUpdateDescribeChg)
	ON_COMMAND(ID_CHANGE_DESCRIBE_2, OnDescribeChg)
	ON_UPDATE_COMMAND_UI(ID_JOB_DESCRIBE_2, OnUpdateDescribeJob)
	ON_COMMAND(ID_JOB_DESCRIBE_2, OnDescribeJob)
	ON_UPDATE_COMMAND_UI(ID_BOOKMARKS, OnUpdateBookmarks)
	ON_COMMAND(ID_BOOKMARKS, OnBookmarks)
	ON_COMMAND(ID_ORGANIZE_BOOKMARKS, OnOrganizeBookmarks)
	ON_UPDATE_COMMAND_UI(ID_FAVORITES, OnUpdateFavorites)
	ON_UPDATE_COMMAND_UI(ID_ORGANIZE_FAVORITES, OnUpdateOrganizeFavorites)
	ON_COMMAND(ID_ORGANIZE_FAVORITES, OnOrganizeFavorites)
	ON_UPDATE_COMMAND_UI(ID_ADD_FAVORITE, OnUpdateAddFavorite)
	ON_COMMAND(ID_ADD_FAVORITE, OnAddFavorite)
	ON_UPDATE_COMMAND_UI(ID_LOGOUT, OnUpdateLogout)
	ON_COMMAND(ID_LOGOUT, OnLogout)
	ON_UPDATE_COMMAND_UI(ID_CHANGELIST_ACTIVATE, OnUpdateViewSwitch)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_ACTIVATE, OnUpdateViewSwitch)
	ON_UPDATE_COMMAND_UI(ID_JOB_ACTIVATE, OnUpdateViewSwitch)
	ON_UPDATE_COMMAND_UI(ID_LABEL_ACTIVATE, OnUpdateViewSwitch)
	ON_UPDATE_COMMAND_UI(ID_USER_ACTIVATE, OnUpdateViewSwitch)
	ON_UPDATE_COMMAND_UI(ID_MRU_PCU1, OnUpdateMruPcu)
	ON_UPDATE_COMMAND_UI(ID_MRU_PCU2, OnUpdateMruPcu)
	ON_UPDATE_COMMAND_UI(ID_MRU_PCU3, OnUpdateMruPcu)
	ON_UPDATE_COMMAND_UI(ID_MRU_PCU4, OnUpdateMruPcu)
	ON_UPDATE_COMMAND_UI(ID_MRU_PCU5, OnUpdateMruPcu)
	ON_UPDATE_COMMAND_UI(ID_MRU_PCU6, OnUpdateMruPcu)
	ON_UPDATE_COMMAND_UI(ID_MRU_PCU7, OnUpdateMruPcu)
	ON_UPDATE_COMMAND_UI(ID_MRU_PCU8, OnUpdateMruPcu)
	ON_UPDATE_COMMAND_UI(ID_MRU_PCU9, OnUpdateMruPcu)
	ON_WM_ACTIVATE()
	ON_WM_ACTIVATEAPP()
	ON_WM_PARENTNOTIFY()
	ON_UPDATE_COMMAND_UI_RANGE(ID_TOOL_1, ID_TOOL_64, OnUpdateTool)
	ON_COMMAND_RANGE(ID_TOOL_1, ID_TOOL_64, OnTool)
	ON_UPDATE_COMMAND_UI_RANGE(ID_BKMK_1, ID_BKMK_128, OnUpdateBkMk)
	ON_COMMAND_RANGE(ID_BKMK_1, ID_BKMK_128, OnBkMk)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FAV_1, ID_FAV_64, OnUpdateBkMk)
	ON_COMMAND_RANGE(ID_FAV_1, ID_FAV_64, OnFav)
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelpFinder)			// should be CFrameWnd::OnHelp
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnHelpFinder)	// should be CFrameWnd::OnContextHelp
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP_GETTINGSTARTEDWITHP4WIN, OnGettingStartedWithP4win)
	ON_MESSAGE(WM_P4INFO, OnP4InitInfo )
	ON_MESSAGE(WM_STATUSADD, OnStatusAdd )
	ON_MESSAGE(WM_STATUSADDARRAY, OnStatusAddArray )
	ON_MESSAGE(WM_STATUSCLEAR, OnStatusClear )
	ON_MESSAGE(WM_CLIENTERROR, OnClientError )
	ON_MESSAGE(WM_P4LISTOPSTAT, OnP4ListOp )
	ON_MESSAGE(WM_P4FILEINFORMATION, OnP4FileInformation )
	ON_MESSAGE(WM_P4DIFF, OnP4Diff )
	ON_MESSAGE(WM_NEWCLIENT, OnNewClient )
	ON_MESSAGE(WM_NEWUSER, OnNewUser )
	ON_MESSAGE(WM_USERPSWDDLG, OnUserPasswordDlg )
	ON_MESSAGE(WM_BROWSECALLBACK1, OnBrowseClientsCallBack)
	ON_MESSAGE(WM_P4DLGDESTROY, OnP4DialogDestroy)
	ON_MESSAGE(WM_HELPERAPP, OnP4HelperApp)
	ON_MESSAGE(WM_PERFORCE_OPTIONS, OnP4OptionsDlg)
	ON_MESSAGE(WM_ACTIVATEMODELESS, OnActivateModeless)
	ON_REGISTERED_MESSAGE(wmActivate, OnExternalActivation )
	ON_REGISTERED_MESSAGE(wmSendcmd,  OnExternalSendcmd )
	ON_REGISTERED_MESSAGE(wmRplycmd,  OnExternalRplycmd )
    ON_REGISTERED_MESSAGE( WM_FINDREPLACE, OnFindReplace )
	ON_COMMAND(ID_NEW, OnNew)
	ON_COMMAND(ID_FILE_REVISIONHISTORYCONT, OnFileRevisionhistory)
	ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnDropDown)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_CURRENTTASK,
};

CWnd *MainFrameCWnd = 0;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_FirstActivation=TRUE;
	m_FullRefreshRequired= TRUE;
	m_pStatusView= NULL;
	m_DoNotAutoPollCtr=0;
	m_Timer=0;
	m_LastUpdateTime=0;
    m_LastUpdateResult=UPDATE_SUCCESS;
	m_ClientError= FALSE;
	m_ModlessUp = FALSE;
	m_HasDisabled = FALSE;
	m_InPopUpMenu = FALSE;
	m_GetCliRootAndContinue = 0;
	m_Need2ExpandDepot = m_Need2Poll4Jobs = 0;
	m_P4QTree = m_P4TLV = 0;
	m_StatusBarWarnLevel = SV_MSG;
	m_CF_FILENAME = RegisterClipboardFormat(_T("FileName"));
	m_CF_FILENAMEW = RegisterClipboardFormat(_T("FileNameW"));
	m_hDevNames = NULL;
	m_hDevMode  = NULL;
	m_rtMargin.top = m_rtMargin.bottom = m_rtMargin.right = m_rtMargin.left = 0;
	m_rtMinMargin.top = m_rtMinMargin.bottom = m_rtMinMargin.right = m_rtMinMargin.left = 0;
	m_bMetric = m_Quitting = FALSE;
	m_IsActive = m_GotInput = TRUE;
	m_MenuBmpCtr = 0;
	wmActivate= TheApp()->m_WM_ACTIVATE;
	wmSendcmd = TheApp()->m_WM_SENDCMD;
	wmRplycmd = TheApp()->m_WM_RPLYCMD;
	m_StatusUpdateInterval = (GET_P4REGPTR()->GetStatusUpdateInterval()+1)/2;
	m_pFRDlg = NULL;
	m_FindWhatFlags = FR_DOWN | FR_HIDEWHOLEWORD;
	m_FindStatusFlags = FR_HIDEWHOLEWORD;
	m_Need2RefreshOldChgs = TRUE;
	m_bNoRefresh = m_ShowOnlyNotInDepot = FALSE;
	m_DeltaUpdateTime = m_LabelUpdateTime = m_BranchUpdateTime = m_UserUpdateTime 
					  = m_ClientUpdateTime = m_JobUpdateTime = m_OldChgUpdateTime = 0;
	m_ReviewListClass = AfxRegisterWndClass(NULL);
}

CMainFrame::~CMainFrame()
{
	delete m_pDropTgt;
	if (m_USER32dll)
		FreeLibrary(m_USER32dll);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Initialize the critical section one time only.
    if (!InitializeCriticalSectionAndSpinCount(&CriticalSection, 0x80000400) ) 
        return -1;

	m_IconsInMenus = GET_P4REGPTR()->IconsInMenus();
	m_osVer.dwOSVersionInfoSize= sizeof(OSVERSIONINFO);
	GetVersionEx(&m_osVer);

	// Determin number of display devices attached to the desktop
	// if there is only one monitor, we can check for resonable placements of dialogs, etc.
	// note that GetSystemMetrics(SM_CMONITORS) will return 0 for Win/95 & NT
	m_NbrDisplays = max(1, GetSystemMetrics(SM_CMONITORS));

	// give all the views the needed pointers to each other
	SetWindowReferences();

	// Set icon to use for small icons
	if ((m_osVer.dwMajorVersion < 5) 
	 || (m_osVer.dwMajorVersion == 5 && m_osVer.dwMinorVersion < 1))
		SetIcon(AfxGetApp()->LoadIcon(IDI_ICONSM), FALSE);

	// Set the main menu style, etc.
	if (m_osVer.dwMajorVersion >= 5)
	{
		MENUINFO cmi;
		cmi.cbSize = sizeof(cmi);
		cmi.fMask = MIM_APPLYTOSUBMENUS | MIM_STYLE;
		cmi.dwStyle = MNS_CHECKORBMP;
		m_USER32dll = LoadLibrary(_T("USER32.dll"));
		if (m_USER32dll)
		{
			typedef int (PASCAL *SETMENUINFO)(HMENU hMenu, LPCMENUINFO lpcmi);
			SETMENUINFO setmenuinfo = (SETMENUINFO)GetProcAddress(m_USER32dll, "SetMenuInfo");
			if  (setmenuinfo)
				(setmenuinfo)(GetMenu()->m_hMenu, &cmi);
		}
	}

	//		do the toolbar thing
	//		Set the check button style for the different right-pane views
	//
	if (!m_wndToolBar.CreateEx(this))//||
	{
		XTRACE(_T("Failed to create toolbar\n"));
		return -1;
	}

	m_wndToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);

	m_wndToolBar.GetToolBarCtrl().SetImageList(
		TheApp()->GetToolBarImageList());
	m_wndToolBar.GetToolBarCtrl().SetDisabledImageList(
		TheApp()->GetToolBarImageList()->GetDisabled());

	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT|TBSTYLE_TRANSPARENT);

	CSize sizeImage(18, 16);
	CSize sizeButton(18 + 7, 16 + 7);
	m_wndToolBar.SetSizes(sizeButton, sizeImage);

	const UINT ddStyle = TBSTYLE_DROPDOWN | BTNS_WHOLEDROPDOWN;

	// This struct contain one entry for each toolbar button
	// in the same order as the buttons are to appear
	// If you add items to this struct, you Must also
	// increase the dimensions of m_MenuIDbm & m_MenuBitmap
	// by the same amount
	static struct btn
	{
		UINT id;
		UINT style;
		int image;
	} btns[] =
	{
	{ID_FILE_GET,			TBBS_BUTTON,	CP4WinToolBarImageList::TBI_SYNC},
	{ID_FILE_OPENEDIT,		TBBS_BUTTON,	CP4WinToolBarImageList::TBI_EDIT},
	{ID_FILE_ADD,			TBBS_BUTTON,	CP4WinToolBarImageList::TBI_ADD},
	{ID_SEPARATOR,			TBSTYLE_SEP,	0},

	{ID_FILE_OPENDELETE,	TBBS_BUTTON,	CP4WinToolBarImageList::TBI_DELETE},
	{ID_SEPARATOR,			TBSTYLE_SEP,	0},

	{ID_FILE_REVERT,		TBBS_BUTTON,	CP4WinToolBarImageList::TBI_REVERT},
	{ID_FILE_LOCK,			TBBS_BUTTON,	CP4WinToolBarImageList::TBI_LOCK},
	{ID_FILE_UNLOCK,		TBBS_BUTTON,	CP4WinToolBarImageList::TBI_UNLOCK},
	{ID_SEPARATOR,			TBSTYLE_SEP,	0},

	{ID_BOOKMARKS,			ddStyle,		CP4WinToolBarImageList::TBI_BOOKMARKS},
	{ID_FAVORITES,			ddStyle,		CP4WinToolBarImageList::TBI_CONNECTIONS},
	{ID_SEPARATOR,			TBSTYLE_SEP,	0},

	{ID_CHANGE_SUBMIT,		TBBS_BUTTON, 	CP4WinToolBarImageList::TBI_SUBMIT},
	{ID_SEPARATOR,			TBSTYLE_SEP,	0},

	{ID_FILE_REVISIONHISTORY,TBBS_BUTTON,	CP4WinToolBarImageList::TBI_HISTORY},
	{ID_FILE_DIFFHEAD,		TBBS_BUTTON,	CP4WinToolBarImageList::TBI_DIFF},
	{ID_FILE_QUICKBROWSE,	TBBS_BUTTON,	CP4WinToolBarImageList::TBI_QUICKBROWSE},
	{ID_FILE_QUICKEDIT,		TBBS_BUTTON,	CP4WinToolBarImageList::TBI_QUICKEDIT},
	{ID_SEPARATOR,			TBSTYLE_SEP,	0},

	{ID_CHANGELIST_ACTIVATE,TBBS_CHECKBOX,	CP4WinToolBarImageList::TBI_PENDING},
	{ID_SUBMITTED_ACTIVATE, TBBS_CHECKBOX,	CP4WinToolBarImageList::TBI_SUBMITTED},
	{ID_BRANCH_ACTIVATE,	TBBS_CHECKBOX,	CP4WinToolBarImageList::TBI_BRANCH},
	{ID_LABEL_ACTIVATE,		TBBS_CHECKBOX,	CP4WinToolBarImageList::TBI_LABEL},
	{ID_CLIENT_ACTIVATE,	TBBS_CHECKBOX,	CP4WinToolBarImageList::TBI_CLIENT},
	{ID_USER_ACTIVATE,		TBBS_CHECKBOX,	CP4WinToolBarImageList::TBI_USER},
	{ID_JOB_ACTIVATE,		TBBS_CHECKBOX,	CP4WinToolBarImageList::TBI_JOB},
	{ID_SEPARATOR,			TBSTYLE_SEP,	0},

	{ID_CANCEL_BUTTON,		TBBS_BUTTON,	CP4WinToolBarImageList::TBI_CANCELBUTTON},
	{ID_PERFORCE_OPTIONS,	TBBS_BUTTON,	CP4WinToolBarImageList::TBI_SETTINGS},
	{ID_PERFORCE_INFO,		TBBS_BUTTON,	CP4WinToolBarImageList::TBI_INFO},
	};
	const int buttonCount = sizeof(btns)/sizeof(btn);
	m_wndToolBar.SetButtons(NULL, buttonCount);

	MENUITEMINFO mii;
	CWindowDC dc(this);
	CDC memdc;
	memdc.CreateCompatibleDC(&dc);
	CBrush brush, brushDis;
	brush.CreateSysColorBrush(COLOR_MENU);
	brushDis.CreateStockObject(WHITE_BRUSH);
	RECT rect = {0, 0, 20, 16};
	CImageList * disabledmenu = TheApp()->GetToolBarImageList()->GetDisabledMenu();
	for(int b = 0; b < buttonCount; b++)
	{
		m_wndToolBar.SetButtonInfo(b, btns[b].id, btns[b].style, btns[b].image);
		if (btns[b].style != TBSTYLE_SEP && m_osVer.dwMajorVersion >= 5 && m_IconsInMenus)
		{
			HICON hIcon = TheApp()->GetToolBarImageList()->ExtractIcon(btns[b].image);
			HICON hIdis = disabledmenu ? disabledmenu->ExtractIcon(btns[b].image) : hIcon;
			if (hIcon)
			{
				// 'm_MenuBmpCtr' is the index of the next empty CBitmap
				// draw the icon on that CBitmap
				m_MenuBitmap[m_MenuBmpCtr].CreateCompatibleBitmap(&dc, 20, 16);
				CBitmap *poldbm = memdc.SelectObject(&m_MenuBitmap[m_MenuBmpCtr]);
				memdc.FillRect(&rect, &brush);
				::DrawIconEx(memdc.m_hDC, 0, 0, hIcon,
					GetSystemMetrics(SM_CXMENUCHECK), // cx
			        GetSystemMetrics(SM_CYMENUCHECK), // cy
					0, brush, DI_NORMAL);         // frame, brush, flags
				if (hIdis)
				{
					m_MenuBitDis[m_MenuBmpCtr].CreateBitmap(20, 16, 1, 1, NULL);
					memdc.SelectObject(&m_MenuBitDis[m_MenuBmpCtr]);
					memdc.FillRect(&rect, &brushDis);
					::DrawIconEx(memdc.m_hDC, 0, 0, hIdis,
						GetSystemMetrics(SM_CXMENUCHECK), // cx
						GetSystemMetrics(SM_CYMENUCHECK), // cy
						0, brush, DI_NORMAL);         // frame, brush, flags
					m_HasDisabled = TRUE;
				}
				memdc.SelectObject(poldbm);
				// Attach the icon to the menu item
				// we do this here even tho some - but not all -
				// items will have the bitmap reattached by SetMenuIcon()
				memset(&mii, _T('\0'), sizeof(mii));
				mii.cbSize = sizeof(mii);
				mii.fMask  = MIIM_BITMAP;
				mii.hbmpItem = HBITMAP(m_MenuBitmap[m_MenuBmpCtr]);
				// Now save the associated menu ID in the corresponding 'm_MenuBmpCtr'
				UINT id;
				switch(btns[b].id)
				{
				case ID_BOOKMARKS:
					{
					m_MenuIDbm[m_MenuBmpCtr++] = 0xFFFFFFF1;
					CMenu* pSubMenu = GetMenu()->GetSubMenu(2);
					pSubMenu->SetMenuItemInfo(pSubMenu->GetMenuItemCount()-4, &mii, TRUE);
					continue;
					}
				case ID_FAVORITES:
					{
					m_MenuIDbm[m_MenuBmpCtr++] = 0xFFFFFFF2;
					CMenu* pSubMenu = GetMenu()->GetSubMenu(3);
					pSubMenu->SetMenuItemInfo(pSubMenu->GetMenuItemCount()-3, &mii, TRUE);
					continue;
					}
				case ID_CANCEL_BUTTON:
					id = ID_FILE_CANCEL;
					break;
				default:
					id = btns[b].id;
				}
				m_MenuIDbm[m_MenuBmpCtr++] = id;
				// Do the actual attach to the main menu
				GetMenu()->SetMenuItemInfo(id, &mii);
				// We have to also possibly attach it to
				// some submenu(s) if the ID occurs in
				// multiple dropdowns
				for (int n = 3; ++n < 10; )
					GetMenu()->GetSubMenu(n)->SetMenuItemInfo(id, &mii);
			}
		}
	}
	// a 0 ID indicates the end of the in use 'm_MenuIDbm's
	m_MenuIDbm[m_MenuBmpCtr++] = 0;

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar, AFX_IDW_DOCKBAR_TOP);

	m_pDropTgt = new CTBDropTarget();
	m_pDropTgt->Register(&m_wndToolBar);

	if (!(GET_P4REGPTR( )->ShowToolBar()))
		SendMessage(WM_COMMAND, ID_VIEW_TOOLBAR, 0);

	// Add remaining menu icons
	//
	if (m_osVer.dwMajorVersion >= 5 && m_IconsInMenus)
	{
		memset(&mii, _T('\0'), sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask  = MIIM_BITMAP;
		mii.hbmpItem = HBMMENU_POPUP_CLOSE;
		GetMenu()->SetMenuItemInfo(ID_APP_EXIT, &mii);
	}

	//		now the status bar stuff
	//
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		XTRACE(_T("Failed to create status bar\n"));
		return -1;
	}

	//		Load the Tools Menu
	//
	LoadToolsMenu();
	
	//		Load the Bookmarks Menu
	//
	LoadBkMkMenu();
	
	//		Load the Favorites Menu
	//
	LoadFavMenu();
	
	//		Set the active views (depot on the left, pending changelists
	//		on the right) and their captions.
	//
	OnViewChanges();
	int iEntire = GET_P4REGPTR()->ShowEntireDepot();
	SetDepotCaption( iEntire );
	
	SetActiveView(m_pDepotView);

	MainFrameCWnd = this;

	//	This timer is here to support auto-update and miscellaneous
	//  tasks.  Will result in OnTimer() being called every .2 secs.  
	//  The interval is a compromise between 
	//  1) wasting lots of CPU in OnTimer and 
	//  2) having a fairly quick response when one of the right-side 
	//     panes needs to be loaded for the first time or a flyover
	//	   message needs to go away
	//
	m_Timer= SetTimer( UPDATE_TIMER, 200, NULL);

	TCHAR cmdLine[] = _T("P4V.exe");
	m_P4Vver = GetExeVersion(cmdLine);
	m_P4QTree = m_P4TLV = m_P4Vver >= 20042 ? 1 : 0;

	if (!m_P4QTree)
		GetMenu()->DeleteMenu(ID_FILE_REVISIONTREE, MF_BYCOMMAND);

	if (!m_P4TLV)
		GetMenu()->DeleteMenu(ID_FILE_ANNOTATIONS, MF_BYCOMMAND);
#if 0
	else
	{
		int j = GetMenu()->GetSubMenu(0)->GetMenuItemCount();
		for (int i=-1; ++i < j; )
		{
			if (GetMenu()->GetSubMenu(0)->GetMenuItemID(i) == ID_FILE_AUTOEDIT)
				GetMenu()->GetSubMenu(0)->DeleteMenu(i+1, MF_BYPOSITION);
		}
	}
#endif

	if (!GET_P4REGPTR()->GetEnableSubChgIntegFilter())
		GetMenu()->DeleteMenu(ID_JOB_SETFILEFILTERINTEG, MF_BYCOMMAND);

	// are we running only to show a rev hist dialog box?
	if (!TheApp()->m_RevHistPath.IsEmpty())
	{
		// Run p4 client to get the server version
		CCmd_Describe *pCmd = new CCmd_Describe;
		pCmd->Init( NULL, RUN_SYNC );
		BOOL cmdStarted= pCmd->Run( P4CLIENT_SPEC, GET_P4REGPTR()->GetP4Client() );
		if(cmdStarted && !pCmd->GetError())
		{
			// since we have the client spec, set Client root in app
			TheApp()->SetClientRoot(TheApp()->GetClientSpecField( _T("Root"), pCmd->GetDescription()));
			if (GET_SERVERLEVEL() >= 22)
		        TheApp()->SetClientSubOpts(TheApp()->GetClientSpecField( _T("SubmitOptions"), pCmd->GetDescription()));
		}
		// now run the rev hist command
		OnFileRevisionhistory();
		delete pCmd;
	}
	else if (!TheApp()->m_FileInfoPath.IsEmpty())
	{
		m_StringList.RemoveAll();
		m_StringList.AddHead(TheApp()->m_FileInfoPath);
		
		CCmd_Opened *pCmd= new CCmd_Opened;
		pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, 0);
		pCmd->SetAlternateReplyMsg( WM_P4FILEINFORMATION );

		if( pCmd->Run( TRUE, FALSE, -1, &m_StringList ) )
			UpdateStatus( LoadStringResource(IDS_REQUESTING_FILE_INFORMATION) );
		else
		{
			delete pCmd;
			::PostMessage(m_hWnd, WM_COMMAND, ID_APP_EXIT, 0);
		}
	}
	else if (!TheApp()->m_DiffPath.IsEmpty())
	{
		// Run p4 client to get the server version
		CCmd_Describe *pCmd1 = new CCmd_Describe;
		pCmd1->Init( NULL, RUN_SYNC );
		BOOL cmdStarted= pCmd1->Run( P4CLIENT_SPEC, GET_P4REGPTR()->GetP4Client() );
		if(cmdStarted && !pCmd1->GetError())
		{
			// since we have the client spec, set Client root in app
			TheApp()->SetClientRoot(TheApp()->GetClientSpecField( _T("Root"), pCmd1->GetDescription()));
			if (GET_SERVERLEVEL() >= 22)
		        TheApp()->SetClientSubOpts(TheApp()->GetClientSpecField( _T("SubmitOptions"), pCmd1->GetDescription()));
		}
		// now run the diff command
		m_StringList.RemoveAll();
		m_StringList.AddHead(LPCTSTR(TheApp()->m_DiffPath));

		CCmd_Diff *pCmd2= new CCmd_Diff;
		pCmd2->Init( m_hWnd, RUN_ASYNC);
		if( pCmd2->Run( &m_StringList, _T("-f"), '\0', TRUE ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_DIFFING_FILE) );
		else
		{
			delete pCmd2;
			CString txt;
			txt.FormatMessage(IDS_CANTDIFF_s, TheApp()->m_DiffPath);
			AfxMessageBox( txt,  MB_ICONSTOP );
			::PostMessage(m_hWnd, WM_COMMAND, ID_APP_EXIT, 0);
		}
	}
	else if (!TheApp()->m_ToolsImportPath.IsEmpty())
	{
		CString errorTxt;
		ImportTools(&(TheApp()->m_ToolsImportPath), &errorTxt);
		if(errorTxt.GetLength() > 0)
			AfxMessageBox(errorTxt, MB_ICONSTOP);
		::PostMessage(m_hWnd, WM_COMMAND, ID_APP_EXIT, 0);
	}
#if 0
	else if (GET_P4REGPTR()->Is1stRun())
	{
			// this is the 1st time P4Win has been run on this machine
			// so don't fire an update; wait until after the dialog
			// asking 1st-timer info has been shown.
	}
#endif
	else	// normal operation
	{
		//	Run the initial update of depot and changelist panes
		UpdateDepotandChangeViews(NO_REDRILL);
		m_Need2ExpandDepot = GET_P4REGPTR()->GetExpandFlag();
	}
	return 0;
}

void CMainFrame::OnFileRevisionhistory() 
{
	// split off revision number, if any
	int i, rev = -1;
	CString itemStr= TheApp()->m_RevHistPath;
	if ((i = itemStr.Find(_T('#'))) != -1)
	{
		rev = _ttoi(itemStr.Right(itemStr.GetLength() - i - 1));
		if (!rev)
			 rev = -1;
		itemStr = itemStr.Left(i);
	}

	CString depotPath = itemStr;
	if (depotPath.GetAt(1) == _T(':'))	// local syntax must be converted to depot syntax
	{
		CCmd_Where *pCmd1 = new CCmd_Where;
		pCmd1->Init(NULL, RUN_SYNC);
		if ( pCmd1->Run(itemStr) && !pCmd1->GetError() && pCmd1->GetDepotFiles()->GetCount() )
		{
			depotPath = pCmd1->GetDepotSyntax();
		}
		delete pCmd1;
	}
	
	// initialize the history command (which is p4 filelog)
	CCmd_History *pCmd= new CCmd_History;
	pCmd->Init(GetDepotWnd(), RUN_ASYNC);
	pCmd->SetCallingWnd(m_hWnd);
	pCmd->SetInitialRev(rev, depotPath);

	// handle any -m flag
	if (TheApp()->m_RevHistCount)
	{
		pCmd->OverrideFetchHistCount(TheApp()->m_RevHistCount);
		pCmd->OverrideFetchCompleteHist(0);
		pCmd->SetEnableShowIntegs(FALSE);
		TheApp()->m_RevHistEnableShowIntegs = FALSE;
	}

	// Run the filelog command to get the history info
	if( pCmd->Run( LPCTSTR(itemStr)) )
	{
		UpdateStatus( LoadStringResource(IDS_REQUESTING_HISTORY) );
	}
	else
	{
		delete pCmd;
		CString txt;
		txt.FormatMessage(IDS_CANTRUNREVHIST_s, TheApp()->m_RevHistPath);
		AfxMessageBox( txt, MB_ICONSTOP );
		::PostMessage(m_hWnd, WM_COMMAND, ID_APP_EXIT, 0);
	}
}

LRESULT CMainFrame::OnP4FileInformation( WPARAM wParam, LPARAM lParam )
{
	CCmd_Opened *pCmd= (CCmd_Opened *) wParam;
	
	m_StringList.RemoveAll();
	if (pCmd->GetError())
	{
		CString txt;
		txt.FormatMessage(IDS_CANTRUNFILEINFO_s, TheApp()->m_FileInfoPath);
		AfxMessageBox( txt, MB_ICONSTOP );
	}
	else
	{
		CString thisuser=GET_P4REGPTR()->GetMyID();
				
		// Initialize the file info dialog
		CFileInfoDlg dlg;

		dlg.m_DepotPath = dlg.m_ClientPath = TheApp()->m_FileInfoPath;

		int key= pCmd->GetServerKey();
		CCmd_Fstat *pCmd2= new CCmd_Fstat;
		
		pCmd2->Init(NULL, RUN_SYNC, HOLD_LOCK, key);

		if (!IsQuitting())
		{
			MSG msg;
			while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				//		get out if app is terminating
				//
				if ( msg.message == WM_QUIT )
					goto CantGetFStat;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		pCmd2->SetIncludeAddedFiles( TRUE );
		if( pCmd2->Run( FALSE, TheApp()->m_FileInfoPath, 0 ) && !pCmd2->GetError() )
		{
			CObList *list = pCmd2->GetFileList ( );
			ASSERT_KINDOF( CObList, list );
			ASSERT( list->GetCount() <= 1 );
			POSITION pos = list->GetHeadPosition( );
			if( pos != NULL )
			{
				CP4FileStats *stats = ( CP4FileStats * )list->GetNext( pos );
				ASSERT_KINDOF( CP4FileStats, stats );
				dlg.m_DepotPath = stats->GetFullDepotPath();
				if(dlg.m_DepotPath.GetLength() == 0)
					dlg.m_DepotPath= LoadStringResource(IDS_NOT_IN_CLIENT_VIEW);
				dlg.m_ClientPath = stats->GetFullClientPath( );
				if(dlg.m_ClientPath.GetLength() == 0)
					dlg.m_ClientPath= LoadStringResource(IDS_NOT_IN_CLIENT_VIEW);
				
				dlg.m_HeadRev.Format(_T("%ld"), stats->GetHeadRev());
				dlg.m_HaveRev.Format(_T("%ld"), stats->GetHaveRev());
				
				dlg.m_HeadAction= stats->GetActionStr(stats->GetHeadAction());
				dlg.m_HeadChange.Format(_T("%ld"), stats->GetHeadChangeNum());
				dlg.m_HeadType= stats->GetHeadType();
				dlg.m_ModTime= stats->GetFormattedHeadTime();
				dlg.m_FileSize= stats->GetFileSize();

				// Check for open/lock by this user
				if(stats->IsMyLock())
					dlg.m_LockedBy= thisuser;
				
				delete stats;
			}		
			else dlg.m_ClientPath= LoadStringResource(IDS_NOT_IN_CLIENT_VIEW);
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
				str += " " + LoadStringResource(IDS_STAR_LOCKED);
			
			dlg.m_StrList.AddHead( str );
			
			delete fs;
		}
		// Display the info
		SET_APP_HALTED(TRUE);
		dlg.DoModal();  
		SET_APP_HALTED(FALSE);
	}

	UpdateStatus(_T(""));
	delete pCmd;
	::PostMessage(m_hWnd, WM_COMMAND, ID_APP_EXIT, 0);
	
	return 0;
}

LRESULT CMainFrame::OnP4Diff(WPARAM wParam, LPARAM lParam)
{
	CCmd_Diff *pCmd= (CCmd_Diff *) wParam;

	if( pCmd->GetDiffErrCount() )
	{
		AfxMessageBox( pCmd->GetDiffErrBuf(),  MB_ICONSTOP );
	}
	else if( !pCmd->GetError() && pCmd->GetDiffRunCount() == 0)
	{
		if (pCmd->GetDiffNbrFiles() == 1)
        {
            CString msg;
            msg.FormatMessage(IDS_CLIENTFILE_s_DOESNOTDIFFER, pCmd->GetDiffFileName());
			AfxMessageBox(msg, MB_OK);
        }
		else if (pCmd->GetDiffErrCount() == 0)
			AfxMessageBox(LoadStringResource(IDS_NONE_OF_THE_SELECTED_CLIENT_FILES_DIFFER), MB_OK);
	}
	else if (pCmd->GetError())
	{
		CString txt;
		txt.FormatMessage(IDS_CANTDIFF_s, TheApp()->m_DiffPath);
		AfxMessageBox( txt,  MB_ICONSTOP );
	}
	ClearStatus();         
	delete pCmd;
	::PostMessage(m_hWnd, WM_COMMAND, ID_APP_EXIT, 0);
	return 0;
}

/*
	_________________________________________________________________

	Create a static splitter with 2 horizontal rows, the bottom for 
	the output (or 'status'), the top to be divided 2 ways,
	the left for the depot, the right for all the others 
	(changelist, branch, etc.).
	
	okay, ideally, i should have a dockable window for the output 
	(still not supported by mfc), and the rest... hmm. maybe MDI? 
	all dockable? dynamically split? ah, the choices...

	and why does the left always have to be the depot?

	and why not have one for the client workspace?
	_________________________________________________________________
*/

BOOL CMainFrame::OnCreateClient( LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	if ( !m_wndHSplitter.CreateStatic(	this, 2, 1,
									WS_CHILD | WS_VISIBLE, 
									AFX_IDW_PANE_FIRST ) )
		return FALSE;   
  	
	// Lowest splitter pane contains status view
	m_wndHSplitter.CreateView( 1,0, RUNTIME_CLASS(CStatusView),
		CSize(50,10), pContext );

	// Create a static splitter, w/ 2 columns, in the top pane
	m_wndVSplitter.CreateStatic(	
		&m_wndHSplitter, 
		1, 2,
		WS_CHILD | WS_VISIBLE | WS_BORDER,  // intuitively, WS_BORDER is mandatory (yeah, right)
		m_wndHSplitter.IdFromRowCol(0,0) );
	
	m_wndVSplitter.CreateView( 0,0, RUNTIME_CLASS(CDepotView),
		CSize(50,10), pContext );

	m_wndVSplitter.CreateView( 0, 1, RUNTIME_CLASS(CDeltaView),
		CSize(50,10), pContext );

	m_pStatusView= (CStatusView *) m_wndHSplitter.GetPane(1,0);
	m_pDepotView=(CDepotView *) m_wndVSplitter.GetPane(0,0);
	m_pDeltaView= (CDeltaView *) m_wndVSplitter.GetPane(0,1);

	m_currentTab=0;
	m_pRightView = m_pDeltaView;

	// Right pane also has a few views that are swapped in
	DWORD style=AFX_WS_DEFAULT_VIEW;
	style &= ~WS_VISIBLE;

	m_pLabelView= new CLabelView;
	m_pLabelView->Create(NULL, NULL, style,
            rectDefault, &m_wndVSplitter, MAKEID(1), pContext);

	m_pBranchView= new CBranchView;
	m_pBranchView->Create(NULL, NULL, style,
            rectDefault, &m_wndVSplitter, MAKEID(2), pContext);

	m_pUserView= new CUserView;
	m_pUserView->Create(NULL, NULL, style,
            rectDefault, &m_wndVSplitter, MAKEID(3), pContext);
	
	m_pClientView= new CClientView;
	m_pClientView->Create(NULL, NULL, style,
            rectDefault, &m_wndVSplitter, MAKEID(4), pContext);

	m_pJobView= new CJobView;
	m_pJobView->Create(NULL, NULL, style,
            rectDefault, &m_wndVSplitter, MAKEID(5), pContext);

	m_pOldChgView= new COldChgView;
	m_pOldChgView->Create(NULL, NULL, style,
            rectDefault, &m_wndVSplitter, MAKEID(6), pContext);

	return TRUE;
}

void CMainFrame::SetWindowReferences()
{
	// Make sure the various views know where each other are, for msg routing
	m_pDepotView->SetBranchWnd(m_pBranchView);
	m_pDepotView->SetChangeWnd(m_pDeltaView);
	m_pDepotView->SetOldChgWnd(m_pOldChgView);
	m_pDepotView->SetLabelWnd(m_pLabelView);
	m_pDepotView->SetClientWnd(m_pClientView);
	m_pDepotView->SetUserWnd(m_pUserView);
    m_pDepotView->SetJobWnd(m_pJobView);
	m_pBranchView->SetDepotWnd(m_pDepotView);
	m_pBranchView->SetChangeWnd(m_pDeltaView);
	m_pClientView->SetDepotWnd(m_pDepotView);
	m_pDeltaView->SetDepotWnd(m_pDepotView);
    m_pDeltaView->SetJobWnd(m_pJobView);
	m_pDeltaView->SetOldChgWnd(m_pOldChgView);
	m_pJobView->SetDepotWnd(m_pDepotView);
	m_pJobView->SetChangeWnd(m_pDeltaView);
	m_pLabelView->SetDepotWnd(m_pDepotView);
	m_pOldChgView->SetDepotWnd(m_pDepotView);
	m_pOldChgView->SetJobWnd(m_pJobView);
	m_pUserView->SetDepotWnd(m_pDepotView);
	m_pUserView->SetClientWnd(m_pClientView);
	m_pUserView->SetBranchWnd(m_pBranchView);
	m_pUserView->SetLabelWnd(m_pLabelView);
}

void CMainFrame::SetRightSplitter(int tabNumber)
{
	ASSERT(IsWindow(m_wndToolBar.m_hWnd));  // Is toolbar a window yet?
	CToolBarCtrl& toolbar= (CToolBarCtrl&) m_wndToolBar.GetToolBarCtrl( );
  
	CMenu *pMenu= GetMenu();
	ASSERT(pMenu != NULL);

	RECT rect;
	m_pRightView->GetClientRect(&rect);
	m_pRightView->SetDlgCtrlID( MAKEID(m_currentTab));
	m_pRightView->ShowWindow(SW_HIDE);
	switch(m_currentTab)
	{
	case 0:
		m_pDeltaView->GetTreeCtrl().ShowWindow(SW_HIDE);
		toolbar.CheckButton(ID_CHANGELIST_ACTIVATE, FALSE);
		break;
	case 1:
		m_pLabelView->GetListCtrl().ShowWindow(SW_HIDE);
		toolbar.CheckButton(ID_LABEL_ACTIVATE, FALSE);
		break;
	case 2:
		m_pBranchView->GetListCtrl().ShowWindow(SW_HIDE);
		toolbar.CheckButton(ID_BRANCH_ACTIVATE, FALSE);
		break;
	case 3:
		m_pUserView->GetListCtrl().ShowWindow(SW_HIDE);
		toolbar.CheckButton(ID_USER_ACTIVATE, FALSE);
		break;
	case 4:
		m_pClientView->GetListCtrl().ShowWindow(SW_HIDE);
		toolbar.CheckButton(ID_CLIENT_ACTIVATE, FALSE);
		break;
	case 5:
		m_pJobView->GetListCtrl().ShowWindow(SW_HIDE);
		toolbar.CheckButton(ID_JOB_ACTIVATE, FALSE);
		break;
	case 6:
		m_pOldChgView->GetListCtrl().ShowWindow(SW_HIDE);
		toolbar.CheckButton(ID_SUBMITTED_ACTIVATE, FALSE);
		break;
	default:
		ASSERT(0);	

	}

	m_currentTab=tabNumber;
	switch(tabNumber)
	{
	case 0:
		m_pRightView = m_pDeltaView;
		m_pDeltaView->GetTreeCtrl().ShowWindow(SW_SHOW);
		toolbar.CheckButton(ID_CHANGELIST_ACTIVATE, TRUE);
		break;
	case 1:
		m_pRightView = m_pLabelView;
		m_pLabelView->GetListCtrl().ShowWindow(SW_SHOW);
		toolbar.CheckButton(ID_LABEL_ACTIVATE, TRUE);
		break;
	case 2:
		m_pRightView = m_pBranchView;
		m_pBranchView->GetListCtrl().ShowWindow(SW_SHOW);
		toolbar.CheckButton(ID_BRANCH_ACTIVATE, TRUE);
		break;
	case 3:
		m_pRightView = m_pUserView;
		m_pUserView->GetListCtrl().ShowWindow(SW_SHOW);
		toolbar.CheckButton(ID_USER_ACTIVATE, TRUE);
		break;
	case 4:
		m_pRightView = m_pClientView;
		m_pClientView->GetListCtrl().ShowWindow(SW_SHOW);
		toolbar.CheckButton(ID_CLIENT_ACTIVATE, TRUE);
		break;
	case 5:
		m_pRightView = m_pJobView;
		m_pJobView->GetListCtrl().ShowWindow(SW_SHOW);
		toolbar.CheckButton(ID_JOB_ACTIVATE, TRUE);
		break;
	case 6:
		m_pRightView = m_pOldChgView;
		m_pOldChgView->GetListCtrl().ShowWindow(SW_SHOW);
		toolbar.CheckButton(ID_SUBMITTED_ACTIVATE, TRUE);
		break;
	default:
		ASSERT(0);
		break;
	}
	m_pRightView->SetDlgCtrlID(m_wndVSplitter.IdFromRowCol(0,1));
	m_pRightView->ShowWindow(SW_SHOW);
	m_pRightView->SendMessage(WM_ACTIVATE, WA_ACTIVE, NULL);
	
	m_wndVSplitter.RecalcLayout();
	m_wndVSplitter.SetActivePane(0,1,NULL);

	m_pRightView->PostMessage(WM_ACTIVATEMODELESS, WA_ACTIVE, NULL);
}


int CMainFrame::GetRightSplitter()
{
	return m_currentTab;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// Lose the document title, since we dont have a persistent doc
	cs.style &= ~FWS_ADDTOTITLE ;  
	return CFrameWnd::PreCreateWindow(cs);
}


// Memory leak workaround:  CStatusbar and CString join forces to leak memory 
// unless panetext is set back to " ".  So do that in OnDestroy() to avoid 
// spurious mem leak messages.  Problem is specific to VC 4.2
void CMainFrame::UpdateStatus(LPCTSTR txt, BOOL forceRedraw)
{
	if (!this)	// To avoid crashing if called while quitting
		return;

	CString msg;

	m_bStatusBarWarnMsg = FALSE;
	if (*txt)
		 msg = txt;
	else if (m_StatusBarWarnLevel)
	{
		 m_bStatusBarWarnMsg = TRUE;
         if(m_StatusBarWarnLevel == SV_ERROR)
             msg = LoadStringResource(IDS_ERROR);
         else
             msg = LoadStringResource(IDS_WARNING);
	}
	else msg.Empty();
	m_wndStatusBar.SetPaneText(1, msg, TRUE);
	if(forceRedraw)
		m_wndStatusBar.RedrawWindow();
}

void CMainFrame::SetStatusBarLevel(StatusView level)
{
	if (TheApp()->m_RunClientWizOnly)
		return;
	m_StatusBarWarnLevel = level ? max(m_StatusBarWarnLevel, level) : SV_MSG;
	m_wndStatusBar.GetStatusBarCtrl( ).SetBkColor(level ? RGB(0xFF,0x90,0x90) : CLR_DEFAULT);
	if (!level && m_bStatusBarWarnMsg)
		UpdateStatus(_T(""), TRUE);
}

/////////////////////////////////////////////////////////////////////
// Status window updates:
// Use AddToStatusLog to append a line to the status view
// Use ClearStatusLog to clear the status view
// Post WM_STATUSADD to run OnStatusAdd to append from outside main thread
//                  WM_ADDTOSTATUS will delete wParam, a (char *)      
// Post WM_STATUSCLEAR to run OnStatusClear to run ClearStatusLog 

void CMainFrame::AddToStatusLog( LPCTSTR txt, StatusView level, bool showDialog)
{
	ASSERT( level != SV_DEBUG || !showDialog );
#ifndef _DEBUG
	if ( level != SV_DEBUG )
#endif
		m_pStatusView->AddItem( txt, level, showDialog );
}

void CMainFrame::AddToStatusLog( CStringArray *pArray, StatusView level, bool showDialog)
{
	ASSERT( level != SV_DEBUG || !showDialog );
	BOOL showIt= ( level != SV_DEBUG );
#ifdef _DEBUG
	showIt=TRUE;
#endif
	if( showIt && pArray->GetSize() )
	{
		// Prevent the status view from flickering by turning off
		// redraws during the data storm
		m_pStatusView->SetRedraw(FALSE);

		int i;
		for( i= 0; i < pArray->GetUpperBound(); i++ )
			m_pStatusView->AddItem( pArray->GetAt(i), level, showDialog, FALSE );

		// Add the last item forcing it to be visible
		m_pStatusView->AddItem( pArray->GetAt(i), level, showDialog, TRUE );

		m_pStatusView->SetRedraw(TRUE);
		m_pStatusView->RedrawWindow();
	}
}

void CMainFrame::ClearStatusLog()
{
	m_pStatusView->Clear();
}

LRESULT CMainFrame::OnStatusAdd(WPARAM wParam, LPARAM lParam)
{
	LPTSTR txt= (LPTSTR) wParam;
	AddToStatusLog(  txt, (StatusView)LOWORD(lParam), HIWORD(lParam) != 0);
	delete [] txt;
	return 0;
}

LRESULT CMainFrame::OnStatusAddArray(WPARAM wParam, LPARAM lParam)
{
	CStringArray *pArray= (CStringArray *) wParam;
	ASSERT_KINDOF( CStringArray, pArray);
	AddToStatusLog(  pArray, (StatusView)LOWORD(lParam), HIWORD(lParam) != 0);
	delete pArray;
	return 0;
}

LRESULT CMainFrame::OnStatusClear(WPARAM wParam, LPARAM lParam)
{
	ClearStatusLog();
	return 0;
}


void CMainFrame::OnDestroy() 
{
	// Release resources used by the critical section object.
    DeleteCriticalSection(&CriticalSection);
	UpdateStatus(_T(" "));
	CFrameWnd::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers for menu items that are almost always
// accessible



void CMainFrame::EditConnectionParam( CCmdUI* pCmdUI, const CString &value )
{
	CString txt;
    txt.FormatMessage(IDS_AmpEDIT_s, value);
	txt += _T("...");
	pCmdUI->SetText ( txt );
	pCmdUI->Enable( !SERVER_BUSY() 
					&& !value.IsEmpty( )
					/*and user is allow to edit the user or client! (get the owner) */
					/*and no modless dialog box is up */
					&& !IsModlessUp() );
}

void CMainFrame::OnUpdateJobNew(CCmdUI* pCmdUI) 
{
  	pCmdUI->Enable(!SERVER_BUSY() && !m_pJobView->GetListCtrl().IsEditInProgress());
}
   
void CMainFrame::OnJobNew() 
{
    m_pJobView->SendMessage(
        WM_COMMAND, 
        MAKEWPARAM(ID_JOB_NEW, 0), 
        0 );
}

void CMainFrame::OnUpdateUserNew(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && !IsModlessUp() 
		&& !m_pUserView->GetListCtrl().IsEditInProgress());		
}

void CMainFrame::OnUserNew() 
{
  	m_pUserView->SendMessage(
        WM_COMMAND, 
        MAKEWPARAM(ID_USER_CREATENEWUSER, 0), 
        0 );	
}

void CMainFrame::OnUpdateBranchNew(CCmdUI* pCmdUI) 
{
  	pCmdUI->Enable(!SERVER_BUSY() && !IsModlessUp() 
		&& !m_pBranchView->GetListCtrl().IsEditInProgress());	
}
   
void CMainFrame::OnBranchNew() 
{
  	m_pBranchView->SendMessage(
        WM_COMMAND, 
        MAKEWPARAM(ID_BRANCH_NEW, 0), 
        0 );
}

void CMainFrame::OnUpdateLabelNew(CCmdUI* pCmdUI) 
{
  	pCmdUI->Enable(!SERVER_BUSY() && !m_pLabelView->GetListCtrl().IsEditInProgress());		
}
   
void CMainFrame::OnLabelNew() 
{
  	m_pLabelView->SendMessage(
        WM_COMMAND, 
        MAKEWPARAM(ID_LABEL_NEW, 0),
        0 );
}

void CMainFrame::OnUpdateClientEditmy(CCmdUI* pCmdUI) 
{
	EditConnectionParam( pCmdUI, TruncateString(GET_P4REGPTR()->GetP4Client(), 50) );
	if (m_pClientView->GetListCtrl().IsEditInProgress())
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnClientEditmy() 
{
	m_pClientView->GetListCtrl().OnClientEditmy();	
}

BOOL CMainFrame::ClientSpecSwitch(CString switchTo, BOOL bAlways /*=FALSE*/, BOOL portWasChangedAlso /*= FALSE*/)
{
	return m_pClientView->GetListCtrl().ClientSpecSwitch(switchTo, bAlways, portWasChangedAlso);
}

void CMainFrame::OnUpdateUserEditmy(CCmdUI* pCmdUI) 
{
	EditConnectionParam( pCmdUI, TruncateString(GET_P4REGPTR()->GetP4User(), 50) );
	if (m_pUserView->GetListCtrl().IsEditInProgress())
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnUserEditmy() 
{
	m_pUserView->GetListCtrl().OnUserEditmy();	
}

void CMainFrame::OnUpdateUserPassword(CCmdUI* pCmdUI) 
{
	m_pUserView->GetListCtrl().OnUpdateUserPassword(pCmdUI, GET_P4REGPTR()->GetP4User() );
}

void CMainFrame::OnUserPassword() 
{
	m_pUserView->GetListCtrl().OnUserPassword();	
}

void CMainFrame::OnUpdateDescribeChg(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SERVER_BUSY( ) );	
}

void CMainFrame::OnDescribeChg() 
{
	m_pOldChgView->GetListCtrl().OnDescribeChg();	
}

void CMainFrame::OnUpdateDescribeJob(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SERVER_BUSY( ) );	
}

void CMainFrame::OnDescribeJob() 
{
	m_pJobView->GetListCtrl().OnDescribeJob();	
}

void CMainFrame::EditJobSpec(CString *jobname) 
{
	m_pJobView->GetListCtrl().EditTheSpec(jobname);	
}

void CMainFrame::OnNew() 
{
	switch(m_currentTab)
	{
	case 0:
		m_pDeltaView->GetTreeCtrl().CallOnChangeNew();
		break;
	case 1:
		OnLabelNew();
		break;
	case 2:
		OnBranchNew();
		break;
	case 3:
		OnUserNew();
		break;
	case 4:
		OnClientspecNew();
		break;
	case 5:
		OnJobNew();
		break;
	case 6:
		MessageBeep(0);
		break;
	default:
		ASSERT(0);
	}
}


////////////////////////////////////////////////////////////////////////////


void CMainFrame::OnUpdateStatus(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

LRESULT CMainFrame::OnExternalActivation(WPARAM wParam, LPARAM lParam)
{
	// Another p4win instance just pulled our chain.  Make sure we arent
	// iconic, then active forground popup, if any

	if(IsIconic())
       ShowWindow(SW_RESTORE);      

	CWnd *pWnd=GetLastActivePopup();
	pWnd->SetForegroundWindow();
	
	return 0;
}

LRESULT CMainFrame::OnExternalSendcmd(WPARAM wParam, LPARAM lParam)
{
	// Another p4win instance wants us to do something
	// For now, we only position our depot to a string

	if (SERVER_BUSY())	// if we are busy, we can't position the depot
		return 0;

	HANDLE hMapFile = OpenFileMapping(
					FILE_MAP_ALL_ACCESS,		// read/write access
					FALSE,						// do not inherit the name
					P4WIN_SHARED_MEMORY_NAME);	// name of mapping object 

	if (hMapFile != NULL) 
	{
		EXPANDPATH *ep = (EXPANDPATH *)MapViewOfFile(hMapFile,    // handle to mapping object
										FILE_MAP_ALL_ACCESS,0, 0, P4WIN_SHARED_MEMORY_SIZE);                   
		if (ep != NULL) 
		{
			if (!ep->flag)
			{
				TCHAR *port = ep->buf + ep->port;
				TCHAR *client=ep->buf + ep->client;
				TCHAR *user = ep->buf + ep->user;
				TCHAR *path = ep->buf + ep->path;
				if (!lstrcmp(*port ? port : GET_P4REGPTR()->GetP4Port(TRUE), 
									GET_P4REGPTR()->GetP4Port())
				 && !lstrcmp(client, GET_P4REGPTR()->GetP4Client())
				 && !lstrcmp(user, GET_P4REGPTR()->GetP4User())
				 && !ep->flag)
				{
					ep->flag = 1;

					if(IsIconic())
						ShowWindow(SW_RESTORE);      

					CWnd *pWnd=GetLastActivePopup();
					pWnd->SetForegroundWindow();

					TheApp()->m_ExpandPath = path;
					TheApp()->m_bFindInChg = TRUE;
					m_Need2ExpandDepot = TRUE;
					UpdateDepotandChangeViews(NO_REDRILL);
				}
			}
			UnmapViewOfFile(ep);
		}
		CloseHandle(hMapFile);
	}
	return 0;
}

LRESULT CMainFrame::OnExternalRplycmd(WPARAM wParam, LPARAM lParam)
{
	// Another p4win instance replied to our request
	// for now this is a no-op
#if 0
	if(IsIconic())
       ShowWindow(SW_RESTORE);      

	CWnd *pWnd=GetLastActivePopup();
	pWnd->SetForegroundWindow();
#endif
	return 0;
}


void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	if(m_pStatusView != NULL && IsWindow(m_pStatusView->m_hWnd))
	{
		// Let the zimbabwe splitter know that this is a resize event, not
		// a splitter adjustment via the splitter bar
		m_wndHSplitter.SetSizing(TRUE);
		CFrameWnd::OnSize(nType, cx, cy);
		m_wndHSplitter.SetSizing(FALSE);
	
		// Make sure the splitter is displayed per new rowinfo
		m_wndHSplitter.Resized();
	}
	else
		CFrameWnd::OnSize(nType, cx, cy);
}


void CMainFrame::UpdateCaption(BOOL updatePCU/* = TRUE*/)
{
	CString txt;
	
	if (updatePCU)
	{
		//		save port-client-user info for use in MRU menu
		CMenu *pMenu= GetMenu();
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
				txt.Format(*q ? _T("\"%s\" %s %s") : _T("%s %s %s"), p, c, u);
				GET_P4REGPTR()->AddMRUPcu( txt );
			}
			loadMRUPcuMenuItems(pMenu);
		}
	}
	
	//		inhouse, we want the caption bar to show the port, client and user,
	//		since we usually have lots of different servers and clients for
	//		testing, etc. customers don't really need it, although i suppose
	//		they too could set the registry key.
	//	
	if(GET_P4REGPTR()->ShowConnectSettings())
	{
		CString port = GET_P4REGPTR()->GetP4Port();
		if (GET_P4REGPTR()->ShowConnectPort1st())
		{
			int i;
			if ((i = port.Find(_T(':'))) != -1)
				port = port.Mid(i+1) + _T(':') + port.Left(i);
		}
		txt.FormatMessage(IDS_PERFORCE_PORT_s_CLIENT_s_USER_s, 
							port, GET_P4REGPTR()->GetP4Client(),
								  GET_P4REGPTR()->GetP4User() );
	}
	else
	{
		//	Get active connection parameters. If any one of the 3 differs
		//  from the permanent value, report in caption.
		//
        CString tempUser= GET_P4REGPTR()->GetP4User(FALSE);
		if( tempUser.GetLength() && tempUser.Compare(GET_P4REGPTR()->GetP4User(TRUE)) != 0)
            tempUser.FormatMessage(IDS_USER_s, tempUser);
        else
            tempUser.Empty();

		CString tempClient= GET_P4REGPTR()->GetP4Client(FALSE);
		if( tempClient.GetLength() && tempClient.Compare(GET_P4REGPTR()->GetP4Client(TRUE)) != 0)
            tempClient.FormatMessage(IDS_CLIENT_s, tempClient);
        else
            tempClient.Empty();

        CString tempPort= GET_P4REGPTR()->GetP4Port(FALSE);
        if( tempPort.GetLength() && tempPort.Compare(GET_P4REGPTR()->GetP4Port(TRUE)) != 0) 
            tempPort.FormatMessage(IDS_PORT_s, tempPort);
        else
            tempPort.Empty();
				
		txt.Empty();
		txt.FormatMessage(IDS_PERFORCE_s_s_s, tempPort, tempClient, tempUser);
		txt.TrimRight();
	}

	SetWindowText(txt);
}



void CMainFrame::OnPerforceOptions( BOOL m_ShowOptionsDlg, 
								    BOOL portWasChanged /* = FALSE */, 
									int contextPage /*=0*/,
									int intialPage /*=0*/ )
{
	// Record whether entire depot was being displayed or file types shown
	BOOL wasShowDeleted=GET_P4REGPTR()->ShowDeleted();
	BOOL wasShowFType=GET_P4REGPTR()->ShowFileType();
	BOOL wasShowAction=GET_P4REGPTR()->ShowOpenAction();
	BOOL wasShowChangeDesc=GET_P4REGPTR()->ShowChangeDesc();
    BOOL allChanges= GET_P4REGPTR()->GetFetchAllChanges();
    int numChanges=GET_P4REGPTR()->GetFetchChangeCount();
    BOOL allJobs= GET_P4REGPTR()->GetFetchAllJobs();
    int numJobs=GET_P4REGPTR()->GetFetchJobCount();
	BOOL wasShowOtherPending= GET_P4REGPTR()->GetEnablePendingChgsOtherClients();
	BOOL wasShowStatusMsgs = GET_P4REGPTR()->GetShowStatusMsgs( );
	int maxStatusLines = GET_P4REGPTR()->GetMaxStatusLines();
	BOOL wasShowHiddenFilesNotInDepot = GET_P4REGPTR()->ShowHiddenFilesNotInDepot();

	CString wasPort= GET_P4REGPTR()->GetP4Port();
    
	if ( m_ShowOptionsDlg )
	{
		// Let the user fiddle with options
		COptionsTreeDlg dlg;
		if (intialPage)
			dlg.m_InitialPage = intialPage;
		else if (contextPage && GET_P4REGPTR()->GetOptionStartUpOption() == 0)
			dlg.m_InitialPage = contextPage;
		else if (GET_P4REGPTR()->GetOptionStartUpOption() == 1)
			dlg.m_InitialPage = GET_P4REGPTR()->GetOptionStartUpPage();

		dlg.DoModal();
	}
	else
		m_FullRefreshRequired=TRUE;

	UpdateCaption();

	if (wasShowStatusMsgs != GET_P4REGPTR()->GetShowStatusMsgs())
		m_pStatusView->SetShowStatusMsgs(GET_P4REGPTR()->GetShowStatusMsgs());
	if (maxStatusLines != GET_P4REGPTR()->GetMaxStatusLines())
		m_pStatusView->OnMaxStatusLines();
		
	if( wasPort != GET_P4REGPTR()->GetP4Port() )
	{
		m_FullRefreshRequired=TRUE;
	}
	// Look for change that would invalidate any depot or delta view
	else if( wasShowDeleted != GET_P4REGPTR()->ShowDeleted() ||
										wasShowFType != GET_P4REGPTR()->ShowFileType())
	{
		m_FullRefreshRequired=TRUE;
		SetDepotCaption( GET_P4REGPTR()->ShowEntireDepot() );
	}
	else if( wasShowAction != GET_P4REGPTR()->ShowOpenAction() ||
			 wasShowChangeDesc != GET_P4REGPTR()->ShowChangeDesc() ||
			 wasShowOtherPending != GET_P4REGPTR()->GetEnablePendingChgsOtherClients() ||
			 wasShowHiddenFilesNotInDepot != GET_P4REGPTR()->ShowHiddenFilesNotInDepot() )
	{
		m_FullRefreshRequired=TRUE;
	}

    // Are we going to fetch a different number of changes?
    if( allChanges != GET_P4REGPTR()->GetFetchAllChanges() ||
            numChanges != GET_P4REGPTR()->GetFetchChangeCount() )
		m_pOldChgView->GetListCtrl().Clear();

    // Are we going to fetch a different number of jobs?
    if( allJobs != GET_P4REGPTR()->GetFetchAllJobs() ||
            numJobs != GET_P4REGPTR()->GetFetchJobCount() )
		m_pJobView->GetListCtrl().Clear();

	// If an option change requires full refresh, start the refresh
	if(m_FullRefreshRequired)
	{
		if(wasPort != GET_P4REGPTR()->GetP4Port() || portWasChanged)
		{
			m_pOldChgView->GetListCtrl().Clear();
			m_pOldChgView->GetListCtrl().ClearFilter();
			m_pBranchView->GetListCtrl().Clear();
			m_pClientView->GetListCtrl().Clear();
			m_pUserView->GetListCtrl().Clear();
			m_pLabelView->GetListCtrl().Clear();
			m_pJobView->GetListCtrl().Clear();
	        UpdateDepotandChangeViews( NO_REDRILL );
		}
		else
			UpdateDepotandChangeViews( REDRILL );

		m_Need2ExpandDepot = GET_P4REGPTR()->GetExpandFlag();
	}
}


LRESULT CMainFrame::OnP4OptionsDlg(WPARAM wParam, LPARAM lParam)
{
	OnPerforceOptions( TRUE, FALSE, int(wParam), int(lParam) );
	return TRUE;
}

void CMainFrame::OnPerforceOptions() 
{
	m_DoNotAutoPollCtr = 0;
	OnPerforceOptions( TRUE );
}

void CMainFrame::OnUpdateP4Options(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(SetMenuIcon(pCmdUI, !SERVER_BUSY() && !IsModlessUp()));		
}


void CMainFrame::OnClose() 
{
	if(SERVER_BUSY())
	{
		INT_PTR rc = IDC_BUTTON3;
		if (!m_Quitting)
		{
			if (m_DlgWndList.GetCount() && APP_HALTED())
			{
				CString txt;
				txt.FormatMessage(IDS_MODELESSDIALOGSUP_n, m_DlgWndList.GetCount());
				if (IDYES != AfxMessageBox(txt, MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2))
					return;
			}
			if (!SERVER_BUSY())
				rc = IDC_BUTTON1;
			else if (TheApp()->m_RunClientWizOnly)
			{
				rc = IDC_BUTTON3;
				CString txt;
				txt.FormatMessage(IDS_CLIENT_s_ALREADY_EXISTS, GET_P4REGPTR()->GetP4Client());
				AfxMessageBox( txt, MB_ICONEXCLAMATION );
			}
			else if((rc = MsgBox(IDS_AN_OPERATION_IS_PENDING__EXIT, 
							MB_ICONEXCLAMATION|MB_DEFBUTTON3)) == IDC_BUTTON2)
				return;
		}
		if (rc == IDC_BUTTON3)
		{
			MSG msg;
			m_Quitting = TRUE;
			UpdateStatus( LoadStringResource(IDS_WAITINGTOEXIT) );
			while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				//	get out if app is terminating
				if ( msg.message == WM_QUIT )
				{
					m_Quitting = FALSE;
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (m_Quitting)
			{
				Sleep(1000);
				::PostMessage(m_hWnd, WM_COMMAND, ID_APP_EXIT, 0);
				return;
			}
		}
	}

	if (GET_P4REGPTR()->GetLogoutOnExit())
		OnLogout();

	if (TheApp()->m_DiffPath.IsEmpty()
	 && TheApp()->m_RevHistPath.IsEmpty()
	 && TheApp()->m_FileInfoPath.IsEmpty()
	 && !TheApp()->m_RunClientWizOnly)
	{
		GET_P4REGPTR( )->SetShowToolBar(m_wndToolBar.IsWindowVisible());
		// If user wants to re-expand depot to last place they were on reconnect, then save current place
		if (GET_P4REGPTR()->GetExpandFlag() == 1)
			GET_P4REGPTR()->AddMRUPcuPath(GetCurrentItemPath());
		// If user wants to re-expand pending changelist on reconnect, then save current expansion
		m_pDeltaView->GetTreeCtrl().SaveExpansion();
	}
	// Kill update timer if reqd
	if(m_Timer != 0)
		KillTimer(UPDATE_TIMER);

	// Close all helper app programs
	for(POSITION pos = m_hWndHelperApp.GetHeadPosition(); pos != NULL; )
	{
		HWND hWnd = (HWND)m_hWndHelperApp.GetNext(pos);
		::PostMessage(hWnd, WM_CLOSE, 0, 0);
	}

	// Close All Modeless Dialogs
	SignalAllModelessDlgs(WM_QUITTING);

	// Hide the main window
	ShowWindow(SW_HIDE);

	// Save window position and state
	if (!TheApp()->m_RunClientWizOnly)
	{
		WINDOWPLACEMENT place;
		if(GetWindowPlacement(&place))
		{		
			GET_P4REGPTR()->SetWindowPosition(CRect(place.rcNormalPosition));
			
			switch(place.showCmd)
			{
			case SW_SHOWMAXIMIZED:
				GET_P4REGPTR()->SetWindowIconic(FALSE);
				GET_P4REGPTR()->SetWindowMaximized(TRUE);
				break;
			case SW_SHOWMINIMIZED:
				GET_P4REGPTR()->SetWindowIconic(TRUE);
				GET_P4REGPTR()->SetWindowMaximized((place.flags != 0));
				break;
			case SW_SHOWNORMAL:
			default:
				GET_P4REGPTR()->SetWindowIconic(FALSE);
				GET_P4REGPTR()->SetWindowMaximized(FALSE);
			}
		}

		// Save splitter positions
		int current, min;

		m_wndHSplitter.GetRowInfo(1, current, min);
		GET_P4REGPTR()->SetHSplitterPosition(current);

		m_wndVSplitter.GetColumnInfo(0, current, min);
		GET_P4REGPTR()->SetVSplitterPosition(current);
	}

	// Tell all worker threads it's time to exit. 
	REQUEST_ABORT();
	DWORD startAbortTime=GetTickCount();
	
	// Attempt to delete temp files in the P4win temp directory
	CString tempPath= GET_P4REGPTR()->GetTempDir();
	CString tempRoot=tempPath;

	if(tempPath.GetLength() > 0)
	{
		WIN32_FIND_DATA findData; 	
		CString fileName;
		tempPath+=_T("\\*.*");
		
		HANDLE findHandle= FindFirstFile( tempPath, &findData );
    	while(findHandle != INVALID_HANDLE_VALUE)
		{
			fileName.Format(_T("%s\\%s"), tempRoot, findData.cFileName);
			if(fileName.Find(_T("-Rev-")) != -1)
			{
				SetFileAttributes(fileName, FILE_ATTRIBUTE_NORMAL);
				DeleteFile(fileName);
			}
			if(!FindNextFile( findHandle, &findData ))
				break;
		}
		FindClose(findHandle);
	}

	// Wait 10 seconds (20 secs if tick count wraps) for worker thread to quit
//fanny: create an event handle for this.
	while(SERVER_BUSY())
	{
		if(GetTickCount() < startAbortTime)
			startAbortTime=0;  
		else if(GetTickCount() - startAbortTime > 10000)
			break;
		Sleep(1000);
	}

	CFrameWnd::OnClose();
}

void CMainFrame::OnUpdatePerforceInfo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(SetMenuIcon(pCmdUI, !SERVER_BUSY()));		
}

void CMainFrame::UpdateDepotandChangeViews(BOOL redrill, int key /*=0*/)
{
	if( GET_SERVERLEVEL() < 4 )
		redrill= NO_REDRILL;
    m_pDepotView->GetTreeCtrl().OnViewUpdate( redrill, key );
}

void CMainFrame::ExpandDepotString(const CString &path, BOOL newPath)
{
    m_pDepotView->GetTreeCtrl().ExpandDepotString( path, newPath );
}

long CMainFrame::PositionChgs(const CString &path, BOOL lookInMine, 
							  BOOL lookInOthers, BOOL addToSelectionSet)
{
	if (0 != m_currentTab)
	{
		m_bNoRefresh = TRUE;
		OnViewChanges();
		m_bNoRefresh = FALSE;
	}
    return m_pDeltaView->GetTreeCtrl().PositionChgs( path, lookInMine, 
										lookInOthers, addToSelectionSet );
}


/*
	_________________________________________________________________

	sends command 'p4 info' to the server. put the result in the 
	status pane, instead of a dialogue (and keep customers from
    cutting and pasting connect info to e-mail to tech support).

	OnPerforceInfo calls CCmd_Info's Run( ), which sends a WM_P4INFO
	message back to the mainframe, which calls OnP4InitInfo( ). why
	have this extra step?
	_________________________________________________________________
*/

void CMainFrame::ShowPerforceInfo()
{
	OnPerforceInfo();
}

void CMainFrame::OnPerforceInfo() 
{
	CCmd_Info *pCmd= new CCmd_Info;
	pCmd->Init( m_hWnd, RUN_ASYNC);
	if( pCmd->Run( ) )
		UpdateStatus( LoadStringResource(IDS_REQUESTING_SERVER_INFO) );
	else
		delete pCmd;
}

LRESULT CMainFrame::OnP4InitInfo( WPARAM wParam, LPARAM lParam )
{
    CmdPtr<CCmd_Info> pCmd(wParam);

	if(pCmd && !pCmd->GetError())
	{
		CP4Info const &info = pCmd->GetInfo();
		if (m_GetCliRootAndContinue)
		{
			TheApp()->SetClientRoot(info.m_ClientRoot);
		}
		else
		{
			BOOL showStatusMsgs = GET_P4REGPTR()->GetShowStatusMsgs( );
			GET_P4REGPTR()->SetShowStatusMsgs( showStatusMsgs );
			CString text = TheApp()->GetAppVersionString();
			int i;
			if ((i = text.ReverseFind(_T(' '))) != -1)
			{
				if ((i = text.Find(_T('.'), i)) != -1)
					text.Delete(i);
			}
            if(info.m_ServerDate.IsEmpty())
			{
    			text.FormatMessage(IDS_USERCLIENTROOTADDRVERSERVERADDRVERLICENSEROOTOS
						, info.m_UserName
						, info.m_ClientName
						, info.m_ClientHost
						, info.m_ClientRoot.IsEmpty( ) ? LoadStringResource(IDS_UNKNOWN) : info.m_ClientRoot
						, info.m_ClientAddress
						, text
						, info.m_ServerAddress
						, info.m_ServerVersion
						, info.m_ServerLicense
						, info.m_ServerRoot
                        , TheApp()->GetWindowsVersion()->GetVersionString()
                        , info.m_ClientP4Charset
                        , info.m_ClientAnsiCodePage
						, info.m_UnicodeMode
                        );
			}
            else if (info.m_CurrentDirectory.IsEmpty())
			{
    			text.FormatMessage(IDS_USERCLIENTROOTADDRVERSERVERADDRDATEVERLICENSEROOTOS
						, info.m_UserName
						, info.m_ClientName
						, info.m_ClientHost
						, info.m_ClientRoot.IsEmpty( ) ? LoadStringResource(IDS_UNKNOWN) : info.m_ClientRoot
						, info.m_ClientAddress
						, text
						, info.m_ServerAddress
						, info.m_ServerDate
						, info.m_ServerVersion
						, info.m_ServerLicense
						, info.m_ServerRoot
                        , TheApp()->GetWindowsVersion()->GetVersionString()
                        , info.m_ClientP4Charset
                        , info.m_ClientAnsiCodePage
						, info.m_UnicodeMode
                        );
			}
            else
			{
    			text.FormatMessage(IDS_USERCLIENTROOTCWDADDRVERSERVERADDRDATEVERLICENSEROOTOS
						, info.m_UserName
						, info.m_ClientName
						, info.m_ClientHost
						, info.m_ClientRoot.IsEmpty( ) ? LoadStringResource(IDS_UNKNOWN) : info.m_ClientRoot
						, info.m_CurrentDirectory
						, info.m_ClientAddress
						, text
						, info.m_ServerAddress
						, info.m_ServerDate
						, info.m_ServerVersion
						, info.m_ServerLicense
						, info.m_ServerRoot
                        , TheApp()->GetWindowsVersion()->GetVersionString()
                        , info.m_ClientP4Charset
                        , info.m_ClientAnsiCodePage
						, info.m_UnicodeMode
                        );
			}
			if (!info.m_ProxyVersion.IsEmpty())
				text.FormatMessage(IDS_PROXYVERSION_INFO, info.m_ProxyVersion, text);

			// write output to status pane without showing the time
			BOOL b1 = GET_P4REGPTR()->GetShowStatusMsgs( );
			BOOL b2 = GET_P4REGPTR()->ShowStatusTime();
			GET_P4REGPTR()->SetShowStatusMsgs( TRUE );
			GET_P4REGPTR()->SetShowStatusTime( FALSE );
			m_pStatusView->SetShowStatusMsgs(GET_P4REGPTR()->GetShowStatusMsgs());
			AddToStatusLog( text );
			GET_P4REGPTR()->SetShowStatusMsgs( b1 );
			GET_P4REGPTR()->SetShowStatusTime( b2 );
			m_pStatusView->SetShowStatusMsgs(GET_P4REGPTR()->GetShowStatusMsgs());
		}
	}

	UpdateStatus(_T(""));

	switch (m_GetCliRootAndContinue)
	{
	case P4INFO_ADDFILE:
		OnFileAddSetDir();
		break;

	case P4INFO_DOSBOX:
		OnCmdPromptContinue();
		break;

	default:
		break;
	}

	return 0;
}





//////////////////////////////////////////////////////////////////
// OnupdateUI's and handlers for view activation
/////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateViewSwitch(CCmdUI* pCmdUI) 
{
	CMenu *pMenu = GetMenu();
	if ( pCmdUI->m_nID == ID_CHANGELIST_ACTIVATE )
	{
		UINT start = ID_SUBMITTED_ACTIVATE;
		UINT end = ID_CHANGELIST_ACTIVATE;
		pMenu->CheckMenuRadioItem( start, end, ID_CHANGELIST_ACTIVATE, MF_BYCOMMAND  ) ;

	}
	pCmdUI->Enable(TRUE);			
}

void CMainFrame::OnUpdateViewDepot(CCmdUI* pCmdUI) 
{
	int map[] = { ID_VIEW_CLIENTVIEW, ID_VIEW_ENTIREVIEW, 
					ID_VIEW_LOCAL_P4, ID_VIEW_LOCAL_TREE, ID_VIEW_FILESNOTINDEPOT };
	pCmdUI->Enable(!SERVER_BUSY());
	UINT iItem = m_ShowOnlyNotInDepot 
			   ? ID_VIEW_FILESNOTINDEPOT : map[ GET_P4REGPTR()->ShowEntireDepot() ];
	if(pCmdUI->m_pMenu)
		pCmdUI->m_pMenu->CheckMenuRadioItem(
			ID_VIEW_CLIENTVIEW, 
			ID_VIEW_ENTIREVIEW, 
			iItem, 
			MF_BYCOMMAND);
}

void CMainFrame::OnViewDepot(UINT nID) 
{
	ASSERT(nID >= ID_VIEW_CLIENTVIEW && nID <= ID_VIEW_ENTIREVIEW);
	m_ShowOnlyNotInDepot = nID == ID_VIEW_FILESNOTINDEPOT;
	if (m_ShowOnlyNotInDepot)
		m_pDepotView->GetTreeCtrl().ClearDepotFilter(FALSE);
	int prevView = GET_P4REGPTR( )->ShowEntireDepot( );
	_sdf map[] = { SDF_CLIENT, SDF_LOCALP4, SDF_LOCALTREE, SDF_LOCALTREE, SDF_DEPOT };
	_sdf view = map[nID - ID_VIEW_CLIENTVIEW];
	SetDepotCaption( view );
	GET_P4REGPTR( )->SetShowEntireDepot( view );
	m_FullRefreshRequired = TRUE;
	BOOL redrill;
	switch(view)
	{
	case SDF_CLIENT:
	case SDF_DEPOT:
		redrill = (prevView == SDF_CLIENT) || (prevView == SDF_DEPOT);
		break;
	case SDF_LOCALTREE:
	case SDF_LOCALP4:
		redrill = (prevView == SDF_LOCALTREE) || (prevView == SDF_LOCALP4);
		break;
	default:
		ASSERT(0);
		redrill = FALSE;
		break;
	}
    UpdateDepotandChangeViews(redrill);
	if (redrill == NO_REDRILL)
		m_Need2ExpandDepot = GET_P4REGPTR()->GetExpandFlag();
}

void CMainFrame::OnViewChanges() 
{
	int	old_tab = m_currentTab;
	SetRightSplitter(0);
	if(GET_P4REGPTR()->GetReloadOnUncover() && !SERVER_BUSY() && !m_bNoRefresh
	 && m_LastUpdateTime !=0 && (old_tab != m_currentTab)
	 && ((m_DeltaUpdateTime + (GET_P4REGPTR()->GetReloadUncoverTime() * 60000)) < GetTickCount()))
		m_pDepotView->GetTreeCtrl().OnViewUpdate( TRUE );
}

void CMainFrame::ViewLabels( )
{
	OnViewLabels( ) ;
}

void CMainFrame::OnViewLabels() 
{
	int	old_tab = m_currentTab;
	SetRightSplitter(1);
	if((m_pLabelView->GetListCtrl().IsClear() || (GET_P4REGPTR()->GetReloadOnUncover() && !m_bNoRefresh))
	 && !SERVER_BUSY() && m_LastUpdateTime !=0 && (old_tab != m_currentTab)
	 && ((m_LabelUpdateTime + (GET_P4REGPTR()->GetReloadUncoverTime() * 60000)) < GetTickCount()))
		m_pLabelView->GetListCtrl().OnViewUpdate();
}

void CMainFrame::ViewBranches( )
{
	OnViewBranches( ) ;
}

void CMainFrame::OnViewBranches() 
{
	int	old_tab = m_currentTab;
	SetRightSplitter(2);
	if((m_pBranchView->GetListCtrl().IsClear() || (GET_P4REGPTR()->GetReloadOnUncover() && !m_bNoRefresh))
	 && !SERVER_BUSY() && m_LastUpdateTime !=0 && (old_tab != m_currentTab)
	 && ((m_BranchUpdateTime + (GET_P4REGPTR()->GetReloadUncoverTime() * 60000)) < GetTickCount()))
		m_pBranchView->GetListCtrl().OnViewUpdate();
}

void CMainFrame::ViewUsers( )
{
	OnViewUsers( ) ;
}

void CMainFrame::OnViewUsers() 
{
	int	old_tab = m_currentTab;
	SetRightSplitter(3);	
	if((m_pUserView->GetListCtrl().IsClear() || (GET_P4REGPTR()->GetReloadOnUncover() && !m_bNoRefresh))
	 && !SERVER_BUSY() && m_LastUpdateTime !=0 && (old_tab != m_currentTab)
	 && ((m_UserUpdateTime + (GET_P4REGPTR()->GetReloadUncoverTime() * 60000)) < GetTickCount()))
		m_pUserView->GetListCtrl().OnViewUpdate();
}

void CMainFrame::ViewClients( )
{
	OnViewClients( ) ;
}

void CMainFrame::OnViewClients() 
{
	int	old_tab = m_currentTab;
	SetRightSplitter(4);
	if((m_pClientView->GetListCtrl().IsClear() || (GET_P4REGPTR()->GetReloadOnUncover() && !m_bNoRefresh))
	 && !SERVER_BUSY() && m_LastUpdateTime !=0 && (old_tab != m_currentTab)
	 && ((m_ClientUpdateTime + (GET_P4REGPTR()->GetReloadUncoverTime() * 60000)) < GetTickCount()))
		m_pClientView->GetListCtrl().OnViewUpdate();
}

void CMainFrame::ViewJobs( )
{
	OnViewJobs( ) ;
}

void CMainFrame::OnViewJobs() 
{
	int	old_tab = m_currentTab;
	SetRightSplitter(5);
	m_pJobView->GetListCtrl().SetCaption( );
	if((m_pJobView->GetListCtrl().IsClear() || (GET_P4REGPTR()->GetReloadOnUncover() && !m_bNoRefresh))
	 && !SERVER_BUSY() && m_LastUpdateTime !=0 && (old_tab != m_currentTab)
	 && ((m_JobUpdateTime + (GET_P4REGPTR()->GetReloadUncoverTime() * 60000)) < GetTickCount()))
		m_pJobView->GetListCtrl().OnViewUpdate();
}

void CMainFrame::OnViewSubmitted() 
{
	int	old_tab = m_currentTab;
	SetRightSplitter(6);
	if ((!m_OldChgUpdateTime || m_pOldChgView->GetListCtrl().IsClear() 
	  || (GET_P4REGPTR()->GetReloadOnUncover() && !m_bNoRefresh))
	 && !SERVER_BUSY() && m_LastUpdateTime !=0 && (old_tab != m_currentTab)
	 && ((m_OldChgUpdateTime + (GET_P4REGPTR()->GetReloadUncoverTime() * 60000)) < GetTickCount()))
	{
		m_pOldChgView->GetListCtrl().OnViewUpdate();
	}
	else if (m_Need2RefreshOldChgs)
	{
		::SendMessage(m_pOldChgView->m_hWnd, WM_RUNUPDATE, 0, 0);
	}
	m_Need2RefreshOldChgs = FALSE;
}

LRESULT CMainFrame::OnClientError(WPARAM wParam, LPARAM lParam)
{
	// activate the client pane so the user can switch-to/make-new client
	m_ClientError= TRUE;
	// wait until all in process commands finish so our 'p4 clients' will run ok
	int t = GET_P4REGPTR()->BusyWaitTime() * 10 + 5000;
	while (SERVER_BUSY() && t > 0)
	{
		if (!PumpMessages( ))
			break;
		Sleep(50);
		t -= 50;
	}
	OnViewClients();	// we want this to get started and hopefully finish while the dialogbox is up
	Sleep(100);			// because our remove of MRU will get restored when the async part of OnViewClients() finishes

	// if there is a local default template in the registry
	// we'll go with that rather than run the wizard
	CString localdeftmplate = GET_P4REGPTR()->GetLocalCliTemplate();
	if (GET_P4REGPTR()->LocalCliTemplateSw() 
	 && !localdeftmplate.IsEmpty() && localdeftmplate.Find(_T("\n\nClient:\t")) == 0)
	{
		m_pClientView->GetListCtrl().DoClientspecNew(TRUE, GET_P4REGPTR()->GetP4Client());
		return 0;
	}

	// format a PCU string so we can remove this invalid one
	CString pcutxt;
	pcutxt.Format( _T("%s %s %s"), GET_P4REGPTR()->GetP4Port(), GET_P4REGPTR()->GetP4Client(), GET_P4REGPTR()->GetP4User());
	CMenu *pMenu= GetMenu();

	// Run the Client Wizard
	CString txt;
	CString port = GET_P4REGPTR()->GetP4Port();
	CString clientname = GET_P4REGPTR()->GetP4Client();
	if (GET_P4REGPTR()->Is1stRun())
		txt.FormatMessage( IDS_YOUMUSTSPECIFYACLIENT_s_s, port, GET_P4REGPTR()->GetP4User() );
	else
		txt.FormatMessage( IDS_NOCLIENT_s_s_s, clientname, port, GET_P4REGPTR()->GetP4User() );
	CClientWizSheet dlg(_T("ClientWorkspace Wizard"), this, 0);
	dlg.SetMsg(txt);
	dlg.SetName(clientname);
	dlg.SetPort(port);
	if (TheApp()->m_RunClientWizOnly)
		dlg.AllowBrowse(FALSE);
	int i;
	INT_PTR rc;
	while(1)
	{
		rc = dlg.DoModal();	// display the Wizard and get the return code

		// remove the MRU for this PCU triplet since it is no good for now
		GET_P4REGPTR()->RmvMRUPcu(pcutxt);
		if (pMenu != NULL)
			loadMRUPcuMenuItems(pMenu);

		// handle the return code
		if( rc == IDCANCEL )	// user bailed
		{
			DoNotAutoPoll();
			break;
		}
		if ( rc == IDIGNORE )	// user chose to browse existing clients
		{
			// Give the viewupdate for clients more time to finish
			for (i = 100; SERVER_BUSY() && i--; )
				Sleep(100);

			// Because MFC tosses the return from this SendMessage
			// We have to pass the return code via a variable :-(
			m_ClientGet = 0;
			::SendMessage(ClientWnd(), WM_WIZFETCHOBJECTLIST, 
							(WPARAM)(this->m_hWnd), WM_BROWSECALLBACK1);
			if (m_ClientGet == 1)
			{
				UpdateDepotandChangeViews(TRUE);	// update to reflect new client
				return 0;
			}
			else if (!m_ClientGet)
				break;
			// else m_Client == -1, so loop again and show 1st Wizard page
			// but first we have to destroy the Adobe bitmap
			dlg.DeleteAdobeBitmap();
		}
		else	// user clicked the Finish button on the create client page
		{
			CString newClient = dlg.GetName();
			CString newRoot   = dlg.GetRoot();
			BOOL bEdit        = dlg.IsEdit();
			BOOL bTmpl        = dlg.IsTmpl();
			CString tmplate   = dlg.GetTmpl();
			if (m_pClientView->GetListCtrl().FindInList(newClient) != -1)
			{
				CString msg;
				msg.FormatMessage(IDS_CANT_RECREATE_EXISTING_CLIENT_s, newClient);
				AfxMessageBox( msg, MB_ICONSTOP );
				dlg.DeleteAdobeBitmap();
			}
			else if (!newClient.IsEmpty() && !newRoot.IsEmpty())
			{
				if (m_pClientView->GetListCtrl().AutoCreateClientSpec(newClient, newRoot, bEdit, 
																	  bTmpl, tmplate))
					return 0;
			}
		}
	}
	// Inform the user we failed to establish a client
	txt.FormatMessage( IDS_NOCLIENT_s_s_s, GET_P4REGPTR()->GetP4Client(), 
						GET_P4REGPTR()->GetP4Port(), GET_P4REGPTR()->GetP4User() );
	if ((i = txt.Find(_T('\n'))) != -1)
	{
		txt = txt.Left(i);
		txt.TrimLeft();
	}
	TheApp()->StatusAdd( txt, SV_WARNING );
	UpdateCaption(FALSE);
	if (TheApp()->m_RunClientWizOnly)
		::PostMessage(m_hWnd, WM_COMMAND, ID_APP_EXIT, 0);
	return 0;
}

LRESULT CMainFrame::OnBrowseClientsCallBack(WPARAM wParam, LPARAM lParam)
{
	if (wParam == IDC_BACK)
	{
		m_ClientGet = -1;
		return IDC_BACK;	// The IDC_BACK gets tossed by MFC :-(
	}
	CString *str = (CString *)lParam;
	GET_P4REGPTR()->SetP4Client(*str, TRUE, FALSE, FALSE);
	UpdateCaption( );
	m_ClientGet = 1;
	return IDOK;	// The IDOK gets tossed by MFC :-(
}

void CMainFrame::SetLastUpdateTime(BOOL updateResult)
{
	m_LastUpdateTime=GetTickCount(); 
	m_LastUpdateResult=updateResult; 
	if( updateResult == UPDATE_SUCCESS )
		m_ClientError= FALSE;
}

void CMainFrame::ClearLastUpdateTime() 
{
	m_LastUpdateTime=0; 
	m_LastUpdateResult= UPDATE_SUCCESS; 
	m_ClientError= FALSE;
}
/*
	_________________________________________________________________

	Timer fires every 5 secs.  Only start an update if 
		1) server not busy
		2) auto poll is on
		3) time since last update finished exceeds poll interval
		4) popup menu is not up
		5) is active or user requests to poll when inactive
	_________________________________________________________________
*/

void CMainFrame::OnTimer(UINT_PTR nIDEvent) 
{
	CFrameWnd::OnTimer(nIDEvent);

	static int statustimer = 0;
	if (m_StatusUpdateInterval && !SERVER_BUSY() || ++statustimer >= m_StatusUpdateInterval)
	{
		m_pStatusView->SetRedraw(TRUE);
		statustimer = 0;
	}

	long time=GetTickCount();

	if( SERVER_BUSY() || m_DoNotAutoPollCtr > 0 )
		return;

	if(GetLastActivePopup() != this)
		return;

	if(APP_HALTED())
	{
		// User is staring at app-modal error message
		m_LastUpdateTime= time;
		return;
	}

    // Don't keep firing auto-updates when we know there
    // is a password error
    if(GET_PWD_ERROR())
        return;

    // Don't keep firing auto-updates when we know the
    // last attempt ended in failure.  The only exception
	// is the case of a client error, where we activate 
	// and refresh the client pane
    if( m_LastUpdateResult == UPDATE_FAILED && m_ClientError)
	{
		OnViewClients();
		if(UpdateRightView())
		{
			m_LastUpdateResult= UPDATE_SUCCESS;
			return;
		}
		m_ClientError= FALSE;
	}
	else if( m_LastUpdateResult == UPDATE_FAILED)
        return;

    if(UpdateRightView())
	{
		XTRACE(_T("OnTimer - updated right view\n"));
		m_LastUpdateTime= time;
		return;
	}

	// See if its time to update depot and changes windows
	if(!GET_P4REGPTR()->GetAutoPoll())
		return;

	// Some users prefer no polling while inactive (default)
	if(!m_GotInput && !GET_P4REGPTR()->GetAutoPollIconic())
		return;

	if((m_LastUpdateTime > time) || ((time-m_LastUpdateTime)/60000 >= GET_P4REGPTR()->GetAutoPollTime()))
	{
		XTRACE(_T("OnTimer - starting update\n"));

		// Clear our flag which indicates whether we got any mousedowns or keystrokes
		m_GotInput = FALSE;

		m_LastUpdateTime = time;

		if (GET_P4REGPTR()->GetAutoPollJobs())
		{
			if (m_currentTab == 5)
				m_Need2Poll4Jobs = TRUE;
			else m_pJobView->GetListCtrl().Clear();
		}

        // TODO: Rename all variables that sound like 'fullupdate' or 'fullrefresh'
        //       so this code is a little easier to read.  With the recent change in
        //       how F5 works, m_FullRefreshRequired in this module means that the
        //       depot + changelist panes have never been properly loaded since application
        //       startup or since a change in connection parameters.  But in CDepotView,
        //       m_FullUpdate means that we don't want to run an incremental update of
        //       the depot (fstat -c x combined with p4 opened -a), rather we want to
        //       get a fresh copy of everything, by running fstat on every file.
        //
        //       But I'm going to save this rename work till 99.1 so I dont have yet 
        //       another massive change to the code in several files.

        if((GET_SERVERLEVEL() > 3))
		{
            // Against a 98.2 server, only try the redrill operation if the existing
            // window contents are known to be valid (!m_FullRefreshRequired).  Never
            // ask for incremental update 
			int lock = 0;
			GET_SERVER_LOCK( lock );
            UpdateDepotandChangeViews(!m_FullRefreshRequired, lock);
		}
        else
            // Against a 97.3 server, if the timer is polling do what 97.3 gui did.
		    UpdateDepotandChangeViews(FALSE, m_FullRefreshRequired);
	}
}

BOOL CMainFrame::UpdateRightView()
{
	BOOL updating=FALSE;

	switch(m_currentTab)
	{
	case 0:
		break;
	case 1:
		if(m_pLabelView->GetListCtrl().IsClear())
		{
			m_pLabelView->GetListCtrl().OnViewUpdate();
			updating=TRUE;
		}
		break;
	case 2:
		if(m_pBranchView->GetListCtrl().IsClear())
		{
			m_pBranchView->GetListCtrl().OnViewUpdate();
			updating=TRUE;
		}
		break;
	case 3:
		if(m_pUserView->GetListCtrl().IsClear())
		{
			m_pUserView->GetListCtrl().OnViewUpdate();
			updating=TRUE;
		}
		break;
	case 4:
		if(m_pClientView->GetListCtrl().IsClear())
		{
			m_pClientView->GetListCtrl().OnViewUpdate();
			updating=TRUE;
		}
		break;
	case 5:
		if(m_pJobView->GetListCtrl().IsClear())
		{
			m_pJobView->GetListCtrl().OnViewUpdate();
			updating=TRUE;
		}
		break;
	case 6:
		if(m_pOldChgView->GetListCtrl().IsClear())
		{
			m_pOldChgView->GetListCtrl().OnViewUpdate();
			updating=TRUE;
		}
		break;
	default:
		ASSERT(0);	

	}
	return updating;
}


void CMainFrame::OnSysColorChange() 
{
	// update image lists
	TheApp()->OnSysColorChange();

	// pass it along to views
	CFrameWnd::OnSysColorChange();

	m_wndToolBar.GetToolBarCtrl().SetImageList(
		TheApp()->GetToolBarImageList());
	m_wndToolBar.GetToolBarCtrl().SetDisabledImageList(
		TheApp()->GetToolBarImageList()->GetDisabled());
}

void CMainFrame::OnUpdateFileAdd(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(SetMenuIcon(pCmdUI, !SERVER_BUSY() 
		&& !m_pDeltaView->GetTreeCtrl().IsEditInProgress()));
}

void CMainFrame::OnFileAdd() 
{
	// In order to initalize the OpenFile dialogbox to the correct initial directory,
	// we need the root of the current client.  So first go get that after setting a
	// flag to indicate to the p4 info completion routine that that is what we are up to.

	if (TheApp()->m_ClientRoot.IsEmpty())
	{
		m_GetCliRootAndContinue = P4INFO_ADDFILE;
		OnPerforceInfo();
	}
	else OnFileAddSetDir();
}

void CMainFrame::OnFileAddSetDir(LPTSTR lpInitDir /*= NULL*/) 
{
	m_GetCliRootAndContinue = 0;
	TheApp()->m_ClientRoot.Replace('/', '\\');

	// TODO: put filter categories in the registry and allow user prefs for file types

	// This loads the internal OPENFILENAME structure
	CFileDialog fDlg(TRUE, NULL, NULL,  
		OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | 
		OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST | OFN_ENABLESIZING,
		LoadStringResource(IDS_MAIN_ADD_FILE_FILTER), this, 
		m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME)); 
	
	// Note that with NT4.0 < SP3, file dialog is limited to ~20 files

	// Set a reasonably large buffer for multi-file selects.  This is big
	// enough for 1500 20 char filenames.  I'm sure someone will complain 
	// that's not enough... 
	#define BUFLEN 32768
	TCHAR buf[BUFLEN];
	// Zero 1st char so commdlg knows we aren't providing a default filename
	buf[0]=_T('\0');

    fDlg.m_ofn.lpstrFile= buf; 
	fDlg.m_ofn.nMaxFile= BUFLEN; 

	// Set the dlg caption
	CString title=LoadStringResource(IDS_ADD_FILES_TO_SOURCE_CONTROL);
	fDlg.m_ofn.lpstrTitle=title;
	// We dont need no stinking file title
	fDlg.m_ofn.lpstrFileTitle=NULL;
	
	// Set the initial directory
	TCHAR initdir[MAX_PATH+1];
	if (lpInitDir && *lpInitDir)
	{
		lstrcpy(initdir, lpInitDir);
	}
	else
	{
		lstrcpy(initdir, GET_P4REGPTR()->GetAddFileCurDir());
		LPTSTR p = TheApp()->m_ClientRoot.GetBuffer(TheApp()->m_ClientRoot.GetLength()+1);
		if (_tcsnicmp(initdir, p, TheApp()->m_ClientRoot.GetLength()))
			lstrcpy(initdir, p);
		TheApp()->m_ClientRoot.ReleaseBuffer();
	}
	fDlg.m_ofn.lpstrInitialDir=initdir;

	// Set the user defined filter
	TCHAR customFilterBuf[512];
	memset(customFilterBuf, 0, 512);
	CString customFilter=GET_P4REGPTR()->GetAddFileFilter();
	CString filterTitle;
	filterTitle.FormatMessage(IDS_CUSTOM_FILTER_s, customFilter.GetLength() ? customFilter : _T("?"));
	lstrcpy(customFilterBuf, filterTitle);
	lstrcpy(customFilterBuf+(filterTitle.GetLength()+1), customFilter);
	fDlg.m_ofn.lpstrCustomFilter= customFilterBuf;
	fDlg.m_ofn.nMaxCustFilter=512;

	// Set custom filter index
	fDlg.m_ofn.nFilterIndex=GET_P4REGPTR()->GetAddFileFilterIndex();
	
	CString errorTxt;
	INT_PTR retcode=fDlg.DoModal();

	if(retcode == IDOK)
	{
		// If the use made a selection w/ a modified custom filter,
		// update the registry with that custom filter
		if(fDlg.m_ofn.nFilterIndex==0)
		{
			// Save any change to the user filter
			TCHAR *ptr=customFilterBuf+(filterTitle.GetLength()+1);
			if(lstrlen(ptr) > 0)
				GET_P4REGPTR()->SetAddFileFilter(ptr);
		}
	
		// Always save index to search filter
		GET_P4REGPTR()->SetAddFileFilterIndex(fDlg.m_ofn.nFilterIndex);

		CStringList files;

		POSITION pos= fDlg.GetStartPosition();

		int ctr=0;
		while(pos != NULL)
		{
			files.AddHead(fDlg.GetNextPathName(pos));
			// Even tho we have a 32K buffer, windows can fail to give us
			// 32K worth of file names - warn if adding 500+ files
			if (++ctr == 500)
				AddToStatus( LoadStringResource(IDS_TOO_MANYFILES_TO_ADD), SV_WARNING );
		}
		
		// Update registry for last add path
		if(files.GetCount())
		{
			CString fname=files.GetHead();
			int lastSlash= fname.ReverseFind(_T('\\'));
			fname= fname.Left(lastSlash);
			GET_P4REGPTR()->SetAddFileCurDir(fname);
		}
		m_pDeltaView->GetTreeCtrl().AddFileList(0, &files);
		
	}
	else if(retcode == IDCANCEL)  // an error
	{
		DWORD exError=CommDlgExtendedError();
		if(exError != 0)
			errorTxt.FormatMessage(IDS_EXTENDED_ERROR_n_IN_FILEDIALOG, exError);
	}
	else
	{
		DWORD error=GetLastError();
		if(error)
		{
			LPVOID lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				error,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);
			errorTxt = (TCHAR *)lpMsgBuf;
		}
		else
			errorTxt=LoadStringResource(IDS_UNKNOWN_FILEDIALOG_ERROR);
	}

	if(errorTxt.GetLength() > 0)
		AfxMessageBox(errorTxt, MB_ICONSTOP);
	
}

void CMainFrame::ActivateFrame(int nCmdShow) 
{
	if(m_FirstActivation)
	{
		m_FirstActivation=FALSE;

		//////////////////
		// Calculate size of main window, making sure it still fits the
		// current screen size.  
		CRect rect= GET_P4REGPTR()->GetWindowPosition();
		if(rect.Width() < 50 || rect.Height() < 50)
		{
			// Might have a bad reg value, so set a reasonable default
			rect= CRect(GetSystemMetrics(SM_CXSCREEN) /10,
					GetSystemMetrics(SM_CYSCREEN) /10,
					GetSystemMetrics(SM_CXSCREEN) * 9/10,
					GetSystemMetrics(SM_CYSCREEN) * 9/10);
		}
		// if there is only one monitor, we can check for resonable placement
		else if (m_NbrDisplays < 2)
		{
			rect.NormalizeRect();
			// If rectangle bigger than screen, shrink/shift as required
			// (some 8-ball may have changed video res since we last ran)
			int reduceX= max( rect.Width() - GetSystemMetrics(SM_CXSCREEN), 0 );
			int reduceY= max( rect.Height() - GetSystemMetrics(SM_CYSCREEN), 0 );
			rect.DeflateRect(reduceX/2, reduceY/2);
			// Move the window fully onto the screen
			int shiftX= max( rect.right - GetSystemMetrics(SM_CXSCREEN), 0);
			int shiftY= max( rect.bottom - GetSystemMetrics(SM_CYSCREEN), 0);
			rect.OffsetRect(-shiftX, -shiftY);
		}

		/////////////////
		// Set the size  of the main window
		WINDOWPLACEMENT place;

		if (nCmdShow == SW_SHOWNORMAL)
		{
			if(GET_P4REGPTR()->GetWindowMaximized())
			{
				nCmdShow=SW_SHOWMAXIMIZED;
				place.flags=WPF_RESTORETOMAXIMIZED;
			}
			else
			{
				nCmdShow=SW_NORMAL;
				place.flags=WPF_SETMINPOSITION;
			}
		}
		else
			place.flags=WPF_SETMINPOSITION;

		place.length= sizeof(WINDOWPLACEMENT);
		place.showCmd= nCmdShow;
		place.ptMinPosition=CPoint(0,0);
		place.ptMaxPosition=CPoint(-::GetSystemMetrics(SM_CXBORDER), -::GetSystemMetrics(SM_CYBORDER));
		place.rcNormalPosition= rect;
		SetWindowPlacement(&place);

		if (!TheApp()->m_DiffPath.IsEmpty())
			nCmdShow=SW_SHOWMINIMIZED;
		if (!TheApp()->m_RevHistPath.IsEmpty()
		 || !TheApp()->m_FileInfoPath.IsEmpty()
		 || !TheApp()->m_SubmitPath.IsEmpty()
		 || TheApp()->m_RunClientWizOnly)
			nCmdShow=SW_SHOWMINIMIZED;

		///////////////////////
		// Set row height for main horizontal splitter
		m_wndHSplitter.GetClientRect(&rect);
		int h=GET_P4REGPTR()->GetHSplitterPosition();
		int hb=rect.Height()-GetSystemMetrics(SM_CYBORDER)*4;
		if(h > hb)
			h=hb;   // Shrink to fit
		else if(h<=0)
			h=100;  // Default. About 5 rows of text, depending on font

		m_wndHSplitter.SetRowInfo( 0, rect.Height()-h-11, 0);
		m_wndHSplitter.SetRowInfo( 1, h, 0);
		m_wndHSplitter.RecalcLayout();

		//////////////////////
		// Set columns width for main vertical splitter
		m_wndVSplitter.GetClientRect(&rect);
		int w=GET_P4REGPTR()->GetVSplitterPosition();
		int wb=rect.Width()-GetSystemMetrics(SM_CXBORDER)*4;
		if(w > wb)
			w=wb;	// Shrink to fit
		else if(w<=0)
			w=wb/2;	// Guess a reasonable default
	
		m_wndVSplitter.SetColumnInfo( 0, w, 0);
		m_wndVSplitter.SetColumnInfo( 1, w, 0);
		m_wndVSplitter.RecalcLayout();
#if	0
		if (GET_P4REGPTR()->Is1stRun())
		{
			CFirstTimeDlg dlg;
			if (dlg.DoModal() == IDOK)
			{
				OnGettingStartedWithP4win();
				UpdateDepotandChangeViews(NO_REDRILL);
			}
			else
			{
				//	Run the initial update of depot and changelist panes
				UpdateDepotandChangeViews(NO_REDRILL);
				m_Need2ExpandDepot = GET_P4REGPTR()->GetExpandFlag();
			}
		}
#endif
	}

	CFrameWnd::ActivateFrame(nCmdShow);
}

void CMainFrame::SetDepotCaption( int iShowEntireDepot )
{
	m_pDepotView->GetTreeCtrl().SetCaption(iShowEntireDepot);
}


void CMainFrame::OnUpdateClientspecNew(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SERVER_BUSY() && !IsModlessUp() 
		&& !m_pClientView->GetListCtrl().IsEditInProgress() );	
}

void CMainFrame::OnClientspecNew() 
{
	m_pClientView->GetListCtrl().ClientspecNew( ) ;
}

void CMainFrame::OnUpdateSubmittedActivate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);			
}

void CMainFrame::loadMRUPcuMenuItems(CMenu *pMenu)
{
	CString str[MAX_MRU_PCUS+1];
	CString temp;
	int		i, j;

	CString SpeedNbr;
	CMenu *pEditMenu = pMenu->GetSubMenu(3);
	CMenu *pHistMenu = pEditMenu->GetSubMenu(4);

	// clear the old PCU menu items
	for (i = -1; ++i < MAX_MRU_PCUS; )
	{
		if (!(pHistMenu->DeleteMenu( ID_MRU_PCU0 + i, MF_BYCOMMAND )))
			break;
	}

	// Get the strings
	for(i = -1; ++i < MAX_MRU_PCUS; )
	{
		int	k = 0;
		str[i] = GET_P4REGPTR()->GetMRUPcu(i);
		if ((j = str[i].Find(_T('@'))) > 0)
			str[i] = str[i].Left(j);
		if (!str[i].IsEmpty()
		 && (str[i].GetAt(0) == _T('\"'))
		 && ((j = str[i].Find(_T('\"'), 1)) > 0))
			k = j;
		if ((j = str[i].Find(_T(' '), k)) > 0)
			str[i].SetAt(j, _T('\t'));
	}

	// Sort the strings
	BOOL swapped;
	do
	{
		swapped = FALSE;
		for(i=0; ++i < MAX_MRU_PCUS; )
		{
			if (str[i-1] > str[i])
			{
				swapped = TRUE;
				temp = str[i-1];
				str[i-1] = str[i];
				str[i] = temp;
			}
		}
	} while (swapped);

	// Load all MRU PCUs into History menu
	for(i=-1, j=1; ++i < MAX_MRU_PCUS; )
	{
		if (str[i].GetLength() > 0)
		{
			if (j != 10)
				 SpeedNbr.Format(_T("&%d "), j);
			else SpeedNbr = _T("&0 ");
			pHistMenu->AppendMenu(MF_STRING, ID_MRU_PCU0+j-1, SpeedNbr+str[i]);
			j++;
		}
	}
}

void CMainFrame::OnUpdateMruPcu(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsModlessUp() ? FALSE : !SERVER_BUSY());
}

void CMainFrame::OnUpdateMruPcuHdr(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsModlessUp() ? FALSE : !SERVER_BUSY());
}

void CMainFrame::OnMruPcu(int nbr) 
{
	int j;
	int firstsep;
	int secondsep;
	CString menuStr;
	CString port;
	CString client;
	CString user;
	CString wasUser = GET_P4REGPTR()->GetP4User();
	CString wasPort = GET_P4REGPTR()->GetP4Port();

	if (IsModlessUp() || !GetMenu()->GetMenuString(ID_MRU_PCU0+nbr, menuStr, MF_BYCOMMAND))
	{
		ASSERT(NULL);
		return;
	}
	menuStr = menuStr.Right(menuStr.GetLength()-3);
	if ((j = menuStr.Find(_T('@'))) > 0)
		menuStr = menuStr.Left(j);
	if (menuStr.GetAt(0) == _T('\"'))
		firstsep = menuStr.Find(_T('\"'),1) + 1;
	else
		firstsep = menuStr.Find(_T('\t'));
	secondsep= menuStr.Find(_T(' '), firstsep+1);
	port  = menuStr.Left(firstsep);
	port.TrimLeft(_T('\"'));
	port.TrimRight(_T('\"'));
	client= menuStr.Mid(firstsep+1, secondsep - firstsep - 1);
	user  = menuStr.Right(menuStr.GetLength() - secondsep - 1);

	if (GET_P4REGPTR()->GetExpandFlag() == 1)
		GET_P4REGPTR()->AddMRUPcuPath(GetCurrentItemPath());

	GET_P4REGPTR()->SetP4Port(port, TRUE, FALSE, FALSE);
	GET_P4REGPTR()->SetP4User(user, TRUE, FALSE, FALSE);
	if (!ClientSpecSwitch(client, TRUE, wasPort != port))// calls OnPerforceOptions()
	{
		GET_P4REGPTR()->SetP4Port(wasPort, TRUE, FALSE, FALSE);	// put old port back if client switched failed
		GET_P4REGPTR()->SetP4User(wasUser, TRUE, FALSE, FALSE);	// put old user back if client switched failed
	}
}

void CMainFrame::OnMruPcu0() 
{
	OnMruPcu(0);
}

void CMainFrame::OnMruPcu1() 
{
	OnMruPcu(1);
}

void CMainFrame::OnMruPcu2() 
{
	OnMruPcu(2);
}

void CMainFrame::OnMruPcu3() 
{
	OnMruPcu(3);
}

void CMainFrame::OnMruPcu4() 
{
	OnMruPcu(4);
}

void CMainFrame::OnMruPcu5() 
{
	OnMruPcu(5);
}

void CMainFrame::OnMruPcu6() 
{
	OnMruPcu(6);
}

void CMainFrame::OnMruPcu7() 
{
	OnMruPcu(7);
}

void CMainFrame::OnMruPcu8() 
{
	OnMruPcu(8);
}

void CMainFrame::OnMruPcu9() 
{
	OnMruPcu(9);
}

void CMainFrame::OnUpdateViewWarnAndErrs(CCmdUI* pCmdUI) 
{
	m_pStatusView->CallOnUpdateViewWarnAndErrs(pCmdUI);
}

void CMainFrame::OnViewWarnAndErrs() 
{
	m_pStatusView->CallOnViewWarnAndErrs();
}



///////////////////////////////////////////////////

// When doing Next/Prev in Describe, painting is turned off to stop flashing.
// This CallBack function is called during an EnumChildWindows() to stop or resume
// painting for all child windows in the process.

BOOL CALLBACK ChildSetRedraw(HWND hwnd, LPARAM lParam)	// LPARAM is TRUE for ON, FALSE for OFF
{
	// Windows that are children of the righthand splitter
	// and are not childern of the current pane are NOT disable/enabled
	// because these are hidden and disabling/re-enabling them cause
	// them to become unhidden (for some unknown reason).
	BOOL b;
	if (::IsChild(MainFrame()->GetHSplitterWnd(), hwnd))
	{
		if (MainFrame()->GetLeftHandWnd() == hwnd 
		 || ::IsChild(MainFrame()->GetLeftHandWnd(), hwnd))
			b = TRUE;
		else if (MainFrame()->GetRightHandWnd() == hwnd 
		 || ::IsChild(MainFrame()->GetRightHandWnd(), hwnd))
			b = TRUE;
		else if (MainFrame()->GetVSplitterWnd() == hwnd)
			b = TRUE;
		else
			b = FALSE;
	}
	else
		b = TRUE;
	if (b)
	{
		// Turn the painting on/off
		// for this visible child window
		// in accord with lParam value
		::SendMessage(hwnd, WM_SETREDRAW, (WPARAM)lParam, 0);
		// Force a repaint if enabling.
		if (lParam)
			::InvalidateRect(hwnd, NULL, TRUE);
	}
	return(TRUE);
}

///////////////////////////////////////////////////

// Switch to the opposite pane

void CMainFrame::SwitchPanes(CView *pView, BOOL bShift)
{
#ifdef _DEBUG
	ASSERT_KINDOF(CView, pView);
#endif

	if(pView == m_pDepotView)
		SetActiveView(m_pRightView);
	else
	{
		if (bShift)
			SetActiveView(m_pStatusView, TRUE);
		else
			SetActiveView(m_pDepotView);
	}
}

void CMainFrame::OnUpdateShowDeletedFiles(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY());
	pCmdUI->SetCheck(GET_P4REGPTR()->ShowDeleted());
}

void CMainFrame::OnShowDeletedFiles() 
{
	BOOL showDeleted = !GET_P4REGPTR()->ShowDeleted();
	GET_P4REGPTR()->SetShowDeleted( showDeleted );
	SetDepotCaption( GET_P4REGPTR()->ShowEntireDepot() );
	m_pDepotView->GetTreeCtrl().OnViewUpdate( TRUE );
}

void CMainFrame::OnUpdateSortByExtension(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY());
	pCmdUI->SetCheck(GET_P4REGPTR()->SortByExtension());
}

void CMainFrame::OnSortByExtension() 
{
	BOOL sortByExtension = !GET_P4REGPTR()->SortByExtension();
	GET_P4REGPTR()->SetSortByExtension( sortByExtension );
	m_pDepotView->GetTreeCtrl().OnViewUpdate( TRUE );
}

void CMainFrame::OnUpdatePositionToPattern(CCmdUI* pCmdUI) 
{
	CString txt = LoadStringResource(IDS_FINDPATTERN);
	pCmdUI->SetText( txt );
	pCmdUI->Enable( NULL == m_pFRDlg );
}

void CMainFrame::OnPositionToPattern() 
{
	CView * pView = GetActiveView();
	int flags = (pView == m_pStatusView) ? m_FindStatusFlags : m_FindWhatFlags;

	if ( NULL == m_pFRDlg )
	{
		m_pFRDlg = new CFindReplaceDialog();  // Must be created on the heap

		m_pFRDlg->m_fr.lStructSize = sizeof(FINDREPLACE);
		m_pFRDlg->m_fr.hwndOwner = this->m_hWnd;
		m_pFRDlg->Create( TRUE, m_FindWhatStr, _T(""), flags | FR_HIDEWHOLEWORD, this ); 
	}
	SetMessageText(LoadStringResource(IDS_FOR_HELP_PRESS_F1));
}

void CMainFrame::OnUpdatePositionToNext(CCmdUI* pCmdUI) 
{
	CView * pView = GetActiveView();
	pCmdUI->Enable( pView != m_pDepotView && pView != m_pDeltaView && !m_FindWhatStr.IsEmpty());
}

void CMainFrame::OnPositionToNext() 
{
	if (!m_FindWhatStr.IsEmpty())
	{
		CView * pView = GetActiveView();
		int flags = (pView == m_pStatusView) ? m_FindStatusFlags : m_FindWhatFlags;
		::PostMessage(pView->m_hWnd, WM_FINDPATTERN, 
				(WPARAM)flags | 0x80000000, (LPARAM)m_FindWhatStr.GetBuffer(0));
	}
}

void CMainFrame::OnUpdatePositionToPrev(CCmdUI* pCmdUI) 
{
	CView * pView = GetActiveView();
	pCmdUI->Enable( pView != m_pDepotView && pView != m_pDeltaView && !m_FindWhatStr.IsEmpty());
}

void CMainFrame::OnPositionToPrev() 
{
	if (!m_FindWhatStr.IsEmpty())
	{
		CView * pView = GetActiveView();
		int flags = (pView == m_pStatusView) ? m_FindStatusFlags : m_FindWhatFlags;
		::PostMessage(pView->m_hWnd, WM_FINDPATTERN, 
				((WPARAM)flags | 0x80000000) ^ FR_DOWN, 
				(LPARAM)m_FindWhatStr.GetBuffer(0));
	}
}

LRESULT CMainFrame::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
	LPFINDREPLACE lpfp = (LPFINDREPLACE)lParam;
	if (m_pFRDlg->FindNext() || m_pFRDlg->IsTerminating())
	{
		CView * pView = GetActiveView();
		m_FindWhatStr = lpfp->lpstrFindWhat;
		if (pView == m_pStatusView)
			m_FindStatusFlags = lpfp->Flags;
		else
			m_FindWhatFlags = lpfp->Flags;
		if (m_pFRDlg->FindNext())
		{
			::PostMessage(pView->m_hWnd, WM_FINDPATTERN, 
				(WPARAM)(lpfp->Flags), (LPARAM)m_FindWhatStr.GetBuffer(0));
			delete m_pFRDlg;
		}
		m_pFRDlg = NULL;
	}
	return 0;
}

void CMainFrame::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && ::CountClipboardFormats());
}

//	User can now copy a file path (in depot or local syntax) to the clipboard
//	and then when an Edit > Paste is done, the Depot Treeview will be
//	expanded to find the file
//
void CMainFrame::OnEditPaste()
{
	if (::OpenClipboard(NULL))
	{
#ifdef UNICODE
		HGLOBAL hGlob = ::GetClipboardData(CF_UNICODETEXT);
#else
		HGLOBAL hGlob = ::GetClipboardData(CF_TEXT);
#endif
		LPCTSTR p;
		if ((hGlob != NULL) && ((p = (LPCTSTR)::GlobalLock(hGlob)) != NULL))
		{
			CString itemStr = p;
			int    i;
			::GlobalUnlock(hGlob);
			if ((i = itemStr.Find(_T('#'))) != -1)
				itemStr = itemStr.Left(i);
			itemStr.TrimLeft();
			itemStr.TrimRight();
			if (itemStr.GetAt(0) != _T('/') && itemStr.GetAt(1) != _T(':'))
			{
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
				m_pDepotView->GetTreeCtrl().RunP4Files(itemStr);
			else if ((GetActiveView() == m_pDepotView)
				  || (GetActiveView() == m_pDeltaView)
				  || (itemStr.GetAt(0) == _T('/') && itemStr.GetAt(1) == _T('/'))
				  || (_istalpha( itemStr.GetAt(0) ) && itemStr.GetAt(1) == _T(':')))
				m_pDepotView->GetTreeCtrl().ExpandDepotString( itemStr, TRUE );
			else
			{
				CP4PaneView * pView = DYNAMIC_DOWNCAST(CP4PaneView, GetActiveView());
				if(pView)
					pView->OnEditPaste(itemStr);
			}
			::CloseClipboard();
		}
#ifdef UNICODE
		else if (((hGlob = ::GetClipboardData(m_CF_FILENAMEW)) != NULL)
#else
		else if (((hGlob = ::GetClipboardData(m_CF_FILENAME)) != NULL)
#endif
		&& ((p = (LPCTSTR)::GlobalLock(hGlob)) != NULL))
		{
            // expand short filenames to long filenames
            CString itemStr = DemanglePath(p);
			::GlobalUnlock(hGlob);
			if ((itemStr.FindOneOf(_T("?*")) == -1) 
				&& _istalpha( itemStr.GetAt(0) ) 
				&& itemStr.GetAt(1) == _T(':'))
				m_pDepotView->GetTreeCtrl().ExpandDepotString( itemStr, TRUE );
			else
				m_pDepotView->GetTreeCtrl().RunP4Files(itemStr);
			::CloseClipboard();
		}
		else
		{
			::CloseClipboard();
			COleDataObject oleobj;
			oleobj.AttachClipboard();
			FORMATETC fe;
			oleobj.BeginEnumFormats();
			while (oleobj.GetNextFormat(&fe))
			{
				if (HGLOBAL hGlobal = oleobj.GetGlobalData(fe.cfFormat))
				{
					TCHAR fmtName[1024];
					if(!GetClipboardFormatName(fe.cfFormat, fmtName, 1024) ||
						lstrcmp(fmtName, _T("CF_VSREFPROJECTITEMS")))
						continue;
					// when a single file is selected in the VS.NET solution explorer,
					// copying it to the clipboard yields a couple of formats.
					// The one registered as "CF_VSREFPROJECTITEMS" appears to
					// consist of the following:
					// DWORD offset;				// size of header?
					// BYTE [offset-sizeof(DWORD)];	// unknown use
					// per file:
					//   NULL terminated string string of WCHAR containing: 
					//    <project GUID>|<project filename>|<filename>
					// list terminating NULL WCHAR
					//
					// Note that only the 1st file in a list of multiple
					// files is found. This is because of potentially having
					// to go to the server when a node is expanded. Since this
					// server call is in the middle of a recursion and since
					// ExpandDepotString() will return while it awaits the
					// server results, looping when ExpandDepotString()
					// returns breaks its suspended recursion because it's
					// not written in a way that allows a second call before
					// the first one finishes.
					char *buf = (char*)GlobalLock(hGlobal);
					DWORD offset = *((DWORD*)buf);
					WCHAR * file = (WCHAR*)(buf + offset);
					m_pDepotView->GetTreeCtrl().SetAdd2ExpandItemList(TRUE);
					while (*file)
					{
						// let CString do conversion from unicode if needed
						CString itemStr(file);
						file += wcslen(file) + 1;

						int curPos = 0;
						// skip project GUID
						if(itemStr.Tokenize(_T("|"), curPos).IsEmpty())
							break;
						// skip project file name
						if(itemStr.Tokenize(_T("|"), curPos).IsEmpty())
							break;
						// get file name
						if((itemStr = itemStr.Tokenize(_T("|"), curPos)).IsEmpty())
							break;
						// might be a solution explorer folder, not a file
						if(itemStr.Mid(1,2) != _T(":\\"))
							continue;

						// I don't think VS passes any wild cards - but it doesn't hurt...
						if ((itemStr.FindOneOf(_T("?*")) != -1) || (itemStr.Find(_T("...")) != -1))
						{
							m_pDepotView->GetTreeCtrl().RunP4Files(itemStr);
							break;
						}
						else
						{
							m_pDepotView->GetTreeCtrl().ExpandDepotString( itemStr, TRUE );
							while (m_pDepotView->GetTreeCtrl().IsExpandDepotContinuing()
								|| SERVER_BUSY())
							{
								if ( !m_pDeltaView->GetTreeCtrl().PumpMessages( ) )
									break;
								Sleep(250);
							}
						}
					}
					GlobalUnlock(buf);
					m_pDepotView->GetTreeCtrl().SetAdd2ExpandItemList(FALSE);
					m_pDepotView->GetTreeCtrl().SelectExpandItemList();
				}
			}
		}
	}
}

//	These routines are called after the whole set of async commands that are run
//	at the time a connection is made to a port have finished.
//	If the user has opted to expand the depot treeview down a branch to a
//	particular node, start that expansion now.
//
void CMainFrame::FinishedGettingChgs(BOOL bNeed2RefreshOldChgs)
{
	while (SERVER_BUSY())
		Sleep(100);
	if (!TheApp()->m_SubmitPath.IsEmpty())
	{
		int n;
		CString str;
		POSITION pos;
		if (TheApp()->m_SubmitPath.GetAt(0) != _T('/'))
		{
			// have to convert to depot syntax
			CCmd_Where *pCmd1 = new CCmd_Where;
			pCmd1->Init(NULL, RUN_SYNC);
			if ( pCmd1->Run(TheApp()->m_SubmitPath) && !pCmd1->GetError() 
				&& pCmd1->GetDepotFiles()->GetCount() )
			{
				TheApp()->m_SubmitPath = pCmd1->GetDepotSyntax();
			}
			delete pCmd1;
			if (TheApp()->m_SubmitPathList.GetCount())
			{
				// copy the list
				CStringList list;
				for (pos = TheApp()->m_SubmitPathList.GetHeadPosition(); pos; )
					list.AddHead(TheApp()->m_SubmitPathList.GetNext(pos));
				TheApp()->m_SubmitPathList.RemoveAll();
				for (POSITION pos = list.GetHeadPosition(); pos; )
				{
					str = list.GetNext(pos);
					if (str.GetAt(0) != _T('/'))
					{
						CCmd_Where *pCmd1 = new CCmd_Where;
						pCmd1->Init(NULL, RUN_SYNC);
						if ( pCmd1->Run(str) && !pCmd1->GetError() 
							&& pCmd1->GetDepotFiles()->GetCount() )
						{
							str = pCmd1->GetDepotSyntax();
						}
						delete pCmd1;
					}
					TheApp()->m_SubmitPathList.AddHead(str);
				}
			}
		}
		long chgnbr = PositionChgs( TheApp()->m_SubmitPath, TRUE, FALSE );
		if (chgnbr >= 0 && TheApp()->m_SubmitPathList.GetCount())
		{
			for (pos = TheApp()->m_SubmitPathList.GetHeadPosition(); pos; )
			{
				// wait a bit in 1/10 sec intervals to see if the server request finishes
				int t=5*GET_P4REGPTR()->BusyWaitTime();
				do
				{
					Sleep(50);
					t -= 50;
				} while (SERVER_BUSY() && t > 0);
				str = TheApp()->m_SubmitPathList.GetNext(pos);
				if (chgnbr != (n = PositionChgs( str, TRUE, FALSE, TRUE )))
				{
					CString txt;
					txt.FormatMessage(IDS_FILEINDIFFERENTCHGS_n_s_n_s, 
						chgnbr, TheApp()->m_SubmitPath, n, str);
					AfxMessageBox( txt,  MB_ICONSTOP );
					::PostMessage(m_hWnd, WM_COMMAND, ID_APP_EXIT, 0);
					return;
				}
			}
		}
		if (chgnbr > 0)
			m_pDeltaView->SendMessage(WM_COMMAND, ID_EDIT_SELECT_ALL, 0);
		Sleep(0);
		SET_BUSYCURSOR();
		// wait a bit in 1/10 sec intervals to see if the server request finishes
		int t=5*GET_P4REGPTR()->BusyWaitTime();
		do
		{
			Sleep(50);
			t -= 50;
		} while (SERVER_BUSY() && t > 0);
		if (chgnbr >= 0)
			m_pDeltaView->GetTreeCtrl().OnChangeSubmit();
		else
		{
			CString txt;
			txt.FormatMessage(IDS_CANTSUBMITFILE_s, TheApp()->m_SubmitPath);
			AfxMessageBox( txt,  MB_ICONSTOP );
			::PostMessage(m_hWnd, WM_COMMAND, ID_APP_EXIT, 0);
		}
		return;
	}

	m_Need2RefreshOldChgs = bNeed2RefreshOldChgs;
	if (m_Need2Poll4Jobs)
	{
		m_Need2Poll4Jobs = 0;
		if (m_currentTab == 5)
			m_pJobView->GetListCtrl().OnViewUpdate( );
	}
	else if (TheApp()->m_InitialView)
	{
		switch (TheApp()->m_InitialView)
		{
		case _T('L'):
			OnViewLabels();
			break;
		case _T('B'):
			OnViewBranches();
			break;
		case _T('U'):
			OnViewUsers();
			break;
		case _T('C'):
			OnViewClients();
			break;
		case _T('J'):
			OnViewJobs();
			break;
		case _T('S'):
			OnViewSubmitted();
			break;
		}
		TheApp()->m_InitialView = _T('\0');
	}
	else ExpandDepotIfNeedBe();
}

void CMainFrame::ExpandDepotIfNeedBe()
{
	if (m_Need2ExpandDepot || !TheApp()->m_ExpandPath.IsEmpty())
	{
		m_Need2ExpandDepot = 0;
		if (TheApp()->m_ExpandPath.IsEmpty())
		{
			ExpandDepotString(GET_P4REGPTR()->GetExpandPath(), TRUE);
		}
		else
		{
			ExpandDepotString(TheApp()->m_ExpandPath, TRUE);
			TheApp()->m_ExpandPath.Empty();
		}
	}
}

CString CMainFrame::GetCurrentItemPath() 
{
	return m_pDepotView->GetTreeCtrl().GetCurrentItemPath();
}

void CMainFrame::OnUpdateWinExplore(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY());
}

void CMainFrame::OnWinExplore() 
{
	CString path = TheApp()->m_ClientRoot;
	if (path.IsEmpty() || path == _T("null"))
		path = _T("C:\\");

	if (path.FindOneOf(_T(" &()[]{}^=;!'+,`~")) != -1)
	{
		path.TrimLeft();
		path.TrimRight();
		path = _T('\"') + path + _T('\"');
	}

    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    CreateProcess(NULL, const_cast<LPTSTR>((LPCTSTR)(TheApp()->GetExplorer() + path)), 
					NULL, NULL, 
#ifdef UNICODE
					FALSE, DETACHED_PROCESS | NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT, 
#else
					FALSE, DETACHED_PROCESS | NORMAL_PRIORITY_CLASS, 
#endif
					P4GetEnvironmentStrings(), 
					NULL, &si, &pi);
}

void CMainFrame::OnUpdateCmdPrompt(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY());
}

void CMainFrame::OnCmdPromptPublic() 
{
	OnCmdPrompt();
}

void CMainFrame::OnCmdPrompt() 
{
	// In order to create the DOS box in a reasonable directory,
	// we need the root of the current client.  So first go get that after setting a
	// flag to indicate to the p4 info completion routine that that is what we are up to.

	if (TheApp()->m_ClientRoot.IsEmpty())
	{
		m_GetCliRootAndContinue = P4INFO_DOSBOX;
		OnPerforceInfo();
	}
	else
		OnCmdPromptContinue();
}

void CMainFrame::OnCmdPromptContinue()
{
	m_GetCliRootAndContinue = 0;
	TheApp()->m_ClientRoot.Replace('/', '\\');

    TCHAR	cmd[MAX_PATH+1];
	GetEnvironmentVariable(_T("ComSpec"), cmd, MAX_PATH);

    // TheApp()->m_ClientRoot may be empty, if not connected to server
    // since CreateProcess expects a full path, including drive, this won't do
    // so, this gyration will ensure that we provide a legal path to CreateProcess
	TCHAR	saveDir[MAX_PATH+1];
	TCHAR	useDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, saveDir);
	SetCurrentDirectory(TheApp()->m_ClientRoot);
    GetCurrentDirectory(MAX_PATH, useDir);
	SetCurrentDirectory(saveDir);
    
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
        P4GetEnvironmentStrings(), useDir, &si, &pi);
}

// Dropdown menus for Bookmark and Favorite toolbar buttons
void CMainFrame::OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	// this function handles the dropdown menus from the toolbar
	NMTOOLBAR* pNMToolBar = (NMTOOLBAR*)pNotifyStruct;
	CRect rect;

	// translate the current toolbar item rectangle into screen coordinates
	// so that we'll know where to pop up the menu
	m_wndToolBar.GetToolBarCtrl().GetRect(pNMToolBar->iItem, &rect);
	rect.top = rect.bottom;
	::ClientToScreen(pNMToolBar->hdr.hwndFrom, &rect.TopLeft());
	CMenu *pMenu = 0;
	switch(pNMToolBar->iItem)
	{
	case ID_FAVORITES:
		pMenu = GetFavoriteMenu();
		break;
	case ID_BOOKMARKS:
		pMenu = GetBookmarkMenu();
		break;
	}
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.left, rect.top + 1, AfxGetMainWnd());
	*pResult = TBDDRET_DEFAULT;
}

// Bookmark Menu and Toolbar button
CMenu * CMainFrame::GetBookmarkMenu() 
{
	CMenu *pMenu= GetMenu();
	pMenu= pMenu->GetSubMenu(2);
	return pMenu->GetSubMenu(22);
}

void CMainFrame::OnUpdateBookmarks(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SERVER_BUSY() );
}

void CMainFrame::OnBookmarks()
{
 	CMenu *pBkMkMenu = GetBookmarkMenu();
 	CRect rect;
 	m_wndHSplitter.GetWindowRect(rect);
 	pBkMkMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.left, rect.top, AfxGetMainWnd());
}

void CMainFrame::OnOrganizeBookmarks() 
{
	CBookmarks	dlg;
	dlg.DoModal();
	LoadBkMkMenu();
}

void CMainFrame::LoadBkMkMenu()
{
	BOOL bSM[MAX_BOOKMARKS+1];
	CString str[MAX_BOOKMARKS+1];
	CString temp;
	int		i, j;
	int		n, s;

	CMenu *pSubMenu;
	CMenu *pBkMkMenu = GetBookmarkMenu();

	// clear the old BkMks submenus
	for (s = -1; ++s < MAX_BOOKMARK_SUBMENUS && ::IsMenu(m_BkMkSubMenu[s].m_hMenu); )
	{
		while (m_BkMkSubMenu[s].DeleteMenu( 0, MF_BYPOSITION ))
			;
		m_BkMkSubMenu[s].DestroyMenu();
	}

	// clear the old BkMks menu
	for (i = -1; ++i < MAX_BOOKMARKS+1; )
	{
		if (!(pBkMkMenu->DeleteMenu( 2, MF_BYPOSITION )))
			break;
	}

	// Get the strings
	for(i = -1; ++i < MAX_BOOKMARKS; )
	{
		str[i] = GET_P4REGPTR()->GetBkMkMenuName(i);
		bSM[i] = GET_P4REGPTR()->GetBkMkIsSubMenu(i);
	}

	if (str[0].GetLength())
	{
		pBkMkMenu->AppendMenu(MF_SEPARATOR);

		// Load all BkMk menu items into BkMk menu
		for(s=i=-1, j=1; ++i < MAX_BOOKMARKS && !bSM[i]; )
		{
			if (str[i].GetLength() > 0)
			{
				int id = ID_BKMK_1+j-1;
				if (id < ID_BKMK_11)
				{
					int nbr = id == ID_BKMK_10 ? 0 : id - ID_BKMK_1 + 1;
					CString speedkey;
					speedkey.FormatMessage(IDS_TAB_ALT_PLUS_n, nbr);
					if ((n = str[i].Find(_T('#'))) == -1)
					{
						str[i] += _T('\t');
						speedkey.TrimLeft();
					}
					else
						str[i].SetAt(n, _T('\t'));
					str[i] += speedkey;
				}
				else
				{
					if ((n = str[i].Find(_T('#'))) != -1)
						str[i].SetAt(n, _T('\t'));
				}
				pBkMkMenu->AppendMenu(MF_STRING, id, str[i]);
				j++;
			}
		}
		while ((i < MAX_BOOKMARKS) && (++s < MAX_BOOKMARK_SUBMENUS))
		{
			CString name = str[i];
			pSubMenu = &m_BkMkSubMenu[s];
			pSubMenu->CreatePopupMenu();
			while (++i < MAX_BOOKMARKS && !bSM[i])
			{
				if (str[i].GetLength() > 0)
				{
					int id = ID_BKMK_1+j-1;
					if (id < ID_BKMK_11)
					{
						int nbr = id == ID_BKMK_10 ? 0 : id - ID_BKMK_1 + 1;
						CString speedkey;
						speedkey.FormatMessage(IDS_TAB_ALT_PLUS_n, nbr);
						if ((n = str[i].Find(_T('#'))) == -1)
						{
							str[i] += _T('\t');
							speedkey.TrimLeft();
						}
						else
							str[i].SetAt(n, _T('\t'));
						str[i] += speedkey;
					}
					pSubMenu->AppendMenu(MF_STRING, id, str[i]);
					j++;
				}
			}
			pBkMkMenu->AppendMenu(MF_POPUP, (UINT_PTR) pSubMenu->GetSafeHmenu(), name);
		}
	}
}

void CMainFrame::OnUpdateBkMk(CCmdUI* pCmdUI)
{
  	pCmdUI->Enable(!SERVER_BUSY());
}

void CMainFrame::OnBkMk(UINT nID)
{
	int i;
	CString menuItem;
	if ((i = GetMenu()->GetMenuString(nID, menuItem, MF_BYCOMMAND)) == 0)
	{
		MessageBeep(0);
		return;
	}
	if ((i = menuItem.Find(_T('\t'))) != -1)
		menuItem = menuItem.Left(i);
	if ((menuItem.GetAt(0) == _T('/'))	// no PCU info & depot syntax?
	 || (menuItem.GetAt(1) == _T(':') && menuItem.GetAt(2) == _T('\\'))) // no PCU & local syntax?
	{
		ExpandDepotString( menuItem, TRUE );
		return;
	}

	CString old_port = GET_P4REGPTR()->GetP4Port();
	CString old_client = GET_P4REGPTR()->GetP4Client();
	CString old_user = GET_P4REGPTR()->GetP4User();
	CString port;
	CString client;
	CString user;
	if (menuItem.GetAt(0) == _T('*'))
	{
		port = old_port;
		menuItem = menuItem.Mid(1);
		menuItem.TrimLeft(_T(' '));
	}
	else
	{
		if ((i = menuItem.Find(_T(' '))) != -1)
		{
			port = menuItem.Left(i);
			menuItem = menuItem.Mid(i);
			menuItem.TrimLeft();
		}
	}
	if (menuItem.GetAt(0) == _T('*'))
	{
		client = old_client;
		menuItem = menuItem.Mid(1);
		menuItem.TrimLeft(_T(' '));
	}
	else
	{
		if ((i = menuItem.Find(_T(' '))) != -1)
		{
			client = menuItem.Left(i);
			menuItem = menuItem.Mid(i);
			menuItem.TrimLeft();
		}
	}
	if (menuItem.GetAt(0) == _T('*'))
	{
		user = old_user;
		menuItem.TrimLeft(_T("* "));
	}
	else
	{
		if ((i = menuItem.Find(_T(' '))) != -1)
		{
			user = menuItem.Left(i);
			menuItem = menuItem.Mid(i);
			menuItem.TrimLeft();
		}
	}
	if (port.IsEmpty() || client.IsEmpty() || user.IsEmpty())
	{
		CString txt;
		GetMenu()->GetMenuString(nID, menuItem, MF_BYCOMMAND);
		txt.FormatMessage(IDS_BKMK_PCU_ERROR, menuItem);
		AfxMessageBox(txt, MB_OK);
		return;
	}

	// are all of port client user the same as the current ones?
	if (port == old_port && user == old_user && client == old_client)
	{
		ExpandDepotString( menuItem, TRUE );
		return;
	}

	// one or more of port, client, user is different
	TheApp()->m_ExpandPath = menuItem;

	// change only the ones that are different
	if (port != old_port)
		if (!GET_P4REGPTR()->SetP4Port(port, TRUE, FALSE, FALSE))
			AfxMessageBox( IDS_UNABLE_TO_WRITE_P4PORT_TO_THE_REGISTRY, MB_ICONEXCLAMATION);

	if (user != old_user)
		if (!GET_P4REGPTR()->SetP4User(user, TRUE, FALSE, FALSE))
			AfxMessageBox( IDS_UNABLE_TO_WRITE_P4USER_TO_THE_REGISTRY, MB_ICONEXCLAMATION);

	if (client != old_client)
	{
		if(!ClientSpecSwitch(client, TRUE, old_port != port))	// calls OnPerforceOptions()
		{	// put the old stuff back for port and user if client switch failed
			if (port != old_port)
				GET_P4REGPTR()->SetP4Port(old_port, TRUE, FALSE, FALSE);
			if (user != old_user)
				GET_P4REGPTR()->SetP4User(old_user, TRUE, FALSE, FALSE);
			return;
		}
	}
	else 
		MainFrame()->OnPerforceOptions(FALSE, old_port != port);// if not changing client, need to call this for port/user chg
}

// Favorites Menu and Toolbar Button
CMenu * CMainFrame::GetFavoriteMenu() 
{
	CMenu *pMenu= GetMenu();
	pMenu= pMenu->GetSubMenu(3);
	return pMenu->GetSubMenu(5);
}

void CMainFrame::OnUpdateFavorites(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SERVER_BUSY() );	
}

void CMainFrame::OnUpdateOrganizeFavorites(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SERVER_BUSY() );	
}

void CMainFrame::OnOrganizeFavorites() 
{
	CFavorites	dlg;
	dlg.DoModal();
	LoadFavMenu();
}

void CMainFrame::LoadFavMenu()
{
	BOOL bSM[MAX_FAVORITES+1];
	CString str[MAX_FAVORITES+1];
	CString temp;
	int		i, j;
	int		n, s;

	CMenu *pSubMenu;
	CMenu *pFavMenu = GetFavoriteMenu();

	// clear the old Favs submenus
	for (s = -1; ++s < MAX_FAVORITE_SUBMENUS && ::IsMenu(m_FavSubMenu[s].m_hMenu); )
	{
		while (m_FavSubMenu[s].DeleteMenu( 2, MF_BYPOSITION ))
			;
		m_FavSubMenu[s].DestroyMenu();
	}

	// clear the old Favs menu
	for (i = -1; ++i < MAX_FAVORITES+1; )
	{
		if (!(pFavMenu->DeleteMenu( 2, MF_BYPOSITION )))
			break;
	}

	// Get the strings
	for(i = -1; ++i < MAX_FAVORITES; )
	{
		bSM[i] = GET_P4REGPTR()->GetFavIsSubMenu(i);
		str[i] = GET_P4REGPTR()->GetFavMenuName(i);
		// remove the stuff from any @ thru any #
		if ((n = str[i].Find(_T('@'))) != -1)
		{
			j = str[i].Find(_T('#'));
			if (j == -1)
				j = str[i].GetLength();
			if (j > n)
			{
				j -= n;
				str[i].Delete(n, j);
			}
		}
	}

	if (str[0].GetLength())
	{
		pFavMenu->AppendMenu(MF_SEPARATOR);

		// Load all Fav menu items into Fav menu
		for(s=i=-1, j=1; ++i < MAX_FAVORITES && !bSM[i]; )
		{
			if (str[i].GetLength() > 0)
			{
				int id = ID_FAV_1+j-1;
				if (id < ID_FAV_11)
				{
					int nbr = id == ID_FAV_10 ? 0 : id - ID_FAV_1 + 1;
					CString speedkey;
					speedkey.FormatMessage(IDS_SHIFT_ALT_PLUS_n, nbr);
					if ((n = str[i].Find(_T('#'))) == -1)
					{
						str[i] += _T('\t');
						speedkey.TrimLeft();
					}
					else
						str[i].SetAt(n, _T('\t'));
					str[i] += speedkey;
				}
				else if ((n = str[i].Find(_T('#'))) != -1)
				{
					str[i].SetAt(n, _T('\t'));
				}
				pFavMenu->AppendMenu(MF_STRING, id, str[i]);
				j++;
			}
		}
		while ((i < MAX_FAVORITES) && (++s < MAX_FAVORITE_SUBMENUS))
		{
			CString name = str[i];
			pSubMenu = &m_FavSubMenu[s];
			pSubMenu->CreatePopupMenu();
			while (++i < MAX_FAVORITES && !bSM[i])
			{
				if (str[i].GetLength() > 0)
				{
					int id = ID_FAV_1+j-1;
					if (id < ID_FAV_11)
					{
						int nbr = id == ID_FAV_10 ? 0 : id - ID_FAV_1 + 1;
						CString speedkey;
						speedkey.FormatMessage(IDS_SHIFT_ALT_PLUS_n, nbr);
						if ((n = str[i].Find(_T('#'))) == -1)
						{
							str[i] += _T('\t');
							speedkey.TrimLeft();
						}
						else
							str[i].SetAt(n, _T('\t'));
						str[i] += speedkey;
					}
					else if ((n = str[i].Find(_T('#'))) != -1)
					{
						str[i].SetAt(n, _T('\t'));
					}
					pSubMenu->AppendMenu(MF_STRING, id, str[i]);
					j++;
				}
			}
			pFavMenu->AppendMenu(MF_POPUP, (UINT_PTR) pSubMenu->GetSafeHmenu(), name);
		}
	}
}

void CMainFrame::OnUpdateFav(CCmdUI* pCmdUI)
{
  	pCmdUI->Enable(!SERVER_BUSY());
}

void CMainFrame::OnFav(UINT nID)
{
	int i;
	int firstsep;
	int secondsep;
	CString menuStr;
	CString port;
	CString client;
	CString user;
	CString wasUser = GET_P4REGPTR()->GetP4User();
	CString wasPort = GET_P4REGPTR()->GetP4Port();

	if (IsModlessUp() || !GetMenu()->GetMenuString(nID, menuStr, MF_BYCOMMAND))
	{
		ASSERT(NULL);
		return;
	}
	if ((i = menuStr.Find(_T('\t'))) != -1)
		menuStr = menuStr.Left(i);

	if (menuStr.GetAt(0) == _T('\"'))
		firstsep = menuStr.Find(_T('\"'),1) + 1;
	else
		firstsep = menuStr.Find(_T(' '));
	if (firstsep == -1)
	{
		port  = menuStr;
		client= GET_P4REGPTR()->GetP4Client();
		user  = wasUser;
	}
	else
	{
		port  = menuStr.Left(firstsep);
		port.TrimLeft(_T('\"'));
		port.TrimRight(_T('\"'));
		secondsep= menuStr.Find(_T(' '), firstsep+1);
		if (secondsep == -1)
		{
			client= menuStr.Mid(firstsep+1);
			user  = wasUser;
		}
		else
		{
			client= menuStr.Mid(firstsep+1, secondsep - firstsep - 1);
			user  = menuStr.Right(menuStr.GetLength() - secondsep - 1);
		}
	}

	if (GET_P4REGPTR()->GetExpandFlag() == 1)
		GET_P4REGPTR()->AddMRUPcuPath(GetCurrentItemPath());

	GET_P4REGPTR()->SetP4Port(port, TRUE, FALSE, FALSE);
	GET_P4REGPTR()->SetP4User(user, TRUE, FALSE, FALSE);
	if (!ClientSpecSwitch(client, TRUE, wasPort != port))// calls OnPerforceOptions()
	{
		GET_P4REGPTR()->SetP4Port(wasPort, TRUE, FALSE, FALSE);	// put old port back if client switched failed
		GET_P4REGPTR()->SetP4User(wasUser, TRUE, FALSE, FALSE);	// put old user back if client switched failed
	}
}

void CMainFrame::OnUpdateAddFavorite(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SERVER_BUSY() );	
}

void CMainFrame::OnAddFavorite() 
{
	BOOL bSM[MAX_FAVORITES+1];
	CString str[MAX_FAVORITES+1];
	CString temp;
	int		i, n;
	int 	j = 0;
	int		k = MAX_FAVORITES+1;
	int		s = MAX_FAVORITES+1;
	int		found = -1;
	LPCTSTR p = GET_P4REGPTR()->GetP4Port();
	LPCTSTR c = GET_P4REGPTR()->GetP4Client();
	LPCTSTR u = GET_P4REGPTR()->GetP4User();
	CString txt;
	CString pcu;
	CString oldpcu;
	CString newpcu;
	if (p && *p && c && *c && u && *u)
	{
		CString port = p;
		if (port.FindOneOf(_T("@#")) != -1)
		{
			txt.FormatMessage(IDS_FAV_CANT_HAVE_AT_OR_POUND_IN_PORT_s, port);
			AfxMessageBox( txt, MB_ICONEXCLAMATION );
			return;
		}
		LPCTSTR q;
		for (q = p; *++q; )
		{
			if (*q == _T(' '))
				break;
		}
		pcu.Format(*q ? _T("\"%s\" %s %s") : _T("%s %s %s"), p, c, u);
	}
	else
	{
		MessageBeep(0);
		return;
	}

	// Get the strings
	for(i = -1; ++i < MAX_FAVORITES; )
	{
		txt = str[i] = GET_P4REGPTR()->GetFavMenuName(i);
		if ((n = txt.Find(_T('#'))) > 0)
			txt = txt.Left(n);
		if ((n = txt.Find(_T('@'))) > 0)
			txt = txt.Left(n);
		if (txt == pcu)
		{
			AddToStatus(pcu + LoadStringResource(IDS_ALREADY_FAVORITE));
			oldpcu = pcu;
			found = i;
		}
		bSM[i] = GET_P4REGPTR()->GetFavIsSubMenu(i);
		if (bSM[i])
		{
			j = i;
			if (s == MAX_FAVORITES+1)
				s = i;
		}
		else if (!(str[i].IsEmpty()))
			j = i;
		else if (k == MAX_FAVORITES+1)
			k = i;
	}
	if (k == MAX_FAVORITES+1)
	{
		MessageBeep(0);
		return;
	}

	// Allow the user to enter a description or make changes
	CFavEditDlg dlg;
	if (found != -1)
	{
		if ((n = str[found].Find(_T('#'))) > 0)
		{
			pcu += _T('\t') + str[found].Mid(n+1);
			txt = str[found].Left(n);
		}
		else txt = str[found];
		if ((n = txt.Find(_T('@'))) != -1)
			txt = txt.Mid(n+1);
		else
			txt.Empty();
		dlg.SetTitle(LoadStringResource(IDS_CHANGE_FAVORITE));
	}
	dlg.SetNewMenuName(pcu);
	dlg.SetIsSubMenu(FALSE);
	dlg.SetRadioShow(2);
	dlg.SetCanCr8SubMenu(FALSE);
	dlg.SetFocusDesc(TRUE);
	if ((dlg.DoModal() != IDOK) || !((dlg.GetNewMenuName()).GetLength()))
		return;

	pcu = dlg.GetNewMenuName();
	if ((n = pcu.Find(_T('\t'))) != -1)
	{
		newpcu = pcu.Left(n);
		CString desc = pcu.Mid(n+1);
		if (!txt.IsEmpty())
			pcu = newpcu + _T('@') + txt;
		else
			pcu = newpcu;
		pcu += _T('#') + desc;
	}
	else
	{
		newpcu = pcu;
		if (!txt.IsEmpty())
			pcu += _T('@') + txt;
	}

	// Handle replacment if p/c/u are same as found
	BOOL bAdded = FALSE;
	if ((found != -1) && (oldpcu == newpcu))
	{
		s = -1;
		str[found] = pcu;
		bAdded = TRUE;
	}

	// Reload the strings
	for(i = j = -1; ++j < MAX_FAVORITES; )
	{
		if (j == s)
		{
			GET_P4REGPTR()->SetFavMenuName(j, pcu);
			GET_P4REGPTR()->SetFavIsSubMenu(j, FALSE);
			bAdded = TRUE;
		}
		else
		{
			GET_P4REGPTR()->SetFavMenuName(j, str[++i]);
			GET_P4REGPTR()->SetFavIsSubMenu(j, bSM[i]);
		}
	}
	if (!bAdded)
	{
		GET_P4REGPTR()->SetFavMenuName(k, pcu);
		GET_P4REGPTR()->SetFavIsSubMenu(k, FALSE);
	}
	LoadFavMenu();
	pcu.Replace(_T('#'), _T(' '));
	AddToStatus(pcu + LoadStringResource(IDS_ADDED_TO_FAVORITES));
}

void CMainFrame::OnUpdateLogout(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SERVER_BUSY() && GET_SERVERLEVEL() >= 18);	
}

void CMainFrame::OnLogout() 
{
	CCmd_Logout *pCmd = new CCmd_Logout;				// run p4 logout
	pCmd->Init(NULL, RUN_SYNC);
	pCmd->Run(FALSE);							// run the logout command
	delete pCmd;
}

void CMainFrame::OnCustomizeTools() 
{
	CToolsDlg	dlg;
	dlg.DoModal();
	LoadToolsMenu();
}

void CMainFrame::LoadToolsMenu()
{
	BOOL bSM[MAX_TOOLS+1];
	CString str[MAX_TOOLS+1];
	CString temp;
	int		i, j;
	int		s;

	CMenu *pMenu= GetMenu();
	CMenu *pToolsMenu = pMenu->GetSubMenu(10);
	CMenu *pSubMenu;

	// clear the old tools submenus
	for (s = -1; ++s < MAX_TOOLS_SUBMENUS && ::IsMenu(m_ToolsSubMenu[s].m_hMenu); )
	{
		while (m_ToolsSubMenu[s].DeleteMenu( 0, MF_BYPOSITION ))
			;
		m_ToolsSubMenu[s].DestroyMenu();
	}

	// clear the old tools menu
	for (i = -1; ++i < MAX_TOOLS+1; )
	{
		if (!(pToolsMenu->DeleteMenu( 3, MF_BYPOSITION )))
			break;
	}

	// Get the strings
	for(i = -1; ++i < MAX_TOOLS; )
	{
		str[i] = GET_P4REGPTR()->GetToolMenuName(i);
		bSM[i] = GET_P4REGPTR()->GetToolIsSubMenu(i);
	}

	if (str[0].GetLength())
	{
		pToolsMenu->AppendMenu(MF_SEPARATOR);

		// Load all Tool menu items into Tools menu
		for(s=i=-1, j=1; ++i < MAX_TOOLS && !bSM[i]; )
		{
			if (str[i].GetLength() > 0)
			{
				pToolsMenu->AppendMenu(MF_STRING, ID_TOOL_1+j-1, str[i]);
				j++;
			}
		}
		while ((i < MAX_TOOLS) && (++s < MAX_TOOLS_SUBMENUS))
		{
			CString name = str[i];
			pSubMenu = &m_ToolsSubMenu[s];
			pSubMenu->CreatePopupMenu();
			j++;
			while (++i < MAX_TOOLS && !bSM[i])
			{
				if (str[i].GetLength() > 0)
				{
					pSubMenu->AppendMenu(MF_STRING, ID_TOOL_1+j-1, str[i]);
					j++;
				}
			}
			pToolsMenu->AppendMenu(MF_POPUP, (UINT_PTR) pSubMenu->GetSafeHmenu(), name);
		}
	}
}

void CMainFrame::OnUpdateToolsImport(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!SERVER_BUSY());
}

void CMainFrame::OnToolsImport()
{
	CFileDialog fDlg(TRUE, NULL, NULL,  
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON,
		LoadStringResource(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_FILTER), this,
		m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME)); 
	
	TCHAR buf[LONGPATH+1];

	lstrcpy(buf, _T("tools.txt"));  

    fDlg.m_ofn.lpstrFile= buf; 
	fDlg.m_ofn.nMaxFile= LONGPATH; 

	// Set the dlg caption
    CString title = LoadStringResource(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_TITLE);
	fDlg.m_ofn.lpstrTitle = title;
	// We dont need no stinking file title
	fDlg.m_ofn.lpstrFileTitle=NULL;
	
	// Set the initial directory
	fDlg.m_ofn.lpstrInitialDir=GET_P4REGPTR()->GetTempDir();

	// Set the user defined filter
	TCHAR customFilterBuf[512] = _T("");
	fDlg.m_ofn.lpstrCustomFilter= customFilterBuf;
	fDlg.m_ofn.nMaxCustFilter=512;

	CString errorTxt;
	INT_PTR retcode=fDlg.DoModal();

	if(retcode == IDOK)
	{
		CString filename = fDlg.GetPathName();
		ImportTools(&filename, &errorTxt);
	}
	else if(retcode == IDCANCEL)  // an error
	{
		DWORD exError=CommDlgExtendedError();
		if(exError != 0)
			errorTxt.FormatMessage(IDS_EXTENDED_ERROR_n_IN_FILEDIALOG, exError);
	}
	else
	{
		DWORD error=GetLastError();
		if(error)
		{
			LPVOID lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				error,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);
			errorTxt = (TCHAR *)lpMsgBuf;
		}
		else
			errorTxt=LoadStringResource(IDS_UNKNOWN_FILEDIALOG_ERROR);
	}

	if(errorTxt.GetLength() > 0)
		AfxMessageBox(errorTxt, MB_ICONSTOP);
}

void CMainFrame::ImportTools(CString *filename, CString *errorTxt)
{
	int toolnbr;
	int nbradded = 0;

	HANDLE hImportFile;

	if ((hImportFile=CreateFile(*filename, GENERIC_READ, 
				FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0)) == INVALID_HANDLE_VALUE)
	{
		DWORD exError=GetLastError();
		errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_WRITE_ERROR_s_n, *filename, exError);
	}
	else
	{
		for (toolnbr=MAX_TOOLS; toolnbr--; )
		{
			CString toolname = GET_P4REGPTR()->GetToolMenuName(toolnbr);
			if (!toolname.IsEmpty())
				break;
		}
		DWORD NumberOfBytesRead;
		DWORD fsize = GetFileSize(hImportFile, NULL);
		HGLOBAL hText= ::GlobalAlloc(GMEM_SHARE | GMEM_ZEROINIT, fsize+4);
		LPTSTR pStr= (LPTSTR) ::GlobalLock( hText );

		if (ReadFile(hImportFile, pStr, fsize, &NumberOfBytesRead, NULL))
		{
#ifdef UNICODE
			if (*pStr == 0xFEFF)
					pStr++;
			else
			{
				HGLOBAL h= ::GlobalAlloc(GMEM_SHARE | GMEM_ZEROINIT, fsize*2+4);
				LPWSTR pUTF= (LPWSTR) ::GlobalLock( h );
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pStr, -1, pUTF, fsize*2+4);
				pStr = pUTF;
				::GlobalUnlock( hText );
				::GlobalFree( hText );
				hText = h;
			}
#else
			if ((unsigned char)*pStr == (unsigned char)0xFF 
				&& (unsigned char)*(pStr+1) == (unsigned char)0xFE)
			{
				AfxMessageBox(IDS_UNABLE_TO_IMPORT_UNICODE_FILE);
				return;
			}
#endif
			while (*pStr)
			{
				if ((*pStr++ == _T('>')) && (*pStr++ == _T('>')))
				{
					if (++toolnbr >= MAX_TOOLS)
					{
						errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_TOO_MANY_TOOLS_s, pStr);
						break;
					}
					else
					{
						LPTSTR p = pStr;
						CString ToolMenuName;
						CString ToolCommand;
						CString ToolArgs;
						CString ToolInitDir;
						CString ToolPromptText;
						BOOL ToolIsConsole;
						BOOL ToolIsPrompt;
						BOOL ToolIsOutput2Status = FALSE;
						BOOL ToolIsCloseOnExit;
						BOOL ToolIsShowBrowse;
						BOOL ToolIsSubMenu;
						BOOL ToolOnContext;
						BOOL ToolIsRefresh;
						CString buffer;
						pStr = GetNextToolToken(pStr, ToolMenuName);
						if (pStr == NULL)
						{
							errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_ERROR_PARSING_s, p);
							break;
						}
						pStr = GetNextToolToken(pStr, ToolCommand);
						if (pStr == NULL)
						{
							errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_ERROR_PARSING_s, ToolMenuName);
							break;
						}
						pStr = GetNextToolToken(pStr, ToolArgs);
						if (pStr == NULL)
						{
							errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_ERROR_PARSING_s, ToolMenuName);
							break;
						}
						pStr = GetNextToolToken(pStr, ToolInitDir);
						if (pStr == NULL)
						{
							errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_ERROR_PARSING_s, ToolMenuName);
							break;
						}
						pStr = GetNextToolToken(pStr, buffer);
						if (pStr == NULL)
						{
							errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_ERROR_PARSING_s, ToolMenuName);
							break;
						}
						ToolIsConsole = buffer.GetAt(0) == _T('0') ? FALSE : TRUE;
						pStr = GetNextToolToken(pStr, buffer);
						if (pStr == NULL)
						{
							errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_ERROR_PARSING_s, ToolMenuName);
							break;
						}
						ToolIsPrompt = buffer.GetAt(0) == _T('0') ? FALSE : TRUE;
						pStr = GetNextToolToken(pStr, buffer);
						if (pStr == NULL)
						{
							errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_ERROR_PARSING_s, ToolMenuName);
							break;
						}
						ToolIsCloseOnExit = buffer.GetAt(0) == _T('0') ? FALSE : TRUE;
						pStr = GetNextToolToken(pStr, buffer);
						if (pStr == NULL)
						{
							errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_ERROR_PARSING_s, ToolMenuName);
							break;
						}
						ToolIsSubMenu = buffer.GetAt(0) == _T('0') ? FALSE : TRUE;
						if (pStr == NULL)
						{
							errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_ERROR_PARSING_s, ToolMenuName);
							break;
						}
						if (*pStr != '\n')
						{
							pStr = GetNextToolToken(pStr, buffer);
							ToolIsShowBrowse = buffer.GetAt(0) == _T('0') ? FALSE : TRUE;
							if (pStr == NULL)
							{
								errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_ERROR_PARSING_s, ToolMenuName);
								break;
							}
							if (*pStr != '\n')
							{
								pStr = GetNextToolToken(pStr, buffer);
								ToolOnContext = buffer.GetAt(0) == _T('0') ? FALSE : TRUE;
								if (pStr == NULL)
								{
									errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_ERROR_PARSING_s, ToolMenuName);
									break;
								}
								if (*pStr != '\n')
								{
									pStr = GetNextToolToken(pStr, buffer);
									ToolIsRefresh = buffer.GetAt(0) == _T('0') ? FALSE : TRUE;
									if (pStr == NULL)
									{
										errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_ERROR_PARSING_s, ToolMenuName);
										break;
									}
									if (*pStr != '\n')
									{
										pStr = GetNextToolToken(pStr, ToolPromptText);
										if (pStr == NULL) 
										{
											errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_ERROR_PARSING_s, ToolMenuName);
											break;
										}
										if (*pStr != '\n')
										{
											pStr = GetNextToolToken(pStr, buffer);
											ToolIsOutput2Status = buffer.GetAt(0) == _T('0') ? FALSE : TRUE;
											if ((pStr == NULL) 
												|| ((*pStr++ != '\n') && (*pStr != '\t')))
											{
												errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_ERROR_PARSING_s, ToolMenuName);
												break;
											}
										}
										else
										{
											pStr++;
											ToolIsOutput2Status = FALSE;
										}
									}
									else
									{
										pStr++;
										ToolPromptText = _T("");
									}
								}
								else
								{
									pStr++;
									ToolIsRefresh = FALSE;
								}
							}
							else
							{
								pStr++;
								ToolOnContext = ToolIsRefresh = FALSE;
							}
						}
						else
						{
							pStr++;
							ToolIsShowBrowse = ToolOnContext = ToolIsRefresh = FALSE;
						}
						GET_P4REGPTR()->SetToolMenuName(toolnbr, ToolMenuName);
						GET_P4REGPTR()->SetToolCommand(toolnbr, ToolCommand);
						GET_P4REGPTR()->SetToolArgs(toolnbr, ToolArgs);
						GET_P4REGPTR()->SetToolInitDir(toolnbr, ToolInitDir);
						GET_P4REGPTR()->SetToolPromptText(toolnbr, ToolPromptText);
						GET_P4REGPTR()->SetToolIsConsole(toolnbr, ToolIsConsole);
						GET_P4REGPTR()->SetToolIsPrompt(toolnbr, ToolIsPrompt);
						GET_P4REGPTR()->SetToolIsOutput2Status(toolnbr, ToolIsOutput2Status);
						GET_P4REGPTR()->SetToolIsCloseOnExit(toolnbr, ToolIsCloseOnExit);
						GET_P4REGPTR()->SetToolIsSubMenu(toolnbr, ToolIsSubMenu);
						GET_P4REGPTR()->SetToolIsShowBrowse(toolnbr, ToolIsShowBrowse);
						GET_P4REGPTR()->SetToolOnContext(toolnbr, ToolOnContext);
						GET_P4REGPTR()->SetToolIsRefresh(toolnbr, ToolIsRefresh);
						++nbradded;
					}
				}
				else while (*pStr && (*pStr++ != _T('\n')))
					;
			}
			LoadToolsMenu();
			if (nbradded)
			{
				CString txt;
				txt.FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_n_TOOLS_ADDED_FROM_s, 
									nbradded, *filename);
				AddToStatusLog(txt, SV_COMPLETION);
			}
		}
		else
			errorTxt->FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_READ_ERROR_s, *filename);
		::GlobalUnlock( hText );
		::GlobalFree( hText );
		CloseHandle(hImportFile);
	}
}

LPTSTR CMainFrame::GetNextToolToken(LPTSTR pStr, CString &token)
{
	LPTSTR p;
	for (p = pStr-1; (TBYTE)(*++p) >= _T(' '); )
		;
	if ((*p != _T('\t')) && (*p != _T('\r')))
		return NULL;
	*p = _T('\0');
	token = pStr;
	return p+1;
}

void CMainFrame::OnUpdateToolsExport(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!SERVER_BUSY());
}

void CMainFrame::OnToolsExport()
{
	CFileDialog fDlg(FALSE, NULL, NULL,  
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NONETWORKBUTTON,
		LoadStringResource(IDS_TOOLS_EXPORT_FILTER), this,
		m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME)); 
	
	TCHAR buf[LONGPATH+1];

	lstrcpy(buf, LoadStringResource(IDS_TOOLS_EXPORT_DEFAULT_FILE));  

    fDlg.m_ofn.lpstrFile= buf; 
	fDlg.m_ofn.nMaxFile= LONGPATH; 

	// Set the dlg caption
    CString title = LoadStringResource(IDS_TOOLS_EXPORT_TITLE);
	fDlg.m_ofn.lpstrTitle=title;
	// We dont need no stinking file title
	fDlg.m_ofn.lpstrFileTitle=NULL;
	
	// Set the initial directory
	fDlg.m_ofn.lpstrInitialDir=GET_P4REGPTR()->GetTempDir();

	// Set the user defined filter
	TCHAR customFilterBuf[512] = _T("");
	fDlg.m_ofn.lpstrCustomFilter= customFilterBuf;
	fDlg.m_ofn.nMaxCustFilter=512;

	CString errorTxt;
	INT_PTR retcode=fDlg.DoModal();

	if(retcode == IDOK)
	{
		CString filename = fDlg.GetPathName();
		HANDLE hExportFile;

		if ((hExportFile=CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 
					FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0)) == INVALID_HANDLE_VALUE)
		{
			DWORD exError=GetLastError();
			errorTxt.FormatMessage(IDS_TOOLS_EXPORT_WRITE_ERROR_s_n, filename, exError);
		}
		else
		{
			DWORD NumberOfBytesWritten;
#ifdef UNICODE
			TCHAR uhdr[] = {0xFEFF};
			WriteFile(hExportFile, uhdr, 2, &NumberOfBytesWritten, NULL);
#endif
			CString recd;
			recd.Format(_T("P4Win Tools File from %s\r\n"), GET_P4REGPTR()->GetP4User());
			WriteFile(hExportFile, recd, recd.GetLength()*sizeof(TCHAR), &NumberOfBytesWritten, NULL);
			recd.FormatMessage(IDS_IMPORTED_FROM_USER, GET_P4REGPTR()->GetP4User());
			WriteFile(hExportFile, recd, recd.GetLength()*sizeof(TCHAR), &NumberOfBytesWritten, NULL);
			for (int i = -1; ++i < MAX_TOOLS; )
			{
				CString ToolMenuName = GET_P4REGPTR()->GetToolMenuName(i);
				if (ToolMenuName.IsEmpty())
					break;
				CString ToolCommand = GET_P4REGPTR()->GetToolCommand(i);
				CString ToolArgs = GET_P4REGPTR()->GetToolArgs(i);
				CString ToolInitDir = GET_P4REGPTR()->GetToolInitDir(i);
				CString ToolPromptText = GET_P4REGPTR()->GetToolPromptText(i);
				if ((ToolMenuName.Find(_T("\t\r\n")) != -1)
				 || (ToolCommand.Find(_T("\t\r\n")) != -1)
				 || (ToolArgs.Find(_T("\t\r\n")) != -1)
				 || (ToolInitDir.Find(_T("\t\r\n")) != -1)
				 || (ToolPromptText.Find(_T("\t\r\n")) != -1))
				{
					recd.FormatMessage(IDS_TOOL_EXPORT_DEF_ERROR_s_s, ToolMenuName, ToolMenuName);
					AddToStatusLog(recd, SV_WARNING);
					continue;
				}
				BOOL ToolIsConsole = GET_P4REGPTR()->GetToolIsConsole(i);
				BOOL ToolIsPrompt = GET_P4REGPTR()->GetToolIsPrompt(i);
				BOOL ToolIsOutput2Status = GET_P4REGPTR()->GetToolIsOutput2Status(i);
				BOOL ToolIsCloseOnExit = GET_P4REGPTR()->GetToolIsCloseOnExit(i);
				BOOL ToolIsSubMenu = GET_P4REGPTR()->GetToolIsSubMenu(i);
				BOOL ToolIsShowBrowse = GET_P4REGPTR()->GetToolIsShowBrowse(i);
				BOOL ToolOnContext = GET_P4REGPTR()->GetToolOnContext(i);
				BOOL ToolIsRefresh = GET_P4REGPTR()->GetToolIsRefresh(i);
				recd.Format(_T(">>%s\t%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%d\r\n"), 
					ToolMenuName, ToolCommand, ToolArgs, ToolInitDir, ToolIsConsole,
					ToolIsPrompt, ToolIsCloseOnExit, ToolIsSubMenu, ToolIsShowBrowse, 
					ToolOnContext, ToolIsRefresh, ToolPromptText, ToolIsOutput2Status);
				WriteFile(hExportFile, recd, recd.GetLength()*sizeof(TCHAR), &NumberOfBytesWritten, NULL);
			}
			CloseHandle(hExportFile);
			recd.FormatMessage(IDS_TOOL_EXPORT_TOOLS_EXPORTED_TO_s, filename);
			AddToStatusLog(recd, SV_COMPLETION);
		}
	}
	else if(retcode == IDCANCEL)  // an error
	{
		DWORD exError=CommDlgExtendedError();
		if(exError != 0)
			errorTxt.FormatMessage(IDS_EXTENDED_ERROR_n_IN_FILEDIALOG, exError);
	}
	else
	{
		DWORD error=GetLastError();
		if(error)
		{
			LPVOID lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				error,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);
			errorTxt = (TCHAR *)lpMsgBuf;
		}
		else
			errorTxt.LoadString(IDS_UNKNOWN_FILEDIALOG_ERROR);
	}

	if(errorTxt.GetLength() > 0)
		AfxMessageBox(errorTxt, MB_ICONSTOP);
	
}

#define BUFSIZE 4096
void CMainFrame::OnTool(UINT nID)
{
	int index = nID - ID_TOOL_1;

	int i;
	int nbrSelected = 1;
	BOOL success;
	TCHAR sw;
	CString txt;
	CString command = GET_P4REGPTR()->GetToolCommand(index);
	CString args = GET_P4REGPTR()->GetToolArgs(index);
	CString initDir = GET_P4REGPTR()->GetToolInitDir(index);
	CString promptText = GET_P4REGPTR()->GetToolPromptText(index);
	BOOL isConsole = GET_P4REGPTR()->GetToolIsConsole(index);
	BOOL isPrompt = GET_P4REGPTR()->GetToolIsPrompt(index);
	BOOL isOutput2Status = GET_P4REGPTR()->GetToolIsOutput2Status(index);
	BOOL isCloseOnExit = GET_P4REGPTR()->GetToolIsCloseOnExit(index);
	BOOL isShowBrowse = GET_P4REGPTR()->GetToolIsShowBrowse(index);
	BOOL isRefresh = GET_P4REGPTR()->GetToolIsRefresh(index);

#ifdef UNICODE
	DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT; 
#else
	DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS;
#endif
	PROCESS_INFORMATION procInfo = {0};
	STARTUPINFO startInfo = {0};
	TCHAR cmdTitle[255];
	GetStartupInfo(&startInfo);
	startInfo.lpReserved= startInfo.lpDesktop= NULL; 
	startInfo.dwFlags = STARTF_USESHOWWINDOW;
	startInfo.wShowWindow = SW_SHOWNORMAL;

	if (!initDir.IsEmpty() && initDir.GetAt(0) == _T('$') && initDir.GetAt(1) == _T('r'))
	{
		if (TheApp()->m_ClientRoot.IsEmpty())
		{
			CCmd_Info cmd;
			cmd.Init( NULL, RUN_SYNC );
			if( cmd.Run( ) && !cmd.GetError() )
			{
				CP4Info const &info = cmd.GetInfo();
				if (!info.m_ClientRoot.IsEmpty( ))
				{
					TheApp()->SetClientRoot(info.m_ClientRoot);
				}
			}
		}
		initDir = ReplaceDollarArg(initDir, 0, TheApp()->m_ClientRoot);
	}

	HANDLE hChildStdoutRd, hChildStdoutWr, hChildStdoutRdDup;
	hChildStdoutWr = hChildStdoutRdDup = 0;
	if(isConsole)
	{
		TCHAR	cmd[MAX_PATH+1];

		GetEnvironmentVariable(_T("ComSpec"), cmd, MAX_PATH);
		// Trunc cmd string if longer than will ft in title
		if (command.GetLength() >= sizeof(cmdTitle)/sizeof(TCHAR))
		{
			CString temp = command.Left(sizeof(cmdTitle)/sizeof(TCHAR) - 10);
			lstrcpy(cmdTitle, temp);
		}
		else
			lstrcpy(cmdTitle, command);
		startInfo.lpTitle= cmdTitle;
		startInfo.dwXCountChars=80; 
		startInfo.dwYCountChars=1000; 
		startInfo.dwFlags=STARTF_USECOUNTCHARS; 
		startInfo.cbReserved2=0; 
		startInfo.lpReserved2=NULL; 
		dwCreationFlags |= CREATE_NEW_CONSOLE;
		if (isCloseOnExit)
			command = cmd + CString(_T(" /c ")) + command;
		else
			command = cmd + CString(_T(" /k ")) + command;
	}
	else
	{
		if (command.IsEmpty() && args.IsEmpty())
		{
			if (initDir == _T("%f"))
			{
				BOOL rc = FALSE;
				CStringList list;
				CView * pView = GetActiveView();
				if (pView == (CView *) m_pDepotView)
					rc = m_pDepotView->GetTreeCtrl().GetSelectedFiles(&list);
				else if (pView == (CView *) m_pDeltaView)
					rc = m_pDeltaView->GetTreeCtrl().GetSelectedFiles(&list);
				if (rc && !list.IsEmpty())
				{
					initDir = list.GetHead();
					if ((i = initDir.ReverseFind(_T('\\'))) != -1)
						initDir = initDir.Left(i);
				}
				else initDir.Empty();
			}
			if (initDir.Find(_T(" ")) != -1)
			{
				initDir.TrimLeft();
				initDir.TrimRight();
				initDir = _T('\"') + initDir + _T('\"');
			}
			command = TheApp()->GetExplorer() + initDir;
		}
	}
	CString commandLine = command + _T(" ");

	CString promptStr;
	if (isPrompt)
	{
		CToolsArgs dlg;
		CString menuName = GET_P4REGPTR()->GetToolMenuName(index);
		CString promptText = GET_P4REGPTR()->GetToolPromptText(index);
		menuName.Remove(_T('&'));
        CString txt;
		if (promptText.IsEmpty())
			txt.FormatMessage(IDS_TOOLS_ARGS_FOR_s, menuName);
		else
			txt.FormatMessage(IDS_TOOLS_s_COLON_s, menuName, promptText);
		dlg.SetTitle(txt);
        txt.FormatMessage(IDS_TOOLS_s_ARGUMENTS_s, command, args);
		dlg.SetStatusText(txt);
		dlg.SetShowBrowse(isShowBrowse);
		INT_PTR rc = dlg.DoModal();
		SetMessageText(_T(""));
		if (rc == IDCANCEL)
			return;
		promptStr = dlg.GetArgs();
	}
	if (!promptStr.IsEmpty())	// look for $D in command line
	{
		bool b = false;
		CString txt = commandLine + _T(" ") + args;
		for (i=0; (i = FindDollarArg(txt, i+1)) != -1; )
		{
			if (txt[i+1] == 'D')
			{
				b = true;
				break;
			}
		}
		if (!b)	// if didn't find $D, stick prompt between cmd and args
			commandLine += promptStr + _T(" ");
	}

	commandLine += args;
	for (i=0; (i = FindDollarArg(commandLine, i+1)) != -1; )
	{
		switch (sw = commandLine.GetAt(i+1))
		{
		case _T('p'):
			commandLine = ReplaceDollarArg(commandLine, i, GET_P4REGPTR()->GetP4Port());
			break;
		case _T('c'):
			commandLine = ReplaceDollarArg(commandLine, i, GET_P4REGPTR()->GetP4Client());
			break;
		case _T('u'):
			commandLine = ReplaceDollarArg(commandLine, i, GET_P4REGPTR()->GetP4User());
			break;
		case _T('P'):
			commandLine = ReplaceDollarArg(commandLine, i, GET_P4REGPTR()->GetP4UserPassword());
			break;
		case _T('D'):
			commandLine = ReplaceDollarArg(commandLine, i, promptStr);
			break;
		case _T('r'):
			{
				if (TheApp()->m_ClientRoot.IsEmpty())
				{
					CCmd_Info cmd;
					cmd.Init( NULL, RUN_SYNC );
					if( cmd.Run( ) && !cmd.GetError() )
					{
						CP4Info const &info = cmd.GetInfo();
						if (!info.m_ClientRoot.IsEmpty( ))
						{
							TheApp()->SetClientRoot(info.m_ClientRoot);
						}
					}
				}
				commandLine = ReplaceDollarArg(commandLine, i, TheApp()->m_ClientRoot);
				break;
			}
		}
	}
	i = FindPercentArg(commandLine, 0);
	if (i != -1)
	{
		if (FindPercentArg(commandLine, i+1) != -1)
		{
			CString txt;
            txt.FormatMessage(IDS_TOOLS_MORE_THAN_ONE_REPLACEABLE_ARG_NOT_ALLOWED_s, commandLine);
            
			AfxMessageBox(txt, MB_ICONSTOP);
			return;
		}
		BOOL bUseOpened = FALSE;
		switch (sw = commandLine.GetAt(i+1))
		{
		case _T('O'):
			bUseOpened = TRUE;
			goto doF;
		case _T('L'):
			if (GetActiveView() != (CView *) m_pDepotView)
				return;
			goto doF;
		case _T('R'):
			if (GetActiveView() != (CView *) m_pDeltaView)
				return;
		case _T('D'):
		case _T('F'):
doF:		if ((nbrSelected = InsertAllSelectedFilesIntoCmdline(commandLine, i, 
										bUseOpened, sw == _T('D'))) == 0)
				txt.FormatMessage( tolower(commandLine.GetAt(i+1)) == _T('o') ?
                                    IDS_TOOLS_NO_OPENED_FILES_SELECTED_CANNOT_RUN_s :
                                    IDS_TOOLS_NO_FILES_SELECTED_CANNOT_RUN_s,
							        commandLine);
			else if (nbrSelected < 1)
				txt.FormatMessage(IDS_TOOLS_UNABLE_TO_DETERMINE_PATH_CANNONT_RUN_s, commandLine);
			if (nbrSelected <= 0)
			{
				AfxMessageBox(txt, MB_ICONSTOP);
				return;
			}
			break;

		case _T('o'):
			bUseOpened = TRUE;
			goto dof;
		case _T('l'):
			if (GetActiveView() != (CView *) m_pDepotView)
				return;
			goto dof;
		case _T('P'):	// Pending Chg
			sw = (TCHAR)tolower(sw);
		case _T('p'):
		case _T('r'):
			if (GetActiveView() != (CView *) m_pDeltaView)
				return;
		case _T('d'):
		case _T('f'):
dof:		ExecOnceForEachFile(i, commandLine, sw,
								initDir.IsEmpty() ? (LPCTSTR)NULL : LPCTSTR(initDir),
								dwCreationFlags, &startInfo, &procInfo, 
								command.IsEmpty(), isRefresh, isOutput2Status);
			return;

		case _T('C'):
		case _T('J'):	// job
			sw = (TCHAR)tolower(sw);
		case _T('c'):
		case _T('j'):	// job
			if (GetActiveView() == (CView *) m_pDeltaView)
				goto dof;
		case _T('s'):	// submitted change
		case _T('a'):	// label
		case _T('b'):	// branch
		case _T('i'):	// client
		case _T('u'):	// user
		case _T('S'):	// submitted change
		case _T('A'):	// label
		case _T('B'):	// branch
		case _T('I'):	// client
		case _T('U'):	// user
			if ((nbrSelected = InsertSelectedObjectIntoCmdline(commandLine, i)) == 0)
				txt.FormatMessage(IDS_TOOLS_NO_OBJECT_SELECTED_CANNOT_RUN_s, commandLine);
			else if (nbrSelected < 1)
				txt.FormatMessage(IDS_TOOLS_UNABLE_TO_DETERMINE_OBJECT_CANNOT_RUN_s, commandLine);
			if (nbrSelected <= 0)
			{
				AfxMessageBox(txt, MB_ICONSTOP);
				return;
			}
			break;
		}
	}
	if (initDir == _T("%f"))
	{
		BOOL rc = FALSE;
		CStringList list;
		CView * pView = GetActiveView();
		if (pView == (CView *) m_pDepotView)
			rc = m_pDepotView->GetTreeCtrl().GetSelectedFiles(&list);
		else if (pView == (CView *) m_pDeltaView)
			rc = m_pDeltaView->GetTreeCtrl().GetSelectedFiles(&list);
		if (rc && !list.IsEmpty())
		{
			initDir = list.GetHead();
			if ((i = initDir.ReverseFind(_T('\\'))) != -1)
				initDir = initDir.Left(i);
		}
		else initDir.Empty();
	}
	if (command.IsEmpty())
	{
		if (nbrSelected > 1)
		{
			txt = GET_P4REGPTR()->GetToolMenuName(index);
			txt.Remove('&');
			txt.FormatMessage(IDS_TOOLS_TOOLS_s_REQUIRES_ONE_FILE_SELECTED, txt);
			AfxMessageBox(txt, MB_ICONSTOP);
			return;
		}
		commandLine.TrimLeft(_T(" \"")); 
		commandLine.TrimRight(_T(" \""));
		success=((INT_PTR)(ShellExecute( m_hWnd, _T("Open.VisualStudio.7.1"), 
						commandLine, NULL, 
						initDir.IsEmpty() ? (LPCTSTR)NULL : initDir,
						SW_SHOWNORMAL)) > 32) ? TRUE : FALSE;
		if (!success && (ERROR_NO_ASSOCIATION == GetLastError()))
		{
			success=((INT_PTR)(ShellExecute(m_hWnd, _T("&Open with MSDEV"), 
						commandLine, NULL, 
						initDir.IsEmpty() ? (LPCTSTR)NULL : initDir,
						SW_SHOWNORMAL)) > 32) ? TRUE : FALSE;
		}
		if (!success && (ERROR_NO_ASSOCIATION == GetLastError()))
		{
			success=((INT_PTR)(ShellExecute(m_hWnd, _T("open"), 
						commandLine, NULL, 
						initDir.IsEmpty() ? (LPCTSTR)NULL : initDir,
						SW_SHOWNORMAL)) > 32) ? TRUE : FALSE;
		}
		isRefresh = FALSE;	// can't wait on a ShellExecuted program
	}
	else
	{
		if (initDir.IsEmpty())
			initDir = TheApp()->m_ClientRoot;
		if (isOutput2Status)
		{
			SECURITY_ATTRIBUTES saAttr = {0};

			saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
			saAttr.bInheritHandle = TRUE; 
			saAttr.lpSecurityDescriptor = NULL;
			// Create a pipe for the child process's STDOUT. 
			if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0))
				isOutput2Status = FALSE;
			else
			{
				// Create noninheritable read handle and close the inheritable read handle. 
				BOOL fSuccess = DuplicateHandle(GetCurrentProcess(), hChildStdoutRd,
					GetCurrentProcess(), &hChildStdoutRdDup , 0, FALSE, DUPLICATE_SAME_ACCESS);
				if( !fSuccess )
				{
					CloseHandle(hChildStdoutWr);
					isOutput2Status = FALSE;
				}
				else
				{
					startInfo.wShowWindow = SW_SHOWMINIMIZED;
					startInfo.hStdError = hChildStdoutWr;
					startInfo.hStdOutput = hChildStdoutWr;
					startInfo.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
				}
				CloseHandle(hChildStdoutRd);
			}
		}
		success=CreateProcess( NULL,	// pointer to name of executable module 
					const_cast<LPTSTR>((LPCTSTR)commandLine),	// pointer to command line string
					NULL, NULL,  // default security rot
					isOutput2Status ? TRUE : FALSE,	// handle inheritance flag 
					dwCreationFlags,	// creation flags 
					P4GetEnvironmentStrings(), // env
					initDir.IsEmpty() ? (LPCTSTR)NULL : LPCTSTR(initDir), // initial dir
					&startInfo, &procInfo);
	}

	CString	oldtext;
	CString	newtext;
	if (!success)
	{
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);
        txt.FormatMessage(IDS_TOOLS_ERROR_n_CANNOT_RUN_s_s, GetLastError(), commandLine, lpMsgBuf);
		if ( GET_P4REGPTR()->ShowCommandTrace( ) )
			TheApp()->StatusAdd(txt, SV_ERROR);
		else
			AfxMessageBox(txt, MB_ICONSTOP);
	}
	else
	{
		if ( GET_P4REGPTR()->ShowCommandTrace( ) )
			TheApp()->StatusAdd(commandLine, SV_TOOL);

		if (isOutput2Status)
		{
			BOOL b = FALSE;
			DWORD dwRead;
			CHAR chBuf[BUFSIZE];
			CHAR *p = chBuf;
			CHAR xxBuf[BUFSIZE];
			CHAR lchBuf[BUFSIZE];
			TCHAR tchBuf[BUFSIZE];
			CHAR *q;
			CHAR *qbgn;

			// Close the write end of the pipe before reading from the 
			// read end of the pipe. 
			CloseHandle(hChildStdoutWr);
			 
			// Let the user know why we aren't responding
            newtext.FormatMessage(IDS_WAITING_FOR_s_TO_FINISH, commandLine);
			AfxGetMainWnd()->GetWindowText(oldtext);
			AfxGetMainWnd()->SetWindowText(newtext);

			// Read output from the child process, and write to Status pane
			for (xxBuf[0] = '\0';;) 
			{
				if (!PumpMessages( ))
					break;
				if (!ReadFile(hChildStdoutRdDup, p, BUFSIZE-4-int(p-chBuf), &dwRead, NULL) 
					|| dwRead == 0)
				{
					if (*chBuf)	// any last bits to be written?
					{
#ifdef UNICODE
						memset(tchBuf, 0, BUFSIZE);
						MultiByteToWideChar(CP_ACP, 0, (LPCSTR)chBuf, int(strlen(chBuf)), tchBuf, BUFSIZE);
						AddToStatus(tchBuf, SV_MSG);
#else
						AddToStatus(chBuf, SV_MSG);
#endif
					}
					break;
				}
				p[dwRead] = 0;
				Sleep(10);
				if (xxBuf[0])
				{
#ifdef UNICODE
					memset(tchBuf, 0, BUFSIZE);
					MultiByteToWideChar(CP_ACP, 0, (LPCSTR)xxBuf, int(strlen(xxBuf)), tchBuf, BUFSIZE);
					AddToStatus(tchBuf, SV_MSG);
#else
					AddToStatus(xxBuf, SV_MSG);
#endif
					xxBuf[0] = '\0';
				}
				if (p[0] == '\r' && p[1] == '\n' && !chBuf[2])
				{
					p += 2;
					continue;
				}
				p = chBuf;
				if (chBuf[strlen(chBuf)-1] == '>')
				{
					for (b=FALSE, q = chBuf + strlen(chBuf); --q >= chBuf; )
					{
						if (*q == '\r' && *(q+1) == '\n' && *(q+3) == ':')
						{
							strcpy(xxBuf, q);
							*q = '\0';
							if (q == chBuf)
								b = TRUE;
							break;
						}
					}
				}
				if (b)
				{
					b = FALSE;
					continue;
				}
				for (qbgn = chBuf, q = qbgn-1; *++q; )
				{
					if (*q == '\r' || *q == '\n')
					{
						strncpy(lchBuf, qbgn, q-qbgn);
						lchBuf[q-qbgn] = '\0';
#ifdef UNICODE
						memset(tchBuf, 0, BUFSIZE);
						MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lchBuf, int(strlen(lchBuf)), tchBuf, BUFSIZE);
						AddToStatus(tchBuf, SV_MSG);
#else
						AddToStatus(lchBuf, SV_MSG);
#endif
						q++;
						while (*q)
						{
							if ((*q >= ' ') || (*q == '\t'))
								break;
							q++;
						}
						qbgn = q;
						if (!*q)
							break;
					}
				}
				if (*qbgn)
				{
					if ((qbgn == chBuf)	&& (strlen(chBuf) > BUFSIZE/2))
					{	// Did we fail to write any output yet the buffer is over half full?
						// if so, we better output what we've got and clear the buffer
#ifdef UNICODE
						memset(tchBuf, 0, BUFSIZE);
						MultiByteToWideChar(CP_ACP, 0, (LPCSTR)chBuf, int(strlen(chBuf)), tchBuf, BUFSIZE);
						AddToStatus(tchBuf, SV_MSG);
#else
						AddToStatus(chBuf, SV_MSG);
#endif
						*(p = chBuf) = '\0';
					}
					else
					{
						strcpy(chBuf, qbgn);
						p = chBuf + strlen(chBuf);
					}
				}
				else
				{
					*(p = chBuf) = '\0';
				}
			}
			AfxGetMainWnd()->SetWindowText(oldtext);
			if (isRefresh)
			{
				m_pBranchView->GetListCtrl().Clear();
				m_pClientView->GetListCtrl().Clear();
				m_pUserView->GetListCtrl().Clear();
				m_pLabelView->GetListCtrl().Clear();
				m_pJobView->GetListCtrl().Clear();
				m_pDepotView->GetTreeCtrl().OnViewUpdate( TRUE );
			}
		}
		else if (isRefresh)
		{
            newtext.FormatMessage(IDS_WAITING_FOR_s_TO_FINISH, commandLine);

			// Let the user know why we aren't responding
			//
			AfxGetMainWnd()->GetWindowText(oldtext);
			AfxGetMainWnd()->SetWindowText(newtext);

			// Disable the main window
			//
			BOOL bReEnableMain = FALSE;
			if (AfxGetMainWnd()->IsWindowEnabled())
			{
				::EnableWindow( AfxGetMainWnd()->GetSafeHwnd(), FALSE );
				bReEnableMain = TRUE;
			}

			// Wait for the spawned app
			//
			WaitForSingleObject( procInfo.hProcess, INFINITE );

			// Re-enable the main window
			//
			if (bReEnableMain)
			{
				::EnableWindow( AfxGetMainWnd()->GetSafeHwnd(), TRUE );
				::SetForegroundWindow( AfxGetMainWnd()->GetSafeHwnd() );
				::SetFocus( AfxGetMainWnd()->GetSafeHwnd() );
			}
			AfxGetMainWnd()->SetWindowText(oldtext);

			m_pBranchView->GetListCtrl().Clear();
			m_pClientView->GetListCtrl().Clear();
			m_pUserView->GetListCtrl().Clear();
			m_pLabelView->GetListCtrl().Clear();
			m_pJobView->GetListCtrl().Clear();
			m_pDepotView->GetTreeCtrl().OnViewUpdate( TRUE );
		}
	}
}

int CMainFrame::FindDollarArg(CString commandLine, int offset)
{
	int n = commandLine.GetLength();
	while ((offset = commandLine.Find(_T('$'), offset)) != -1)	// look for "%$ " where x is alpha
	{
		if (offset+2 > n)
		{
			offset = -1;
			break;
		}
		if (((offset+2 == n) || ((TBYTE)commandLine.GetAt(offset+2) <= _T(' '))
							 || ((TBYTE)commandLine.GetAt(offset+2) == _T('/')) 
							 || ((TBYTE)commandLine.GetAt(offset+2) == _T('\\')))
		 && (_istalpha(commandLine.GetAt(offset+1))))
			break;
		else offset+=2;
	}
	return offset;
}

CString CMainFrame::ReplaceDollarArg(CString commandLine, int i, LPCTSTR str)
{
	CString result = i ? commandLine.Left(i) : "";
	result += str;
	result += commandLine.Right(commandLine.GetLength() - i - 2);
	return result;
}

int CMainFrame::FindPercentArg(CString commandLine, int offset)
{
	int n = commandLine.GetLength();
	while ((offset = commandLine.Find(_T('%'), offset)) != -1)	// look for "%x " where x is alpha
	{
		if (offset+2 > n)
		{
			offset = -1;
			break;
		}
		if (((offset+2 == n) 
			|| (!_istalpha(commandLine.GetAt(offset+2)) && !_istdigit(commandLine.GetAt(offset+2))))
		 && (_istalpha(commandLine.GetAt(offset+1))))
			break;
		else offset+=2;
	}
	return offset;
}

int CMainFrame::GetOpenedFiles( CStringList *list )
{
	CCmd_Opened *pCmd= new CCmd_Opened;
	int rc = 0;
    int key=0;

	// Set up and run Opened synchronously
	pCmd->Init(NULL, RUN_SYNC);
	if( pCmd->Run(FALSE, FALSE, -1, list) && !pCmd->GetError() 
		&& !SERVER_BUSY() && GET_SERVER_LOCK(key))
	{
		CObList *coblist= pCmd->GetList();
		ASSERT_KINDOF(CObList, coblist);

		list->RemoveAll();

		POSITION pos= coblist->GetHeadPosition();
		while(pos != NULL)
		{
			if ( !m_pDeltaView->GetTreeCtrl().PumpMessages( ) )
			{
				list->RemoveAll();
				break;
			}
			CP4FileStats *fs= (CP4FileStats *) coblist->GetNext(pos);
			CCmd_Fstat *pCmd2= new CCmd_Fstat;
			pCmd2->Init(NULL, RUN_SYNC, HOLD_LOCK, key);
			pCmd2->SetIncludeAddedFiles( TRUE );
			if( pCmd2->Run( FALSE, fs->GetFullDepotPath(), 0 ) && !pCmd2->GetError() )
			{
				CObList *coblist2 = pCmd2->GetFileList( );
				ASSERT_KINDOF( CObList, coblist2 );
				ASSERT( coblist2->GetCount() <= 1 );
				POSITION pos = coblist2->GetHeadPosition( );
				if( pos != NULL )
				{
					CP4FileStats *stats = ( CP4FileStats * )coblist2->GetNext( pos );
					ASSERT_KINDOF( CP4FileStats, stats );
					CString clientPath = stats->GetFullClientPath( );
					if( !clientPath.IsEmpty() )
						list->AddHead( clientPath);
					delete stats;
				}		
			}
			delete pCmd2;
			delete fs;
		}
	    RELEASE_SERVER_LOCK(key);
		rc = int(list->GetCount());
	}
	delete pCmd;
	return rc;
}

int CMainFrame::InsertSelectedObjectIntoCmdline(CString &cmdLine, int offset)
{
	CP4PaneView * pView = DYNAMIC_DOWNCAST(CP4PaneView, GetActiveView());

	CString cmdli = cmdLine;
	CString cmdliBefore = cmdli.Left(offset);
	CString cmdliAfter  = _T("");
	int i = cmdli.Find(_T(' '), offset);
	if (i != -1)
		cmdliAfter = cmdli.Right(cmdli.GetLength() - i - 1);

	CString txt = pView->GetSelectedItemText();
	
	if (txt.IsEmpty())
		return 0;

	cmdLine = cmdliBefore + txt + _T(" ") + cmdliAfter;
	return 1;
}

int CMainFrame::InsertAllSelectedFilesIntoCmdline(CString &cmdLine, int offset, 
												  BOOL bUseOpened, BOOL bDepotSyntax)
{
	CView * pView = GetActiveView();
	if ((pView != (CView *) m_pDepotView) && (pView != (CView *) m_pDeltaView))
		return(NULL);

	CString cmdli = cmdLine;
	CString cmdliBefore = cmdli.Left(offset);
	CString cmdliAfter;
	int i = cmdli.Find(_T(' '), offset);
	if (i != -1)
		cmdliAfter = cmdli.Right(cmdli.GetLength() - i - 1);
	CStringList list;
	POSITION pos;
	BOOL rc;

	if (bUseOpened)
	{
		if (pView == (CView *) m_pDepotView)
		{
			m_pDepotView->GetTreeCtrl().AssembleStringList(&list);
			rc = list.GetCount() != 0;
		}
		else
		{
			list.RemoveAll();
			rc = 1;
		}
		if (rc > 0)
			rc = GetOpenedFiles(&list);
		if (!rc)
			return(rc);
	}
	else if (pView == (CView *) m_pDepotView)
	{
		if (bDepotSyntax)
		{
			m_pDepotView->GetTreeCtrl().AssembleStringList(&list);
			rc = list.GetCount() != 0;
		}
		else rc = m_pDepotView->GetTreeCtrl().GetSelectedFiles(&list);
	}
	else
	{
		if (bDepotSyntax)
		{
			m_pDeltaView->GetTreeCtrl().AssembleStringList(&list);
			if (list.IsEmpty())
				return(-1);
			while (list.GetTail().IsEmpty())	// remove any jobs that snuck in
				list.RemoveTail();
			rc = list.GetCount() != 0;
		}
		else rc = m_pDeltaView->GetTreeCtrl().GetSelectedFiles(&list);
	}

	if (!rc)
		return(-1);

	if (list.IsEmpty())
		return 0;

	cmdli.Empty();
	for(pos=list.GetHeadPosition(); pos != NULL; )
	{
		cmdli += _T("\"") + list.GetNext(pos) + _T("\" ");
	}

	cmdLine = cmdliBefore + cmdli + cmdliAfter;
	return list.GetCount() != 0;
}

void CMainFrame::ExecOnceForEachFile(int offset, CString &cmdLine, 
									 TCHAR sw,   LPCTSTR lpInitDirectory,
									 DWORD dwCreationFlags,
									 LPSTARTUPINFO lpStartupInfo, 
									 LPPROCESS_INFORMATION lpProcessInformation,
									 BOOL bNoCommand, BOOL bRefresh, BOOL isOutput2Status)
{
	CView * pView = GetActiveView();
	if ((pView != (CView *) m_pDepotView) && (pView != (CView *) m_pDeltaView))
		return;

	CString cmdli = cmdLine;
	CString cmdliBefore = cmdli.Left(offset);
	CString cmdliAfter;
	int i = cmdli.Find(_T(' '), offset);
	if (i != -1)
		cmdliAfter = cmdli.Right(cmdli.GetLength() - i - 1);
	CStringList list;
	CString txt;
	POSITION pos;
	BOOL rc;

	if (sw == _T('o'))
	{
		if (pView == (CView *) m_pDepotView)
		{
			m_pDepotView->GetTreeCtrl().AssembleStringList(&list);
			rc = list.GetCount() != 0;
		}
		else
		{
			list.RemoveAll();
			rc = 1;
		}
		if (rc > 0)
			rc = GetOpenedFiles(&list);
	}
	else if (pView == (CView *) m_pDepotView)
	{
		if (sw == _T('d'))	// depot syntax
		{
			m_pDepotView->GetTreeCtrl().AssembleStringList(&list);
			rc = list.GetCount() != 0;
		}
		else rc = m_pDepotView->GetTreeCtrl().GetSelectedFiles(&list);
	}
	else
	{
		if (sw == _T('d'))	// depot syntax
		{
			m_pDeltaView->GetTreeCtrl().AssembleStringList(&list);
			while (list.GetTail().IsEmpty())	// remove any jobs that snuck in
				list.RemoveTail();
			rc = list.GetCount() != 0;
		}
		else if (sw == _T('j'))	// jobs
		{
			list.RemoveAll();
			for( INT_PTR i = m_pDeltaView->GetTreeCtrl().GetSelectedCount()-1; i >= 0; i--)
			{
				txt = m_pDeltaView->GetTreeCtrl().GetItemText( m_pDeltaView->GetSelectedItem( int(i) ) );
				if( txt.ReverseFind( _T('#') ) == -1)	// don't add any files that snuck in
					list.AddHead( txt );
			}
			rc = list.GetCount() != 0;
		}
		else if ((sw == 'p') || (sw == 'c'))	// pending change
		{
			CString defaultStr;
			CString changeStr;
			defaultStr.LoadString(IDS_DEFAULTCHANGELISTNAME);
			changeStr.LoadString(IDS_CHANGE);
			list.RemoveAll();
			txt = m_pDeltaView->GetItemText( m_pDeltaView->GetSelectedItem( 0 ) );
			if (txt.Find(defaultStr) == 0)
			{
				txt =_T("default");
			}
			else if (txt.Find(changeStr) == 0)
			{
				txt = txt.Right(txt.GetLength() - changeStr.GetLength());
				txt.TrimLeft();
				if ((i = txt.Find(_T(' '))) != -1)
					txt = txt.Left(i);
			}
			list.AddHead( txt );
			rc = list.GetCount() != 0;
		}
		else rc = m_pDeltaView->GetTreeCtrl().GetSelectedFiles(&list);
	}

	if (!rc)
	{
		txt.FormatMessage(IDS_TOOLS_NO_FILES_SELECTED_CANNOT_RUN_s, cmdLine);
		AfxMessageBox(txt, MB_ICONSTOP);
		return;
	}

	if (list.IsEmpty())
	{
		txt.FormatMessage(IDS_TOOLS_UNABLE_TO_DETERMINE_PATH_CANNONT_RUN_s, cmdLine);
		AfxMessageBox(txt, MB_ICONSTOP);
		return;
	}

	HANDLE hChildStdoutRd, hChildStdoutWr, hChildStdoutRdDup;
	hChildStdoutWr = hChildStdoutRdDup = 0;
	BOOL bNeed2Refresh = FALSE;
	for(pos=list.GetHeadPosition(); pos != NULL; )
	{
		CString listItem = list.GetNext(pos);
		CString initDir;
		if (lpInitDirectory && *lpInitDirectory == _T('%'))
		{
			initDir = listItem;
			if ((i = initDir.ReverseFind(_T('\\'))) != -1)
				initDir = initDir.Left(i);
		}
		else
			initDir = lpInitDirectory;
		if (bNoCommand)
		{
			cmdli = listItem;
			cmdli.TrimLeft(_T(" \"")); 
			cmdli.TrimRight(_T(" \""));
			rc=((INT_PTR)(ShellExecute(m_hWnd, _T("Open.VisualStudio.7.1"), 
						cmdli, NULL, initDir,
						SW_SHOWNORMAL)) > 32) ? TRUE : FALSE;
			if (!rc && (ERROR_NO_ASSOCIATION == GetLastError()))
			{
				rc=((INT_PTR)(ShellExecute(m_hWnd, _T("&Open with MSDEV"), 
							cmdli, NULL, initDir,
							SW_SHOWNORMAL)) > 32) ? TRUE : FALSE;
			}
			if (!rc && (ERROR_NO_ASSOCIATION == GetLastError()))
			{
				rc=((INT_PTR)(ShellExecute(m_hWnd, _T("open"), 
							cmdli, NULL, initDir,
							SW_SHOWNORMAL)) > 32) ? TRUE : FALSE;
			}
		}
		else
		{
			if ((sw == _T('j')) || (sw == _T('p')) || (sw == _T('c')))
				 cmdli = cmdliBefore + listItem + _T(" ") + cmdliAfter;
			else cmdli = cmdliBefore + _T("\"") + listItem + _T("\" ") + cmdliAfter;
			if (initDir.IsEmpty())
				initDir = TheApp()->m_ClientRoot;
			if (isOutput2Status)
			{
				SECURITY_ATTRIBUTES saAttr = {0};

				saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
				saAttr.bInheritHandle = TRUE; 
				saAttr.lpSecurityDescriptor = NULL;
				// Create a pipe for the child process's STDOUT. 
				if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0))
					isOutput2Status = FALSE;
				else
				{
					// Create noninheritable read handle and close the inheritable read handle. 
					BOOL fSuccess = DuplicateHandle(GetCurrentProcess(), hChildStdoutRd,
						GetCurrentProcess(), &hChildStdoutRdDup , 0, FALSE, DUPLICATE_SAME_ACCESS);
					if( !fSuccess )
					{
						CloseHandle(hChildStdoutWr);
						isOutput2Status = FALSE;
					}
					else
					{
						lpStartupInfo->wShowWindow = SW_SHOWMINIMIZED;
						lpStartupInfo->hStdError = hChildStdoutWr;
						lpStartupInfo->hStdOutput = hChildStdoutWr;
						lpStartupInfo->dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
					}
					CloseHandle(hChildStdoutRd);
				}
			}
			rc=CreateProcess( NULL,	// pointer to name of executable module 
						const_cast<LPTSTR>((LPCTSTR)cmdli),	// pointer to command line string
						NULL, NULL,  // default security rot
						isOutput2Status ? TRUE : FALSE,	// handle inheritance flag 
						dwCreationFlags,	// creation flags 
						P4GetEnvironmentStrings(), // env
						initDir.IsEmpty() ? (LPCTSTR)NULL : LPCTSTR(initDir), // initial dir
						lpStartupInfo, lpProcessInformation);
		}

		CString	oldtext;
		CString	newtext;
		if (!rc)
		{
			LPVOID lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);
            txt.FormatMessage(IDS_TOOLS_ERROR_n_CANNOT_RUN_s_s, GetLastError(), cmdli, lpMsgBuf);
			if ( GET_P4REGPTR()->ShowCommandTrace( ) )
				TheApp()->StatusAdd(txt, SV_ERROR);
			else
				AfxMessageBox(txt, MB_ICONSTOP);
			break;
		}
		else
		{
			if ( GET_P4REGPTR()->ShowCommandTrace( ) )
				TheApp()->StatusAdd(cmdli, SV_TOOL);

			if (isOutput2Status)
			{
				BOOL b = FALSE;
				DWORD dwRead;
				CHAR chBuf[BUFSIZE];
				CHAR *p = chBuf;
				CHAR xxBuf[BUFSIZE];	// holds "/r/nC:/dir>" strings until next read; last one will not be written
				CHAR lchBuf[BUFSIZE];
				TCHAR tchBuf[BUFSIZE];
				CHAR *q;
				CHAR *qbgn;

				// Close the write end of the pipe before reading from the 
				// read end of the pipe. 
				CloseHandle(hChildStdoutWr);
				 
				// Let the user know why we aren't responding
				newtext.FormatMessage(IDS_WAITING_FOR_s_TO_FINISH, cmdli);
				AfxGetMainWnd()->GetWindowText(oldtext);
				AfxGetMainWnd()->SetWindowText(newtext);

				// Read output from the child process, and write to Status pane
				for (xxBuf[0] = '\0';;) 
				{
					if (!PumpMessages( ))
						break;
					if (!ReadFile(hChildStdoutRdDup, p, BUFSIZE-4-int(p-chBuf), &dwRead, NULL) 
						|| dwRead == 0)
					{
						if (*chBuf)	// any last bits to be written?
						{
#ifdef UNICODE
							memset(tchBuf, 0, BUFSIZE);
							MultiByteToWideChar(CP_ACP, 0, (LPCSTR)chBuf, int(strlen(chBuf)), tchBuf, BUFSIZE);
							AddToStatus(tchBuf, SV_MSG);
#else
							AddToStatus(chBuf, SV_MSG);
#endif
						}
						break;
					}
					p[dwRead] = 0;
					Sleep(10);
					if (xxBuf[0])
					{
	#ifdef UNICODE
						memset(tchBuf, 0, BUFSIZE);
						MultiByteToWideChar(CP_ACP, 0, (LPCSTR)xxBuf, int(strlen(xxBuf)), tchBuf, BUFSIZE);
						AddToStatus(tchBuf, SV_MSG);
	#else
						AddToStatus(xxBuf, SV_MSG);
	#endif
						xxBuf[0] = '\0';
					}
					if (p[0] == '\r' && p[1] == '\n' && !chBuf[2])
					{
						p += 2;
						continue;
					}
					p = chBuf;
					if (chBuf[strlen(chBuf)-1] == '>')
					{
						for (b=FALSE, q = chBuf + strlen(chBuf); --q >= chBuf; )
						{
							if (*q == '\r' && *(q+1) == '\n' && *(q+3) == ':')
							{
								strcpy(xxBuf, q);
								*q = '\0';
								if (q == chBuf)
									b = TRUE;
								break;
							}
						}
					}
					if (b)
					{
						b = FALSE;
						continue;
					}
					for (qbgn = chBuf, q = qbgn-1; *++q; )
					{
						if (*q == '\r' || *q == '\n')
						{
							strncpy(lchBuf, qbgn, q-qbgn);
							lchBuf[q-qbgn] = '\0';
#ifdef UNICODE
							memset(tchBuf, 0, BUFSIZE);
							MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lchBuf, int(strlen(lchBuf)), tchBuf, BUFSIZE);
							AddToStatus(tchBuf, SV_MSG);
#else
							AddToStatus(lchBuf, SV_MSG);
#endif
							q++;
							while (*q)
							{
								if ((*q >= ' ') || (*q == '\t'))
									break;
								q++;
							}
							qbgn = q;
							if (!*q)
								break;
						}
					}
					if (*qbgn)
					{
						if ((qbgn == chBuf)	&& (strlen(chBuf) > BUFSIZE/2))
						{	// Did we fail to write any output yet the buffer is over half full?
							// if so, we better output what we've got and clear the buffer
#ifdef UNICODE
							memset(tchBuf, 0, BUFSIZE);
							MultiByteToWideChar(CP_ACP, 0, (LPCSTR)chBuf, int(strlen(chBuf)), tchBuf, BUFSIZE);
							AddToStatus(tchBuf, SV_MSG);
#else
							AddToStatus(chBuf, SV_MSG);
#endif
							*(p = chBuf) = '\0';
						}
						else
						{
							strcpy(chBuf, qbgn);
							p = chBuf + strlen(chBuf);
						}
					}
					else
					{
						*(p = chBuf) = '\0';
					}
				}
				AfxGetMainWnd()->SetWindowText(oldtext);
				if (bRefresh)
					bNeed2Refresh = TRUE;
			}
			else if (bRefresh)
			{
				newtext.FormatMessage(IDS_WAITING_FOR_s_TO_FINISH, cmdli);

				// Let the user know what we aren't responding
				//
				AfxGetMainWnd()->GetWindowText(oldtext);
				AfxGetMainWnd()->SetWindowText(newtext);

				// Disable the main window
				//
				BOOL bReEnableMain = FALSE;
				if (AfxGetMainWnd()->IsWindowEnabled())
				{
					::EnableWindow( AfxGetMainWnd()->GetSafeHwnd(), FALSE );
					bReEnableMain = TRUE;
				}

				// Wait for the spawned app
				//
				WaitForSingleObject( lpProcessInformation->hProcess, INFINITE );

				// Re-enable the main window
				//
				if (bReEnableMain)
				{
					::EnableWindow( AfxGetMainWnd()->GetSafeHwnd(), TRUE );
					::SetForegroundWindow( AfxGetMainWnd()->GetSafeHwnd() );
					::SetFocus( AfxGetMainWnd()->GetSafeHwnd() );
				}
				AfxGetMainWnd()->SetWindowText(oldtext);
				bNeed2Refresh = TRUE;
			}
		}
	}
	if (bNeed2Refresh)
	{
		m_pBranchView->GetListCtrl().Clear();
		m_pClientView->GetListCtrl().Clear();
		m_pUserView->GetListCtrl().Clear();
		m_pLabelView->GetListCtrl().Clear();
		m_pJobView->GetListCtrl().Clear();
		m_pDepotView->GetTreeCtrl().OnViewUpdate( TRUE );
	}
}

void CMainFrame::OnUpdateTool(CCmdUI* pCmdUI)
{
	int index = pCmdUI->m_nID - ID_TOOL_1;

	if (GET_P4REGPTR()->GetToolIsRefresh(index) && SERVER_BUSY())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	int i, n;
	int max = 0x7FFFFFFF;
	BOOL bNoCmd;
	BOOL root;
	CString command = GET_P4REGPTR()->GetToolCommand(index);
	CString args = command + _T(' ') + GET_P4REGPTR()->GetToolArgs(index);
	CString initDir = GET_P4REGPTR()->GetToolInitDir(index);

	if (initDir == _T("%f"))
	{
		BOOL canDo;
		CView * pView = GetActiveView();
		if (pView == (CView *) m_pDepotView) 
		{
			canDo = (m_pDepotView->GetTreeCtrl().GetSelectedCount() > 0
				  && m_pDepotView->GetTreeCtrl().GetSelectedCount() <= max
				  && !(m_pDepotView->GetTreeCtrl().AnyHaveChildren())
				  && m_pDepotView->GetTreeCtrl().AnyInView());
		}
		else if (pView == (CView *) m_pDeltaView)
		{
			canDo = (m_pDeltaView->GetTreeCtrl().GetSelectedCount() > 0
				  && m_pDeltaView->GetTreeCtrl().GetSelectedCount() <= max
				  && m_pDeltaView->GetTreeCtrl().GetItemLevel(m_pDeltaView->GetSelectedItem(0), &root)== 2
				  && m_pDeltaView->GetTreeCtrl().IsAFile(m_pDeltaView->GetSelectedItem(0)));
		}
		else canDo = FALSE;
		if (!canDo)
		{
			pCmdUI->Enable(FALSE);
			return;
		}
	}

	bNoCmd = command.IsEmpty() ? TRUE : FALSE;
	i = 0;
	n = args.GetLength();
	while ((i = args.Find(_T('%'), i)) != -1)	// look for "%x " where x is alpha
	{
		if (i+2 > n)
		{
			i = -1;
			break;
		}
		if (((i+2 == n) || (!_istalpha(args.GetAt(i+2)) && !_istdigit(args.GetAt(i+2))))
		 && (_istalpha(args.GetAt(i+1))))
			break;
		else i+=2;
	}
	if (i != -1)
	{
		switch (args.GetAt(i+1))
		{
		case _T('L'):
			if (bNoCmd)
				max = 1;
		case _T('l'):
			pCmdUI->Enable((GetActiveView() == (CView *) m_pDepotView) 
				&& m_pDepotView->GetTreeCtrl().GetSelectedCount() > 0
				&& m_pDepotView->GetTreeCtrl().GetSelectedCount() <= max
				&& !(m_pDepotView->GetTreeCtrl().AnyHaveChildren())
				&& m_pDepotView->GetTreeCtrl().AnyInView());
			break;

		case _T('R'):
			if (bNoCmd)
				max = 1;
		case _T('r'):
			pCmdUI->Enable((GetActiveView() == (CView *) m_pDeltaView) 
				&& m_pDeltaView->GetTreeCtrl().GetSelectedCount() > 0
				&& m_pDeltaView->GetTreeCtrl().GetSelectedCount() <= max
				&& m_pDeltaView->GetTreeCtrl().GetItemLevel(m_pDeltaView->GetSelectedItem(0), &root)== 2
				&& m_pDeltaView->GetTreeCtrl().IsAFile(m_pDeltaView->GetSelectedItem(0)));
			break;

		case _T('F'):
			if (bNoCmd)
				max = 1;
		case _T('f'):
			{
				CView * pView = GetActiveView();
				if (pView == (CView *) m_pDepotView) 
				{
					pCmdUI->Enable((GetActiveView() == (CView *) m_pDepotView) 
						&& m_pDepotView->GetTreeCtrl().GetSelectedCount() > 0
						&& m_pDepotView->GetTreeCtrl().GetSelectedCount() <= max
						&& !(m_pDepotView->GetTreeCtrl().AnyHaveChildren())
						&& m_pDepotView->GetTreeCtrl().AnyInView());
				}
				else if (pView == (CView *) m_pDeltaView)
				{
					pCmdUI->Enable((GetActiveView() == (CView *) m_pDeltaView) 
						&& m_pDeltaView->GetTreeCtrl().GetSelectedCount() > 0
						&& m_pDeltaView->GetTreeCtrl().GetSelectedCount() <= max
						&& m_pDeltaView->GetTreeCtrl().GetItemLevel(m_pDeltaView->GetSelectedItem(0), &root)== 2
						&& m_pDeltaView->GetTreeCtrl().IsAFile(m_pDeltaView->GetSelectedItem(0)));
				}
				else pCmdUI->Enable(FALSE);
				break;
			}

		case _T('D'):
		case _T('d'):
			{
				if (bNoCmd)
					max = 0;
				CView * pView = GetActiveView();
				if (pView == (CView *) m_pDepotView) 
				{
					pCmdUI->Enable((GetActiveView() == (CView *) m_pDepotView) 
						&& m_pDepotView->GetTreeCtrl().GetSelectedCount() > 0
						&& m_pDepotView->GetTreeCtrl().GetSelectedCount() <= max);
				}
				else if (pView == (CView *) m_pDeltaView)
				{
					pCmdUI->Enable((GetActiveView() == (CView *) m_pDeltaView) 
						&& m_pDeltaView->GetTreeCtrl().GetSelectedCount() > 0
						&& m_pDeltaView->GetTreeCtrl().GetSelectedCount() <= max
						&& m_pDeltaView->GetTreeCtrl().GetItemLevel(m_pDeltaView->GetSelectedItem(0), &root)== 2
						&& m_pDeltaView->GetTreeCtrl().IsAFile(m_pDeltaView->GetSelectedItem(0)));
				}
				else pCmdUI->Enable(FALSE);
				break;
			}

		case _T('O'):
		case _T('o'):
			{
				pCmdUI->Enable(!SERVER_BUSY()
							&& (m_pDeltaView->GetTreeCtrl().AnyInDefault() || m_pDeltaView->GetTreeCtrl().AnyNbredChg()));
				break;
			}

		case _T('C'):	// changelist (either pending or submitted)
		case _T('c'):
			if (GetActiveView() == (CView *) m_pOldChgView)
				goto subchg;
		case _T('P'):
		case _T('p'):	// pending change
			pCmdUI->Enable((GetActiveView() == (CView *) m_pDeltaView) 
					&& m_pDeltaView->GetTreeCtrl().GetSelectedCount() == 1
					&& m_pDeltaView->GetTreeCtrl().GetItemLevel(m_pDeltaView->GetSelectedItem(0), &root)== 1);
			break;

		case _T('S'):
		case _T('s'):	// submitted change
subchg:		pCmdUI->Enable(GetActiveView() == (CView *) m_pOldChgView
							&& m_pOldChgView->GetListCtrl().GetSelectedItem() != -1); 
			break;

		case _T('A'):
		case _T('a'):	// label
			pCmdUI->Enable(GetActiveView() == (CView *) m_pLabelView
							&& m_pLabelView->GetListCtrl().GetSelectedItem() != -1); 
			break;

		case _T('B'):
		case _T('b'):	// branch
			pCmdUI->Enable(GetActiveView() == (CView *) m_pBranchView
							&& m_pBranchView->GetListCtrl().GetSelectedItem() != -1); 
			break;

		case _T('I'):
		case _T('i'):	// client
			pCmdUI->Enable(GetActiveView() == (CView *) m_pClientView
							&& m_pClientView->GetListCtrl().GetSelectedItem() != -1); 
			break;

		case _T('U'):
		case _T('u'):	// user
			pCmdUI->Enable(GetActiveView() == (CView *) m_pUserView
							&& m_pUserView->GetListCtrl().GetSelectedItem() != -1); 
			break;

		case _T('J'):
		case _T('j'):	// job
			if (GetActiveView() == (CView *) m_pJobView)
				pCmdUI->Enable(m_pJobView->GetSelectedItem() != -1); 
			else if ((GetActiveView() == (CView *) m_pDeltaView))
			{
				pCmdUI->Enable(m_pDeltaView->GetTreeCtrl().GetSelectedCount() > 0
					&& m_pDeltaView->GetTreeCtrl().GetItemLevel(m_pDeltaView->GetSelectedItem(0), &root)== 2
					&& !(m_pDeltaView->GetTreeCtrl().IsAFile(m_pDeltaView->GetSelectedItem(0))));
			}
			else pCmdUI->Enable(FALSE);
			break;
		}
	}
}

void CMainFrame::AddToolsToContextMenu(CP4Menu *pPopMenu)
{
	CMenu *pSubMenu;
	BOOL bSM[MAX_TOOLS+1];
	CString str[MAX_TOOLS+1];
	int		i;
	int		s;

	// Get the strings
	for(i = -1; ++i < MAX_TOOLS; )
	{
		str[i] = GET_P4REGPTR()->GetToolMenuName(i);
		bSM[i] = GET_P4REGPTR()->GetToolIsSubMenu(i);
	}

	if (str[0].GetLength())
	{
		BOOL bGotSep = FALSE;

		// Load all Tool menu items into Tools menu
		for(s=i=-1; ++i < MAX_TOOLS && str[i].GetLength() > 0; )
		{
			if (GET_P4REGPTR()->GetToolOnContext(i))
			{
				if (!bSM[i])
				{
					// Add the menu item
					if (!bGotSep)
						pPopMenu->AppendMenu(MF_SEPARATOR);
					pPopMenu->AppendMenu(MF_STRING, ID_TOOL_1+i, str[i]);
					// now call the enable/disable routine
					CCmdUI cmdUI;
					cmdUI.m_pMenu = pPopMenu;
					cmdUI.m_nIndex = pPopMenu->GetMenuItemCount( )-1;
					cmdUI.m_nIndexMax = cmdUI.m_nIndex + 1;
					cmdUI.m_nID = ID_TOOL_1+i;
					OnUpdateTool(&cmdUI);
					// if the item got disabled, remove it.
					if (MF_DISABLED & pPopMenu->GetMenuState(ID_TOOL_1+i, MF_BYCOMMAND))
					{
						pPopMenu->DeleteMenu(ID_TOOL_1+i, MF_BYCOMMAND);
						if (!bGotSep)
							pPopMenu->DeleteMenu(pPopMenu->GetMenuItemCount( )-1, MF_BYPOSITION);
					}
					else
						bGotSep = TRUE;
				}
				else // it is a submenu
				{
					CString name = str[i];
					m_ContextToolsSubMenu[++s].m_hMenu = NULL;
					pSubMenu = &m_ContextToolsSubMenu[s];
					pSubMenu->CreatePopupMenu();
					BOOL b = FALSE;
					while (++i < MAX_TOOLS && !bSM[i])
					{
						if (GET_P4REGPTR()->GetToolOnContext(i))
						{
							pSubMenu->AppendMenu(MF_STRING, ID_TOOL_1+i, str[i]);
							// now call the enable/disable routine
							CCmdUI cmdUI;
							cmdUI.m_pMenu = pSubMenu;
							cmdUI.m_nIndex = pSubMenu->GetMenuItemCount( )-1;
							cmdUI.m_nIndexMax = cmdUI.m_nIndex + 1;
							cmdUI.m_nID = ID_TOOL_1+i;
							OnUpdateTool(&cmdUI);
							// if the item got disabled, remove it.
							if (MF_DISABLED & pSubMenu->GetMenuState(ID_TOOL_1+i, MF_BYCOMMAND))
								pSubMenu->DeleteMenu(ID_TOOL_1+i, MF_BYCOMMAND);
							else
								b = TRUE;
						}
					}
					if (b)
					{
						if (!bGotSep)
						{
							pPopMenu->AppendMenu(MF_SEPARATOR);
							bGotSep = TRUE;
						}
						pPopMenu->AppendMenu(MF_POPUP, (UINT_PTR) pSubMenu->GetSafeHmenu(), name);
					}
					else
						pSubMenu->DestroyMenu( );
					if (i < MAX_TOOLS && bSM[i])
						i--;
				}
			}
		}
	}
}

LRESULT CMainFrame::OnP4ListOp(WPARAM wParam, LPARAM lParam)
{
	int key;
	CCmd_ListOpStat *pCmd= (CCmd_ListOpStat *) wParam;
	
	if(!pCmd->GetError())
	{
		switch(pCmd->GetCommand())
		{
		case P4DELETE:
			if  (m_StringList2.GetCount())
			{
				key= pCmd->GetServerKey();

				CCmd_ListOpStat *pCmd2= new CCmd_ListOpStat;
				pCmd2->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key );

				if( pCmd2->Run( &m_StringList2, P4EDIT, m_SelectedChange ) )
					UpdateStatus( LoadStringResource(IDS_REQUEST_OPEN_EDIT) );
				else
				{
					if( key != 0 )
						RELEASE_SERVER_LOCK(key);
					delete pCmd2;
				}
				break;
			}
		case P4EDIT:
			key= pCmd->GetServerKey();
			UpdateDepotandChangeViews(REDRILL, key);
			break;
		default:
			ASSERT(0);
		}
	}

	pCmd->DeleteStatList();
	delete pCmd;
	return 0;
}

BOOL CMainFrame::PrintString(CString &string, CString caption) 
{
	RECT rPrint;
	BOOL rc = FALSE;
	CPrintDialog dlg(FALSE);

	// get the info from Page Setup
	dlg.m_pd.hDevNames = m_hDevNames;
	dlg.m_pd.hDevMode  = m_hDevMode;

	if (dlg.DoModal() == IDOK)
	{
		// Get a handle to the printer device context (DC).
		CDC dcPrinter;
		DOCINFO docinfo;
		HDC hdc = dlg.GetPrinterDC();
		if (hdc)
		{
			ASSERT(hdc);
  
			// create a CDC and attach it to the default printer
			dcPrinter.Attach(hdc);   
			
			// call StartDoc() to begin printing
			memset(&docinfo, 0, sizeof(docinfo));
			docinfo.cbSize = sizeof(docinfo);
			docinfo.lpszDocName = caption;
		}

		// if it fails, complain and exit gracefully
		if (!hdc || (dcPrinter.StartDoc(&docinfo) < 0))
		{
			AfxMessageBox(IDS_PRINTER_WOULDNT_INIT, MB_ICONSTOP);
		}
		else
		{
			// start a page
			if (dcPrinter.StartPage() < 0)
			{
				AfxMessageBox(IDS_PRINTER_COULDNT_START_PAGE, MB_ICONSTOP);
				dcPrinter.AbortDoc();
			}
			else
			{
				// map mode of printer DC
				int mapMode = 0;

				// Create a printer font
				CFont cFont;

				CString face= GET_P4REGPTR()->GetFontFace();
				int size= GET_P4REGPTR()->GetFontSize();
				int weight= GET_P4REGPTR()->GetFontWeight();
				BOOL isItalic= GET_P4REGPTR()->GetFontItalic();
				LOGFONT logFont;

				memset( &logFont, 0, sizeof(LOGFONT) );
				logFont.lfPitchAndFamily= FIXED_PITCH | FF_DONTCARE;
				lstrcpy(logFont.lfFaceName, face.GetBuffer(face.GetLength()));
				logFont.lfHeight= -abs(size);
				logFont.lfWeight = weight;			//Regular	
				logFont.lfItalic = (BYTE) isItalic;
				logFont.lfCharSet = DEFAULT_CHARSET;
				
				cFont.CreateFontIndirect( &logFont );

				CGdiObject* pOldFont = dcPrinter.SelectObject(&cFont);

				// calculate the width and height of the printable area of the paper
				CSize	wSize;		// Printer 'window' sizes
				CSize	vSize;		// Printer viewport sizes
				dcPrinter.SetMapMode(MM_LOENGLISH);
				wSize = dcPrinter.GetWindowExt();
				vSize = dcPrinter.GetViewportExt();
				vSize.cy = 0 - vSize.cy;

				if (m_osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
				{
					wSize.cx *= 8;
					wSize.cy *= 8;
					vSize.cx *= 8;
					vSize.cy *= 8;
				}

				dcPrinter.SetMapMode(mapMode = MM_ISOTROPIC);
				dcPrinter.SetWindowExt(wSize.cx, wSize.cy);
				dcPrinter.SetViewportExt(vSize.cx, vSize.cy);

				// subtract off margins from the paper size gotten from the WindowExt
				if (m_bMetric)	// if Metric, gotta convert to inches 1/100 of inches
				{
					rPrint.top = (long)((m_rtMargin.top - m_rtMinMargin.top)/25.4);
					rPrint.left = (long)((m_rtMargin.left - m_rtMinMargin.bottom)/25.4);
					rPrint.right = (long)(wSize.cx - (m_rtMargin.right - m_rtMinMargin.right)/25.4);
					rPrint.bottom = (long)(wSize.cy - (m_rtMargin.bottom - m_rtMinMargin.right)/25.4);
				}
				else	// gotta convert from 1/1000" to MM_LOENGLISH's 1/100"
				{
					rPrint.top = (m_rtMargin.top - m_rtMinMargin.top)/10;
					rPrint.left = (m_rtMargin.left - m_rtMinMargin.bottom)/10;
					rPrint.right = wSize.cx - (m_rtMargin.right - m_rtMinMargin.right)/10;
					rPrint.bottom = wSize.cy - (m_rtMargin.bottom - m_rtMinMargin.right)/10;
				}

				// loop to print individual pages, if necessary because of length of 'string'
				// first time, try to fit entire 'string' om a single page
				CString outBuf = string;	// build current print page here
				CString overflow;			// holds what is left to be printed
				do
				{
					RECT rect;	// workarea printable RECT

					overflow.Empty();
					outBuf.TrimRight(_T("\r\n"));
					rect.top = rPrint.top;
					rect.left = rPrint.left;
					rect.right = rPrint.right;
					rect.bottom = rPrint.bottom;
					dcPrinter.DrawText(outBuf, &rect, 
						DT_CALCRECT | DT_EXPANDTABS | DT_NOPREFIX | DT_WORDBREAK);
					if (rect.bottom > rPrint.bottom)
					{									// won't all fit on one page
						CString last;		// last paragraph extracted from overflow
						CString test;		// == outBuf + last
						rect.bottom = 0;
						overflow = outBuf;	// move all that's left to overflow
						outBuf.Empty();		// and start building paragraph by paragraph
						// break into individual pages at CR-LF (paragraph)
						while (1)
						{
							int i;
							if ((i = overflow.Find(_T('\n'))) == -1)	// if last paragraph
							{
								last = overflow;
								overflow.Empty();
							}
							else	// extract next paragraph
							{
								last = overflow.Left(i+1);
								overflow = overflow.Right(overflow.GetLength() - i - 1);
								last.TrimRight(_T("\r\n"));
							}
							test = outBuf + _T("\r\n") + last;
							rect.top = rPrint.top;
							rect.left = rPrint.left;
							rect.right = rPrint.right;
							rect.bottom = rPrint.bottom;
							dcPrinter.DrawText(test, &rect, 
								DT_CALCRECT | DT_EXPANDTABS | DT_NOPREFIX | DT_WORDBREAK);
							if (rect.bottom > rPrint.bottom)
							{							// last paragraph was too much
								if (outBuf.IsEmpty())
									outBuf = last;		// at least print first part of too big initial paragraph
								else					// put last paragraph back for next loop
									overflow = last + _T("\r\n") + overflow;
								break;
							}
							outBuf += _T("\r\n") + last;	// it'll fit; add last paragraph to output buffer
						}
						outBuf.TrimLeft(_T("\r\n"));
					}
					rect.top = rPrint.top;
					rect.left = rPrint.left;
					rect.right = rPrint.right;
					rect.bottom = rPrint.bottom;
					dcPrinter.DrawText(outBuf, &rect, 
						DT_EXPANDTABS | DT_NOPREFIX | DT_WORDBREAK);
					outBuf = overflow;		// move any leftovers to do in next loop
					if (!outBuf.IsEmpty())	// anything more to print?
					{
						dcPrinter.EndPage();			// end this page
						if (dcPrinter.StartPage() < 0)	// and start a new one
						{
							AfxMessageBox(IDS_PRINTER_COULDNT_START_PAGE, MB_ICONSTOP);
							dcPrinter.AbortDoc();
							break;
						}
						if (dcPrinter.GetMapMode() != mapMode)
						{
							dcPrinter.SetMapMode(MM_ISOTROPIC);
							dcPrinter.SetWindowExt(wSize.cx, wSize.cy);
							dcPrinter.SetViewportExt(vSize.cx, vSize.cy);
						}
					}
				} while (!outBuf.IsEmpty());

				dcPrinter.EndPage();
				dcPrinter.EndDoc();
				dcPrinter.SelectObject(pOldFont);
				rc = TRUE;
			}
		}
		// Clean up.
		if (hdc)
			CDC::FromHandle(hdc)->DeleteDC();
	}
	return rc;
}

void CMainFrame::OnPageSetup()
{
	PageSetup();
}

void CMainFrame::PageSetup()
{
	INT_PTR rc;
	CPageSetupDialog dlg;

	// use any previous page info
	if (m_hDevNames)
		dlg.m_psd.hDevNames = m_hDevNames;
	if (m_hDevMode)
	{
		dlg.m_psd.hDevMode = m_hDevMode;
		dlg.m_psd.rtMargin.top = m_rtMargin.top;
		dlg.m_psd.rtMargin.left = m_rtMargin.left;
		dlg.m_psd.rtMargin.right = m_rtMargin.right;
		dlg.m_psd.rtMargin.bottom = m_rtMargin.bottom;
	}

	if ((rc = dlg.DoModal()) == IDOK)	// get the user's preferences
	{							// save the good stuff
		AfxGetApp()->SelectPrinter(dlg.m_psd.hDevNames, dlg.m_psd.hDevMode, TRUE);
		m_hDevNames = dlg.m_psd.hDevNames;
		m_hDevMode  = dlg.m_psd.hDevMode;
		m_rtMargin.top = dlg.m_psd.rtMargin.top;
		m_rtMargin.left = dlg.m_psd.rtMargin.left;
		m_rtMargin.right = dlg.m_psd.rtMargin.right;
		m_rtMargin.bottom = dlg.m_psd.rtMargin.bottom;
		m_rtMinMargin.top = dlg.m_psd.rtMinMargin.top;
		m_rtMinMargin.left = dlg.m_psd.rtMinMargin.left;
		m_rtMinMargin.right = dlg.m_psd.rtMinMargin.right;
		m_rtMinMargin.bottom = dlg.m_psd.rtMinMargin.bottom;
		m_bMetric = (dlg.m_psd.Flags & PSD_INTHOUSANDTHSOFINCHES) ? FALSE : TRUE;
	}
	else if (rc == IDCANCEL)
	{
		DWORD errcode = CommDlgExtendedError();
		if (errcode)
		{
			CString txt;
			txt.FormatMessage(IDS_UNABLE_TO_SETUP_PRINTER_n, errcode);
			TheApp()->StatusAdd(txt, SV_ERROR);
		}
	}
}

void CMainFrame::OnUpdateMakeDefaultPcu(CCmdUI* pCmdUI) 
{
  	pCmdUI->Enable(!SERVER_BUSY());
}

void CMainFrame::OnMakeDefaultPcu() 
{
	BOOL rc = !GET_P4REGPTR()->SetP4Port(GET_P4REGPTR()->GetP4Port(), TRUE, TRUE, TRUE);
	rc = rc || !GET_P4REGPTR()->SetP4Client(GET_P4REGPTR()->GetP4Client(), TRUE, TRUE, TRUE);
	rc = rc || !GET_P4REGPTR()->SetP4User(GET_P4REGPTR()->GetP4User(), TRUE, TRUE, TRUE);
	if (rc)
		AfxMessageBox(IDS_UNABLE_TO_WRITE_P4DEFAULT_TO_THE_REGISTRY, MB_ICONEXCLAMATION);

}

void CMainFrame::ResumeAutoPoll()
{
	if (m_DoNotAutoPollCtr > 0)
		m_DoNotAutoPollCtr--;
	else if (m_DoNotAutoPollCtr < 0)
		m_DoNotAutoPollCtr = 0;
	if (!m_DoNotAutoPollCtr)
		WaitAWhileToPoll();
}

void CMainFrame::WaitAWhileToPoll()
{
	if(!GET_P4REGPTR()->GetAutoPoll())
		return;

	long time=GetTickCount();
	long period=GET_P4REGPTR()->GetAutoPollTime() * 60000;

	m_LastUpdateTime= max(m_LastUpdateTime, time + 20000 - period);
}

int CMainFrame::CreateNewChangeList(int key, CString *description/*=NULL*/, BOOL autoOK/*=FALSE*/)
{
	return m_pDeltaView->GetTreeCtrl().CreateNewChangeList(key, description, autoOK);
}

void CMainFrame::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	static BOOL bNeed2Clear = FALSE;

	SetGotUserInput( );
	if (bNeed2Clear && ((nItemID < ID_TOOL_1) || (nItemID > ID_TOOL_1 + MAX_TOOLS - 1)))
	{
		bNeed2Clear = FALSE;
		SetMessageText(_T(""));
	}
	CFrameWnd::OnMenuSelect(nItemID, nFlags, hSysMenu);
	if ((nItemID >= ID_TOOL_1) && (nItemID <= ID_TOOL_1 + MAX_TOOLS - 1))
	{
		int index = nItemID - ID_TOOL_1;
		CString txt;
		CString command = GET_P4REGPTR()->GetToolCommand(index);
		CString args = GET_P4REGPTR()->GetToolArgs(index);
		BOOL isPrompt = GET_P4REGPTR()->GetToolIsPrompt(index);
		txt = command + (isPrompt ? LoadStringResource(IDS_TOOLS_PROMPT_FOR_ARGS) : _T(" ")) + args;
		SetMessageText(txt);
		bNeed2Clear = TRUE;
	}
}

void CMainFrame::OnUpdateViewChgsByUser(CCmdUI* pCmdUI)
{
	CString txt = m_pUserView->GetListCtrl().GetSelectedItemText();
	if (txt.IsEmpty())
		txt = GET_P4REGPTR()->GetP4User();
	pCmdUI->SetText(LoadStringResource(IDS_CHGS_BY_USER) + TruncateString(txt, 50));
	pCmdUI->Enable(!SERVER_BUSY());
}

void CMainFrame::OnViewChgsByUser()
{
	CString txt = m_pUserView->GetListCtrl().GetSelectedItemText();
	if (txt.IsEmpty())
		txt = GET_P4REGPTR()->GetP4User();
	m_pOldChgView->GetListCtrl().FilterByUser(txt);
	if (m_currentTab != 6)
		OnViewSubmitted();
}

void CMainFrame::OnUpdateViewChgsByClient(CCmdUI* pCmdUI)
{
	CString txt = m_pClientView->GetListCtrl().GetSelectedItemText();
	if (txt.IsEmpty())
		txt = GET_P4REGPTR()->GetP4Client();
	pCmdUI->SetText(LoadStringResource(IDS_CHGS_BY_CLIENT) + TruncateString(txt, 50));
	pCmdUI->Enable(!SERVER_BUSY());
}

void CMainFrame::OnViewChgsByClient()
{
	CString txt = m_pClientView->GetListCtrl().GetSelectedItemText();
	if (txt.IsEmpty())
		txt = GET_P4REGPTR()->GetP4Client();
	m_pOldChgView->GetListCtrl().FilterByClient(txt);
	if (m_currentTab != 6)
		OnViewSubmitted();
}

BOOL CMainFrame::IsFileInList(CString *filename, CStringList *list)
{
	int i;
	int lgth;

	CString listItem;
	CString s = ((i = filename->Find(_T('#'))) == -1) ? *filename : filename->Left(i);

	for(POSITION pos=list->GetHeadPosition(); pos != NULL; )
	{
		listItem = list->GetNext(pos);
		if (listItem.Mid((lgth = listItem.GetLength()) - 4) == _T("/..."))
		{
			listItem = listItem.Left(lgth - 3);
			s = s.Left(lgth - 3);
		}
		if (s == listItem)
			return TRUE;
	}	

	return FALSE;
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	if (GET_P4REGPTR()->TryResetingFocus())
	{
		if (m_DlgWndList.GetCount() && nState == WA_ACTIVE)
			PostMessage(WM_ACTIVATEMODELESS, WA_ACTIVE, (LPARAM)pWndOther);
	}
}

LRESULT CMainFrame::OnActivateModeless(WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = 0;
	CWnd* pWndOther = (CWnd*)lParam;
	CDialog *pDlg;
	POSITION pos1;
	for( pos1 = m_DlgWndList.GetTailPosition(); pos1 != NULL; )
	{
		pDlg = (CDialog*) m_DlgWndList.GetPrev( pos1 );
		if (!::IsWindow(pDlg->m_hWnd))
			continue;
		if (::IsIconic(pDlg->m_hWnd))
			continue;
		if (pDlg == pWndOther)
			continue;
		hwnd = pDlg->m_hWnd;
	}
	if (hwnd)
		::SetFocus(hwnd);
	return 0;
}

void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID) 
{
	CFrameWnd::OnActivateApp(bActive, dwThreadID);
	SetGotUserInput( );
	WaitAWhileToPoll( );
	m_IsActive = bActive;
	if (bActive)
	{
		if (m_hWndLastActive)
			::SetFocus(m_hWndLastActive);
	}
	else
	{
		m_hWndLastActive = ::GetLastActivePopup(m_hWnd);
		if (m_hWndLastActive == m_hWnd)
			m_hWndLastActive = 0;
	}
}

void CMainFrame::OnParentNotify(UINT message, LPARAM lParam) 
{
	SetGotUserInput( );
	CFrameWnd::OnParentNotify(message, lParam);
}

// This funtion gets the enviroment string from the OS
// and then sets the 4 P4 environment variables to
// match the current port/client/user/password
LPVOID CMainFrame::P4GetEnvironmentStrings()
{
	SetEnvironmentVariable(_T("P4PORT"), GET_P4REGPTR()->GetP4Port());
	SetEnvironmentVariable(_T("P4CLIENT"), GET_P4REGPTR()->GetP4Client());
	SetEnvironmentVariable(_T("P4USER"), GET_P4REGPTR()->GetP4User());
	if (GET_SECURITYLEVEL() < 2)
	{
		CString p4passwd = GET_P4REGPTR()->GetP4UserPassword();
 		if (!p4passwd.IsEmpty())
			SetEnvironmentVariable(_T("P4PASSWD"), p4passwd);
	}
	return GetEnvironmentStrings();
}

// Called from Toolbar's OnDragEnter
DROPEFFECT CMainFrame::OnDragEnter(CWnd* pWnd, COleDataObject* 
                           pDataObject, DWORD dwKeyState, CPoint point )
{
	m_DragEnterTime = GetTickCount();
	int m_DragEnterTabNbr = CvtPointToTabNbr(point.x, point.y);
	if (m_DragEnterTabNbr >= 0)
	{
#ifdef _DEBUG
		CString txt;
		txt.Format(_T("CTBDropTarget::OnDragEnter - Button=%d\n"), m_DragEnterTabNbr);
		XTRACE(txt);
#endif
		return m_CurDropEffect = SERVER_BUSY() ? DROPEFFECT_NONE : DROPEFFECT_COPY;
	}
	return m_CurDropEffect = DROPEFFECT_NONE;
} 

// Called from Toolbar's OnDragLeave
void CMainFrame::OnDragLeave(CWnd* pWnd)
{
	m_DragEnterTime = 0;
	XTRACE(_T("CTBDropTarget::OnDragLeave\n"));
}
 
// Called from Toolbar's OnDragOver
DROPEFFECT CMainFrame::OnDragOver(CWnd* pWnd, COleDataObject* 
           pDataObject, DWORD dwKeyState, CPoint point)
{
	int i= CvtPointToTabNbr(point.x, point.y);
	if (i < 0)
		return m_CurDropEffect = DROPEFFECT_NONE;
	if (m_CurDropEffect == DROPEFFECT_COPY)
	{
		if ((m_DragEnterTabNbr == i)
		 && (GetTickCount() > m_DragEnterTime + 750))
		{
			if (i != m_currentTab)
			{
				m_bNoRefresh = TRUE;
				switch(i)
				{
					case 0:
						OnViewChanges();
						break;
					case 1:
						OnViewLabels();
						break;
					case 2:
						OnViewBranches();
						break;
					case 3:
						OnViewUsers();
						break;
					case 4:
						OnViewClients();
						break;
					case 5:
						OnViewJobs();
						break;
					case 6:
						OnViewSubmitted();
						break;
				}
				m_bNoRefresh = FALSE;
			}
			else
				m_DragEnterTime = GetTickCount();
		}
		else if (m_DragEnterTabNbr != i)
		{
			m_DragEnterTabNbr = i;
			m_DragEnterTime = GetTickCount();
		}
	}
	else if ((m_CurDropEffect == DROPEFFECT_NONE) && !SERVER_BUSY())
	{
		m_DragEnterTabNbr = i;
		m_DragEnterTime = GetTickCount();
		m_CurDropEffect = DROPEFFECT_COPY;
	}
	return m_CurDropEffect;
}

// Called from Toolbar's OnDrop
BOOL CMainFrame::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, 
                 DROPEFFECT dropEffect, CPoint point )
{ 
	AddToStatus( LoadStringResource(IDS_DROPPING_ON_TOOLBAR_HAS_NO_EFFECT));
    return FALSE;
}

// This routine coverts the HitTest return values
// to corresponding values for m_currentTab
int CMainFrame::CvtPointToTabNbr(int x, int y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;
	CToolBarCtrl& toolbarctrl= (CToolBarCtrl&) m_wndToolBar.GetToolBarCtrl( );
	int i = toolbarctrl.HitTest(&pt);
	if (i >= 0)
	{
		switch(i - 20)
		{
		case 0:
			return 0;
		case 1:
			return 6;
		case 2:
			return 2;
		case 3:
			return 1;
		case 4:
			return 4;
		case 5:
			return 3;
		case 6:
			return 5;
		}
	}
	return -1;
}

CString CMainFrame::GetDragFromJob()
{
	return m_pJobView->GetListCtrl().m_DragFromItemName;
}

CString CMainFrame::GetDragFromClient()
{
	return m_pClientView->GetListCtrl().m_DragFromItemName;
}

CString CMainFrame::GetDragFromUser()
{
	return m_pUserView->GetListCtrl().m_DragFromItemName;
}

LRESULT CMainFrame::OnNewClient( WPARAM wParam, LPARAM lParam )
{
	m_pClientView->GetListCtrl().OnNewClient(wParam, lParam);
	SignalAllModelessDlgs(WM_NEWCLIENT);
	return 0;
}

LRESULT CMainFrame::OnNewUser( WPARAM wParam, LPARAM lParam )
{
	m_pUserView->GetListCtrl().OnNewUser(wParam, lParam);
	SignalAllModelessDlgs(WM_NEWUSER);
	return 0;
}

LRESULT CMainFrame::OnUserPasswordDlg( WPARAM wParam, LPARAM lParam ) 
{
	return m_pUserView->GetListCtrl().OnUserPasswordDlg( int(wParam), int(lParam) );
}

void CMainFrame::GetClients(CStringArray *list)
{
	return m_pClientView->GetListCtrl().GetListItems(list);
}

void CMainFrame::GetUsers(CStringArray *list)
{
	return m_pUserView->GetListCtrl().GetListItems(list);
}

// This is only used with menu items that have icons
// It is only called when no OnUpdate function
// exists for the currently active pane
// It disables the menu item and clears the icon
void CMainFrame::OnUpdateMenuWithIcon(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SetMenuIcon(pCmdUI, FALSE));
}

void CMainFrame::OnUpdateViewUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SetMenuIcon(pCmdUI, !SERVER_BUSY()));
}

void CMainFrame::OnViewUpdate()
{
	// depot view handles refresh for itself
	// right hand views do their own refreshes on ID_VIEW_UPDATE_RIGHT
	CP4PaneView * active = DYNAMIC_DOWNCAST(CP4PaneView,GetActiveView());
	if (active == m_pDepotView		// Depot is active
	 || (!active && !m_currentTab))	// Status is active, and right pane is pending changes
		m_pDepotView->GetTreeCtrl().OnViewUpdate();
	else if (active)
		active->SendMessage(WM_COMMAND, ID_VIEW_UPDATE_RIGHT);
}

void CMainFrame::OnUpdateCancelCommand(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SetMenuIcon(pCmdUI, SERVER_BUSY()));
}

void CMainFrame::OnCancelButton()
{
	if (SERVER_BUSY() && IDYES == AfxMessageBox(IDS_CANCEL_AREYOUSURE, 
									MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2))
		OnCancelCommand();
}

void CMainFrame::OnCancelCommand()
{
	if (SERVER_BUSY())
		global_cancel = 1;
}

BOOL CMainFrame::SetMenuIcon(CCmdUI* pCmdUI, BOOL bEnable)
{
	// Check that we are running win/2k or later
	// and we want to show icons in menus
	// and that we have a menu (rather than a toolbar button)
	// and that is has a valid handle
	// and that it is not a (top level) context menu
	if (m_osVer.dwMajorVersion >= 5
	 && m_IconsInMenus
	 && pCmdUI->m_pMenu 
	 && IsMenu(pCmdUI->m_pMenu->m_hMenu) 
	 && pCmdUI->m_pParentMenu != pCmdUI->m_pMenu)
	{
		UINT id = pCmdUI->m_nID;
		if (bEnable || m_HasDisabled)
		{
			UINT look4id;
			switch(id)
			{
			case ID_FILTER_SETVIEW:
			case ID_LABELFILTER_SETVIEW:
			case ID_LABELFILTER_SETVIEWREV:
			case ID_FILTERBYOWNER:
			case ID_JOB_SETFILTER:
			case ID_FILTERCLIENTS:
			case ID_FILTERBRANCHBYOWNER:
				look4id = ID_VIEW_FILTEREDVIEW;
				break;
			case ID_FILTER_CLEARVIEW:
			case ID_LABELFILTER_CLEARVIEW:
			case ID_CLEAROWNERFILTER:
			case ID_JOB_REMOVEFILTER:
			case ID_CLEARCLIENTFILTER:
			case ID_CLEARBRANCHOWNERFILTER:
				look4id = ID_VIEW_CLEARFILTER;
				break;
			default:
				look4id = id;
				break;
			}
			for (int i=-1; ++i < m_MenuBmpCtr; )
			{
				if (m_MenuIDbm[i] == look4id)
				{
					MENUITEMINFO mii;
					memset(&mii, _T('\0'), sizeof(mii));
					mii.cbSize = sizeof(mii);
					mii.fMask  = MIIM_BITMAP;
					mii.hbmpItem = bEnable ? HBITMAP(m_MenuBitmap[i]) : HBITMAP(m_MenuBitDis[i]);
					GetMenu()->SetMenuItemInfo(id, &mii);
					if (pCmdUI->m_pParentMenu == GetMenu())
						for (int n = 3; ++n < 10; )
							GetMenu()->GetSubMenu(n)->SetMenuItemInfo(id, &mii);
					break;
				}
			}
		}
		else
		{
			// Set the menu label to what it already is!
			// This has the side effect of erasing the bitmap
			CString str;
			if (pCmdUI->m_pMenu->GetMenuString(id, str, MF_BYCOMMAND))
				pCmdUI->SetText(str);
		}
	}
	return bEnable;
}

void CMainFrame::SaveToolBarBitmap(int iconnbr, UINT id)
{
	CImageList * disabledmenu = TheApp()->GetToolBarImageList()->GetDisabledMenu();
	HICON hIcon = TheApp()->GetToolBarImageList()->ExtractIcon(iconnbr);
	HICON hIdis = disabledmenu ? disabledmenu->ExtractIcon(iconnbr) : hIcon;
	if (hIcon)
	{
		MENUITEMINFO mii;
		CWindowDC dc(this);
		CDC memdc;
		memdc.CreateCompatibleDC(&dc);
		CBrush brush, brushDis;
		brush.CreateSysColorBrush(COLOR_MENU);
		brushDis.CreateStockObject(WHITE_BRUSH);
		RECT rect = {0, 0, 20, 16};
		// 'm_MenuBmpCtr' is the index of the next empty CBitmap
		// draw the icon on that CBitmap
		m_MenuBitmap[m_MenuBmpCtr].CreateCompatibleBitmap(&dc, 20, 16);
		CBitmap *poldbm = memdc.SelectObject(&m_MenuBitmap[m_MenuBmpCtr]);
		memdc.FillRect(&rect, &brush);
		::DrawIconEx(memdc.m_hDC, 0, 0, hIcon,
			GetSystemMetrics(SM_CXMENUCHECK), // cx
			GetSystemMetrics(SM_CYMENUCHECK), // cy
			0, brush, DI_NORMAL);         // frame, brush, flags
		if (hIdis)
		{
			m_MenuBitDis[m_MenuBmpCtr].CreateBitmap(20, 16, 1, 1, NULL);
			memdc.SelectObject(&m_MenuBitDis[m_MenuBmpCtr]);
			memdc.FillRect(&rect, &brushDis);
			::DrawIconEx(memdc.m_hDC, 0, 0, hIdis,
				GetSystemMetrics(SM_CXMENUCHECK), // cx
				GetSystemMetrics(SM_CYMENUCHECK), // cy
				0, brush, DI_NORMAL);         // frame, brush, flags
			m_HasDisabled = TRUE;
		}
		memdc.SelectObject(poldbm);
		// Attach the icon to the menu item
		// we do this here even tho some - but not all -
		// items will have the bitmap reattached by SetMenuIcon()
		memset(&mii, _T('\0'), sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask  = MIIM_BITMAP;
		mii.hbmpItem = HBITMAP(m_MenuBitmap[m_MenuBmpCtr]);
		// Now save the associated menu ID in the corresponding 'm_MenuBmpCtr'
		m_MenuIDbm[m_MenuBmpCtr++] = id;
		// Do the actual attach to the main menu
		GetMenu()->SetMenuItemInfo(id, &mii);
	}
}

HWND CMainFrame::OldChgsWnd()
{
	CP4PaneView * pv = DYNAMIC_DOWNCAST(CP4PaneView, m_pOldChgView);
	if(pv)
	{
		// target is a CP4PaneView, so we actually want to
		// send messages to its content window
		return pv->GetContent()->GetWnd()->m_hWnd;
	}
	else
		return m_pOldChgView->m_hWnd;
}

HWND CMainFrame::BranchWnd()
{
	CP4PaneView * pv = DYNAMIC_DOWNCAST(CP4PaneView, m_pBranchView);
	if(pv)
	{
		// target is a CP4PaneView, so we actually want to
		// send messages to its content window
		return pv->GetContent()->GetWnd()->m_hWnd;
	}
	else
		return m_pBranchView->m_hWnd;
}

HWND CMainFrame::LabelWnd()
{
	CP4PaneView * pv = DYNAMIC_DOWNCAST(CP4PaneView, m_pLabelView);
	if(pv)
	{
		// target is a CP4PaneView, so we actually want to
		// send messages to its content window
		return pv->GetContent()->GetWnd()->m_hWnd;
	}
	else
		return m_pLabelView->m_hWnd;
}

HWND CMainFrame::ClientWnd()
{
	CP4PaneView * pv = DYNAMIC_DOWNCAST(CP4PaneView, m_pClientView);
	if(pv)
	{
		// target is a CP4PaneView, so we actually want to
		// send messages to its content window
		return pv->GetContent()->GetWnd()->m_hWnd;
	}
	else
		return m_pClientView->m_hWnd;
}

HWND CMainFrame::UserWnd()
{
	CP4PaneView * pv = DYNAMIC_DOWNCAST(CP4PaneView, m_pUserView);
	if(pv)
	{
		// target is a CP4PaneView, so we actually want to
		// send messages to its content window
		return pv->GetContent()->GetWnd()->m_hWnd;
	}
	else
		return m_pUserView->m_hWnd;
}

HWND CMainFrame::JobWnd()
{
	CP4PaneView * pv = DYNAMIC_DOWNCAST(CP4PaneView, m_pJobView);
	if(pv)
	{
		// target is a CP4PaneView, so we actually want to
		// send messages to its content window
		return pv->GetContent()->GetWnd()->m_hWnd;
	}
	else
		return m_pJobView->m_hWnd;
}

void CMainFrame::OnGettingStartedWithP4win()
{
	int i;
	CString str = TheApp()->GetHelpFilePath();
	HINSTANCE hinst=0;
	if ((i = str.ReverseFind(_T('.'))) != -1)
	{
		str = str.Left(i) + _T("-gs.pdf");
		hinst = ShellExecute( m_hWnd, _T("open"), str, NULL, NULL, SW_SHOWNORMAL);
		if( (INT_PTR) hinst > 32)
			return;  // successfull launch
		switch( (INT_PTR) hinst )
		{
		case SE_ERR_NOASSOC:
			i = IDS_SE_ERR_NOASSOC;
			break;
		case SE_ERR_FNF:
		case SE_ERR_PNF:
			if (IDYES == AfxMessageBox(IDS_GET_P4WINGS_FROM_WEB, MB_ICONQUESTION|MB_YESNO))
			{
				hinst = ShellExecute( m_hWnd, _T("open"), 
							LoadStringResource(IDS_P4DOCS_URL),
						NULL, NULL, SW_SHOWNORMAL);
				if( (INT_PTR) hinst > 32)
					return;  // successfull launch
			}
			i = IDS_SE_ERR_FNF;
			break;
		default:
			i = IDS_SE_ERR_OTHER;
			break;
		}
	}
	CString txt;
	txt.FormatMessage(i, str, hinst);
	TheApp()->StatusAdd( txt, SV_ERROR );
}

int CMainFrame::GetClientColNamesAndCount(CStringArray &cols)
{
	return m_pClientView->GetListCtrl().GetColNamesAndCount(cols);
}

int CMainFrame::GetUserColNamesAndCount(CStringArray &cols)
{
	return m_pUserView->GetListCtrl().GetColNamesAndCount(cols);
}

void
CMainFrame::SetModelessWnd(CDialog *pDlg)
{
	m_DlgWndList.AddHead((CObject *)pDlg);
}

LRESULT CMainFrame::OnP4DialogDestroy(WPARAM wParam, LPARAM lParam)
{
	CDialog *pDlg = (CDialog *)lParam;
	POSITION pos1, pos2;
	for( pos1 = m_DlgWndList.GetHeadPosition(); ( pos2 = pos1 ) != NULL; )
	{
       if( (CDialog*) m_DlgWndList.GetNext( pos1 ) == pDlg )
	   {
           m_DlgWndList.RemoveAt( pos2 );
		   break;
	   }
	}
	GetDesktopWindow()->ArrangeIconicWindows();
	delete pDlg;
	return TRUE;
}

void CMainFrame::OnUpdateCloseAllModelessDlgs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_DlgWndList.GetCount() && !SERVER_BUSY());
}

void
CMainFrame::CloseAllModelessDlgs()
{
	if (!SERVER_BUSY())
		SignalAllModelessDlgs(WM_CLOSE);
}

void
CMainFrame::SignalAllModelessDlgs(UINT msg)
{
	CDialog *pDlg;
	POSITION pos1, pos2;
	for( pos1 = m_DlgWndList.GetHeadPosition(); ( pos2 = pos1 ) != NULL; )
	{
		pDlg = (CDialog*) m_DlgWndList.GetNext( pos1 );
		if (!::IsWindow(pDlg->m_hWnd))
			continue;
		pDlg->SendMessage(msg, 0, 0);
		switch(msg)
		{
		case WM_CLOSE:
			m_DlgWndList.RemoveAt( pos2 );
			break;
		case WM_QUITTING:
			delete pDlg;
			break;
		}
	}
}

DWORD CMainFrame::GetExeVersion( TCHAR *exeName )
{
	DWORD dummy;
	DWORD infoSize = GetFileVersionInfoSize( exeName, &dummy );

	if( !infoSize )
		return 0;
	
	BYTE* buf = new BYTE[infoSize];

	if( !buf || !GetFileVersionInfo( exeName, 0, infoSize, buf ) )
	{
		delete buf;
		return 0;
	}

	VS_FIXEDFILEINFO* info;
	UINT is;

	if( !VerQueryValue( buf, TEXT("\\"), (LPVOID*)&info, &is ) || !is )
	{
		delete buf;
		return 0;
	}
	
	DWORD tmp = HIWORD( info->dwProductVersionMS ) * 10 + LOWORD( info->dwProductVersionMS );
	delete buf;
	return tmp;
}

LRESULT CMainFrame::OnP4HelperApp(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case HAM_ADDHANDLE:
		m_hWndHelperApp.AddTail((CObject *)lParam);
		break;

	case HAM_RMVHANDLE:
	  {
		POSITION pos1, pos2;
///		CObject* pa;

		for( pos1 = m_hWndHelperApp.GetHeadPosition(); ( pos2 = pos1 ) != NULL; )
		{
			if( (CObject*)m_hWndHelperApp.GetNext( pos1 ) == (CObject*)lParam )
			{
///				pa = list.GetAt( pos2 ); // Save the old pointer for
///										//deletion.
				m_hWndHelperApp.RemoveAt( pos2 );
///				delete pa; // Deletion avoids memory leak.
				break;
			}
		}
		break;
	  }
	}
	return 0;
}

#include <afxpriv.h>
void CMainFrame::SetMessageText(LPCTSTR lpszText)
{
	if (m_DoNotAutoPollCtr)
	{
		CString msg = CString(lpszText) + LoadStringResource(IDS_POLLINGOFF);
		SendMessage(WM_SETMESSAGESTRING, 0, (LPARAM)msg.GetBuffer());
	}
	else
		SendMessage(WM_SETMESSAGESTRING, 0, (LPARAM)lpszText);
}

BOOL CMainFrame::IsClientFilteredOut(CP4Client *client, CString *user, CString *curcli, CString *defcli)
{
	// if no filter, nothing is filtered out
	int filterFlags = GET_P4REGPTR()->GetClientFilteredFlags();
	if (!filterFlags)
		return FALSE;

	// default and current clients never get filtered out
	CString curclient = curcli ? *curcli : GET_P4REGPTR()->GetP4Client();
	if (!curclient.Compare(client->GetClientName()))
		return FALSE;

	CString defclient = defcli ? *defcli : GET_P4REGPTR()->GetP4Client(TRUE);
	if (!defclient.Compare(client->GetClientName()))
		return FALSE;

	if (filterFlags & 0x10 && Compare(client->GetOwner(), user ? *user : GET_P4REGPTR()->GetP4User()))
		return TRUE;
	if (filterFlags & 0x01 && Compare(client->GetOwner(), GET_P4REGPTR()->GetClientFilterOwner()))
		return TRUE;
	if (filterFlags & 0x20)
	{
		CString host = client->GetHost();
		if ((host.GetLength() != 0) && Compare(host, GET_P4REGPTR()->GetHostname()))
			return TRUE;
	}
	else if (filterFlags & 0x02)
	{
		CString host = client->GetHost();
		if ((host.GetLength() != 0) && Compare(host, GET_P4REGPTR()->GetClientFilterHost()))
			return TRUE;
	}
	if (filterFlags & 0x04)
	{
		CString fnd = GET_P4REGPTR()->GetClientFilterDesc();
		CString txt = client->GetDescription();
		fnd.MakeLower();
		txt.MakeLower();
		if (txt.Find(fnd) == -1)
    		return TRUE;
	}
	return FALSE;
}

BOOL CMainFrame::IsBranchFilteredOut(CP4Branch *branch, CString *user)
{
	int filterFlags = GET_P4REGPTR()->GetBranchFilteredFlags();
	if (!filterFlags)
		return FALSE;

	if (filterFlags & 0x10 && Compare(branch->GetOwner(), user ? *user : GET_P4REGPTR()->GetP4User()))
		return TRUE;
	if (filterFlags & 0x01 && Compare(branch->GetOwner(), GET_P4REGPTR()->GetBranchFilterOwner()))
		return TRUE;

	return FALSE;
}
