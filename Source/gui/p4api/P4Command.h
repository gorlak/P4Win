/*
 * Copyright 1997, 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

// P4Command.h   
//
// CP4Command is base class for GUI P4 client API command classes
//
// Constructor, client is used for command sequences:
//	CP4Command::CP4Command(CGuiClient *client=NULL); 
//							
// Initialize:
//	CP4Command::Init(HWND replyWnd, BOOL asynch);
// 
// Execution:
//	Command classes should have Run() member functions to set up argc and argv, do
//	setup for result lists, etc, then call the base function: 
//	CP4Command::Run();
//
// Parsing Server Info:
//	Command classes can over-ride the following functions.  The base class functions
//	will just spew the results to the status window:
//	CP4Command::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
//	CP4Command::OnOutputText(LPCTSTR data, int length);
//	CP4Command::OnOutputError(char level, LPCTSTR errBuf, LPCTSTR errMsg);
//	CP4Command::OnErrorPause(LPCTSTR errBuf, Error *e);
//
// Result Set:
//	Command classes should provide access functions as required to fetch results
//
// Destruction:
//	Gui can decide whether to delete result set
//	CP4Command::DeleteResults()
//

#ifndef __P4COMMAND__
#define __P4COMMAND__


#include "GuiClient.h"

#define P4DESCRIBE		1
#define P4BRANCH_SPEC	2
#define P4CHANGE_SPEC	3
#define P4CLIENT_SPEC	4
#define P4DEPOT_SPEC	5
#define P4JOB_SPEC		6
#define P4LABEL_SPEC	7
#define P4PROTECT_SPEC	8
#define P4USER_SPEC		9
#define P4DESCRIBELONG	10
#define	P4GROUP_SPEC	11


#define P4DELETE		1
#define P4EDIT			2
#define P4LOCK			3
#define P4UNLOCK		4
#define P4REVERT		5
#define P4REOPEN		6
#define P4INTEG			7
#define	P4ADD			8
#define P4VIRTREVERT	9
#define P4REVERTUNCHG	10

#define RUN_ASYNC       TRUE
#define RUN_SYNC        FALSE
#define HOLD_LOCK       TRUE
#define LOSE_LOCK       FALSE



// Command reply messagesm sent back to the gui.  These messages will ALWAYS be sent
// with a wParam that is '(WPARAM) this'.

#define	WM_P4ADD				WM_USER+200
#define WM_P4AUTORESOLVE		WM_USER+201
#define WM_P4AUTORESPREVIEW     WM_USER+202
#define WM_P4BRANCHDEL			WM_USER+203
#define WM_P4BRANCHES			WM_USER+205
#define WM_P4BRANCHINTEG		WM_USER+207
#define WM_P4BRANCHINTEGREV     WM_USER+208
#define WM_P4CHANGES			WM_USER+211    // done CCmd_Changes
#define WM_P4CHANGESUBMIT		WM_USER+213
#define WM_P4CLIENTS			WM_USER+216
#define WM_P4CLIENTSPEC			WM_USER+217
#define WM_P4CSTAT				WM_USER+218
#define WM_P4DELETE				WM_USER+219    // done CCmd_Delete
#define WM_P4EXPANDSUBDIR		WM_USER+220

#define WM_P4DESCRIBE			WM_USER+230	   // done CCmd_Describe	
#define WM_P4DIFF				WM_USER+232
#define WM_P4DIFF2				WM_USER+233
#define WM_P4DIFF2_FILE1		WM_USER+234
#define WM_P4DIFF2_FILE2		WM_USER+235
#define WM_P4EDITSPEC			WM_USER+236    // done CCmd_EditSpec
#define WM_P4ERROR				WM_USER+237
#define WM_P4FIX				WM_USER+238
#define WM_P4FIXES				WM_USER+239
#define WM_P4FSTAT				WM_USER+240
#define WM_P4GET				WM_USER+241
#define WM_P4GETWHATIF			WM_USER+242
#define WM_P4HISTORY			WM_USER+243
#define WM_P4INTEGRATE			WM_USER+244
#define WM_P4INTEGRATE2			WM_USER+245
#define WM_P4INFO				WM_USER+246
#define WM_P4JOBS				WM_USER+247
#define WM_P4JOBSPEC			WM_USER+248
#define WM_P4UNRESOLVED			WM_USER+249
#define WM_P4DIRS				WM_USER+250
#define WM_P4DEPOTS				WM_USER+251
#define WM_P4DIRSTAT            WM_USER+252
#define WM_P4FILEINFORMATION	WM_USER+253
#define	WM_P4FILES				WM_USER+254
#define	WM_P4REVERT				WM_USER+255

#define WM_P4LABELS				WM_USER+261
#define WM_P4LABELSPEC			WM_USER+262
#define WM_P4LABELSYNC			WM_USER+263
#define WM_P4LISTOPSTAT			WM_USER+264
#define WM_P4MAXCHANGE			WM_USER+265    // done CCmd_MaxChange
#define WM_P4MERGE2				WM_USER+266
#define WM_P4MERGE3				WM_USER+267
#define WM_P4OSTAT				WM_USER+268
#define WM_P4OPENED				WM_USER+270	
#define WM_P4PASSWORD			WM_USER+271	
#define WM_P4PREPBROWSE			WM_USER+273
#define WM_P4PREPEDIT			WM_USER+274
#define WM_P4PREPEDITGET		WM_USER+275
#define WM_P4REFRESH			WM_USER+276
#define WM_P4LISTOP				WM_USER+277
#define WM_P4RESOLVE			WM_USER+278
#define WM_P4SENDSPEC			WM_USER+279
#define WM_P4FILEREVERT			WM_USER+280
#define WM_P4RESOLVED			WM_USER+281
#define WM_P4DESCRIBEALT		WM_USER+282	   // alternate done CCmd_Describe
#define WM_P4INTEGCHG			WM_USER+283	   // alternate done CCmd_Describe
#define	WM_P4DIFFCHANGEEDIT		WM_USER+284	   // alternate done CCmd_Diff
#define	WM_P4RECOVER			WM_USER+285
#define	WM_THEIRFINDINDEPOT		WM_USER+286		// alternate done CCmd_AutoResolve
#define	WM_THEIRHISTORY			WM_USER+287		// alternate done CCmd_AutoResolve
#define	WM_THEIRPROPERTIES		WM_USER+288		// alternate done CCmd_AutoResolve
#define	WM_P4ENDDESCRIBE		WM_USER+289		// Posted when modeless describe dialog closed
#define	WM_P4ENDFILEINFORMATION	WM_USER+290		// Posted when modeless file info dialog closed
#define	WM_P4ENDHISTORY			WM_USER+291		// Posted when modeless rev hsotory dialog closed
#define WM_P4LOGIN				WM_USER+292
#define WM_P4TICKETS			WM_USER+293
#define	WM_P4ENDSPECEDIT		WM_USER+294		// Posted when modeless spec edit dialog closed
#define	WM_P4ENDDIFF2OUTPUT		WM_USER+295		// Posted when modeless diff2 output dialog closed
#define	WM_P4ENDFINDFILES		WM_USER+296		// Posted when modeless find files dialog closed


#define	WM_DOCUSTOMGET			WM_USER+308
#define	WM_DOINTEGRATE1			WM_USER+309
#define WM_P4UNFIX				WM_USER+310
#define WM_P4UNGET				WM_USER+311
#define WM_P4USERS				WM_USER+315
#define WM_P4USERSPEC			WM_USER+316
#define WM_P4WHERE				WM_USER+317
#define	WM_P4CHGROLLBACK		WM_USER+318
#define WM_P4CHGROLLBACKPREVIEW	WM_USER+319
#define	WM_P4REVHISTFORANNOTATE	WM_USER+320
#define WM_DOLABELDELFILES		WM_USER+321
#define WM_DOLABELSYNCCLI		WM_USER+322
#define WM_DOLABELSYNC			WM_USER+323
#define	WM_ONDODELETEFIXES		WM_USER+324
#define	WM_UPDATEHAVEREV		WM_USER+325
#define WM_P4UPPERBOUND			WM_USER+398     // Used to test command values only, not a command
#define WM_P4STATUS				WM_USER+399


// A magic number indicating the maximum number of times
// we will try to find nodes in the depot pane or changelist
// pane before throwing in the towel and just redrawing both
// windows
#define MAX_FILESEEKS 500
#define MAX_FILESTATS MAX_FILESEEKS

#define MAX_P4ARGS 30

class CGuiClientUser;

class P4KeepAlive : public KeepAlive
{
    public:
		int IsAlive();
} ;

class CP4Command : public CObject
{
    friend CGuiClientUser;

// Construction
public:
	CP4Command(CGuiClient *client=NULL);

	DECLARE_DYNCREATE(CP4Command)

	virtual ~CP4Command();

    CGuiClient * GetClient() { return m_pClient; }
// Attributes	
protected:

    ///////////////////////
    // Support for threading:
    // A pointer to the singlelock, which can be accessed by public functions
    // GetServerKey() and SetServerKey()
    int m_ServerKey;

    // Do we have a valid key?
    BOOL m_HaveServerLock;

    // Should we unlock at the end of ExecCommand() (for m_Asynchronous commands only)
    BOOL m_HoldServerLock;

    // Are we running asynchronous from the thread that called Run()?
    BOOL m_Asynchronous;

    // A pointer to our own thread if asynchronous
    CWinThread *m_pTaskThread;  

    // Are we running (always synchronously) as a child task
	BOOL m_IsChildTask;

	BOOL m_UsedTagged;
	BOOL m_RanInit;
	BOOL m_ClosedConn;
	CString m_TaskName;
	CString m_Function;
	CGuiClient *m_pClient;

	// The reply window for messages
	HWND m_ReplyWnd;
	UINT m_ReplyMsg;

	// The keep alive instance for supporting cancel
	P4KeepAlive m_cb;


	// The arg set
private:
    CStringArray m_args;
    CArray<char*,char*> m_argsA;
protected:
    INT_PTR GetArgc() const { return m_args.GetSize(); }
    LPCTSTR GetArgv(int index) const { return m_args.GetAt(index); }
	int m_BaseArgs;

	// Basic list-in and list-out support
	POSITION m_posStrListIn;
	CStringList *m_pStrListIn;
	CStringList m_StrListOut;

	// Did the output exceed the number of file swe are
	// willing to update in the tree views?
	BOOL m_HitMaxFileSeeks;

	// Was the depot filtered on open files? See mainfraime.h enum _dft for values
	int m_RedoOpenedFilter;

    // Basic context storage
    HTREEITEM m_CallerItemRef;
    CString m_CallerTextRef;

	// P4 error and return values
	BOOL m_FatalError;			// Use to track warning-level errors that blow a command sequence
	BOOL m_FatalErrorCleared;	// set if m_FatalError was cleared because it was considered minor
	BOOL m_TriggerError;		// Use to record a trigger error
	BOOL m_SyntaxError;
    BOOL m_PWD_DlgCancelled;
	BOOL m_IgnorePermissionErrs;// Set TRUE to ignore permission errors
	CString m_ErrorTxt;			// Alternate error text

	BOOL m_RetryUnicodeMode;
 	
public:
    BOOL GetServerLock(DWORD timeout);
    void ReleaseServerLock();
    int  GetServerKey( );
    void SetServerKey(int key);
    BOOL HaveServerLock() { return m_HaveServerLock; }
	BOOL HitMaxFileSeeks() { return m_HitMaxFileSeeks; }
	void SetHitMaxFileSeeks(BOOL b) { m_HitMaxFileSeeks = b; }
	int  GetRedoOpenedFilter() { return m_RedoOpenedFilter; }
	void SetRedoOpenedFilter(int i) { m_RedoOpenedFilter = i; }

    virtual BOOL Init(HWND replyWnd, BOOL asynch, BOOL holdLock=FALSE, int key=0);
	BOOL Run();
protected:
	virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
	virtual void OnOutputStat( StrDict *varList );
	virtual void OnOutputText(LPCTSTR data, int length);
	virtual void OnOutputError(char level, LPCTSTR errBuf, LPCTSTR errMsg);
	virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
	virtual void OnErrorPause(LPCTSTR errBuf, Error *e);
	virtual void DeleteResults();
    virtual BOOL PWDRequired() const { return TRUE; }
    virtual void OnInputData(StrBuf *strBuf, Error *e);
    virtual int OnResolve( ClientMerge *m, Error *e );
	virtual void OnPrompt( const StrPtr &msg, StrBuf &rsp, int noEcho, Error *e );

    // Support for queueable commands
    virtual BOOL IsQueueable() const { return FALSE; }
public:
    HTREEITEM GetItemRef() { return m_CallerItemRef; }
    LPCTSTR GetTextRef() { return m_CallerTextRef; }
    void SetItemRef(HTREEITEM item) { m_CallerItemRef=item; }
    void SetTextRef(LPCTSTR str) { m_CallerTextRef=str; }

public:
	int GetError() const { return m_FatalError; }
	int GetErrorCleared() const { return m_FatalErrorCleared; }
	void ClearError() { m_FatalError = FALSE; }
	int GetTriggerError() const { return m_TriggerError; }
	void SetTriggerError() { m_TriggerError = TRUE; }
    int PWDDlgCancelled() const { return m_PWD_DlgCancelled; }
	LPCTSTR GetErrorText() const { return m_ErrorTxt; }
	LPCTSTR GetTaskName() const { return m_TaskName; }

	HWND GetReplyWnd() const { ASSERT(IsWindow(m_ReplyWnd)); return m_ReplyWnd; }
	UINT GetReplyMsg() const { return m_ReplyMsg; }
	void SetAlternateReplyMsg( UINT msg ) { m_ReplyMsg= msg; }
protected:
	BOOL IsChildTask() const { return m_IsChildTask; }
    BOOL IsAsynchronous() const { return m_Asynchronous; }

protected:	
	int AddArg(LPCTSTR arg);
    int AddArg(int arg);
	void ClearArgs(int baseArgs=0);
	virtual BOOL NextListArgs();	// return TRUE to indicate done; FALSE to keep running
	
	BOOL InitConnection();
public:
	void CloseConn(Error *e);

public:
    void AsyncExecCommand();
	CString GetP4Command( );
	void ExecCommand();    
protected:
	// Inside ExecCommand(), after the connection to server is established:
	// Usefull for calling other commands
	virtual void PreProcess(BOOL& done);
	virtual void PostProcess();

	// Inside ExecCommand(), after invoking a server command (inside the loop)
	// Usefull for calling NextListArgs(), etc
	virtual void ProcessResults(BOOL& done);

    // tell UI that error was caused by a bad client
    void PostClientError();
};

// subclass KeepAlive to implement a customized IsAlive
// function.
	
template <class Cmd>
class CmdPtr
{
    Cmd *m_ptr;
public:
    CmdPtr(WPARAM wParam)
    {
        m_ptr = (Cmd *)wParam;
        ASSERT(m_ptr != 0);
        ASSERT(dynamic_cast<Cmd*>(m_ptr));
    }
    ~CmdPtr()
    {
        delete m_ptr;
    }
    operator Cmd* () { return m_ptr; }
    Cmd* operator -> () { return m_ptr; }
};
/////////////////////////////////////////////////////////////////////////////
#endif //__P4COMMAND__







