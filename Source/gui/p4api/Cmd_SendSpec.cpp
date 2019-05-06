/*
 * Copyright 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */


// Cmd_SendSpec.cpp

#include "stdafx.h"
#include "p4win.h"
#include "Cmd_SendSpec.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_SendSpec, CP4Command)


CCmd_SendSpec::CCmd_SendSpec(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4SENDSPEC;
	m_TaskName= _T("SendSpec");
	m_NewChangeNum= 0;
}

BOOL CCmd_SendSpec::Run( int specType, LPCTSTR specText, BOOL submit, 
						 BOOL force /*=FALSE*/, BOOL reopen /*=FALSE*/, 
						 int unchangedFlag /*=0*/, BOOL uFlag/*=FALSE*/ )
{
	ASSERT( specType==P4CHANGE_SPEC || !submit );

	m_Submit= submit;
	m_ForceEdit= force;
	m_Reopen= reopen;
	m_UnchangedFlag= unchangedFlag;
	m_SpecText= specText;
    m_SpecTextSent = false;
	m_SpecType= specType;

	ClearArgs();
	
	switch (m_SpecType)
	{
	case P4BRANCH_SPEC:
		AddArg(_T("branch"));  break;
	case P4CHANGE_SPEC:
		if(submit)
			AddArg(_T("submit"));
		else
			AddArg(_T("change"));  
		break;
	case P4CLIENT_SPEC:
        AddArg(_T("client"));  break;
	case P4JOB_SPEC:
		AddArg(_T("job"));     break;
	case P4LABEL_SPEC:
		AddArg(_T("label"));   break;
	case P4USER_SPEC:
		AddArg(_T("user"));	   break;
	default:
		ASSERT(0);
		return FALSE;
	}

	if( uFlag && GET_SERVERLEVEL() >= 23 )	// 2007.2 or later?
		AddArg(_T("-u"));
	else if( force || uFlag )
		AddArg(_T("-f"));

	if (submit)
	{
		if (GET_SERVERLEVEL() >= 22)	// 2006.2 or later?
		{
			if( !force )			// has the -f already been added?
				AddArg(_T("-f"));	// if not, add it

			switch(unchangedFlag)
			{
			case 0:
				AddArg(reopen ? _T("submitunchanged+reopen") : _T("submitunchanged"));
				break;

			case 1:
				AddArg(reopen ? _T("revertunchanged+reopen") : _T("revertunchanged"));
				break;

			default:
			case 2:
				AddArg(reopen ? _T("leaveunchanged+reopen") : _T("leaveunchanged"));
				break;
			}
		}
		else
		{
			if( reopen && GET_SERVERLEVEL() >= 13)
				AddArg(_T("-r"));
			
			if( unchangedFlag==1 && GET_SERVERLEVEL() >= 21)	// 2006.1 (only)
				AddArg(_T("-R"));
		}
	}

	AddArg( _T("-i") );

	return CP4Command::Run();
}


void CCmd_SendSpec::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
    //i18n: completion message in data
	BOOL processedMessage=FALSE;

	switch(m_SpecType)
	{
	case P4BRANCH_SPEC:
		if(StrNCmp(data, _T("Branch "), 7) == 0)
		{
			TheApp()->StatusAdd(msg, SV_COMPLETION);
			processedMessage=TRUE;
		}
		break;

	case P4CLIENT_SPEC:
		if(StrNCmp(data, _T("Client "), 7) == 0)
		{
			TheApp()->StatusAdd(msg, SV_COMPLETION);
			processedMessage=TRUE;
		}
		break;

	case P4JOB_SPEC:
		if(StrNCmp(data, _T("Job "), 4) == 0)
		{
			// Extract the new job name
            LPCTSTR pRest = StrChr(data+4, _T(' '));
            if(pRest && (!StrCmp(pRest+1, _T("saved.")) || 
                         !StrCmp(pRest+1, _T("not changed."))))
			{
                CString temp(data + 4);
				temp=temp.Left(temp.Find(_T(' ')));
				m_NewJobName= temp;
			}

			TheApp()->StatusAdd(msg, SV_COMPLETION);
			processedMessage=TRUE;
		}
		break;

	case P4LABEL_SPEC:
		if(StrNCmp(data, _T("Label "), 6) == 0)
		{
			TheApp()->StatusAdd(msg, SV_COMPLETION);
			processedMessage=TRUE;
		}
		break;

	case P4USER_SPEC:
		if(StrNCmp(data, _T("User "), 5) == 0)
		{
			TheApp()->StatusAdd(msg, SV_COMPLETION);
			processedMessage=TRUE;
		}
		break;
	
	case P4CHANGE_SPEC:
		if(!m_Submit)
		{
			if(StrNCmp(data, _T("Change"), 6) ==0 )
			{
				m_NewChangeNum= _ttol(data+7);
				ASSERT(m_NewChangeNum);
				processedMessage=TRUE;
			}
			break;
		}
		else
		{
			// Typically many lines are returned - The only thing we need is the new
			// change number
            
            static TCHAR rc[] = _T("renamed change ");
            static TCHAR sc[] = _T("Submitting change ");

            if(!StrNCmp(data, _T("Change"), 6) && !StrStr(data, _T("updated")))
			{
				// Its either 'Change n1 renamed change n2 and submitted'
				//         or 'Change n1 submitted'
				m_NewChangeNum = _ttol(data+7);
                LPCTSTR pRenamed = StrStr(data+7, rc);
				if(pRenamed)
					m_NewChangeNum = _ttol(pRenamed + StrLen(rc));
				ASSERT(m_NewChangeNum);
			}
			else if(!StrNCmp(data, sc, StrLen(sc)))
            {
                // It's 'Submitting change n1'
    			m_NewChangeNum = _ttol(data+StrLen(sc));
				ASSERT(m_NewChangeNum);
            }
			else if(StrStr(data, _T(" - must get")) || StrStr(data, _T(" - must resolve")))
				TheApp()->StatusAdd(msg, SV_WARNING);
			else if(StrCmp(data, _T("Specification not corrected -- giving up.")) == 0)
				TheApp()->StatusAdd(msg, SV_WARNING);
			else
				TheApp()->StatusAdd(msg);

			processedMessage=TRUE;
			break;
		}

	default:
			ASSERT(0);
	}

	if(!processedMessage)
		CP4Command::OnOutputInfo( level, data, msg);
}


BOOL CCmd_SendSpec::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg)
{
    m_ErrorTxt.Empty();

    // Omit message about line number, if present
    CString omit= _T("\nError detected at line");

    LPCTSTR pStartOmit= StrStr(errMsg, omit);

    if( pStartOmit )
    {
        LPCTSTR pEndOmit = StrChr(pStartOmit+1, _T('\n'));

        if( pEndOmit )
            m_ErrorTxt = CString(errMsg).Left(int(pStartOmit - errMsg)) + pEndOmit;
    }
    else
        m_ErrorTxt = errMsg;

	if (StrStr(errBuf, _T("You cannot use the default branch view;")))
		m_ErrorTxt += LoadStringResource(IDS_YOUMUSTCHANGETHEVIEW);

    // replace message about submit failure, if present
    int startSubmit = m_ErrorTxt.Find(_T("Submit failed"));
    if(startSubmit > -1)
	{
		m_ErrorTxt = m_ErrorTxt.Left(startSubmit)
            + LoadStringResource(IDS_SUBMIT_FAILED_CORRECT_AND_RESUBMIT);
	}
	else if ((errBuf != errMsg) && StrStr(errBuf, _T("Submit failed")))
	{
		m_ErrorTxt.TrimRight();
		startSubmit = m_ErrorTxt.ReverseFind(_T('\n'));
		if(startSubmit > -1)
		{
			m_ErrorTxt = m_ErrorTxt.Left(startSubmit+1)
				+ LoadStringResource(IDS_SUBMIT_FAILED_CORRECT_AND_RESUBMIT);
		}
		m_ErrorTxt += _T("\n");
	}

    // Put the error into the status bar.  Let the UI thread pop the
	// error dialog so there isnt a window on-topness problem
	TheApp()->StatusAdd( m_ErrorTxt, SV_WARNING );

	m_FatalError= TRUE;
	return TRUE;
}

void CCmd_SendSpec::OnInputData(StrBuf *strBuf, Error *e)
{
    if(!m_SpecTextSent)
    {
        *strBuf << CharFromCString(m_SpecText);
        m_SpecTextSent = true;
    }
}

