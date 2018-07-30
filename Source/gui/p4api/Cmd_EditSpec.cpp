/*
 * Copyright 1997, 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

// Cmd_EditSpec.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_editspec.h"
#include "cmd_describe.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_EditSpec, CP4Command)

LPCTSTR CCmd_EditSpec::g_blankDesc = _T("<enter description here>");


CCmd_EditSpec::CCmd_EditSpec(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4EDITSPEC;
	m_TaskName= _T("EditSpec");
	m_ForceEdit= m_uFlag= FALSE;
	m_IsNewClient= m_IsNewUser= FALSE;
	m_IsRequestingNew= FALSE;
	m_AllowChangeSubmit= FALSE;
	m_SubmitOnlyChged= FALSE;
	m_SubmitOnlySeled= FALSE;
	m_AutoUpdateSpec= FALSE;
	m_UseLocalDefTmplate= FALSE;
	m_NewRoot= _T("");
	m_Caller= NULL;
}

/////////////////////////////////////////////////////////////////
// Spec edit support
//
// 1)	In PreProcess, run a 'describe' or 'spectype -o' command to get a copy of
//		the specification.
//
// 2)	Upon successful retrieval of the existing or template spec, return to the UI,
//		which can call this->DoSpecDlg() to sets up and invoke the CP4SpecDlg dialog.
//
// 3)	The dialog will run CCmd_SendSpec when the user hits "OK", to run a
//		'spectype -i' command, attempting to get the server to eat the new spec.
//
// 4)	If CCmd_SendSpec fails, it shows the user an error dialog, the user can then 
//		decide to re-edit or can just hit CANCEL.
//
// Note that the server's efforts to automatically re-edit the spec are ignored in
// Callbacks.cpp, since we have handled this in 4), above.
//
// Upon completion, the gui can obtain info by 1) inspecting the CP4Branch, CP4Client, etc whose
// pointer it passed in to EditSpec().  2) by calling GetDialogExitCode() to get the exit code
// for the last time the dialog was edited, and 3) by calling GetSpecText() to retrieve the 
// complete text of the spec that was sent to the server
//
////////////////////////////////////////////////////////////////

// Special overload for Change Specifications
BOOL CCmd_EditSpec::Run(long changeNum, BOOL allowSubmit, 
						BOOL force/*=FALSE*/, 
						BOOL submitOnlyChged/*=FALSE*/,
						BOOL submitOnlySeled/*=FALSE*/, 
						BOOL uFlag/*=FALSE*/)
{
	// This command must be Initialized to hold server lock,
	// so server will be available for sendspec at end of
	// the edit sequence
	ASSERT( m_HoldServerLock );

	static TCHAR buf[20];

	m_AllowChangeSubmit= allowSubmit;
	m_ForceEdit        = force;
	m_uFlag            = uFlag;
	m_SubmitOnlyChged  = submitOnlyChged;
	m_SubmitOnlySeled  = submitOnlySeled;

	m_OldChangeNum= changeNum;
	m_NewChangeNum= changeNum;

	if(changeNum == 0)
		return(Run(P4CHANGE_SPEC));
	else
	{
		_ltot(changeNum, buf, 10);
		return(Run(P4CHANGE_SPEC, buf, NULL));
	}
}

// Special overload for Client Specifications
BOOL CCmd_EditSpec::Run( int specType, LPCTSTR newName, LPCTSTR tName, CObject *item)
{
	// This command must be Initialized to hold server lock,
	// so server will be available for sendspec at end of
	// the edit sequence
	ASSERT( m_HoldServerLock );

    m_TemplateName= tName;
    return Run( specType, newName, item);
}

BOOL CCmd_EditSpec::Run(int specType, LPCTSTR itemName, CObject *item)
{
	// This command must be Initialized to hold server lock,
	// so server will be available for sendspec at end of
	// the edit sequence
	ASSERT( m_HoldServerLock );

	m_SpecType= specType;
	m_ItemName= itemName;
	m_pSpecObj= item;
	
	return CP4Command::Run();
}

void CCmd_EditSpec::PreProcess(BOOL &done)
{
    // Set up and run Describe synchronously
	CCmd_Describe cmd(m_pClient);
	cmd.Init(NULL, RUN_SYNC);

    BOOL cmdStarted;
    if(m_TemplateName.GetLength())
    {
		if( m_SpecType == P4CLIENT_SPEC )
        {
			// Monkey with p4client for this command only, by over-riding for
			// just this connection
			m_pClient->SetClient(m_ItemName);
        }

        cmdStarted=cmd.Run(m_SpecType, m_ItemName, m_TemplateName, m_ForceEdit, 0, m_uFlag);
    }
    else
        cmdStarted=cmd.Run(m_SpecType, m_ItemName, NULL, m_ForceEdit, 0, m_uFlag);

	if(cmdStarted && !cmd.GetError())
	{
	    m_SpecIn= cmd.GetDescription();
		m_SpecStr= cmd.GetSpecStr();
		done=FALSE;
    }
	else
	{
		if(CString(cmd.GetErrorText()).Find(_T(" - over license quota")) != -1)
			m_ErrorTxt= cmd.GetErrorText();
		else
	 		m_ErrorTxt= LoadStringResource(IDS_UNABLE_TO_RUN_DESCRIBE);
	   	m_FatalError=TRUE;
	   	done=TRUE;
	}

	if(m_FatalError)
	    return;
    
	done=TRUE;
}


// In order to show the description of the jobs,
// we need to fiddle with the spec as follows:
//	1) enclose each job line in double quotes (to make a single 'word')
//	2) change all double quotes within job descriptions to single quotes
//	3) change the TAB#SPACE to SPACE-SPACE to improve readability.
BOOL CCmd_EditSpec::PreprocessChgSpec()
{
	int i, j, k = 0;
	if ((j = m_SpecIn.Find(_T("\n\nFiles:\n"))) == -1)
		 j = m_SpecIn.GetLength();
	if ((i = m_SpecIn.Find(_T("\n\nJobs:\n"))) != -1) 
	{
		k = i;
		while (((i = m_SpecIn.Find(_T('\"'), i)) != -1) && (i < j))
			m_SpecIn.SetAt(i++, _T('\''));
		i = k;
		while (i < j)
		{
			if (((i = m_SpecIn.Find(_T("\n\t"), i)) != -1) && (i < j))
			{
				m_SpecIn.Insert(i += 2, _T("\""));
				j++;
			}
			else break;
			k = i;
			if (((i = m_SpecIn.Find(_T("\t# "), i)) != -1) && (i < j))
			{
				if (((k = m_SpecIn.Find(_T("ignore"), k)) != -1) && (k < i))
				{
					for (int n = 6; n--; )
						m_SpecIn.SetAt(k++, _T(' '));
					m_SpecIn.SetAt(i++, _T(' '));
					m_SpecIn.SetAt(i++, _T(' '));
				}
				else
				{
					m_SpecIn.SetAt(i++, _T(' '));
					m_SpecIn.SetAt(i++, _T('-'));
				}
			}
			else break;
			if (((i = m_SpecIn.Find(_T("\n"), i)) != -1) && (i <= j))
			{
				m_SpecIn.Insert(i++, _T("\""));
				j++;
			}
			else break;
		}
	}
	if  ((k) && ((i = m_SpecStr.Find(_T(";Jobs;")))  != -1)
			 && ((j = m_SpecStr.Find(_T(";Files;"))) != -1))
	{
		if (((i = m_SpecStr.Find(_T("words:"), i)) != -1) && (i < j))
			m_SpecStr.SetAt(i+6, _T('1'));
	}
	return TRUE;
}


// DoSpecDlg() is a public function that is called by the UI 
// after this command has executed.  So it runs in the foreground
// thread
BOOL CCmd_EditSpec::DoSpecDlg(CWnd* caller)
{
	ASSERT(m_ClosedConn);
	ASSERT(!m_FatalError);

	m_SpecSheet= new CP4SpecSheet(caller);

	m_SpecDlg.SetCallingCommand(this);
	m_SpecDlg.SetCallingWnd(m_CallingWnd = caller->m_hWnd);
	m_SpecDlgExit = IDCANCEL;
	
	switch(m_SpecType)
	{
	case P4CHANGE_SPEC:
		if (! m_SpecDlg.SetSpec(m_SpecIn, m_SpecStr, m_SpecType, m_AllowChangeSubmit) )
			break;
		m_SpecDlg.SetChangeParms((m_OldChangeNum != 0 || m_NewChangeNum != 0), 
								m_AllowChangeSubmit, m_SubmitOnlyChged,
								TRUE, m_SubmitOnlySeled, FALSE);
		if (!m_SpecSheet->Create(IDD_SPECSHEET, caller))	// display the edit/submit dialog box
		{
			m_SpecSheet->DestroyWindow();	// some error! clean up
			delete m_SpecSheet;
		}
		return TRUE;
	
	case P4CLIENT_SPEC:
		m_SpecDlg.SetClientParms(m_NewRoot, m_AutoUpdateSpec);
	case P4JOB_SPEC:
	case P4BRANCH_SPEC:
	case P4LABEL_SPEC:
	case P4USER_SPEC:
		if ( ! m_SpecDlg.SetSpec(m_SpecIn, m_SpecStr, m_SpecType, m_AllowChangeSubmit) )
			break;

		if (!m_SpecSheet->Create(IDD_SPECSHEET, caller))	// display the edit dialog box
		{
			m_SpecSheet->DestroyWindow();	// some error! clean up
			delete m_SpecSheet;
		}
		return TRUE;
				
	default:
		ASSERT(0);
	}
	delete m_SpecSheet;
	return FALSE;
}

void CCmd_EditSpec::EndSpecDlg(int exitCode)
{
	m_SpecDlgExit = exitCode;
	
	switch(m_SpecType)
	{
	case P4CHANGE_SPEC:
		if(m_SpecDlgExit == IDOK || m_SpecDlgExit == IDALTERNATE)
		{
			m_SpecOut= m_SpecDlg.GetSpec();  	// Get the spec
			m_ChangeDesc= m_SpecDlg.GetChangeDesc();
		}
		break;
	
	case P4CLIENT_SPEC:
		m_SpecDlg.SetClientParms(m_NewRoot, m_AutoUpdateSpec);
	case P4JOB_SPEC:
	case P4BRANCH_SPEC:
	case P4LABEL_SPEC:
	case P4USER_SPEC:
		if(m_SpecDlgExit == IDOK)
		{
			m_SpecOut=m_SpecDlg.GetSpec();  	// Get the spec
			m_SpecDlg.GetCP4Wrapper(m_pSpecObj);		// Get the client info
			m_SyncAfter = m_SpecDlg.IsSyncAfter();
		}
		break;
				
	default:
		CDialog *dlg = (CDialog *)m_SpecSheet;
		dlg->EndDialog(exitCode);
		return;
	}
	::PostMessage(m_CallingWnd, WM_P4ENDSPECEDIT, (WPARAM)this, m_SpecDlgExit);
	m_SpecDlg.DoCleanup();
}