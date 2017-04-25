//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Class CP4Command - an async wrapper for P4 commands

#include "stdafx.h"
#define TRACE_HERE
#include "P4Win.h"
#include "p4command.h"
#include "getpwddlg.h"
#include "mainfrm.h"
#include "GuiClientUser.h"
#include "Cmd_Login.h"
#include "md5.h"

#include <process.h>

const CString g_fPassword = _T("-P ");

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// MultiProcessorSleep reg setting: 0==off; ODD == GUI sleep; > EVEN == worker sleep
static int m_MultiProcessorSleep = 0;

//		Prototype for taskthread
//
UINT TaskThread( LPVOID pParam );

int P4KeepAlive::IsAlive()
{
	if (global_cancel)
	{
		global_cancel = 0; 
		return 0;
	} 
	else 
		return 1;
}

IMPLEMENT_DYNCREATE(CP4Command, CObject)

CP4Command::CP4Command(CGuiClient *client /* =NULL */) : m_pClient(client)
{
	m_pStrListIn= NULL;
	m_UsedTagged=FALSE;
	m_RanInit=FALSE;
	m_ClosedConn=TRUE;
    m_PWD_DlgCancelled=FALSE;
    m_ServerKey=0;
    m_HaveServerLock = FALSE;
	m_HitMaxFileSeeks= FALSE;
	m_RedoOpenedFilter=FALSE;
	m_FatalError = m_FatalErrorCleared = m_TriggerError = m_IgnorePermissionErrs = FALSE;

	if(m_pClient != NULL)
	{
		m_IsChildTask=TRUE;
	}
	else
	{
		// There is no longer any need to call enviro.Reload(); enviro is no longer a global variable

		m_IsChildTask=FALSE;
        m_pClient = new CGuiClient();
		ASSERT ( m_pClient ); 
	}
			
    // set the array to grow by MAX_P4ARGS, but don't actually set size
    m_args.SetSize(0, MAX_P4ARGS);
    m_argsA.SetSize(0, MAX_P4ARGS);

	m_TaskName=_T("Command base class");
	m_Function=_T("Function unassigned");
	m_pTaskThread = NULL;
	m_ReplyWnd=NULL;

	m_MultiProcessorSleep = GET_P4REGPTR()->GetMultiProcessorSleep();
	if (m_MultiProcessorSleep & 0x01)
		m_MultiProcessorSleep = 0 - m_MultiProcessorSleep;
}

CP4Command::~CP4Command()
{
	Error e;
	CloseConn(&e);
	
    // delete ANSI arg strings
    for(int i = 0; i < m_argsA.GetSize(); i++)
        delete m_argsA.GetAt(i);

	// Make sure the caption bar gets updated if the user
	// changed a setting
	if( m_Asynchronous )
	{
		// can't use MainFrame()-> construct
		// because mainfram might have closed.
		CMainFrame * mainWnd = MainFrame();
		if (mainWnd)
			mainWnd->UpdateCaption(!(m_FatalError || m_FatalErrorCleared));
	}
}


void CP4Command::CloseConn(Error *e)
{
    if(!m_ClosedConn)
    {
        // we're through with the clientuser, let it point back to parent command, if any
        m_pClient->PopCommandPtr(this);

	    if(!m_IsChildTask)
	    {
		    if( m_RanInit )
		    {
			    try
			    {
				    m_pClient->Final(e);
					if (e->Test())
					{
						CString msg= FormatError(e);
						if (e->IsFatal())
						{
							if (msg.Find(_T("TCP receive interrupted by client")) != -1)
							{
								m_FatalError = TRUE;
								msg = LoadStringResource(IDS_INTERRUPTEDBYCLIENT);
							}
							TheApp()->StatusAdd(msg, SV_ERROR);
						}
						XTRACE(msg);
					}
			    }
			    catch(...)
			    {
				    // Probably called Final for a client
				    // that never was connected
				    ASSERT(0);
			    }	
		    }
            delete m_pClient;
			m_pClient = 0;
	    }
	    m_ClosedConn=TRUE;
    }
	else if (!m_IsChildTask && m_pClient)
		delete m_pClient;
    XTRACE(_T("Task %s Connection Closed\n"), GetTaskName( ));
}


BOOL CP4Command::Init(HWND replyWnd, BOOL asynch, BOOL holdLock/*=FALSE*/, int key/*=0*/)
{
    // We need a reply window
	ASSERT( m_pClient != NULL || IsWindow(replyWnd) );

   	m_ReplyWnd= replyWnd;
	m_Asynchronous= asynch;
    	
    if( key != 0 )
    {
        m_ServerKey= key;
        m_HaveServerLock= TRUE;
    }

    m_HoldServerLock=holdLock;

    XTRACE(_T("Task %s Initialized\n"), GetTaskName( ));
	return TRUE;
}

BOOL CP4Command::Run()
{
    if(!m_IsChildTask)
    {
        //  In general, no command can start unless SERVER_BUSY() is false
        //  or the command carries the key in m_pSingleLock.  The exception is:
        //
        //  IsQueueable() and we are are running with m_Asynchronous
        //  This means the command is capable of carrying all relevant context 
        //  information and can afford to sit in a queue after a task thread
        //  is spawned.
        

		if( !m_HaveServerLock && SERVER_BUSY() )
		{
            if( !( IsQueueable() && m_Asynchronous ) )
		    {
                XTRACE(_T("Run() bailing out: %s\n"), m_TaskName);
			    return FALSE;
		    }
		}
	}
    
	if(m_Asynchronous)
	{
		ASSERT(!m_IsChildTask);
        if( !IsQueueable() && SERVER_BUSY() && !m_HaveServerLock )
   	    {
            ASSERT(0);
            CString bloodInUrine;
 		    bloodInUrine.FormatMessage( IDS_P4WIN_UNRECOVERABLE_ERROR__COMMAND_IS_s, 
                                    m_TaskName);
 		    AfxMessageBox( bloodInUrine, MB_ICONSTOP );
            // this is not excessive, since we will likely GPF if we try
            // to continue, and we'll never know why we gpf'd
 		    ExitProcess(1);  
   	    }
        if( m_HaveServerLock )
            AsyncExecCommand();
        else
        {
            XTRACE(_T("Queuing Task: %s\n"), GetTaskName( ));
		    QUEUE_COMMAND(this);
        }
	}
	else
	{
        // Dont try to lock the server.  If its the primary thread,
        // we wont be starting any other commands.  If we are a
        // child command under another async command, it already
        // took out a lock
        XTRACE(_T("Task %s Running Synchronous\n"), GetTaskName( ));
		ExecCommand();
        XTRACE(_T("Task %s Completed Synchronous\n"), GetTaskName( ));
	}
	
	return TRUE;
}



////////////////////////////////////////////////////////
// P4 argument collection

int CP4Command::AddArg( LPCTSTR arg )
{
    ASSERT(arg != NULL);
 	ASSERT( m_args.GetSize() < MAX_P4ARGS );

    m_args.Add(arg);
    return m_args.GetSize();
}

int CP4Command::AddArg( int arg )
{
 	ASSERT( m_args.GetSize() < MAX_P4ARGS );

    CString sArg;
    sArg.Format(_T("%d"),arg);

    m_args.Add(sArg);
    return m_args.GetSize();
}

void CP4Command::ClearArgs( int baseArgs)
{
    if(m_args.GetSize() > baseArgs)
        m_args.RemoveAt(baseArgs, m_args.GetSize() - baseArgs);
}

BOOL CP4Command::NextListArgs()
{
	ClearArgs(m_BaseArgs);  // Clear all but the base args

    ASSERT(m_posStrListIn != NULL);
	
	// Pull another 10 files off the list
	for(int i=0; m_posStrListIn != NULL && i<20; i++)
		AddArg(m_pStrListIn->GetNext(m_posStrListIn));

	// Caller knows not to call again when the list is empty
	if(m_posStrListIn == NULL)
		m_pStrListIn->RemoveAll();

	return FALSE;	// if we returned TRUE, it would immediately terminate command
}


/*
	_________________________________________________________________

	It takes no less than three functions to neatly run commands asynchronously:

	1) AsyncExecCommand() spawns the worker thread and returns immediately

	2) TaskThread(), a non-member function, launders the execution to member 
		function ExecCommand().  This intermediate step is required because the 
		"this" pointer gets in the way of sending a member function pointer 
		to AfxBeginThread()
		
	3) ExecCommand() actually does the work, under an independent thread, but 
		with full member function access to class property
	_________________________________________________________________
*/

void CP4Command::AsyncExecCommand()
{
    // Set priority below normal to avoid gui freeze up
	m_pTaskThread=AfxBeginThread(TaskThread, (LPVOID) this,
				THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL);
	m_pTaskThread->m_bAutoDelete=TRUE;   // Tinker w/ priority here if reqd
	m_pTaskThread->ResumeThread();
	if (m_MultiProcessorSleep < 0)
		Sleep(0 - m_MultiProcessorSleep);
}


BOOL CP4Command::InitConnection()
{
    XTRACE(_T("Task %s Initializing Connection\n"), GetTaskName( ));
    ASSERT(m_ClosedConn == TRUE);
	m_pClient->PushCommandPtr(this);
	m_ClosedConn=FALSE;
	if(m_IsChildTask)
	{
		m_pClient->SetVersion(((CP4winApp *) AfxGetApp())->m_version);
		m_pClient->SetVar( "prog", "P4Win");
		return TRUE;
	}
	else
	{
		Error e;
		e.Clear();
        if(m_UsedTagged)
            m_pClient->UseTaggedProtocol();
        else
            m_pClient->UseSpecdefsProtocol();
       
		m_pClient->Init(&e);
		if(!e.Test())
		{
			m_RanInit=TRUE;

			// Allow the user to Cancel the command if desired
			global_cancel = 0;		// don't want a stale value!
			m_pClient->SetBreak( &m_cb );

			// Notify that we are p4win
			m_pClient->SetVersion(((CP4winApp *) AfxGetApp())->m_version);
			m_pClient->SetVar( "prog", "P4Win");

			// Record the permanent client if we havent already, then
			// set the client to the active client
            //
			CString client= m_pClient->GetClient().Text();
			if( client.Compare( GET_P4REGPTR()->GetP4Client(TRUE)) != 0)
				GET_P4REGPTR()->SetP4Client( client, FALSE, TRUE, FALSE );
	
            client= GET_P4REGPTR()->GetP4Client();
			m_pClient->SetClient( client);

   			// Record the permanent user if we havent already, then
			// set the user to the active user
			// note: user must be set before calling m_pClient->GetPassword().Text();
			//
			CString user= CharToCString(m_pClient->GetUser().Text());
			if( user.Compare( GET_P4REGPTR()->GetP4User(TRUE)) != 0)
				GET_P4REGPTR()->SetP4User( user, FALSE, TRUE, FALSE );
			
            user= GET_P4REGPTR()->GetP4User();
			m_pClient->SetUser(user);

			// Record the permanent password if we havent already, then
			// set the password to the active password
            //
            CString password = m_pClient->GetPassword().Text();
			if( password.Compare( GET_P4REGPTR()->GetP4UserPassword(TRUE)) != 0)
			{	// Note that a Perm password is actually set only if it has not yet been set
				// this is because we no longer write passwords to the Registry (3rd arg).
				GET_P4REGPTR()->SetP4Password( password, FALSE, TRUE, FALSE );	// set Perm
			}
			
			int lev;
			if ((lev = GET_SERVERLEVEL()) >= 18)
			{
  				m_pClient->SetPassword( _T("") );	// use the ticket for 2004.2 and later
			}
			else if (!lev)	// must be a new port
			{
				GET_P4REGPTR()->SetP4Password( password, TRUE, FALSE, FALSE );	// also set Temp
			}
			else
			{
	            password= GET_P4REGPTR()->GetP4UserPassword();
  				m_pClient->SetPassword( password );
			}

			// Record the hostname
			CString hostname= m_pClient->GetHost().Text();
			GET_P4REGPTR()->SetHostname(hostname);
		}
		else
		{
			m_ErrorTxt= RemoveTabs( FormatError(&e) );
			m_FatalError=TRUE;
            // Sometimes a blatantly wrong port results in the error msg being fired
            // back too fast for cmainframe to receive it, so pause for a few millisecs
            Sleep(500);
		}
		return !e.Test();
	}
}

void CP4Command::ExecCommand()
{
	BOOL done=FALSE;
	m_FatalError=FALSE;

	// Initialize connection
	if(!InitConnection())
	{
		TheApp()->StatusAdd(m_ErrorTxt, SV_ERROR);
		done=TRUE;
	}
	
	// Prerequisite commands run here
	if(!done)
		PreProcess(done);

	// Loop to enable list processing
	while(!done)
	{
		// Check for possible abort request
		if(APP_ABORTING())
		{
			ReleaseServerLock();
			ExitThread(0);
		}

		// Clear any error
		m_ErrorTxt.Empty(); 
		m_FatalError=m_SyntaxError=FALSE;
        {
            // a bit of ugliness in order to provide SetArgv with
            // 8-bit strings...

            int i;
            for(i = 0; i < m_argsA.GetSize(); i++)
            {
                delete m_argsA.GetAt(i);
            }
            m_argsA.SetSize(m_args.GetSize());
            for(i = 0; i < m_args.GetSize(); i++)
            {
				CharString cs = CharFromCString(m_args[i]);
                char *argA = new char[strlen(cs)+1];
                strcpy(argA, cs);
                m_argsA.SetAt(i, argA);
            }
        }
		m_Function = m_args[0];

		// Show the command in the output pane if the user wants it
		if ( GET_P4REGPTR()->ShowCommandTrace( ) )
			TheApp()->StatusAdd( CString ( "Executing " ) + GetP4Command( ) );  

        // Run the command - run it in a loop so it is restartable
		BOOL restart;	// if true, we need to loop back and try again
		do
		{
			restart = m_RetryUnicodeMode = false;
			m_pClient->SetArgv( m_args.GetSize() - 1, m_argsA.GetData() + 1 );
			m_pClient->Run( CharFromCString(m_Function) );
#ifdef UNICODE
			if(m_RetryUnicodeMode)
			{
				m_pClient->SetTrans();
				restart = true;
			}
			else
#endif
			if (PWDRequired() && (GET_PWD_ERROR() 
							   || GET_NOPWD_SET() || GET_PWDNOTALLOW()))	// Password Error?
			{
				CString password;
				BOOL permanent = FALSE;
				BOOL need2login= FALSE;

				if (GET_PWD_ERROR())
				{
					while (!MainFrameCWnd)			// wait until MainFrame is intitialzed
						Sleep(100);

					CGetPwdDlg dlg(MainFrameCWnd);	// force Pswd dialog to be a child of MainFrame

					if(dlg.DoModal( ) == IDCANCEL)			// popup the Password dialog
					{
						m_PWD_DlgCancelled = TRUE;			// they canceled
						// Clear the error so we don't ask unnecessarily if they switch users next
						SET_PWD_ERROR(FALSE);
						break;								// break out of the loop
					}
					password  = dlg.GetPassword();	// get the clear-text password
					if (GET_SECURITYLEVEL() >= 2)	// if high security, forget any password
					{
						GET_P4REGPTR()->SetP4Password(_T(""), TRUE, TRUE, TRUE);
						SetEnvironmentVariable(_T("P4PASSWD"), NULL);
					}
					else
					{
						permanent = dlg.IsWriteToRegistry();
						// if not perm & is a 2004.2+ server, we have to clear any reg entry
						if (!permanent && GET_SERVERLEVEL() >= 18)
						{
							GET_P4REGPTR()->SetP4Password(_T(""), TRUE, TRUE, TRUE);
							SetEnvironmentVariable(_T("P4PASSWD"), NULL);
						}
					}
				}
				else if (GET_PWDNOTALLOW())
				{
					SET_PWDNOTALLOW(FALSE);
					password  = GET_P4REGPTR()->GetP4UserPassword();
					if (GET_SECURITYLEVEL() >= 2)	// if high security, forget any password
						GET_P4REGPTR()->SetP4Password(_T(""), TRUE, FALSE, FALSE);
				}
				else	// we need to login using the password they set
				{
					SET_NOPWD_SET(FALSE);					// clear the error
					password = GET_P4REGPTR()->GetP4UserPassword();
					need2login = TRUE;
					if (GET_SECURITYLEVEL() >= 2)	// if high security, forget the password
						GET_P4REGPTR()->SetP4Password(_T(""), TRUE, FALSE, FALSE);
				}

				int err = 0;

				if (need2login || GET_SERVERLEVEL() >= 18)	// 2004.1 server or later?
				{
					BOOL b = false;
					CCmd_Login *pCmd = new CCmd_Login;				// run p4 login
					CString portStr = m_pClient->GetPort().Text();	// get current port
					pCmd->GetClient()->SetPort(portStr);			// run login against cur port
					CString userStr = m_pClient->GetUser().Text();	// get current user
					pCmd->GetClient()->SetUser(userStr);			// run login against cur user
					// In certain cases we have to temporialy 'unbusy' the server
					if (!m_ServerKey && !m_Asynchronous && SERVER_BUSY())
					{
						((CP4winApp *) AfxGetApp())->m_CS.ClearServerBusy();
						b = true;						// remeber we 'unbusied' the server
					}
					pCmd->Init(NULL, RUN_SYNC, (m_ServerKey ? HOLD_LOCK : LOSE_LOCK), m_ServerKey);
					pCmd->Run(password);				// run the login command
					err = pCmd->GetError();
					delete pCmd;
					if (b)				// if we 'unbusied' the server, mark it busy again
						((CP4winApp *) AfxGetApp())->m_CS.SetServerBusy();
					m_pClient->SetPassword(_T(""));		// clear any old ticket or password
				}
				else									// 2003.2 server or earlier
				{
					MD5 md5;							// hash the clear-text
					StrBuf foo;
					foo.Set(CharFromCString(password));
					StrPtr *sptr;
					sptr = &foo;
					md5.Update(*sptr);
					md5.Final(foo);
					password = CharToCString(foo.Text());
					GET_P4REGPTR()->SetP4Password( password, TRUE, permanent, permanent );
					m_pClient->SetPassword(password); // finally add the pswd to the client obj
				}
				if (!err)	// were there any problems (i.e. did p4 login run ok?)
				{
					SET_PWD_ERROR(FALSE);			// clear the error indicators
					SET_NOPWD_SET(FALSE);
					SET_PWDNOTALLOW(FALSE);
					m_FatalError = false;
					restart = true;					// and restart the original command
					if ( GET_P4REGPTR()->ShowCommandTrace( ) )
						TheApp()->StatusAdd( CString ( "Re-executing " ) + GetP4Command( ) );  
				}
			}
		} while(restart);

		if( m_FatalError )
			done = TRUE;
		else
			ProcessResults(done);
	}
   
	// Follow-up commands run here
	if(!m_FatalError)
		PostProcess();

	// Make sure server status gets cleared BEFORE the interface thread
	// can act on a completion message
	if(!m_IsChildTask)
	{
		Error e;
		// Clear server busy status
        if(m_Asynchronous && !m_HoldServerLock)
		    ReleaseServerLock();

        // Clear any password error if the PWD worked
        if(!m_FatalError && PWDRequired())
		{
            SET_PWD_ERROR(FALSE);
			SET_NOPWD_SET(FALSE);
			SET_PWDNOTALLOW(FALSE);
			if (GET_SERVERLEVEL() >= 18)	// 2004.1 server or later?
			{								// clear password to force ticket use
				GET_P4REGPTR()->SetP4Password( _T(""), FALSE, TRUE, FALSE );	// set Prem
				GET_P4REGPTR()->SetP4Password( _T(""), TRUE, FALSE, FALSE );	// also set Temp
			}
		}

		// Make sure the connection closed, or NT3.51 will bite it when
		// next command is init'ed
		CloseConn(&e);
		if (e.Test() && !e.IsFatal())	// Fatals are taken care of in CloseConn
		{
			CString msg = FormatError(&e);
			if (msg.Find(_T("WSAECONNRESET")) != -1)	// WSAECONNRESETs are fatal
				m_FatalError = TRUE;
			TheApp()->StatusAdd(msg, m_FatalError ? SV_ERROR : SV_WARNING);
		}

		// Finally, post back to ui thread
		if(m_ReplyWnd != NULL)
			::PostMessage( m_ReplyWnd, m_ReplyMsg, (WPARAM) this, 0);
	}
}


/*
	_________________________________________________________________

	At this point, the command is ready to run, so blast ahead.  The
    control over what command can run or be queued is in the Run()
    member function.  See CP4CommandStatus for queue management.  At
    present, only 3 commands are queueable (and have IsQueueable() override
    functions).  Those commands can sit in queue for some time, and
    should not encounter errors upon return. All relevant context 
    information is carried by a queueable command.

    For a command sequence that must not be interruptible, call the 
    first command's Init() with HOLD_LOCK.  When that command returns,
    use GetServerKey() to grab the key, and pass that key into the
    next command in the sequence via Init().  The last command should
    be run with LOSE_KEY in its Init(), or alternatively ReleaseServerLock()
    can be called to drop the lock and re-enable the queue.
    _________________________________________________________________
*/

UINT TaskThread( LPVOID pParam )
{
	CP4Command *pCmd = ( CP4Command * ) pParam;
    {
        // pCmd may be deleted by the time ExecCommand returns
        // so we can't be using it afterwards.
#ifdef _DEBUG
        // to aid in debugging prematurely deleted commands
        // this string must be cleaned up before calling AfxEndThread
        // or a memory leak will result, hence the extra curly braces.
        CString taskName(pCmd->GetTaskName());
        
        XTRACE(_T("Async Task: %s Beginning\n"), taskName);
#endif
       	if (m_MultiProcessorSleep > 0)
			Sleep(m_MultiProcessorSleep);
		pCmd->ExecCommand( );
#ifdef _DEBUG
        XTRACE(_T("Async Task: %s Complete\n"), taskName);
#endif
    }
    AfxEndThread( 0 );
	return 0;
}


///////////////////////////////////////
// Default handlers for server output

void CP4Command::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	// Check for possible abort request
	if(APP_ABORTING() && m_Asynchronous)
	{
		ReleaseServerLock();
		ExitThread(0);
	}

	TheApp()->StatusAdd(msg, SV_WARNING);
}

void CP4Command::OnOutputStat( StrDict *varList )
{
	// Only specific commands use this function
	ASSERT(0);
}

void CP4Command::OnOutputText(LPCTSTR data, int length)
{
	// Only specific commands use this function
	ASSERT(0);
}

void CP4Command::OnOutputError(char level, LPCTSTR errBuf, LPCTSTR errMsg)
{
	if(APP_ABORTING() && m_Asynchronous)
	{
		ReleaseServerLock();
		ExitThread(0);
	}

	//		Most if not all "errors" returned here are info messages
	//		P4_OPENED "//depot/... - file(s) not opened anywhere."
	//
	CString txt( errBuf );
	CString msg( errMsg );

	if (( txt.Find( _T("Perforce password") ) > -1 ) 
	 || ( txt.Find( _T("please login") ) > -1 ))
	{
		SET_PWD_ERROR(TRUE);
        TheApp()->StatusAdd( msg, SV_WARNING );
		m_ErrorTxt= LoadStringResource(IDS_OPERATION_CANNOT_COMPLETED_BECAUSE_BAD_PASSWORD);
        m_FatalError=TRUE;
		return;
	}
	if ( txt.Find( _T("Password not allowed at this server security level") ) > -1 ) 
	{
		SET_PWDNOTALLOW(TRUE);
		m_ErrorTxt= msg;
        m_FatalError=TRUE;
		return;
	}
	if ((txt.Find(_T("Password must be set ")) > -1) 
	 || (txt.Find(_T("server requires the password to be reset")) > -1))
	{
		m_FatalError=TRUE;
		HWND hWnd= AfxGetMainWnd()->GetSafeHwnd();
		if( hWnd != NULL )
		{
	        TheApp()->StatusAdd( m_ErrorTxt = msg, SV_WARNING );
			BOOL b = false;
			// In certain cases we have to temporialy 'unbusy' the server
			if (!m_ServerKey && !m_Asynchronous && SERVER_BUSY())
			{
				((CP4winApp *) AfxGetApp())->m_CS.ClearServerBusy();
				b = true;						// remeber we 'unbusied' the server
			}
			if (IDOK == ::SendMessage(hWnd, WM_USERPSWDDLG, (WPARAM)TRUE, (LPARAM)m_ServerKey))
			{
				m_pClient->SetPassword(GET_P4REGPTR()->GetP4UserPassword());
				SET_NOPWD_SET(TRUE);
			}
			if (b)				// if we 'unbusied' the server, mark it busy again
				((CP4winApp *) AfxGetApp())->m_CS.SetServerBusy();
			return;
		}
	}
#ifdef UNICODE
	if (txt.Find(_T("Unicode clients require a unicode enabled server.") ) > -1 )
	{
		SET_UNICODE(FALSE);
		m_RetryUnicodeMode = TRUE;
		return;
	}
	else if (txt.Find(_T("Unicode server permits only unicode enabled clients.") ) > -1 )
	{
		SET_UNICODE(TRUE);
		CString charset= GET_P4REGPTR()->GetP4Charset();
		if(charset.IsEmpty())
		{
			m_ErrorTxt= _T("Unicode server permits only unicode enabled clients.  You must set P4CHARSET to use this server");
			m_FatalError=TRUE;
			TheApp()->StatusAdd( m_ErrorTxt, SV_WARNING );  
			return;
		}
		m_RetryUnicodeMode = TRUE;
		return;
	}
#endif

	if( txt.Find( _T("Request too large") ) > -1 && txt.Find(_T("maxresults")) > -1 )
	{
		if (txt != msg)
			TheApp()->StatusAdd( msg, SV_WARNING );  

		int semicolon= txt.Find(_T(";"));
		if( semicolon > -1 )
			txt= txt.Left( semicolon );
		txt+= LoadStringResource(IDS_TO_FULLY_REFRESH_THE_DISPLAY_YOU_NEED_TO_ADJUST_MAXRESULTS);
		txt+= GET_P4REGPTR()->GetP4User();
		txt+=  LoadStringResource(IDS_quote_nl_SEE_P4WIN_HELP_FOR_INFO_REGARDING_MAXRESULTS);
		
		TheApp()->StatusAdd( txt, SV_ERROR );  
        m_FatalError=TRUE;
		return;
	}

    if (( txt.Find( _T("You don't have permission") ) > -1 )
	 ||	( txt.Find( _T("no permission for operation") ) > -1 ))
	{
		if (m_IgnorePermissionErrs)
			return;

		TheApp()->StatusAdd( msg, SV_WARNING );  
		m_FatalError=TRUE;
		return;
	}


	if(txt.Find(_T(" - over license quota")) != -1)
	{
		m_ErrorTxt=txt;			// save off the error message first
		int i = msg.Find('\n');
		int j = msg.Find('\n', i+1);
		if (i < j && i != -1)
		{
			txt = msg.Left(i+1) + msg.Mid(j);
			msg = txt;	// have to use a temp variable
		}
		TheApp()->StatusAdd(msg, SV_WARNING );
		m_FatalError=TRUE;
		return;
	}

	if(txt.Find(_T("Client template (-t) only allowed for new client")) != -1)
	{
		TheApp()->StatusAdd(LoadStringResource(IDS_CREATE_CLIENT_FROM_TEMPLATE_ONLY_ALLOWED_FOR_NEW_CLIENT), SV_WARNING );
		m_FatalError=TRUE;
		return;
	}

	if( txt.Find( _T("Must create client ")) != -1 ||
		txt.Find( _T(" - use 'client' command to create it.") ) != -1 )
	{
        PostClientError();
		m_FatalError = TRUE; 
		return;
	}
	else if ((TheApp()->m_RunClientWizOnly)
		  && (txt.Find(_T(" not opened on this client")) == -1) 
		  && (txt.Find(_T(" not in client view")) == -1))
	{
		HWND hWnd= AfxGetMainWnd()->m_hWnd;
		if( hWnd != NULL )
			::PostMessage(hWnd, WM_COMMAND, ID_APP_EXIT, 0);
	}

	if( GET_SERVERLEVEL() > 0 && GET_SERVERLEVEL() < 3 )
    {
		TheApp()->StatusAdd( LoadStringResource(IDS_ERROR_P4WIN_REQUIRES_PERFORCE_SERVER_97_3_OR_NEWER), SV_ERROR );
        m_FatalError = TRUE; 
		return;
	}
	
	if ( txt.Find( _T("Purely numeric name not allowed") ) > -1 )
		m_FatalErrorCleared=TRUE;

	//		Subclasses of CP4Command should call HandledCmdSpecificError
	//
	//		If a known error was not encountered, we still report the
	//		problem to the status window, but not with the SV_ERROR 
	//		display code.  Otherwise, if the error/warning occurrs 
	//		a few hundred times, the user has to click 'OK' a few too
	//		many times for comfort.  We also don't set m_FatalError,
	//		because there on occasion is a message sent to OutputError
	//		that is not fatal..
	//
    CString sErrBuf(errBuf);
    CString sErrMsg(errMsg);
	if( !HandledCmdSpecificError( sErrBuf, sErrMsg ))
	{
		TheApp()->StatusAdd( errMsg, SV_WARNING );  
	}
}

BOOL CP4Command::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg)
{
	return FALSE;
}

void CP4Command::OnErrorPause(LPCTSTR errBuf, Error *e)
{
	// Check for possible abort request
	if(APP_ABORTING() && m_Asynchronous)
	{
		ReleaseServerLock();
		ExitThread(0);
	}
	
	TheApp()->StatusAdd(errBuf, SV_ERROR);  
}

void CP4Command::ProcessResults(BOOL& done)
{
	// Note: for commands without input lists, this default will
	//       do absolutely nothing
	if(m_pStrListIn == NULL || m_pStrListIn->IsEmpty())
		done = TRUE;
	else if(!done)
		done = NextListArgs();
}

void CP4Command::PreProcess(BOOL& done)
{
	// Do nothing by default
}

void CP4Command::PostProcess()
{
	// Do nothing by default
}

void CP4Command::DeleteResults()
{
	// Do nothing by default
}

void CP4Command::OnInputData(StrBuf *strBuf, Error *e)
{
    // object violently by default
    ASSERT(0);
}

int CP4Command::OnResolve( ClientMerge *m, Error *e )
{
    // object violently by default
    ASSERT(0);
    return CMS_QUIT;
}

void CP4Command::OnPrompt( const StrPtr &msg, StrBuf &rsp, int noEcho, Error *e )
{
	ASSERT(0);
}

/*
	_________________________________________________________________
	
	put the client commands in the status pane, so we can debug.
	since the users can see them, substitute asterisks for the password
	if there is one.

	hey! sometimes the command is blank, and the args are 0.
	like for ostat.
	_________________________________________________________________
*/

CString CP4Command::GetP4Command( )
{
	CString msg ;
	BOOL debug= FALSE;
#ifdef _DEBUG
	debug=TRUE;
#endif

	if( !debug && m_TaskName == _T("Password") )
		msg= _T("p4 passwd");
	else
	{
		int args = m_args.GetSize() ;
		
		while ( args-- )
			msg = CString ( _T(" ") ) + m_args[ args ] + msg;
		msg = _T("p4") + msg;
	}
	return msg;
}

void CP4Command::SetServerKey(int lock)
{
    ASSERT(lock);
    m_ServerKey= lock;
    m_HaveServerLock=TRUE;
}

int CP4Command::GetServerKey()
{
    ASSERT(m_HaveServerLock);
    return m_ServerKey;
}

BOOL CP4Command::GetServerLock(DWORD timeout)
{
    ASSERT(!m_HaveServerLock);

    if(m_Asynchronous)
    {
        XTRACE(_T("Async Task: %s Attempting lock...\n"), GetTaskName());
        m_HaveServerLock=GET_SERVER_LOCK( m_ServerKey );

        #ifdef _DEBUG
            if(m_HaveServerLock)
                XTRACE(_T("Async Task: %s Got lock...\n"), GetTaskName());
            else
                XTRACE(_T("Async Task: %s Failed to lock...\n"), GetTaskName());
        #endif
    }
    else
        // What is a syncro task doing getting a lock?
        ASSERT(0);

    return m_HaveServerLock;
}

void CP4Command::ReleaseServerLock()
{
    if(m_Asynchronous)
    {
        ASSERT(m_HaveServerLock);
        XTRACE(_T("Async Task: %s Releasing lock\n"), GetTaskName());
        RELEASE_SERVER_LOCK( m_ServerKey);
        m_HaveServerLock=FALSE;
        m_ServerKey=0;
    }
//    else
        // What is a syncro task doing releasing a lock?
//        ASSERT(0);
    // since ReleaseServerLock is called by command clients that
    // can't determine if the command is syncro, can't faile here
}

void CP4Command::PostClientError()
{
    // Attempt to activate the clients pane, and let the UI know that the
    // error was due to a bad client
    HWND hWnd= AfxGetMainWnd()->m_hWnd;
    if( hWnd != NULL )
        ::PostMessage(hWnd, WM_CLIENTERROR, 0, 0);
}