//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4win.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "P4win.h"
#include "hlp\p4win.hh"

#include "MainFrm.h"
#include "Document.h"
#include "DepotView.h"
#include "resource.h"
#include "TokenString.h"
#include "NewWindowDlg.h"
#include "RegKeyEx.h"
#include "cmd_info.h"
#include "Cmd_Login.h"
#include "ImageList.h"
#include <mapi.h>
#include <winver.h>

#include "GuiClientUser.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int global_cancel = 0;
CString startingfolder;

/////////////////////////////////////////////////////////////////////////////
// CP4winApp

BEGIN_MESSAGE_MAP(CP4winApp, CP4GuiApp)
	//{{AFX_MSG_MAP(CP4winApp)
	ON_COMMAND(ID_NEW_WINDOW, OnNewWindow)
	ON_UPDATE_COMMAND_UI(ID_NEW_WINDOW, OnUpdateNewWindow)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CP4winApp construction

CP4winApp::CP4winApp()
{
	EnableHtmlHelp();
	m_IdleCounter = 1;
	m_InitialView = _T('\0');
	m_WarningDialog= TRUE;
	m_TestFlag= FALSE;
	m_hNWSRVLOC = LoadLibrary(_T("NWSRVLOC.dll"));
	m_RevHistCount = 0;
	m_RevHistEnableShowIntegs = TRUE;
	m_viewImageList = 0;
	m_toolBarImageList = 0;
	m_RevHistLast = 0;
	m_bFindInChg = FALSE;

	DWORD dummy;
	TCHAR cmdLine[] = _T("P4Merge.exe");
	m_P4Merge = GetFileVersionInfoSize( cmdLine, &dummy ) > 0 ? 1 : 0;
	if (m_P4Merge)
		m_P4MergeVer = MainFrame()->GetExeVersion(cmdLine);
}

CP4winApp::~CP4winApp()
{
	if(m_viewImageList)
	{
		m_viewImageList->Detach();
		delete m_viewImageList;
	}
	if(m_toolBarImageList)
	{
		m_toolBarImageList->Detach();
		delete m_toolBarImageList;
	}
	if(m_hMutex != NULL)
		CloseHandle(m_hMutex);
	if (m_hNWSRVLOC)
		FreeLibrary(m_hNWSRVLOC);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CP4winApp object

CP4winApp theApp;

/////////////////////////////////////////////////////////////////////////////
// command line parsing

static enum Arg { none, port, user, client, password, view, charset, 
				  selection, fileinfo, revhist, revhistcnt, submit, 
				  diff, find, icons, cr8cli, toolsimp } nextArg = none;

void CP4winApp::ParseArg(LPCTSTR pArg)
{
    // usage: [switches]
    // switches:
    //      -q          : disable warnings
    //      -p port     : set p4port
    //      -u user     : set p4user
    //      -c client   : set p4client
    //      -P password : set p4password
    //      -v view     : set initial view
	//		-C charset	: set initial character set
	//		-s selection: set initial selection
	//		-I fileinfo : set requested file properties path
	//		-H revhist  : set requested revision history path
	//		-m revhistcnt: set requested revision history count
	//		-S submit	: submit changelist containing filepath
	//		-D diff		: diff filepath against depot
	//		-F find		: find filepath in depot pane
	//		-i colors	: set requested icons
	//		-( cr8cli	: run new client wizard (if client doesn't exist)
	//		-T toolsimp	: import custom tools from a file

    if(nextArg == none)
    {
        if(lstrlen(pArg) == 2)
        {
            switch(pArg[1])
            {
			case _T('Q'): m_TestFlag = TRUE;
            case _T('q'): m_WarningDialog = FALSE; return;
            case _T('p'): nextArg = port; return;
            case _T('u'): nextArg = user; return;
            case _T('c'): nextArg = client; return;
            case _T('P'): nextArg = password; return;
            case _T('v'): nextArg = view; return;
			case _T('C'): nextArg = charset; return;
			case _T('s'): nextArg = selection; return;
			case _T('I'): nextArg = fileinfo; return;
			case _T('H'): nextArg = revhist; return;
			case _T('m'): nextArg = revhistcnt; return;
			case _T('S'): nextArg = submit; return;
			case _T('D'): nextArg = diff; return;
			case _T('F'): nextArg = find; return;
			case _T('i'): nextArg = icons; return;
			case _T('('): nextArg = cr8cli; return;
			case _T('T'): nextArg = toolsimp; return;
            }
        }
    }
    else
    {
        Arg thisArg = nextArg;
        nextArg = none;
        switch(thisArg)
        {
        case port:
            if(lstrlen(pArg))
            {
                GET_P4REGPTR()->SetP4Port(pArg, TRUE, FALSE, FALSE);
                return;
            }
        case user:
            if(lstrlen(pArg))
            {
                GET_P4REGPTR()->SetP4User(pArg, TRUE, FALSE, FALSE);
                return;
            }
		case cr8cli:
			m_WarningDialog = FALSE;
			m_RunClientWizOnly = TRUE;;
        case client:
            if(lstrlen(pArg))
            {
                GET_P4REGPTR()->SetP4Client(pArg, TRUE, FALSE, FALSE);
                return;
            }
        case password:
            if(lstrlen(pArg))
            {
                GET_P4REGPTR()->SetP4Password(pArg, TRUE, FALSE, FALSE);
				CCmd_Login *pCmd = new CCmd_Login;				// run p4 login
				CString portStr = GET_P4REGPTR()->GetP4Port();	// get current port
				pCmd->GetClient()->SetPort(portStr);			// run login against cur port
				CString userStr = GET_P4REGPTR()->GetP4User();	// get current user
				pCmd->GetClient()->SetUser(userStr);			// run login against cur user
				pCmd->Init(NULL, RUN_SYNC, LOSE_LOCK, 0);
				pCmd->Run(pArg);								// run the login command
				delete pCmd;
                return;
            }
        case view:
            if(lstrlen(pArg))
            {
				m_InitialView = ::toupper( pArg[0] );
                return;
            }
        case charset:
            if(lstrlen(pArg))
            {
                GET_P4REGPTR()->SetP4Charset(pArg, TRUE, FALSE, FALSE);
				GET_P4REGPTR()->SetP4CharsetFromCmdli(TRUE);	// must follow SetP4Charset()!
                return;
            }
        case selection:
            if(lstrlen(pArg))
            {
				m_WarningDialog = FALSE;
                m_ExpandPath = DemanglePath(pArg);
				if (m_ExpandPath.GetAt(0) != _T('/')
				 && m_ExpandPath.GetAt(1) != _T(':')
				 && m_ExpandPath.GetAt(0) != _T('\\'))
				{
					TCHAR buf[MAX_PATH+1];
					if (GetCurrentDirectory(sizeof(buf)-1, buf))
					{
						CString str = CString(buf) + _T('\\') + m_ExpandPath;
						m_ExpandPath = str;
					}
				}
				m_bFindInChg = TRUE;
                return;
            }
		case fileinfo:
            if(lstrlen(pArg))
            {
				m_WarningDialog = FALSE;
                m_FileInfoPath = DemanglePath(pArg);
                return;
            }
		case revhist:
            if(lstrlen(pArg))
            {
				m_WarningDialog = FALSE;
                m_RevHistPath = DemanglePath(pArg);
                return;
            }
		case revhistcnt:
            if(lstrlen(pArg))
            {
				m_RevHistCount = _tstoi(pArg);
                return;
            }
		case submit:
            if(lstrlen(pArg))
            {
				m_WarningDialog = FALSE;
				if (m_SubmitPath.IsEmpty())
					m_SubmitPath = DemanglePath(pArg);
				else
					m_SubmitPathList.AddTail(DemanglePath(pArg));
				nextArg = submit;
                return;
            }
		case diff:
            if(lstrlen(pArg))
            {
				m_WarningDialog = FALSE;
                m_DiffPath = DemanglePath(pArg);
                return;
            }
		case toolsimp:
            if(lstrlen(pArg))
            {
				m_WarningDialog = FALSE;
                m_ToolsImportPath = DemanglePath(pArg);
                return;
            }
		case icons:
			GET_P4REGPTR()->SetUse256colorIcons((_tstoi(pArg) == 256) ? TRUE : FALSE);
			return;
        }
    }
	if (*(pArg+1) == _T(':'))
	{
		nextArg = revhist;
		ParseArg(pArg);
		return;
	}
    ASSERT(0);
    m_bGoodArgs = false;
}

/////////////////////////////////////////////////////////////////////////////
// CP4winApp initialization

BOOL CP4winApp::InitInstance()
{
	if(!CP4GuiApp::InitInstance())
        return FALSE;

	LoadStdProfileSettings(0);  // Do NOT track MRU files

	// Get Perforce connect info from registry
	m_RegInfo.ReadRegistry();

    m_bGoodArgs = true;
    ParseCommandLineArgs();
    if(!m_bGoodArgs)
    {
        MessageBox(NULL, LoadStringResource(IDS_INVALID_COMMAND_LINE_ARGS__USAGE), 
			             LoadStringResource(IDS_P4WINUSAGE), MB_ICONEXCLAMATION);
        return FALSE;
    }

	///////////////////
	// Test for previous instance, by attempting to open a named mutex
	// If an instance is already running, see if user wants to activate it

	m_WM_ACTIVATE= RegisterWindowMessage(_T("Activate Previous P4Win"));
	m_WM_SENDCMD = RegisterWindowMessage(_T("P4Win Send Command"));
	m_WM_RPLYCMD = RegisterWindowMessage(_T("P4Win Reply Command"));
	m_hMutex= OpenMutex(SYNCHRONIZE, FALSE, _T("Running P4Win Instance"));
	if(m_hMutex==NULL)
	{
		// This is the first instance
		m_hMutex= CreateMutex(NULL, FALSE, _T("Running P4Win Instance"));
	}
	else if (!m_ExpandPath.IsEmpty())
	{
		BOOL b = FALSE;
		HANDLE hMapFile = CreateFileMapping(
							INVALID_HANDLE_VALUE,		// use paging file
							NULL,						// default security 
							PAGE_READWRITE,				// read/write access
							0,							// max. object size 
							P4WIN_SHARED_MEMORY_SIZE,	// buffer size  
							P4WIN_SHARED_MEMORY_NAME);	// name of mapping object
		 
		if (hMapFile != NULL && hMapFile != INVALID_HANDLE_VALUE) 
		{ 
			EXPANDPATH *ep = (EXPANDPATH *)MapViewOfFile(hMapFile,   // handle to mapping object
											FILE_MAP_ALL_ACCESS, 0, 0, P4WIN_SHARED_MEMORY_SIZE);           
			if (ep != NULL) 
			{ 
				TCHAR *p = ep->buf;
				lstrcpy(p, GET_P4REGPTR()->GetP4Port());
				ep->port = static_cast<int>(p - ep->buf);
				p += lstrlen(p) + 1;
				lstrcpy(p, GET_P4REGPTR()->GetP4Client());
				ep->client = static_cast<int>(p - ep->buf);
				p += lstrlen(p) + 1;
				lstrcpy(p, GET_P4REGPTR()->GetP4User());
				ep->user = static_cast<int>(p - ep->buf);
				p += lstrlen(p) + 1;
				lstrcpy(p, m_ExpandPath);
				ep->path = static_cast<int>(p - ep->buf);
				::SendMessage( HWND_BROADCAST, m_WM_SENDCMD, (WPARAM)1, (LPARAM)0 );
				Sleep(500);
				if (ep->flag)
					b = TRUE;
				UnmapViewOfFile(ep);
			}
			CloseHandle(hMapFile);
			if (b)
				return FALSE;
		}
	}
	else if( m_WarningDialog )
	{
		//		See if user wants to activate previous instance
		//
		int ret = AfxMessageBox(IDS_P4WIN_IS_ALREADY_RUNNING__OPEN_THE_OLD_WINDOW,
				        MB_YESNOCANCEL  | MB_ICONEXCLAMATION ) ;

		//		only when user picks NO do we run the app
		//
		switch( ret )
		{
		case IDYES: 
			::SendMessage( HWND_BROADCAST, m_WM_ACTIVATE, 0, 0 );
		case IDCANCEL: 
			return FALSE;
		}
	}

	// Load up the shared image lists
	m_viewImageList = new CP4ViewImageList();
	m_toolBarImageList = new CP4WinToolBarImageList();
	if(GET_P4REGPTR()->Use256colorIcons())
	{
		m_viewImageList->Use256ColorIcons();
		m_toolBarImageList->Use256ColorIcons();
	}
	m_viewImageList->Create();
	m_toolBarImageList->Create();
	
	// Load up the busy cursor, after reading registry cwinapp
	m_hBusyCursor= GET_P4REGPTR()->GetP4BusyCursor() ? ::LoadCursorFromFile(_T("P4_Busy.ani")) : NULL;
	if(m_hBusyCursor==NULL)
		m_hBusyCursor= LoadStandardCursor(IDC_WAIT);

	// Write our version number to the registry so P4wei knows what version we are.
	LPCTSTR sKey = _T("Software\\Perforce\\P4Win\\");
	LPCTSTR sVersion = _T("Version");
    CRegKeyEx key;
    if(ERROR_SUCCESS == key.Create(HKEY_CURRENT_USER, sKey, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE))
        key.SetValueString(m_appVersion, sVersion);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CP4winDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CDepotView));
	AddDocTemplate(pDocTemplate);

	int i;
	CString ver = ((CP4GuiApp*)AfxGetApp())->GetAppVersionString();
	CString info = ver.Mid(3,3) + _T(" ");
	if ((i = ver.ReverseFind(_T('.'))) != -1)
		ver.Delete(i);
#ifdef UNICODE
    info += _T("U");
#endif
	CString info_ver = info + ver;
	info_ver.Replace(' ', '.');
	CharString cs = CharFromCString(info_ver);
	strcpy(m_version, cs);

	OnFileNew();

	// Make sure our name is P4Win, not P4win or p4win
	if (m_pszAppName && *m_pszAppName && *(m_pszAppName+1) == '4')
	{
		TCHAR *p = (TCHAR *)m_pszAppName;
		*p = _totupper(*m_pszAppName);
		*(p+2) = _totupper(*(m_pszAppName+2));
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// Set a new busy state for Perforce server

BOOL CP4winApp::GetServerLock(int &key) 
{ 
	// Actually set busy
	BOOL gotLock= m_CS.GetServerLock( key); 

	// Get and set cursor position - Windows will then generate the necessary
	// WM_SETCURSOR messages to get the right OnSetCursor() function called

    if( gotLock )
    {
	    POINT pt;
	    if(::GetCursorPos(&pt))
		    ::SetCursorPos(pt.x, pt.y);
    }
    return gotLock;
}

void CP4winApp::ReleaseServerLock(int &key) 
{ 
	m_CS.ReleaseServerLock( key ); 

	// Get and set cursor position - Windows will then generate the necessary
	// WM_SETCURSOR messages to get the right OnSetCursor() function called

    POINT pt;
	if(::GetCursorPos(&pt))
	    ::SetCursorPos(pt.x, pt.y);
}


////////////////////////////////////////////////////////////////////////////
// Access functions for shared image list.  

int CP4winApp::GetFileImageIndex(CP4FileStats *fs, BOOL IsChangesWindow)
{
	// Start at first file
	int state;

	// What type of file is it?
	CString fileType = fs->GetHeadType();
	int iType = ((fileType.Find(_T("text")) != -1) || (fileType.Find(_T("symlink")) != -1)) ? 0 : 1;

	// get base image:
	if(!IsChangesWindow && fs->GetHaveRev() > fs->GetHeadRev() &&
			fs->GetMyOpenAction() == 0 )
	{
		// A ghost file.  We Have the file, but its outside the
		// client view.
		state = CP4ViewImageList::FSB_GHOST;
	}
	else if(iType)
	{
		// normal binary file
		state = CP4ViewImageList::FSB_BINARY;
	}
	else
	{
		// normal text file
		state = CP4ViewImageList::FSB_TEXT;
	}

	// lock state overlay
	if(fs->IsMyLock() || fs->IsMyOpenExclusive())
		state |= CP4ViewImageList::FSB_YOUR_LOCK;
	else if(fs->IsOtherLock() || fs->IsOtherOpenExclusive())
		state |= CP4ViewImageList::FSB_THEIR_LOCK;

	// my action overlay
	switch(fs->GetMyOpenAction())
	{
	case F_ADD:
	case F_BRANCH:
	case F_IMPORT:
		state |= CP4ViewImageList::FSB_YOUR_ADD;
		break;
	case F_EDIT:
	case F_INTEGRATE:
		state |= CP4ViewImageList::FSB_YOUR_EDIT;
		break;
	case F_DELETE:
		state |= CP4ViewImageList::FSB_YOUR_DELETE;
		break;
	}

	// other action overlay
	switch(fs->GetOtherOpenAction())
	{
	case F_ADD:
	case F_BRANCH:
	case F_IMPORT:
		state |= CP4ViewImageList::FSB_THEIR_ADD;
		break;
	case F_EDIT:
	case F_INTEGRATE:
		state |= CP4ViewImageList::FSB_THEIR_EDIT;
		break;
	case F_DELETE:
		state |= CP4ViewImageList::FSB_THEIR_DELETE;
		break;
	}

	// sync state overlay, depends on which view we're in
	if(IsChangesWindow)
	{
		if(fs->IsUnresolved())
			state |= CP4ViewImageList::FSB_NOT_SYNCED;
	}
	else
	{
		if(fs->GetHaveRev())
		{
			if(fs->GetHaveRev() >= fs->GetHeadRev() || 
				fs->GetMyOpenAction() == F_ADD)
				state |= CP4ViewImageList::FSB_SYNCED;
			else if(fs->GetHaveRev() < fs->GetHeadRev())
				state |= CP4ViewImageList::FSB_NOT_SYNCED;
		}
	}
	return CP4ViewImageList::GetFileIndex(state);
}

/*
	_________________________________________________________________
*/

void CP4winApp::StatusAdd( LPCTSTR txt, StatusView level, bool showDialog )
{
	//		get out if we call with garbage or an empty string
	//
	if ( txt == NULL )
		return;
	if ( lstrlen( txt ) < 1 )
		return;

	LPTSTR msg = new TCHAR[ lstrlen( txt ) + 1 ];
	lstrcpy( msg, txt );

	//		don't post the message until we get a window. since i 
	//		put up messages constantly, i can easily post before a 
	//		window is up. this is cheesy, but simpler than doing an
	//		onidle(), and after all, these are just boring status messages.
	//
	CWnd *pWnd = AfxGetApp( )->m_pMainWnd;
	if ( pWnd )
		::PostMessage( pWnd->m_hWnd, WM_STATUSADD, ( WPARAM ) msg, MAKELPARAM(level, showDialog ? 1 : 0));	
	else
		delete [ ] msg;
}

void CP4winApp::StatusAdd( CStringArray *pArray, StatusView level, bool showDialog )
{
	//		get out if we call with garbage or an empty string
	//
	if ( pArray == NULL )
		return;
	
	//		don't post the message until we get a window. since i 
	//		put up messages constantly, i can easily post before a 
	//		window is up. this is cheesy, but simpler than doing an
	//		onidle(), and after all, these are just boring status messages.
	//

	CWnd *pWnd = AfxGetApp( )->m_pMainWnd;
	if ( pWnd )
		::PostMessage( pWnd->m_hWnd, WM_STATUSADDARRAY, ( WPARAM ) pArray, MAKELPARAM(level, showDialog ? 1 : 0));	
	else
		delete [ ] pArray;
}


void CP4winApp::WinHelp(DWORD dwData, UINT nCmd) 
{
	// MFC automatically assigns help IDs for all menu commands,
	// dialogs and frames (see p4win.hm as produced by makehelp.bat).  
	// For the time being, only helpIDs that come from the p4win.hh 
	// file (sequential IDs that start at 1) will be taken as context 
	// help.  Every other help request will be sent to the help contents.
	//
	// TODO:  The menu commands (0x10001 to 0x1ffff) could all have popup
	//        context menues.  But if a single menu command help id is
	//        missing in the help file, there will be an ugly message
	//        for the user.
	//
	//        An alternative is to direct all menu command IDs to a single
	//        topic in the help file
	//
	//		  Yet another option is to use the IDs in p4win.hm for everything
	//        and then set up enough aliases so that nothing falls thru.  The
	//        robohelp help compiler warns about .hm entries that dont have
	//        topics.

	if(dwData < 0x10000 && nCmd != HH_HELP_FINDER)
		CWinApp::HtmlHelp(dwData, HH_HELP_CONTEXT);
	else
		CWinApp::HtmlHelp(dwData, HH_HELP_FINDER);
}


// Spawn a helper app.  For console applications, a batch file is used to ensure
// that filename arguments with ebmedded spaces are passed correctly.  

BOOL CP4winApp::RunApp(int app, RunAppMode mode, HWND hWnd, BOOL isUnicode,
					   RUNAPPTHREADINFO *lprati, CString &errorText, 
					   LPCTSTR arg1, LPCTSTR arg2, LPCTSTR arg3, LPCTSTR arg4, 
					   LPCTSTR arg5, LPCTSTR arg6, LPCTSTR arg7, LPCTSTR arg8, 
					   LPCTSTR arg9, LPCTSTR arg10,LPCTSTR arg11,LPCTSTR arg12,
					   LPCTSTR arg13,LPCTSTR arg14,LPCTSTR arg15,LPCTSTR arg16,
					   LPCTSTR arg17)
{
	CString orgArgX;
	CString orgArgY;
	TCHAR	buf1[LONGPATH*2 + 1];
	TCHAR	buf2[LONGPATH + 1];
	TCHAR	buf3[LONGPATH + 1];
	TCHAR	buf4[LONGPATH + 1];
	TCHAR	buf5[LONGPATH + 1];
	int		i;

	// Find out if we're running NT or win95
	OSVERSIONINFO osVer;
	osVer.dwOSVersionInfoSize= sizeof(OSVERSIONINFO);
	GetVersionEx(&osVer);
	
	// Set up the spawn operation
#ifdef UNICODE
	DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT; 
#else
	DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS;
#endif
	PROCESS_INFORMATION procInfo;
	STARTUPINFO startInfo;
	TCHAR cmdTitle[255];
	GetStartupInfo(&startInfo);
	startInfo.lpReserved= startInfo.lpDesktop= NULL; 
	startInfo.dwFlags |= STARTF_USESHOWWINDOW;
	startInfo.wShowWindow = SW_SHOWNORMAL;

	// Find out what app we're running
	CString appName, appText, title;
	int numArgs = 0;
	int bInternal = 0;
	BOOL isConsole=FALSE;
	BOOL isClose=FALSE;
	BOOL bSquid=FALSE;

	switch(app)
	{
	case DIFF_APP:
	{
		orgArgX = arg4;
		orgArgY = arg6;

		// First, get the file extension, if any, and find out if
		// it has an associated diff for that extension
		appName.Empty();
		CString extension = GetFilesExtension(arg2);
		if (!extension.CompareNoCase(_T("tmp")))
			extension = GetFilesExtension(arg1);
		if(!extension.IsEmpty())
			appName= GET_P4REGPTR()->GetAssociatedDiff(extension);

		if (!appName.IsEmpty())
		{
			bInternal = 0;
			CString appProg = appName;
			if ((i = appProg.ReverseFind(_T('\\'))) > -1)
				appProg = appProg.Right(appProg.GetLength() - i - 1);
			if (appProg == _T("P4Diff.exe"))
				bInternal = 2;
			else if (!appProg.CompareNoCase(_T("Squid.exe")))
				bSquid = TRUE;
		}
		else if ((bInternal = GET_P4REGPTR()->GetDiffInternal()) >= 2)
		{
			bInternal = 2;
			appName= _T("P4Diff.exe");
		}
		else if (bInternal == 1)
		{
			if (m_P4Merge && (osVer.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS))
			{
				appName= _T("P4Merge.exe");
			}
			else
			{
				bInternal = 2;
				appName= _T("P4Diff.exe");
			}
		}
		else
		{
			appName= GET_P4REGPTR()->GetDiffApp();
			CString appProg = appName;
			if ((i = appProg.ReverseFind(_T('\\'))) > -1)
				appProg = appProg.Right(appProg.GetLength() - i - 1);
			if (appProg == _T("P4Diff.exe"))
				bInternal = 2;
			else if (!appProg.CompareNoCase(_T("Squid.exe")))
				bSquid = TRUE;
			else
			{
				// non-Perforce diff app may require DOS console to work correctly
				isConsole= GET_P4REGPTR()->GetDiffAppIsConsole();
				isClose  = GET_P4REGPTR()->GetDiffAppIsClose();
			}
		}
		appText= LoadStringResource(IDS_APP_DIFF);
		title= LoadStringResource(IDS_TITLE_DIFF);
		numArgs=2;
		if (arg1!=NULL && lstrlen(arg1) > 0)
		{
			if (*(arg1+1) == _T(':'))
			{
				if (GetFileAttributes(arg1) == -1)
				{
					appText.FormatMessage(IDS_DIFF_FILENOTFOUND, arg1);
					AddToStatus(appText, SV_ERROR);
					return FALSE;
				}
			}
		}
		else
		{
			ASSERT(0);
		}
		if (arg2!=NULL && lstrlen(arg2) > 0)
		{
			if (*(arg2+1) == _T(':'))
			{
				if (GetFileAttributes(arg2) == -1)
				{
					appText.FormatMessage(IDS_DIFF_FILENOTFOUND, arg2);
					AddToStatus(appText, SV_ERROR);
					return FALSE;
				}
			}
		}
		else
		{
			ASSERT(0);
		}
		if (bInternal == 2)
		{
			ASSERT(arg7==NULL && arg8==NULL && arg9==NULL);
			switch (GET_P4REGPTR()->GetWhtSpFlag())
			{
			case 1:
				arg7 = _T("-b");
				break;
			case 2:
				arg7 = _T("-w");
				break;
			default:
				arg7 = _T("-e");
				break;
			}
			_stprintf(buf1, _T("%d"), -1);	// Temp until implment passing context
			_stprintf(buf2, _T("%d"), GET_P4REGPTR()->GetTabWidth());
			arg8 = buf1;
			arg9 = buf2;
			// On win/9x, we can't have a nice long command line - throw away -r and -l
			if (!arg3 || (osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS))
			{
				arg3 = arg7;
				arg4 = arg8;
				arg5 = arg9;
				arg6 = NULL;
				numArgs=5;
			}
			else if (!arg5)
			{
				ASSERT(arg3!=NULL && lstrlen(arg3) > 0);
				ASSERT(arg4!=NULL && lstrlen(arg4) > 0);
				arg5 = arg7;
				arg6 = arg8;
				arg7 = arg9;
				arg8 = NULL;
				numArgs=7;
			}
			else
				numArgs=9;
		}
		else if (bInternal)
		{
			ASSERT(arg7==NULL && arg8==NULL && arg9==NULL);
			LPCTSTR sav1 = arg1;
			LPCTSTR sav2 = arg2;
			LPCTSTR sav4 = arg4;
			LPCTSTR sav6 = arg6;
			switch (GET_P4REGPTR()->GetWhtSpFlag())
			{
			case 0:
				arg1 = _T("-dl");
				break;
			case 1:
				arg1 = _T("-db");
				break;
			case 2:
				arg1 = _T("-dw");
				break;
			case 3:
				arg1 = _T("-da");
				break;
			default:
				arg1 = _T(" ");
				break;
			}
			_stprintf(buf1, _T("%d"), GET_P4REGPTR()->GetTabWidth());
			arg2 = _T("-tw");
			arg3 = buf1;
			arg4 = _T("-nl");
			arg5 = sav4 ? sav4 : sav1;
			arg6 = _T("-nr");
			arg7 = sav6 ? sav6 : sav2;
			arg8 = sav1;
			arg9 = sav2;
			numArgs=9;
		}
		else if (bSquid)
		{
			// On win/9x, we can't have a nice long command line - throw away -r and -l
			if (!arg3 || (osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS))
			{
				arg3 = arg4 = arg5 = arg6 = NULL;
				numArgs=2;
			}
			else
				numArgs = arg5 ? 6 : 4;
		}
		break;
	}
	case EDIT_APP:
	    appName= GET_P4REGPTR()->GetEditApp();
		appText= LoadStringResource(IDS_APP_EDITOR);
		title.FormatMessage(IDS_TITLE_EDITOR_s, appName);
		numArgs=1;
		isConsole= GET_P4REGPTR()->GetEditAppIsConsole();
//		isClose  = GET_P4REGPTR()->GetEditAppIsClose();
		ASSERT(arg1!=NULL && lstrlen(arg1) > 0);
		ASSERT(arg2==NULL && arg3==NULL && arg4==NULL);
		break;
	case TREE_APP:
#ifdef UNICODE
		switch(MainFrame()->HaveP4QTree())
		{
			default:
			case 1:
				if (MainFrame()->GetP4Vver() < 20051)
				{
					DWORD ver = MainFrame()->GetP4Vver();
					DWORD yr = ver / 10;
					DWORD pt = ver - (yr*10);
					CString txt;
					txt.FormatMessage(IDS_P4VTOOOLD, yr, pt, _T("2005.1"));
					AfxMessageBox(txt, MB_ICONEXCLAMATION);
					return FALSE;
				}
				appName= _T("p4v.exe");
				startInfo.wShowWindow = SW_SHOWNORMAL;
				ASSERT(arg10 !=NULL && lstrlen(arg10) > 0);
				ASSERT(arg15==NULL);
				numArgs = (arg11 == NULL) ? 10 : (arg13 == NULL) ? 12 : 14;
				switch(numArgs)
				{
				case 10:
					if (WideCharToMultiByte(CP_UTF8, 0, arg10, -1, 
											(LPSTR)buf2, sizeof(buf2), NULL, NULL)
					 && MultiByteToWideChar(1252, 0, (LPSTR)buf2, -1, buf1, sizeof(buf1)))
						arg10 = buf1;
					break;
				case 12:
					if (WideCharToMultiByte(CP_UTF8, 0, arg12, -1, 
											(LPSTR)buf2, sizeof(buf2), NULL, NULL)
					 && MultiByteToWideChar(1252, 0, (LPSTR)buf2, -1, buf1, sizeof(buf1)))
						arg12 = buf1;
					break;
				case 14:
					if (WideCharToMultiByte(CP_UTF8, 0, arg14, -1, 
											(LPSTR)buf2, sizeof(buf2), NULL, NULL)
					 && MultiByteToWideChar(1252, 0, (LPSTR)buf2, -1, buf1, sizeof(buf1)))
						arg14 = buf1;
					break;
				default:
					ASSERT(0);
					break;
				}
				break;
			case 2:
				appName= _T("p4tree.exe");
				startInfo.wShowWindow = SW_SHOWMAXIMIZED;
				ASSERT(arg9 !=NULL && lstrlen(arg9) > 0);
				ASSERT(arg14==NULL);
				numArgs = (arg10 == NULL) ? 9 : (arg12 == NULL) ? 11 : 13;
				break;
			case 3:
				appName= _T("P4QTree.exe");
				startInfo.wShowWindow = SW_SHOWMAXIMIZED;
				ASSERT(arg9 !=NULL && lstrlen(arg9) > 0);
				ASSERT(arg14==NULL);
				numArgs = (arg10 == NULL) ? 9 : (arg12 == NULL) ? 11 : 13;
				break;
		}
		appText= LoadStringResource(IDS_APP_TREE);
		title.FormatMessage(IDS_TITLE_EDITOR_s, appName);	// Can use the same string as the editor
		break;
#else
		return FALSE;
#endif
	case ANNOTATE_APP:
#ifdef UNICODE
		if (MainFrame()->HaveTLV())
		{
			if (MainFrame()->GetP4Vver() < 20051)
			{
				DWORD ver = MainFrame()->GetP4Vver();
				DWORD yr = ver / 10;
				DWORD pt = ver - (yr*10);
				CString txt;
				txt.FormatMessage(IDS_P4VTOOOLD, yr, pt, _T("2005.1"));
		        AfxMessageBox(txt, MB_ICONEXCLAMATION);
				return FALSE;
			}
			appName= _T("p4v.exe");
			startInfo.wShowWindow = SW_SHOWNORMAL;
			ASSERT(arg10 !=NULL && lstrlen(arg10) > 0);
			ASSERT(arg15==NULL);
			numArgs = (arg11 == NULL) ? 10 : (arg13 == NULL) ? 12 : 14;
			switch(numArgs)
			{
			case 10:
				if (WideCharToMultiByte(CP_UTF8, 0, arg10, -1, 
										(LPSTR)buf2, sizeof(buf2), NULL, NULL)
					 && MultiByteToWideChar(1252, 0, (LPSTR)buf2, -1, buf1, sizeof(buf1)))
					arg10 = buf1;
				break;
			case 12:
				if (WideCharToMultiByte(CP_UTF8, 0, arg12, -1, 
										(LPSTR)buf2, sizeof(buf2), NULL, NULL)
					 && MultiByteToWideChar(1252, 0, (LPSTR)buf2, -1, buf1, sizeof(buf1)))
					arg12 = buf1;
				break;
			case 14:
				if (WideCharToMultiByte(CP_UTF8, 0, arg14, -1, 
										(LPSTR)buf2, sizeof(buf2), NULL, NULL)
					 && MultiByteToWideChar(1252, 0, (LPSTR)buf2, -1, buf1, sizeof(buf1)))
					arg14 = buf1;
				break;
			default:
				ASSERT(0);
				break;
			}
		}
		startInfo.wShowWindow = SW_SHOWNORMAL;
		appText= LoadStringResource(IDS_APP_TREE);
		title.FormatMessage(IDS_TITLE_EDITOR_s, appName);	// Can use the same string as the editor
		break;
#else
		return FALSE;
#endif
	case MERGE_APP:
	{
		orgArgX = arg3;
		orgArgY = arg5;

		ASSERT(arg1!=NULL && lstrlen(arg1) > 0);
		ASSERT(arg2!=NULL && lstrlen(arg2) > 0);
		ASSERT(arg3!=NULL && lstrlen(arg3) > 0);
		ASSERT(arg4!=NULL && lstrlen(arg4) > 0);

		// First, get the file extension, if any, and find out if
		// it has an associated Merge for that extension
		appName.Empty();
		CString extension = GetFilesExtension(arg3);
		if (!extension.CompareNoCase(_T("tmp")))
			extension = GetFilesExtension(arg2);
		if (!extension.CompareNoCase(_T("tmp")))
			extension = GetFilesExtension(arg1);
		if(!extension.IsEmpty())
			appName= GET_P4REGPTR()->GetAssociatedMerge(extension);

		if (!appName.IsEmpty())
		{
			numArgs=4;
			bInternal=0;
		}
		else
		{
			bInternal = GET_P4REGPTR()->GetMergeInternal();
			switch (bInternal)
			{
			case 0:
			{
				appName= GET_P4REGPTR()->GetMergeApp();
				CString appProg = appName;
				if ((i = appProg.ReverseFind(_T('\\'))) > -1)
					appProg = appProg.Right(appProg.GetLength() - i - 1);
				if ((appProg == _T("P4WinMrg.exe")) && (appName.Find(_T("raxis")) == -1))
					bInternal = 1;
				else
				{
					isConsole= GET_P4REGPTR()->GetMergeAppIsConsole();
					isClose  = GET_P4REGPTR()->GetMergeAppIsClose();
					numArgs=4;
				}
				break;
			}
			default:
			case 2:
			{
				if (m_P4Merge && (osVer.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS))
				{
					appName= _T("P4Merge.exe");
					break;
				}
				bInternal = 1;
			}
			case 1:
				appName= _T("P4WinMrg.exe");
				break;
			}
		}
		if (bInternal == 1)
		{
			BOOL bNSF = GET_P4REGPTR()->GetMergeNSF();
			isConsole = isClose = FALSE;
			LPCTSTR svArg = arg5;
			arg9 = arg4;	// result
			arg8 = arg3;	// yours
			arg7 = arg2;	// theirs
			arg6 = arg1;	// base
			switch (GET_P4REGPTR()->GetMrgWhtSpFlag())
			{
			case 1:
				arg5 = _T("-b");
				break;
			case 2:
				arg5 = _T("-w");
				break;
			default:
				arg5 = _T("-s");
				break;
			}
			lstrcpy(buf4, m_bNoCRLF ? _T("-lf") : _T("-crlf"));
			_stprintf(buf3, _T("\"-i%d\""), GET_P4REGPTR()->GetMrgTabWidth());
            if(bNSF)
                lstrcpy(buf2, _T("-nsf"));
            else
            {
                CString msg;
                msg.FormatMessage(IDS_WILL_REPLACE_s_IF_MERGE_ACCEPTED, arg8);	// yours
                lstrcpy(buf2, msg);
            }
			_stprintf(buf1, _T("\"-t%s\""), svArg);
			arg4 = buf4;
			arg3 = buf3;
			arg2 = buf2;
			arg1 = buf1;
			numArgs=9;
		}
		else if (bInternal)
		{
			isConsole = isClose = FALSE;
			LPCTSTR svArg = arg5;
			arg17 = arg4;	// result
			arg16 = arg3;	// yours
			arg15 = arg2;	// theirs
			arg14 = arg1;	// base
			switch (GET_P4REGPTR()->GetMrgWhtSpFlag())
			{
			case 0:
				arg13 = _T("-dl");
				break;
			case 1:
				arg13 = _T("-db");
				break;
			case 2:
				arg13 = _T("-dw");
				break;
			case 3:
				arg13 = _T("-da");
				break;
			default:
				arg13 = _T(" ");
				break;
			}
			_stprintf(buf5, _T("\"%s\""), *arg6 ? arg6 : arg14);
			_stprintf(buf4, _T("%d"), GET_P4REGPTR()->GetMrgTabWidth());
            CString msg;
            msg.FormatMessage(IDS_WILL_REPLACE_s_IF_MERGE_ACCEPTED, arg16);	// yours
			if (msg.GetAt(0) == _T('-') && msg.GetAt(1) == _T('m'))
				msg = msg.Mid(2);
            lstrcpy(buf3, msg);
			_stprintf(buf2, _T("\"%s\""), arg16);
			_stprintf(buf1, _T("\"%s\""), svArg);
			arg12 = m_bNoCRLF ? _T("unix") : _T("win");
			arg11 = _T("-le");
			arg10 = buf4;
			arg9 = _T("-tw");
			arg8 = buf3;
			arg7 = _T("-nm");
			arg6 = buf2;
			arg5 = _T("-nr");
			arg4 = buf1;
			arg3 = _T("-nl");
			arg2 = buf5;
			arg1 = _T("-nb");
			numArgs=17;
		}
		appText= LoadStringResource(IDS_APP_MERGE);
		title.FormatMessage(IDS_TITLE_MERGE, appName);
		break;
	}
	case P4_APP:
	    appName= _T("p4");
		appText= LoadStringResource(IDS_APP_P4);
		title.FormatMessage(IDS_TITLE_P4, appName);
		ASSERT(arg1!=NULL && lstrlen(arg1) > 0);
		ASSERT(arg2!=NULL && lstrlen(arg2) > 0);
		ASSERT(arg3!=NULL && lstrlen(arg3) > 0);
		ASSERT(arg4!=NULL && lstrlen(arg4) > 0);
		ASSERT(arg5!=NULL && lstrlen(arg5) > 0);
		ASSERT(arg6!=NULL && lstrlen(arg6) > 0);
		ASSERT(arg7!=NULL && lstrlen(arg7) > 0);
		for (numArgs = 7; ++numArgs < 14; )
		{
			if (numArgs == 8)
			{
				if (!arg9)
					break;
			}
			else if (numArgs == 9)
			{
				if (!arg10)
					break;
			}
			else if (numArgs == 10)
			{
				if (!arg11)
					break;
			}
			else if (numArgs == 11)
			{
				if (!arg12)
					break;
			}
			else if (numArgs == 12)
			{
				if (!arg13)
					break;
			}
			else if (numArgs == 13)
			{
				if (!arg14)
					break;
			}
		}
		startInfo.dwFlags = STARTF_USESHOWWINDOW;
		startInfo.wShowWindow = SW_SHOWNORMAL;
		dwCreationFlags |= CREATE_NEW_CONSOLE;
		break;
	default:
		ASSERT(0);
		return FALSE;
	}
	if(isConsole)
	{
		lstrcpy(cmdTitle, title);
		startInfo.lpTitle= cmdTitle;
		startInfo.dwXCountChars=80; 
		startInfo.dwYCountChars=1000; 
		startInfo.dwFlags=STARTF_USECOUNTCHARS; 
		startInfo.cbReserved2=0; 
		startInfo.lpReserved2=NULL; 
	}

	// If we drew a blank for the appname, return the error
	if(appName.GetLength()==0)
	{
		errorText.FormatMessage(IDS_MISSING_APPNAME_s_s, appText, appText);
		return FALSE;
	}

	// Copy the arguments, and make sure args with embedded spaces have quotes
	// this is primitive, but it saves code in calling functions
	CString args[17];
	args[0]=arg1;
	if(numArgs > 1)
	{
		args[1]=arg2;
		if(numArgs > 2)
		{
			args[2]=arg3;
			if(numArgs > 3)
			{
				args[3]=arg4;
				if(numArgs > 4)
				{
					args[4]=arg5;
					if(numArgs > 5)
					{
						args[5]=arg6;
						if(numArgs > 6)
						{
							args[6]=arg7;
							if(numArgs > 7)
							{
								args[7]=arg8;
								if(numArgs > 8)
								{
									args[8]=arg9;
									if(numArgs > 9)
									{
										args[9]=arg10;
										if(numArgs > 10)
										{
											args[10]=arg11;
											if(numArgs > 11)
											{
												args[11]=arg12;
												if(numArgs > 12)
												{
													args[12]=arg13;
													if(numArgs > 13)
													{
														args[13]=arg14;
														if(numArgs > 14)
														{
															args[14]=arg15;
															if(numArgs > 15)
															{
																args[15]=arg16;
																if(numArgs > 16)
																{
																	args[16]=arg17;
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	for(i=0; i<numArgs; i++)
	{
		args[i].TrimRight();
		args[i].TrimLeft();
		
		if(args[i].Find(_T(' ')) != -1 && args[i][0] != _T('\"'))
		{
			TCHAR buf[1024];
			lstrcpy(buf, args[i]);
			args[i].Format(_T("\"%s\""), buf);
		}
	}

	appName.TrimRight();
	appName.TrimLeft();
	if(appName.Find(_T(' ')) != -1 && appName[0] != _T('\"'))
	{
		TCHAR buf[1024];
		lstrcpy(buf, appName);
		appName.Format(_T("\"%s\""), buf);
	}

	// Pass the P4CHARSET to P4Merge.exe if its version is 2005.2 or later and have Unicode files
	CString appFileName;
	if ((i = appName.ReverseFind(_T('\\'))) != -1)
		appFileName = appName.Mid(i+1);
	else
		appFileName = appName;
	if (isUnicode && !appFileName.CollateNoCase(_T("P4Merge.exe")) && m_P4MergeVer >= 20052)
	{
		CString charset;
		if (isUnicode == 16)
			charset = _T("utf16");
		else
			charset = GET_P4REGPTR()->GetP4Charset();
		if (charset.GetLength() > 0)
			appName += _T(" -C ") + charset + _T(" ");
	}

	BOOL success;
	TCHAR commandLine[1024];
	CString tempFile;

	// Set up the command line
	CString commLine=appName;
	
	i = 0;
	if (app == DIFF_APP)
	{
		if (!GET_P4REGPTR()->GetDiffInternal()
		 &&  GET_P4REGPTR()->GetDiffAppOptArgChk() 
		 && *(GET_P4REGPTR()->GetDiffOptArgs()))
		{
			CString optArgs = GET_P4REGPTR()->GetDiffOptArgs();
			optArgs.Replace(_T("%1"), _T("%#1"));
			optArgs.Replace(_T("%2"), _T("%#2"));
			optArgs.Replace(_T("%L"), _T("%#L"));
			optArgs.Replace(_T("%R"), _T("%#R"));
			commLine += CString(_T(" ")) + optArgs;
			i =  commLine.Replace(_T("%#1"), args[0]);
			i *= commLine.Replace(_T("%#2"), args[1]);
			commLine.Replace(_T("%#L"), orgArgX);
			commLine.Replace(_T("%#R"), orgArgY);
			_stprintf(buf2, _T("%d"), GET_P4REGPTR()->GetTabWidth());
			commLine.Replace(_T("%W"), buf2);
		}
	}
	else if (app == MERGE_APP)
	{
		if (!GET_P4REGPTR()->GetMergeInternal()
		 &&  GET_P4REGPTR()->GetMergeAppOptArgChk() 
		 && *(GET_P4REGPTR()->GetMergeOptArgs()))
		{
			CString optArgs = GET_P4REGPTR()->GetMergeOptArgs();
			commLine += CString(_T(" ")) + optArgs;
			optArgs.Replace(_T("%1"), _T("%#1"));
			optArgs.Replace(_T("%2"), _T("%#2"));
			optArgs.Replace(_T("%3"), _T("%#3"));
			optArgs.Replace(_T("%4"), _T("%#4"));
			optArgs.Replace(_T("%T"), _T("%#T"));
			optArgs.Replace(_T("%Y"), _T("%#Y"));
			i =  commLine.Replace(_T("%1"), args[0]);
			i *= commLine.Replace(_T("%2"), args[1]);
			i *= commLine.Replace(_T("%3"), args[2]);
			i *= commLine.Replace(_T("%4"), args[3]);
			orgArgY.TrimLeft();
			commLine.Replace(_T("%T"), orgArgY);
			commLine.Replace(_T("%Y"), orgArgX);
			_stprintf(buf2, _T("%d"), GET_P4REGPTR()->GetMrgTabWidth());
			commLine.Replace(_T("%W"), buf2);
		}
	}
	if (!i)
	{
		for(i=0; i<numArgs; i++)
			commLine+=_T(" ")+args[i];
	}

	/* We have to do nasty things with the system settings to fool win/2k/xp/98 
	into bringing the correct window to the foreground after a spawn with wait 
	finishes. Starting with Windows 98 & Windows 2000 the OS restricts which 
	processes can set the foreground window. A process can set the 
	foreground window only if certain conditions are met - none of which 
	can be met after a spawn with wait finishes. Therefore change the 
	system setting that controls this new, useless behavior to disable it 
	before the spawn is done; then restore the previous settings after 
	the correct window has been brought to the foreground. Furthermore we
	have to use the actual value of SPI_*ETFOREGROUNDLOCKTIMEOUT rather
	than the defines because they are not defined on NT.
	*/

	DWORD_PTR dwLockTimeout = 0;
	SystemParametersInfo(0x2000 /*==SPI_GETFOREGROUNDLOCKTIMEOUT*/, NULL, &dwLockTimeout, NULL);
	if (dwLockTimeout && (mode == RA_WAIT))
		SystemParametersInfo(0x2001 /*==SPI_SETFOREGROUNDLOCKTIMEOUT*/, 0, (LPVOID)0, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);

	if(isConsole)
	{
		// Win95 command shell does not have a scrollable screen buffer,
		// so pipe diff results to more by using a batch file
		if((osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (app == DIFF_APP))
		{
	  		TCHAR tempPath[LONGPATH];
  			HANDLE h=0;

			commLine+=_T(" | more");

			if(GetTempPath(LONGPATH, tempPath) == 0)
  			{
  				errorText.FormatMessage(IDS_COULD_NOT_RUN_s_TEMP_COMMAND_PATH_ERROR, appText);
				if (dwLockTimeout && (mode == RA_WAIT))
					SystemParametersInfo(0x2001 /*==SPI_SETFOREGROUNDLOCKTIMEOUT*/, 0, (LPVOID)dwLockTimeout, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
  				return FALSE;
  			}

  			for(i=0; i<100; i++)
  			{
  				tempFile.Format(_T("%sP4W%05ld.BAT"), tempPath, i);
  				h=CreateFile(tempFile, GENERIC_WRITE, 0, 0, CREATE_NEW, 0, 0);
  				if(h != INVALID_HANDLE_VALUE)
  					break;
  			}
  		
  			if(i==100)
  			{
  				errorText.FormatMessage(IDS_COULD_NOT_RUN_s_COULD_NOT_OPEN_TEMP_COMMAND_FILE, appText);
				if (dwLockTimeout && (mode == RA_WAIT))
					SystemParametersInfo(0x2001 /*==SPI_SETFOREGROUNDLOCKTIMEOUT*/, 0, (LPVOID)dwLockTimeout, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
  				return FALSE;
  			}

  			DWORD numWrite;
  			if(!WriteFile(h, commLine, lstrlen(commLine), &numWrite, 0))
  			{
  				CloseHandle(h);
  				errorText.FormatMessage(IDS_COULD_NOT_RUN_s_COULD_NOT_WRITE_TEMP_COMMAND_FILE, appText);
				if (dwLockTimeout && (mode == RA_WAIT))
					SystemParametersInfo(0x2001 /*==SPI_SETFOREGROUNDLOCKTIMEOUT*/, 0, (LPVOID)dwLockTimeout, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
  				return FALSE;
  			}

  			CloseHandle(h);

			commLine = tempFile;
		}
		else
		{
			TCHAR	cmd[MAX_PATH+1];

			GetEnvironmentVariable(_T("ComSpec"), cmd, MAX_PATH);
			commLine = (CString)cmd + (isClose ? _T(" /c start /wait ") : _T(" /k ")) + commLine;
		}

		lstrcpy(commandLine, commLine);

		SetLastError(0);
		success=CreateProcess( NULL,	// pointer to name of executable module 
					commandLine,	// pointer to command line string
					NULL, NULL,  // default security rot
					FALSE,	// handle inheritance flag 
					dwCreationFlags | CREATE_NEW_CONSOLE,	// creation flags 
					NULL, NULL, //default env and curdir
					&startInfo, &procInfo);
	}
	else
	{
		SetLastError(0);
		lstrcpy(commandLine, commLine);
		success=CreateProcess( NULL,	// pointer to name of executable module 
						commandLine,	// pointer to command line string
						NULL, NULL,  // default security rot
						FALSE,	// handle inheritance flag 
						dwCreationFlags,	// creation flags 
						NULL, // env
						NULL, //default dir
						&startInfo, &procInfo);					
	}

	if(success)
	{
		if(mode == RA_WAIT)
		{
			CString	oldtext;
			CString	newtext;
            newtext.FormatMessage(IDS_WAITING_FOR_s_TO_FINISH, appName);

			// Let the user know what we aren't responding
			//
			AfxGetMainWnd()->GetWindowText(oldtext);
			AfxGetMainWnd()->SetWindowText(newtext);

			// Disable the main window and any topmost dialog
			//
			BOOL bReEnableMain = app == MERGE_APP;
			if (AfxGetMainWnd()->IsWindowEnabled())
			{
				EnableWindow( AfxGetMainWnd()->GetSafeHwnd(), FALSE );
				bReEnableMain = TRUE;
			}
			BOOL bReEnable = FALSE;
			// if we weren't given a topmost dialog, see if we can figure it out
			if (!hWnd)
			{
				CWnd *pWnd= AfxGetMainWnd()->GetForegroundWindow();	// get the foreground window
				if (pWnd->GetWindow(GW_OWNER) == AfxGetMainWnd())	// make sure it's ours!
					hWnd = pWnd->GetSafeHwnd();
			}
			if (hWnd)
			{
				EnableWindow( hWnd, FALSE );
				bReEnable = TRUE;
			}
			::ShowWindow(MainFrame()->m_hWnd, SW_SHOWMINNOACTIVE);

			// Wait for the spawned app
			//
			while (WaitForSingleObject( procInfo.hProcess, 1000 ) == WAIT_TIMEOUT)
			{
				MSG msg;

				while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
				{
					if ( msg.message == WM_COMMAND )	// ignore all commands
						continue;
					if ( msg.message == WM_TIMER )		// Don't poll while waiting
						MainFrame()->WaitAWhileToPoll( );
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					if ( msg.message == WM_QUIT )	// get out if app is terminating
						break;
				}
			}
			CloseHandle(procInfo.hProcess);

			// Re-enable the windows
			//
			if (bReEnable)
				::ShowWindow(hWnd, SW_RESTORE);
			if (bReEnableMain)
			{
				::EnableWindow( AfxGetMainWnd()->GetSafeHwnd(), TRUE );
				::SetForegroundWindow( AfxGetMainWnd()->GetSafeHwnd() );
				::SetFocus( AfxGetMainWnd()->GetSafeHwnd() );
			}
			if (bReEnable)
			{
				::EnableWindow( hWnd, TRUE );
				::SetForegroundWindow( hWnd );
				::SetFocus( hWnd );
			}
			AfxGetMainWnd()->SetWindowText(oldtext);
			::ShowWindow(MainFrame()->m_hWnd, SW_RESTORE);
			::SendMessage(MainFrame()->m_hWnd, m_WM_ACTIVATE, 0, 0);
		}
		else if (mode == RA_THREAD || mode == RA_THREADWAIT)
		{
			// Give the spawned program time to get initalized
			WaitForInputIdle(procInfo.hProcess, 1000);
			Sleep(1000);	// And since WaitForInputIdle() is not reliable, wait some more

			// Now fire up the thread which will delete the temp file(s);
			// this thread does WaitForSingleObject(procInfo.hProcess, INFINITE)
			// before deleting any files. It also calls CloseHandle(procInfo.hProcess)
			lprati->hProcess = procInfo.hProcess;
			CWinThread *pTaskThread=AfxBeginThread(lprati->pfnThreadProc, 
						(LPVOID) lprati, THREAD_PRIORITY_NORMAL, 
						0, CREATE_SUSPENDED, NULL);
			pTaskThread->m_bAutoDelete=TRUE;   // Tinker w/ priority here if reqd
			pTaskThread->ResumeThread();

			// If the mode is RA_THREADWAIT, wait until the spawned process exits
			if (mode == RA_THREADWAIT)
			{
				Sleep(333);
				ShowWindow(m_pMainWnd->m_hWnd, SW_HIDE);
				WaitForSingleObject( procInfo.hProcess, INFINITE );
				// Window is NOT re-shown because RA_THREADWAIT is only used
				// when P4Win is run with the -D [filename] flag.
				// P4Win will now exit since the Diff program has exited.
			}
		}
		else
			CloseHandle(procInfo.hProcess);

		// procInfo.hProcess was close by each of the 3 cases above, 
		// but we still must close procInfo.hThread.
		CloseHandle(procInfo.hThread);

		// Note: checking exit codes is futile.  
		//	CMD and COMMAND return random numbers, no matter what happens
	}
	else
	{
		CString errTxt;
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
			errTxt = (TCHAR *)lpMsgBuf;
			errTxt.TrimRight();
		}
		else
			errTxt=LoadStringResource(IDS_UNKNOWN_ERROR);
		errorText.FormatMessage(IDS_FAILED_TO_EXECUTE_s_s_s_n, appText, appName, errTxt, error);
		if (commLine.GetLength() > 127)
		{
			// Find out if we're running NT or win95
			OSVERSIONINFO osVer;
			osVer.dwOSVersionInfoSize= sizeof(OSVERSIONINFO);
			GetVersionEx(&osVer);
			BOOL brittleWare= (osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);

    		if(brittleWare)
				errTxt.FormatMessage(IDS_s_COMMAND_LINE_TOO_LONG_FOR_WIN9X_s_n, 
										appText, commLine, commLine.GetLength());
			else
				errTxt.FormatMessage(IDS_CHECK_PERFORCE_OPTIONS_s, appText);
		}
		else
			errTxt.FormatMessage(IDS_CHECK_PERFORCE_OPTIONS_s, appText);
		errorText += errTxt;
	}

	// restore previous system setting (see comment above,
	// at 1st call to SystemParametersInfo(), for details)
	if (dwLockTimeout && (mode == RA_WAIT))
		SystemParametersInfo(0x2001 /*==SPI_SETFOREGROUNDLOCKTIMEOUT*/, 0, (LPVOID)dwLockTimeout, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);

	if(isConsole && !tempFile.IsEmpty())
  		_tunlink(tempFile);
	return success;
}


// Spawn a viewer. App is referenced by name, and console apps will not be catered to.
// This is a stripped down version of CP4WinApp::RunApp()
BOOL CP4winApp::RunViewerApp(LPCTSTR app, LPCTSTR fileName)
{
	if( app==NULL || lstrlen(app) == 0 ||
		fileName==NULL || lstrlen(fileName) == 0)
	{
		// Dont ASSERT(0); since this error occurrs for any binary file w/ no associated app
		return FALSE;
	}

	// Set up the spawn operation
	PROCESS_INFORMATION procInfo;
	STARTUPINFO startInfo;
	
	GetStartupInfo(&startInfo);
	startInfo.lpReserved= startInfo.lpDesktop= NULL; 
	startInfo.dwFlags |= STARTF_USESHOWWINDOW;
	startInfo.wShowWindow = SW_SHOWNORMAL;

	CString arg=fileName;
	arg.TrimRight();
	arg.TrimLeft();
		
	if(arg.Find(_T(' ')) != -1 && arg[0] != _T('\"'))
	{
		TCHAR buf[1024];
		lstrcpy(buf, arg);
		arg.Format(_T("\"%s\""), buf);
	}


	CString appName=app;
	appName.TrimRight();
	appName.TrimLeft();
	if(appName.Find(_T(' ')) != -1 && appName[0] != _T('\"'))
	{
		TCHAR buf[1024];
		lstrcpy(buf, appName);
		appName.Format(_T("\"%s\""), buf);
	}
			
	// Set up the command line
	CString commLine=appName;
	commLine+=_T(" ")+arg;
	
	BOOL success=CreateProcess( NULL,	// pointer to name of executable module 
						const_cast<LPTSTR>((LPCTSTR)commLine),	// pointer to command line string
						NULL, NULL,  // default security rot
						FALSE,	// handle inheritance flag 
#ifdef UNICODE
						NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT, 
#else
						NORMAL_PRIORITY_CLASS,	// creation flags 
#endif
						NULL, NULL, //default env and curdir
						&startInfo, &procInfo);					
	
	if(success)
	{
		// Note:  checking exit codes is futile.  CMD and COMMAND return random numbers, no
		//        matter what happens
		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
	}

	return success;
}


/////////////////////////////////////////////////////////////////////////////////
// Minor text formatting functions used throughout app, so multiline edit boxes 
// can translate text with embedded tabs, as generated by command line clients

// Utility function to remove tabs and replace all LF's with CR-LFs
CString RemoveTabs(LPCTSTR text)
{
	SET_BUSYCURSOR();
	CString out=text;	// avoid billions of re-allocs by setting approx length
	out+=text;			// then double the length of the buffer
	out="";				// and empty the buffer without freeing it

	int i=0;
	
	while(text[i] != _T('\0'))
	{
		if(text[i]!=_T('\t'))
		{
			if(text[i]==_T('\n') && text[max(0,i-1)]!=_T('\r'))
				out += _T("\r\n");
			else
				out += text[i];
		}

		i++;
	}
	
	// Strip off a trailing newline char
	if(out.GetLength() > 0 && out[out.GetLength()-1] == _T('\n'))
		out=out.Left(out.GetLength()-2);

	return out;
}


CString MakeCRs(LPCTSTR text)
{
    // replace LF with CR-LF
    // then strip any trailing CR-LFs

    int size = lstrlen(text) + 1;
    CString out;
    LPTSTR pOutBuf = out.GetBufferSetLength(size);
    LPTSTR pOut = pOutBuf;
    // keep track of where any trailing CRs are located so they can be chopped
    // without repeatedly scanning the string (which may be very long)
    // this pointer points to the char following the last non-CR character
    LPTSTR pLastNonCR = pOut;
    // there will be probably be a final realloc, and this is probably a temp 
    // string anyway, so minimize the number of reallocs rather than the size
    const int growBy = 4096;    

#ifdef UNICODE
	for(LPCTSTR pIn = text; *pIn != _T('\0'); pIn++)
#else
	for(LPCTSTR pIn = text; *pIn != _T('\0'); pIn = CharNext(pIn))
#endif
	{
        // make sure there will be enough room for a character
        // even if it is a double byte char, or an expanded CR
        if(pOut - pOutBuf + 2 > size - 1)
        {
            int oldSize = static_cast<int>(pOut - pOutBuf);
            int nLastNonCR = static_cast<int>(pLastNonCR - pOutBuf);
            out.ReleaseBuffer(oldSize);
            size += growBy;
            pOutBuf = out.GetBufferSetLength(size);
            pOut = pOutBuf + oldSize;
            pLastNonCR = pOutBuf + nLastNonCR;
        }
		if(*pIn==_T('\r'))
        {
            *pOut++ = *pIn;
            if(pIn[1] == _T('\n'))
                *pOut++ = *++pIn;
            else
                pLastNonCR = pOut;
        }
        else if(*pIn==_T('\n'))
        {
            // a LF with no preceding CR, so insert a CR
            *pOut++ = _T('\r');
            *pOut++ = _T('\n');
        }
		else
        {
#ifdef UNICODE
			*pOut++ = *pIn;
#else
			_tccpy(pOut, pIn);
            pOut = CharNext(pOut);
#endif
            pLastNonCR = pOut;
        }
	}
    // chop off any trailing newline chars
    *pLastNonCR = 0;
    out.ReleaseBuffer();
	
	return out;
}

CString MakeLFs(LPCTSTR text)
{
    // replace CR with CR-LF
    // then strip any trailing CR-LFs

    int size = lstrlen(text) + 1;
    CString out;
    LPTSTR pOutBuf = out.GetBufferSetLength(size);
    LPTSTR pOut = pOutBuf;
    // keep track of where any trailing CRs are located so they can be chopped
    // without repeatedly scanning the string (which may be very long)
    // this pointer points to the char following the last non-CR character
    LPTSTR pLastNonCR = pOut;
    // there will be probably be a final realloc, and this is probably a temp 
    // string anyway, so minimize the number of reallocs rather than the size
    const int growBy = 4096;    

#ifdef UNICODE
	for(LPCTSTR pIn = text; *pIn != _T('\0'); pIn++)
#else
	for(LPCTSTR pIn = text; *pIn != _T('\0'); pIn = CharNext(pIn))
#endif
	{
        // make sure there will be enough room for a character
        // even if it is a double byte char, or an expanded CR
        if(pOut - pOutBuf + 2 > size - 1)
        {
            int oldSize = static_cast<int>(pOut - pOutBuf);
            int nLastNonCR = static_cast<int>(pLastNonCR - pOutBuf);
            out.ReleaseBuffer(oldSize);
            size += growBy;
            pOutBuf = out.GetBufferSetLength(size);
            pOut = pOutBuf + oldSize;
            pLastNonCR = pOutBuf + nLastNonCR;
        }
		if(*pIn==_T('\r'))
        {
            *pOut++ = _T('\r');
            *pOut++ = _T('\n');
        }
		else
        {
#ifdef UNICODE
			*pOut++ = *pIn;
#else
            _tccpy(pOut, pIn);
            pOut = CharNext(pOut);
#endif
            pLastNonCR = pOut;
        }
	}
    // chop off any trailing newline chars
    *pLastNonCR = 0;
    out.ReleaseBuffer();
	
	return out;
}

CString UnMakeCRs(LPCTSTR text)
{
    // remove any CR characters
    // then strip any trailing LFs

    int size = lstrlen(text) + 1;
    CString out;
    LPTSTR pOut = out.GetBufferSetLength(size);
    // keep track of where any trailing LFs are located so they can be chopped
    // without repeatedly scanning the string (which may be very long)
    // this pointer points to the char following the last non-LF character
    LPTSTR pLastNonLF = pOut;

#ifdef UNICODE
	for(LPCTSTR pIn = text; *pIn != _T('\0'); pIn++)
#else
	for(LPCTSTR pIn = text; *pIn != _T('\0'); pIn = CharNext(pIn))
#endif
	{
        if(*pIn != _T('\r'))
        {
            if(*pIn == _T('\n'))
                *pOut++ = *pIn;
            else
            {
#ifdef UNICODE
			*pOut++ = *pIn;
#else
                _tccpy(pOut, pIn);
                pOut = CharNext(pOut);
#endif
                pLastNonLF = pOut;
            }
        }
	}
    // chop off any trailing newline chars
    *pLastNonLF = 0;
    out.ReleaseBuffer();
	
	return out;
}

CString PadCRs(LPCTSTR text)
{
	CString out;
		
	// Replace CR and TABs with spaces.  This allows multi-line descriptions
	// to be displayed on a single line
	int len=lstrlen(text);
	LPCTSTR ptr=text;
	LPTSTR ptrout=out.GetBuffer(len);
	LPTSTR ptrstart = ptrout;
	for(int i=0; i<len; i++)
	{
		if ((*ptr==_T('\r')) || (*ptr==_T('\n')) || (*ptr==_T('\t')))
		{
			if ((ptrout > ptrstart) && (*(ptrout-1) != _T(' ')))
				*ptrout++ = _T(' ');
			ptr++;
		}
		else
		{
			*ptrout++ = *ptr++;
		}
	}
	
	*ptrout=_T('\0');
	out.ReleaseBuffer();
	
	return out;
}

CString WrapDesc(LPCTSTR text, int maxcol)
{
	int count = 0;
    int size = lstrlen(text) + 1;
    CString out;
	CString savestr;
    LPTSTR pOut = out.GetBufferSetLength(size*2);
	LPTSTR pLastWhite = pOut;
	LPTSTR pLWnext = pOut;
	LPTSTR pBgnLine = pOut;
	LPTSTR p;
	for(LPCTSTR pIn = text; *pIn != _T('\0'); )
	{
		if ((*pIn == _T('\r')) || (*pIn == _T('\n')))
		{
			count = 0;
			pLastWhite = pBgnLine = pOut;
		}
		if (*pIn <= _T(' '))
			pLWnext = pOut;
#ifdef UNICODE
		*pOut++ = *pIn++;
#else
		_tccpy(pOut, pIn);
		pOut = CharNext(pOut);
		pIn = CharNext(pIn);
#endif
		if ((++count > maxcol) && (pLastWhite != pBgnLine) 
			&& (*pIn != _T('\r')) && (*pIn != _T('\n')))
		{
			*pOut = _T('\0');
#ifdef UNICODE
			pLastWhite++;
#else
			pLastWhite = CharNext(pLastWhite);
#endif
			savestr = pLastWhite;
			*pLastWhite++ = _T('\r');
			*pLastWhite++ = _T('\n');
			lstrcpy(pLastWhite, savestr);
			pOut = pLastWhite + lstrlen(pLastWhite);
			pLWnext = pBgnLine = pLastWhite;
			for (count = 0, p = pBgnLine; p < pOut; )
			{
#ifdef UNICODE
				p++;
#else
				p = CharNext(p);
#endif
				if ((*p == _T('\r')) || (*p == _T('\n')))
				{
					count = 0;
					pBgnLine = p;
				}
				else
					count++;
				if (*p <= _T(' ') && *p)
					pLWnext = p;
			}
		}
		pLastWhite = pLWnext;

	}
	*pOut = _T('\0');
    out.ReleaseBuffer();
	
	return out;
}

/////////////////////////////////////////////////
// Utility functions to perform string comparisons according
// to the case-sensitivity of the server

int Compare(LPCTSTR str1, LPCTSTR str2)
{
	// If server level not set, we also dont know if server is nocase
	ASSERT(GET_SERVERLEVEL());

	if(IS_NOCASE())
		return _tcsicmp(str1, str2);
	else
		return _tcscmp(str1, str2);
}

int nCompare(LPCTSTR str1, LPCTSTR str2, int n)
{
	// If server level not set, we also dont know if server is nocase
	ASSERT(GET_SERVERLEVEL());

	if(IS_NOCASE())
		return _tcsnicmp(str1, str2, n);
	else
		return _tcsncmp(str1, str2, n);
}

int nCommon(LPCTSTR str1, LPCTSTR str2)
{
	// If server level not set, we also dont know if server is nocase
	ASSERT(GET_SERVERLEVEL());

	int commonLength=0;
	LPCTSTR ptr1= str1;
	LPCTSTR ptr2= str2;

	if(IS_NOCASE())
	 	while( ::toupper(*ptr1) == ::toupper(*ptr2) && 
            *ptr1 != _T('\0') && *ptr2 != _T('\0'))
		{		
			commonLength++;
			ptr1++;
			ptr2++;
		}	
	else
		while( *ptr1 == *ptr2 && *ptr1 != _T('\0') && *ptr2 != _T('\0'))
		{		
			commonLength++;
			ptr1++;
			ptr2++;
		}
	return commonLength;
}

void TrimRightMBCS(CString &str, TCHAR *chars)
{
#ifdef UNICODE
	str.TrimRight(chars);
#else
	int len;
	LPTSTR pBuf = str.GetBuffer(len = str.GetLength());
	TCHAR *pchBgn = (TCHAR *)pBuf;
	TCHAR *pchLst = _tcsdec(pchBgn, pchBgn + len);
	BOOL b = TRUE;
	while (b)
	{
		b = FALSE;
		for (TCHAR *pchChk = chars; *pchChk; pchChk = _tcsinc(pchChk))
		{
			if (*pchChk == *pchLst)
			{
				*pchLst = _T('\0');
				pchLst = _tcsdec(pchBgn, pchLst);
				b = TRUE;
				break;
			}
		}
	}
	str.ReleaseBuffer(-1);
#endif
}

// This functions assumes that oldchar and newchar are the same width!
// Since this is used to replace \ with /, this works fine.
void ReplaceMBCS(CString &str, TCHAR oldchar, TCHAR newchar)
{
#ifdef UNICODE
	str.Replace(oldchar, newchar);
#else
#ifdef _DEBUG
	TCHAR oldbuf[8];
	TCHAR newbuf[8];
	memset(oldbuf, _T('\0'), sizeof(oldbuf));
	memset(newbuf, _T('\0'), sizeof(newbuf));
	oldbuf[0] = oldchar;
	newbuf[0] = newchar;
	ASSERT(lstrlen(oldbuf) == lstrlen(newbuf));
#endif

	LPTSTR pBuf = str.GetBuffer(str.GetLength());
	for (TCHAR *pch = (TCHAR *)pBuf; *pch; pch = _tcsinc(pch))
	{
		if (*pch == oldchar)
			*pch =  newchar;
	}
	str.ReleaseBuffer(-1);
#endif
}

int FindMBCS(CString &str, TCHAR findchar, int skip /*=0*/)
{
#ifdef UNICODE
	return str.Find(findchar, skip);
#else
	ASSERT(skip >= 0);

	int i;
	LPTSTR pBuf = str.GetBuffer(str.GetLength());
	TCHAR *pchBgn = (TCHAR *)pBuf;
	TCHAR *pch;
	for (i=-1, pch=pchBgn; *pch; pch = _tcsinc(pch))
	{
		if (skip)
		{
			skip--;
		}
		else if (*pch == findchar)
		{
			i = pch - pchBgn;
			break;
		}
	}
	str.ReleaseBuffer(-1);
	return i;
#endif
}

int ReverseFindMBCS(CString &str, TCHAR findchar)
{
#ifdef UNICODE
	return str.ReverseFind(findchar);
#else
	int len;
	LPTSTR pBuf = str.GetBuffer(len = str.GetLength());
	TCHAR *pchBgn = (TCHAR *)pBuf;
	TCHAR *pchLst = _tcsdec(pchBgn, pchBgn + len);
	while (pchBgn < pchLst)
	{
		if (*pchLst == findchar)
			break;
		pchLst = _tcsdec(pchBgn, pchLst);
	}
	if (pchBgn < pchLst)
		len = pchLst - pchBgn;
	else
		len = *pchBgn == findchar ? 0 : -1;
	str.ReleaseBuffer(-1);
	return len;
#endif
}

// Function to get a file's extension
CString GetFilesExtension(LPCTSTR filename)
{
	CString extension = filename;
	int slash= extension.ReverseFind(_T('\\'));
	if(slash != -1)
		extension=extension.Mid(slash+1);

	int dot= extension.ReverseFind(_T('.'));
	if(dot == -1)
		extension.Empty();
	else
		extension=extension.Mid(dot+1);
	return extension;
}

// Function to compare 2 path&filenames in MS
// TreeView order - which is definitely weird
// (see comment below). This means x.ext comes
// before x-y.ext even tho - comes before .
int fCompare(LPCTSTR str1, LPCTSTR str2, BOOL ext1st/*=FALSE*/)
{
	CString bas1 = str1;
	CString bas2 = str2;
	// Since we do our own sorting when we are in "sort by extension"
	// we don't have to do stupid win32 stuff
	if (ext1st)
	{
		int rc;
		CString ext1 = GetFilesExtension(str1);
		CString ext2 = GetFilesExtension(str2);
		if ((rc = ext1.CompareNoCase(ext2)) == 0)
			 rc = bas1.CompareNoCase(bas2);
		return rc;
	}
	// In general, the Win32 sort order is this: 
	//		Non-alpha-numeric (punctuation) characters in ASCII or ANSI order 
	//		Numeric characters in numeric order 
	//		Alphabetic characters in case-insensitive alphabetic order 
	// For Win32 it is the same, with two exceptions: 
	// the hyphen or minus (-) symbol and the single-quote or apostrophe ( ' ). 
	// These two characters are ignored when sorting strings because they are 
	// allowed to be embedded in English-language words. For example, "its" and
	// "it's" and "co-op" and "coop." The presence of these characters embedded
	// in words causes certain searches to break incorrectly, so they are changed
	// to be treated just like other diacritical marks embedded in text strings;
	// that is, they're ignored.  These rather remarkable words are from
	// ms-help://MS.VSCC/MS.MSDNVS/dnaskdr/html/drgui49.htm and
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnaskdr/html/drgui49.asp
	bas1.Remove(_T('\''));
	bas1.Remove(_T('-'));
	bas2.Remove(_T('\''));
	bas2.Remove(_T('-'));
	return bas1.CompareNoCase(bas2);
}

/**********************************************************************/

void CopyTextToClipboard(LPCTSTR txt)
{
	if( lstrlen(txt) > 0 )
	{
		COleDataSource *pSource= new COleDataSource();
		HGLOBAL hText= ::GlobalAlloc(GMEM_SHARE, (lstrlen(txt)+1)*sizeof(TCHAR));
		LPTSTR pStr= (LPTSTR) ::GlobalLock( hText );
		lstrcpy( pStr, txt );
		::GlobalUnlock( hText );
	#ifdef UNICODE
		pSource->CacheGlobalData( CF_UNICODETEXT, hText );
	#else
		pSource->CacheGlobalData( CF_TEXT, hText );
	#endif
		pSource->SetClipboard();
	}
}

/**********************************************************************/

void CP4winApp::OnUpdateNewWindow(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->IsModlessUp() ? FALSE : !SERVER_BUSY());
}

void CP4winApp::OnNewWindow() 
{
	if (MainFrame()->IsModlessUp())
		return;

	CNewWindowDlg newWindowDlg;
	newWindowDlg.DoModal();
}


void CP4winApp::GetFileType(const CString &itemStr, int &BaseType, int &StoreType,
							BOOL &TypeK, BOOL &TypeW, BOOL &TypeX, BOOL &TypeO,
							BOOL &TypeM, BOOL &TypeL, BOOL &TypeS, DWORD_PTR &NbrRevs, BOOL &Unknown)
{
	TypeS = FALSE;

	// shortcut for file not in depot
	if(itemStr.Find(_T(" <")) == -1)
	{
		BaseType = -1;
		StoreType = -1;
		Unknown = FALSE;
		return;
	}

	int		plus;
	TCHAR	c;

	if (itemStr.Find(_T(" <text")) != -1)
		BaseType = 0;
	else if (itemStr.Find(_T(" <binary")) != -1)
		BaseType = 1;
	else if (itemStr.Find(_T(" <symlink")) != -1)
		BaseType = 2;
	else if (itemStr.Find(_T(" <resource")) != -1)
		BaseType = 3;
	else if (itemStr.Find(_T(" <apple")) != -1)
		BaseType = 4;
	else if (itemStr.Find(_T(" <unicode")) != -1)
		BaseType = 5;
	else if (itemStr.Find(_T(" <utf16")) != -1)
		BaseType = 6;

	StoreType = 0;
	Unknown = FALSE;

	if ((plus = itemStr.Find(_T("+"), itemStr.Find(_T(" <")))) != -1)
	{
		while (((c = itemStr.GetAt(++plus)) != _T('>')) && c)
		{
			switch (c)
			{
			case _T('x'):
				TypeX = TRUE;
				break;

			case _T('w'):
				TypeW = TRUE;
				break;

			case _T('k'):
				TypeK = TRUE;
				break;

			case _T('o'):
				TypeO = TRUE;
				TypeK = TRUE;
				break;

			case _T('m'):
				TypeM = TRUE;
				break;

			case _T('l'):
				TypeL = TRUE;
				break;

			case _T('C'):
				StoreType = 1;
				break;

			case _T('D'):
				StoreType = 2;
				break;

			case _T('F'):
				StoreType = 3;
				break;

			case _T('S'):
				TypeS = TRUE;
				if (_istdigit(itemStr.GetAt(plus+1)))
				{
					CString str = itemStr.Mid(plus+1);
					NbrRevs = static_cast<DWORD_PTR>(_tstof(str));
					if (NbrRevs < 1)
						NbrRevs = 1;
					while (_istdigit(itemStr.GetAt(++plus)))
						;
					plus--;
				}
				else
					NbrRevs = 1;
				break;

			default:
				Unknown = TRUE;
				break;
			}
		}
	}
	else if (itemStr.Find(_T(" <xtext>")) != -1)
	{
		BaseType = 0;
		TypeX = TRUE;
	}
	else if (itemStr.Find(_T(" <ktext>")) != -1)
	{
		BaseType = 0;
		TypeK = TRUE;
	}
	else if (itemStr.Find(_T(" <kxtext>")) != -1)
	{
		BaseType = 0;
		TypeX = TRUE;
		TypeK = TRUE;
	}
	else if (itemStr.Find(_T(" <xbinary>")) != -1)
	{
		BaseType = 1;
		TypeX = TRUE;
	}
	else if (itemStr.Find(_T(" <ctext>")) != -1)
	{
		BaseType = 0;
		StoreType = 1;
	}
	else if (itemStr.Find(_T(" <cxtext>")) != -1)
	{
		BaseType = 0;
		StoreType = 2;
		TypeX = TRUE;
	}
	else if (itemStr.Find(_T(" <ltext>")) != -1)
	{
		BaseType = 0;
		StoreType = 3;
	}
	else if (itemStr.Find(_T(" <lxtext>")) != -1)
	{
		BaseType = 0;
		TypeS = TypeX = TRUE;
	}
	else if (itemStr.Find(_T(" <ubinary>")) != -1)
	{
		BaseType = 1;
		StoreType = 3;
	}
	else if (itemStr.Find(_T(" <tempobj>")) != -1)
	{
		BaseType = 1;
		TypeS = TypeW = TRUE;
	}
	else if ((itemStr.Find(_T(" <xtempobj>")) != -1)
		  || (itemStr.Find(_T(" <tempxobj>")) != -1))
	{
		BaseType = 1;
		TypeS = TypeW = TypeX = TRUE;
	}
	else if (itemStr.Find(_T(" <xunicode>")) != -1)
	{
		BaseType = 5;
		TypeX = TRUE;
	}
}

/*
	_________________________________________________________________
*/

LPCTSTR CP4winApp::GetClientSpecField( LPCTSTR fieldname, LPCTSTR spectext )
{
	static CString field;

	int start, end;

	field.Empty();
    CString spec= spectext;
    int lgth = spec.GetLength();

    for( start=end=0; spec[end] && end < lgth; end++)
    {
        if( spec[end] == _T('\n') )
        {
			CString line= spec.Mid( start, end-start+1 );
			start= end+1;

			if( line.Find(fieldname) == 0 )
			{
				int i;
				if ((i = line.Find(_T('\t'))) != -1)
				{
					field = line.Mid( line.Find(_T('\t')) + 1 );
				}
				else if ((i = spec.Find(_T("\n\n"), start)) != -1)
				{
					field = spec.Mid(start, i-start);
					field.TrimLeft();
				}
				field.TrimRight();
				break;
			}
		}
	}

	return field;
}

void AddToStatus(LPCTSTR txt, StatusView level, bool showDialog)
{
	if(AfxGetThread() == (CWinThread*) AfxGetApp())
		MainFrame()->AddToStatusLog(txt, level, showDialog);
	else
		TheApp()->StatusAdd(txt, level, showDialog);
}

BOOL CP4winApp::OnIdle(LONG lCount) 
{
	// if lCount is 0, this is the first call since going idle
	// therefore set a new idle flag so that the toolbar button
	// activate-or-not functions will process the selection list again
	//
	// We use += 2 to keep m_IdleCounter odd so that it will never
	// roll over and become 0 - which is our flag to indicate that
	// we are Not in the idle loop.
	//
	if (!lCount)
		m_IdleCounter += 2;
	m_IdleFlag = m_IdleCounter;

	BOOL rc = CP4GuiApp::OnIdle(lCount);

	m_IdleFlag = 0;
	return rc;
}

BOOL CP4winApp::CallP4RevisionTree(CString filepath)
{
	CString errorText;
	CString client = GET_P4REGPTR()->GetP4Client();
	CString port = GET_P4REGPTR()->GetP4Port();
	CString user = GET_P4REGPTR()->GetP4User();
	CString password= GET_P4REGPTR()->GetP4UserPassword();
	CString charset = GET_P4REGPTR()->GetP4Charset();
	CString winhandle;
	winhandle.Format(_T("%d"), MainFrame()->m_hWnd);
	CString cmd;
	cmd.Format(_T("\"tree %s\""), filepath);

	int i = 0;
	LPCTSTR argptr[10];

	if (password.GetLength() > 0)
	{
		argptr[i++] = _T("-P");
		argptr[i++] = password;
	}
	if (charset.GetLength() > 0)
	{
		argptr[i++] = _T("-C");
		argptr[i++] = charset;
	}
	if(MainFrame()->HaveP4QTree())
	{
		argptr[i++] = _T("-win");
		argptr[i++] = winhandle;
		argptr[i++] = _T("-cmd");
		argptr[i++] = cmd;
	}
	else
	{
		argptr[i++] = _T("-j");
		argptr[i++] = filepath;
	}
	while (i < 10)
		argptr[i++] = NULL;
	if (!TheApp()->RunApp(TREE_APP, RA_NOWAIT, AfxGetApp( )->m_pMainWnd->m_hWnd, 
				FALSE, NULL, errorText, 
				_T("-p"), port, _T("-c"), client, _T("-u"), user,  
				argptr[0], argptr[1], argptr[2], argptr[3], 
				argptr[4], argptr[5], argptr[6], argptr[7], argptr[8]))
	{
		AddToStatus( LoadStringResource(IDS_UNABLETORUNREVTREE), SV_WARNING );
		return FALSE;
	}
	return TRUE;
}

BOOL CP4winApp::CallP4A(CString annpath, CString logpath, int revnbr)
{
	CString errorText;
	CString rev;
	CString client = GET_P4REGPTR()->GetP4Client();
	CString port = GET_P4REGPTR()->GetP4Port();
	CString user = GET_P4REGPTR()->GetP4User();
	CString password= GET_P4REGPTR()->GetP4UserPassword();
	CString charset = GET_P4REGPTR()->GetP4Charset();
	CString winhandle;
	winhandle.Format(_T("%d"), MainFrame()->m_hWnd);

	int i = 0;
	LPCTSTR argptr[10];

	if (password.GetLength() > 0)
	{
		argptr[i++] = _T("-P");
		argptr[i++] = password;
	}

	if (charset.GetLength() > 0)
	{
		argptr[i++] = _T("-C");
		argptr[i++] = charset;
	}

	if (MainFrame()->HaveTLV())
	{
		argptr[i++] = _T("-win");
		argptr[i++] = winhandle;
		argptr[i++] = _T("-cmd");
		CString cmd;
		cmd.Format(_T("\"%s %s\""), (GET_P4REGPTR()->GetTLVIncInteg() 
								  && MainFrame()->GetP4Vver() >= 20052)
						? _T("annotate -i") : _T("annotate"), annpath);	// command passed to V
		argptr[i++] = cmd;
		while (i < 10)
			argptr[i++] = NULL;
		if (!TheApp()->RunApp(ANNOTATE_APP, RA_NOWAIT, 
					AfxGetApp( )->m_pMainWnd->m_hWnd, FALSE, NULL, errorText, 
					_T("-p"), port, _T("-c"), client, _T("-u"), user,  
					argptr[0], argptr[1], argptr[2], argptr[3], 
					argptr[4], argptr[5], argptr[6], argptr[7], argptr[8]))
		{
			AddToStatus( LoadStringResource(IDS_UNABLETORUNP4A), SV_WARNING );
			return FALSE;
		}
	}
	return TRUE;
}

// Since client roots are entered by the user,
// they can appear in all sorts of weird shapes and forms.
// So always use this routune to set m_ClientRoot
// and clean up any messes afterwards.
BOOL CP4winApp::SetClientRoot(LPCTSTR clientroot)
{
	m_ClientRoot = clientroot;

	// handle client with multiple roots 
	// by running p4 info to get the real root
	int i;
	if ((i = m_ClientRoot.FindOneOf(_T("\r\n\t"))) != -1)
	{
		BOOL b = FALSE;
		CCmd_Info cmd;
		cmd.Init( NULL, RUN_SYNC );
		if( cmd.Run( ) && !cmd.GetError() )
		{
			CP4Info const &info = cmd.GetInfo();
			if (!info.m_ClientRoot.IsEmpty( ))
			{
				m_ClientRoot = info.m_ClientRoot;
				b = TRUE;
			}
		}
		if (!b)
			m_ClientRoot = m_ClientRoot.Left(i);
	}

	m_ClientRoot.Replace(_T('/'), _T('\\'));
	m_ClientRoot.TrimLeft(_T('\"'));
	m_ClientRoot.TrimRight(_T("\"\\"));
	switch(m_ClientRoot.GetLength())
	{
	case 0:
		m_ClientRoot = _T("\\");
		break;
	case 1:
		ASSERT(0);
		return FALSE;
	case 2:
		if (m_ClientRoot.GetAt(1) == _T(':'))
			m_ClientRoot += _T('\\');
		break;
	}
	return TRUE;
}

BOOL CP4winApp::SetClientSubOpts(LPCTSTR clientSubOpts)
{
	m_ClientSubOpts = 0;

	if (clientSubOpts)
	{
		if (_tcsstr(clientSubOpts, _T("revert")))
			m_ClientSubOpts = REVERTUNCHANGED;
		else if (_tcsstr(clientSubOpts, _T("leave")))
			m_ClientSubOpts = LEAVEUNCHANGED;
		else if (_tcsstr(clientSubOpts, _T("submit")))
			m_ClientSubOpts = SUBMITUNCHANGED;

		if (m_ClientSubOpts && _tcsstr(clientSubOpts, _T("reopen")))
			m_ClientSubOpts += REOPEN_MASK;
	}

	return m_ClientSubOpts > 0;
}

BOOL CP4winApp::digestIsSame(CP4FileStats *fs, BOOL retIfNotExist/*=FALSE*/, 
							 void *clientPtr/*=NULL*/)
{
	if (fs->GetDigest().IsEmpty())	// prior to 2005.1, we don't have the digest;
		return TRUE;				// so revert to old behavior

	Error e;
	CP4Command *pcmd = NULL;
	CGuiClient *client = (CGuiClient *)clientPtr;

	if (!client)
	{
		pcmd = new CP4Command;
		client = pcmd->GetClient();
		client->SetTrans();
		client->Init(&e);
		if( e.Test() )
		{
			delete pcmd;
			return FALSE;
		}
	}

	FileSysType ft;
	if (fs->IsTextFile())
		ft = (fs->GetHeadType().Find(_T("unicode")) != -1) ? FST_UNICODE 
		   : (fs->GetHeadType().Find(_T("utf16")) != -1) ? FST_UTF16 : FST_TEXT;
	else if (fs->GetHeadType().Find(_T("apple")) != -1)
		ft = FST_APPLETEXT;
	else if (fs->GetHeadType().Find(_T("resource")) != -1)
		ft = FST_RESOURCE;
	else if (fs->GetHeadType().Find(_T("symlink")) != -1)
		ft = FST_SYMLINK;
	else
		ft = FST_BINARY;
	FileSys *f = FileSys::Create( ft );
	if( e.Test() )
	{
		if (pcmd) delete pcmd;
		return FALSE;
	}

	StrBuf clientPath;
	clientPath << CharFromCString(fs->GetFullClientPath());
	f->Set(clientPath);

	StrBuf md5;
	f->Digest(&md5, &e);
	delete f;
	if (pcmd) delete pcmd;
    if( e.Test() )
		return retIfNotExist;
	return CharToCString(md5.Value()) == fs->GetDigest();
}

BOOL CP4winApp::localDigest(CP4FileStats *fs, CString *digest, BOOL retIfNotExist/*=FALSE*/, 
							 void *clientPtr/*=NULL*/)
{
	Error e;
	CP4Command *pcmd = NULL;
	CGuiClient *client = (CGuiClient *)clientPtr;
	*digest = _T("");

	if (!client)
	{
		pcmd = new CP4Command;
		client = pcmd->GetClient();
		client->SetTrans();
		client->Init(&e);
		if( e.Test() )
		{
			delete pcmd;
			return FALSE;
		}
	}

	FileSysType ft;
	if (fs->IsTextFile())
		ft = (fs->GetHeadType().Find(_T("unicode")) != -1) ? FST_UNICODE 
		   : (fs->GetHeadType().Find(_T("utf16")) != -1) ? FST_UTF16 : FST_TEXT;
	else if (fs->GetHeadType().Find(_T("apple")) != -1)
		ft = FST_APPLETEXT;
	else if (fs->GetHeadType().Find(_T("resource")) != -1)
		ft = FST_RESOURCE;
	else if (fs->GetHeadType().Find(_T("symlink")) != -1)
		ft = FST_SYMLINK;
	else
		ft = FST_BINARY;
	FileSys *f = FileSys::Create( ft );
	if( e.Test() )
	{
		if (pcmd) delete pcmd;
		return FALSE;
	}

	StrBuf clientPath;
	clientPath << CharFromCString(fs->GetFullClientPath());
	f->Set(clientPath);

	StrBuf md5;
	f->Digest(&md5, &e);
	delete f;
	if (pcmd) delete pcmd;
    if( e.Test() )
		return retIfNotExist;
	*digest = CharToCString(md5.Value());
	return TRUE;
}

/********************************************************************/

int GetNbrNL(const CString *str)
{
	int i = 0;
	int n = 0;
	while ((i = str->Find(_T('\n'), i)) != -1)
	{
		n++;
		i++;
	}
	return n;
}

CImageList * CP4winApp::GetImageList() 
{ 
	return m_viewImageList; 
}

void CP4winApp::OnSysColorChange()
{
	// Make sure image lists gets a new background color
	m_viewImageList->OnSysColorChange(::GetSysColor(COLOR_WINDOW));
	m_toolBarImageList->OnSysColorChange(::GetSysColor(COLOR_3DFACE));
}

// Use the shell to display a "Choose Directory" dialog box for the user.
CString CP4winApp::BrowseForFolder(HWND hWnd, LPCTSTR startat, LPCTSTR lpszTitle, UINT nFlags)
{
	startingfolder = startat;
	CString strResult = _T("");

	LPMALLOC lpMalloc;  // pointer to IMalloc
	if (::SHGetMalloc(&lpMalloc) != NOERROR)
		return strResult; // failed to get allocator

	TCHAR szDisplayName[_MAX_PATH];
	TCHAR szBuffer[_MAX_PATH];
	BROWSEINFO browseInfo;
	browseInfo.hwndOwner = hWnd;
	browseInfo.pidlRoot = NULL; // set root at Desktop
	browseInfo.pszDisplayName = szDisplayName;
	browseInfo.lpszTitle = lpszTitle;   // passed in
	browseInfo.ulFlags = nFlags;   // also passed in
	browseInfo.lpfn = BrowseCallbackProc;      // callback func
	browseInfo.lParam = 0;      // not used
	LPITEMIDLIST lpItemIDList;

	if ((lpItemIDList = ::SHBrowseForFolder(&browseInfo)) != NULL)
	{
		// Get the path of the selected folder from the item ID list.
		if (::SHGetPathFromIDList(lpItemIDList, szBuffer))
		{
			// At this point, szBuffer contains the path the user chose.
			if (szBuffer[0] == '\0')
			{
				// SHGetPathFromIDList failed, or
				// SHBrowseForFolder failed.
				AfxMessageBox(IDS_FAILED_GET_DIRECTORY, MB_ICONSTOP|MB_OK);
			}
			else
			{
				// We have a path in szBuffer - Prepare to return it.
				strResult = szBuffer;
			}
		}
		else
		{
			// The thing referred to by lpItemIDList 
			// might not have been a file system object.
			// For whatever reason, SHGetPathFromIDList
			// didn't work!
			AfxMessageBox(IDS_FAILED_GET_DIRECTORY, MB_ICONSTOP|MB_OK);
		}
	}
	// cleanup
	lpMalloc->Free(lpItemIDList);
	lpMalloc->Release();      
	return strResult;
}

INT CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData) 
{
	switch(uMsg) 
	{
	case BFFM_INITIALIZED:
		// WParam is TRUE since we are passing a path.
		// It would be FALSE if we were passing a pidl.
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)startingfolder.GetBuffer());
		break;
	}
	return 0;
}

CString DemanglePath(LPCTSTR path)
{
	// expand short filenames to long filenames
	StrBuf demangled;
	NtDemanglePath(const_cast<char*>((const char *)CharFromCString(path)), &demangled);
	return CharToCString(demangled.Text());
}

CString FormatError(Error *e, int flags)
{
	StrBuf buf;
	e->Fmt( &buf, flags ) ;

	return CharToCString(buf.Text());
}

int FindNoCase(CString str, CString substr, int offset/*=0*/)
{
	str.MakeLower();
	substr.MakeLower();
	return str.Find(substr, offset);
}
