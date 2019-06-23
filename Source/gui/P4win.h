//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4win.h : main header file for the P4WIN application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#define OS_NT 1

//--------------------
// clientapi headers:
#pragma warning ( push )
#pragma warning ( disable: 4244 )
#include <clientapi.h>
#include <clientmerge.h>
#include <i18napi.h>
#include <charcvt.h>
#include <ntmangle.h>
#include <datetime.h>
#include <enviro.h>
#pragma warning ( pop )
//--------------------

#include <wchar.h>
#include "resource.h"       // main symbols
#include "P4Registry.h"
#include "P4FileStats.h"
#include "P4CommandStatus.h"
#include "StringUtil.h"
#include "Utf8String.h"
#include "P4GuiApp.h"
#include "P4ListBox.h"

#include "StatusView.h"

// RunApp() modes
enum RunAppMode
{
	RA_NOWAIT,		// RunApp()'s RUNAPPTHREADINFO * should be NULL
	RA_WAIT,		// RunApp()'s RUNAPPTHREADINFO * should be NULL
	RA_THREAD,		// RunApp()'s RUNAPPTHREADINFO * must point to a filled in RUNAPPTHREADINFO struct
	RA_THREADWAIT,	// RunApp()'s RUNAPPTHREADINFO * must point to a filled in RUNAPPTHREADINFO struct
};

// Option values for Drag & Drop to Labelview
enum _lld
{
	LDD_MENU,		// Display Menu
	LDD_ADD,		// Add selected files to label
	LDD_DELETE,		// Delete selected files from menu
	LDD_SYNC,		// Sync client to selected files in label
	LDD_FILTER,		// Filter Labeview on selected files
	LDD_FILTERREV,	// Filter Labeview on selected files and revisions
	LDD_ADD2VIEW,	// Add selected files to label view
	LDD_NOTINUSE	// Last value - must not be used!
};

// Show Depot Files
enum _sdf
{
	SDF_CLIENT,		// Client View of Depot
	SDF_DEPOT,		// Entire Depot
	SDF_LOCALP4,	// Local Perforce Files
	SDF_LOCALTREE,	// All Local Files in Client Tree
	SDF_FILESNOTINDEPOT	// Local File Not in Depot
};

// Sync types
enum _SyncType
{
	SYNC_DONT=0,	// Don't sync
	SYNC_HEAD,		// sync to #head
	SYNC_HAVE		// sync to #have
};

// File Revision ComboBox Values
enum _FileRevComboValues
{
	COMBO_CHGNBR,
	COMBO_LABEL,
	COMBO_DATE,
	COMBO_CLIENT,
};

// Client Spec Submit Options numberical values
enum _ClientSubOpts
{
	UNDEFINED_SUBOPTS,			// 0
	SUBMITUNCHANGED,			// 1
	REVERTUNCHANGED,			// 2
	LEAVEUNCHANGED,				// 3
	REOPEN_MASK,
	SUBMITUNCHANGED_REOPEN,		// 5
	REVERTUNCHANGED_REOPEN,		// 6
	LEAVEUNCHANGED_REOPEN,		// 7
};

// IPC communication structure when requesting another instance of P4Win to
// position its depot to a given path
typedef	struct	_EXPANDPATH
{
	int 	port;	// offset from buf to port string
	int 	client;	// offset from buf to client string
	int 	user;	// offset from buf to user string
	int 	path;	// offset from buf to path string
	int		flag;	// 0 -> no p4win has handled; 1-> a running p4win has handled
	TCHAR   buf[1];	// actually P4WIN_SHARED_MEMORY_SIZE - 5*sizeof(int) bytes
}	EXPANDPATH;

#define	P4WIN_SHARED_MEMORY_NAME	_T("P4Win Shared Memory")
#define	P4WIN_SHARED_MEMORY_SIZE	4096

// RunApp() struct for use with RA_THREAD mode
typedef	struct	_RUNAPPTHREADINFO	// This whole struct is passed to the worker thread - not just the pParam
{
	AFX_THREADPROC pfnThreadProc;	// Points to the controlling function for the worker thread to be called after the CreateProcess() call; must be a non-member function [filled in by caller]
	LPVOID         pParam;			// Points to caller's data to be passed to the worker thread [filled in by caller]
	HANDLE         hProcess;		// Handle of Created Process [filled in by RunApp()]
}	RUNAPPTHREADINFO;

// Define a path long enough that it doesnt break.  The standard MAX_PATH
// defined in stdlib is fine for NT paths, but a depot path could be quite
// a bit longer.  This is a magic number, since it must be used to pre-allocate
// buffers that are passed to tree control api's etc.  Just need to make sure
// it's long enough that things dont break.  Should I have picked 2047? or 2049?

#define LONGPATH 2048

#define	LISTVIEWNAMEBUFSIZE 1028

#define DIFF_APP 1
#define EDIT_APP 2
#define MERGE_APP 3
#define P4_APP 4
#define	TREE_APP 5
#define	ANNOTATE_APP 6

#define	PENDINGCHG		1
#define	SUBMITTEDCHG	2
#define	LABELVIEW		4
#define	LABELNOSEL		40
#define	CLIENTVIEW		5
#define	USERVIEW		6
#define JOBVIEW			7


//		okay, so some weird bug cropped up, god knows why.
//		check out the description for job000458 for the skinny.
//
#define QQBUG_JOB000458 _T("QQ")

#ifdef _DEBUG
// TRACE control:  
// #define TRACE_HERE above #include p4win.h if you want the module's
// TRACE's to fire.  The code below is cribbed from AFX.H.  When not
// a debug build, AfxTrace dissappears.
#ifndef TRACE_HERE
    #define XTRACE 1 ? (void)0 : ::AfxTrace
#else
	#define XTRACE TRACE
#endif	// TRACE_HERE

#else
    #define XTRACE //
#endif	// _DEBUG


// A handy macro for getting at the registry from other modules
#define GET_P4REGPTR() ((CP4winApp *) AfxGetApp())->GetRegPtr()
#define SERVER_BUSY() ((CP4winApp *) AfxGetApp())->m_CS.IsServerBusy()
#define CLEAR_SERVERINFO() ((CP4winApp *) AfxGetApp())->m_CS.Reset()
#define QUEUE_COMMAND(x) ((CP4winApp *) AfxGetApp())->m_CS.QueueCommand(x)
#define GET_SERVER_LOCK(x) ((CP4winApp *) AfxGetApp())->GetServerLock(x)
#define RELEASE_SERVER_LOCK(x) ((CP4winApp *) AfxGetApp())->ReleaseServerLock(x)
#define SET_SERVERLEVEL(x) ((CP4winApp *) AfxGetApp())->m_CS.SetServerLevel(x)
#define GET_SERVERLEVEL() ((CP4winApp *) AfxGetApp())->m_CS.GetServerLevel()
#define SET_SECURITYLEVEL(x) ((CP4winApp *) AfxGetApp())->m_CS.SetSecurityLevel(x)
#define GET_SECURITYLEVEL() ((CP4winApp *) AfxGetApp())->m_CS.GetSecurityLevel()
#define SET_CLIENTROOT(x) ((CP4winApp *) AfxGetApp())->m_CS.SetClientRoot(x)
#define GET_CLIENTROOT() ((CP4winApp *) AfxGetApp())->m_CS.GetClientRoot()
#define SET_NOCASE(x) ((CP4winApp *) AfxGetApp())->m_CS.SetServerNoCase(x)
#define IS_NOCASE() ((CP4winApp *) AfxGetApp())->m_CS.IsServerNoCase()
#define APP_ABORTING() ((CP4winApp *) AfxGetApp())->m_CS.IsAppAborting()
#define REQUEST_ABORT() ((CP4winApp *) AfxGetApp())->m_CS.RequestAbort()
#define SET_APP_HALTED(x) ((CP4winApp *) AfxGetApp())->SetAppHalted(x)
#define APP_HALTED() ((CP4winApp *) AfxGetApp())->IsAppHalted()
#define SET_BUSYCURSOR() ((CP4winApp *) AfxGetApp())->SetBusyCursor()
#define GET_IMAGELIST() ((CP4winApp *) AfxGetApp())->GetImageList()
#define GET_TBIMAGELIST() ((CP4winApp *) AfxGetApp())->GetToolBarImageList()
#define GET_PWD_ERROR() ((CP4winApp *) AfxGetApp())->m_CS.GetPWDError()
#define SET_PWD_ERROR(x) ((CP4winApp *) AfxGetApp())->m_CS.SetPWDError(x)
#define GET_NOPWD_SET() ((CP4winApp *) AfxGetApp())->m_CS.GetNoPWDSet()
#define SET_NOPWD_SET(x) ((CP4winApp *) AfxGetApp())->m_CS.SetNoPWDSet(x)
#define GET_PWDNOTALLOW() ((CP4winApp *) AfxGetApp())->m_CS.GetPWDnotAllow()
#define SET_PWDNOTALLOW(x) ((CP4winApp *) AfxGetApp())->m_CS.SetPWDnotAllow(x)
#define IS_UNICODE() ((CP4winApp *) AfxGetApp())->m_CS.IsServerUnicode()
#define SET_UNICODE(x) ((CP4winApp *) AfxGetApp())->m_CS.SetServerUnicode(x)

void AddToStatus(LPCTSTR txt, StatusView level = SV_MSG, bool showDialog = false);

// The production 99.1 server is the first server to support the new source-driven
// syntax for integrate
#define LEVEL_NEWINTEG 7


//		stop code litter
//
#define g_CRLF _T("\r\n")
#define	LOCALDEFTEMPLATE _T("@default@")


//	Structure used for calling OnIntegrate in DepotView.cpp from OldChgView.cpp
typedef struct	_INTEGCHG
{
	BOOL		useBranch;
	int			changeList;
	CStringList *filelist;
}	INTEGCHG;

class CP4WinToolBarImageList;
class CP4ViewImageList;

//	Global variable that when not zero means cancel the current operation
extern int global_cancel;

/////////////////////////////////////////////////////////////////////////////
// CP4winApp:
// See P4win.cpp for the implementation of this class
//


class CP4winApp : public CP4GuiApp
{
protected:
    bool m_bGoodArgs;
    void ParseArg(LPCTSTR pArg);

public:
	CP4winApp();
	~CP4winApp();

	 // Registered message for single-instance control
	UINT m_WM_ACTIVATE;   
	UINT m_WM_SENDCMD;   
	UINT m_WM_RPLYCMD;   

	HANDLE m_hMutex;
	HMODULE	m_hNWSRVLOC;
	CP4Registry m_RegInfo;
	CP4ViewImageList *m_viewImageList;
	CP4WinToolBarImageList *m_toolBarImageList;
	char m_version[50];
	int  m_AppHalted;
	int  m_P4Merge;		// 0==not installed, 1==installed
	DWORD m_P4MergeVer;	// p4merge's version eg 20052
	BOOL m_WarningDialog;
	BOOL m_TestFlag;
	CP4CommandStatus m_CS;
	BOOL m_bNoCRLF;
	BOOL m_HasPlusMapping;
	int m_ClientSubOpts;
	CString m_ClientRoot;
	CString m_ExpandPath;
	BOOL	m_bFindInChg;
	CString m_RevHistPath;
	int     m_RevHistCount;
	int     m_RevHistMore;
	int     m_RevHistLast;
	BOOL	m_RevHistEnableShowIntegs;
	BOOL	m_RunClientWizOnly;
	CString m_FileInfoPath;
	CString m_SubmitPath;		// Contains first file to submit
	CStringList m_SubmitPathList;	// Contain all other files to submit
	CString m_DiffPath;
	CString m_FindPath;
	CString m_ToolsImportPath;
	TCHAR m_InitialView;
	int m_IdleCounter;
	int m_IdleFlag;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CP4winApp)
	public:
	virtual BOOL InitInstance();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HH_HELP_CONTEXT);
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CP4winApp)
	afx_msg void OnNewWindow();
	afx_msg void OnUpdateNewWindow(CCmdUI* pCmdUI);
	//}}AFX_MSG


public:
	inline CP4Registry *GetRegPtr() { return &m_RegInfo; }
	inline BOOL IsAppHalted() { return m_AppHalted > 0; }
	inline LPCTSTR GetExplorer() { return GET_P4REGPTR()->GetExplorer() ? GET_P4REGPTR()->GetAltExpl() : GET_P4REGPTR()->Get2Panes() ? _T("Explorer.exe /e,") : _T("Explorer.exe "); }
	BOOL GetServerLock( int &key );
    void ReleaseServerLock( int &key );
	void SetAppHalted(BOOL state) { if ((m_AppHalted += state ? 1 : -1) < 0) m_AppHalted = 0; } 
	void StatusAdd(LPCTSTR txt, StatusView level = SV_MSG, bool showDialog = false);
	void StatusAdd(CStringArray *pArray, StatusView level = SV_MSG, bool showDialog = false);
	BOOL ApplyCommandLineArgs();
	BOOL RunApp(int app, RunAppMode mode, HWND hWnd, BOOL isUnicode,
					   RUNAPPTHREADINFO *lprati,
					   CString &errorText, 
					   LPCTSTR arg1=NULL, LPCTSTR arg2=NULL, 
					   LPCTSTR arg3=NULL, LPCTSTR arg4=NULL,
					   LPCTSTR arg5=NULL, LPCTSTR arg6=NULL, 
					   LPCTSTR arg7=NULL, LPCTSTR arg8=NULL,
					   LPCTSTR arg9=NULL, LPCTSTR arg10=NULL,
					   LPCTSTR arg11=NULL,LPCTSTR arg12=NULL,
					   LPCTSTR arg13=NULL,LPCTSTR arg14=NULL,
					   LPCTSTR arg15=NULL,LPCTSTR arg16=NULL,LPCTSTR arg17=NULL);
	BOOL RunViewerApp(LPCTSTR app, LPCTSTR fileName);
	LPCTSTR GetClientSpecField( LPCTSTR fieldname, LPCTSTR spectext );
	BOOL CallP4RevisionTree(CString filepath);
	BOOL CallP4A(CString annpath, CString logpath, int revnbr=-1);


	CImageList *GetImageList();
	CP4ViewImageList *GetViewImageList() { return m_viewImageList; }
	CP4WinToolBarImageList *GetToolBarImageList() { return m_toolBarImageList; }
	int GetFileImageIndex(CP4FileStats *fs, BOOL IsChangesWindow=FALSE);
	void GetFileType(const CString &itemStr,int &BaseType,int &StoreType,BOOL &TypeK,BOOL &TypeW,BOOL &TypeX,BOOL &TypeO,BOOL &TypeM,BOOL &TypeL,BOOL &TypeS,DWORD_PTR &NbrRevs,BOOL &Unknown);
	BOOL SetClientRoot(LPCTSTR clientroot);
	BOOL SetClientSubOpts(LPCTSTR clientSubOpts);
	void OnSysColorChange();
	CString BrowseForFolder(HWND hWnd, LPCTSTR startat, LPCTSTR lpszTitle, UINT nFlags);
	BOOL digestIsSame(CP4FileStats *fs, BOOL retIfNotExist=FALSE, void *client=NULL);
	BOOL localDigest(CP4FileStats *fs, CString *digest, BOOL retIfNotExist=FALSE, void *clientPtr=NULL);
	DECLARE_MESSAGE_MAP()
};

// Text formatting functions, so multiline edit boxes can translate text with embedded
// tabs, as generated by command line clients
CString RemoveTabs(LPCTSTR text);
CString MakeCRs(LPCTSTR text);
CString MakeLFs(LPCTSTR text);
CString UnMakeCRs(LPCTSTR text);
CString PadCRs(LPCTSTR text);
CString WrapDesc(LPCTSTR text, int maxcol);

// Utility functions to perform string comparisons according
// to the case-sensitivity of the server
int nCommon(LPCTSTR str1, LPCTSTR str2);
int nCompare(LPCTSTR str1, LPCTSTR str2, int n);
int Compare(LPCTSTR str1, LPCTSTR str2);

// MBCS Utility functions to replace CString functions that don't handle MBCS
void TrimRightMBCS(CString &str, TCHAR *chars);
void ReplaceMBCS(CString &str, TCHAR oldchar, TCHAR newchar);
int  FindMBCS(CString &str, TCHAR findchar, int skip=0);
int  ReverseFindMBCS(CString &str, TCHAR findchar);

// Function to get a file's extension
CString GetFilesExtension(LPCTSTR filename);

// Function to compare files in TreeView order
int fCompare(LPCTSTR str1, LPCTSTR str2, BOOL ext1st=FALSE);

// Function to count the number of NewLines in a CString
int GetNbrNL(const CString *str);

// Callback function to set the initial folder for folder-browsing
INT CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData);

// Utility clipboard function 
void CopyTextToClipboard(LPCTSTR txt);

// convert short file name to long
CString DemanglePath(LPCTSTR path);

// format a p4 api error
CString FormatError(Error *e, int flags = EF_NEWLINE);

// Find substring in string, not case sensitive
int FindNoCase(CString str, CString substr, int offset=0);

inline CString CharToCString(const char *c)
{
#ifdef _UNICODE
	return CString(UCS2String(IS_UNICODE() != FALSE, c));
#else
	return CString(c);
#endif
}

inline CharString CharFromCString(CString const &s)
{
#ifdef _UNICODE
	if(IS_UNICODE())
		return UTF8String(s);
	else
		return AnsiString(s);
#else
	return s;
#endif
}

/////////////////////////////////////////////////////////////////////////////
inline CP4winApp * TheApp()
{
    CP4winApp *app = dynamic_cast<CP4winApp *>(AfxGetApp());
    ASSERT(app);
    return app;
}
