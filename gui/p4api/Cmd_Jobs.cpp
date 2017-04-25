//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Jobs.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_jobs.h"
#include "p4job.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Jobs, CP4Command)


CCmd_Jobs::CCmd_Jobs(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4JOBS;
	m_TaskName= _T("Jobs");

	m_bFilter = FALSE;
}


BOOL CCmd_Jobs::Run( LPCTSTR jobStatus, BOOL bFastJobs/*=TRUE*/, 
					CStringList *viewSpec/*=NULL*/, BOOL bIncIntegs/*=FALSE*/)
{
	ClearArgs();
	AddArg(_T("jobs"));
	m_BaseArgs= AddArg(_T("-l"));
	if( GET_SERVERLEVEL() >= 6)
	{
		if (!GET_P4REGPTR()->GetFetchAllJobs() )
		{
			int count= GET_P4REGPTR()->GetFetchJobCount();
			if( count > 0 )
			{
				AddArg(_T("-m"));
				m_BaseArgs= AddArg(count);
				if( GET_SERVERLEVEL() > 7 )
					m_BaseArgs= AddArg(_T("-r"));
			}
		}
		if (bIncIntegs)
			m_BaseArgs= AddArg(_T("-i"));
	}
	if( jobStatus != NULL )
	{
		CString stat = jobStatus;

		if ( m_bFilter && !stat.IsEmpty( ) )
		{
			AddArg( _T("-e") ); 
			m_BaseArgs= AddArg( jobStatus );
		}
		else
		{
			if ( !stat.IsEmpty( ) )
			{
				if( GET_SERVERLEVEL() < 3 )
				{
					AddArg( _T("-s")  );
					m_BaseArgs= AddArg(jobStatus);
				}
				else
				{
					m_BaseArgs= AddArg( CString ( _T("-e status=") ) + jobStatus ); 
				}
			}
		}
	}
	// We may or may not have a view spec to limit the fetching of 
	// jobs, so sometimes we have an input list and sometimes not
	ASSERT(viewSpec==NULL || viewSpec->IsKindOf(RUNTIME_CLASS(CStringList)));
	if(viewSpec != NULL && viewSpec->GetCount())
	{
		m_pStrListIn= viewSpec;
		m_posStrListIn= m_pStrListIn->GetHeadPosition();
		NextListArgs();
	}

	m_UsedTagged = (!bFastJobs && m_FieldNames.GetSize() && (GET_SERVERLEVEL() >= 8)) ? TRUE : FALSE;
	return CP4Command::Run();
}


void CCmd_Jobs::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	// Parse into a CP4Job and send that back
	CP4Job *job= new CP4Job;
	if( job->Create(data, &m_FieldCodes) )
	    m_List.AddTail(job);
    else
    {
        #ifdef _DEBUG
            CString errMsg;
            errMsg.Format(_T("Job parse failed:\n%s"), data);
            TheApp()->StatusAdd(errMsg, SV_DEBUG);
        #endif
        delete job;
    }
}


void CCmd_Jobs::OnOutputStat( StrDict *varList )
{
	// Check for possible abort request
	if(APP_ABORTING())
	{
		ReleaseServerLock();
		ExitThread(0);
	}
	else
	{
        CP4Job *job= new CP4Job;
		if(job->Create(varList, &m_FieldNames, &m_FieldCodes))
		    m_List.AddTail(job);
		else
			delete job;
	}
}


/*
	_________________________________________________________________
*/

IMPLEMENT_DYNCREATE(CCmd_JobSpec, CP4Command)


CCmd_JobSpec::CCmd_JobSpec( CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4JOBSPEC;
	m_TaskName= _T("JobSpec");
	m_pSpec = NULL;
}


BOOL CCmd_JobSpec::Run( )
{
	m_pSpec = new CString;
	ASSERT ( m_pSpec );
	ClearArgs();
	AddArg( _T("jobspec") );
	AddArg( _T("-o") );

	return CP4Command::Run();
}


void CCmd_JobSpec::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	*m_pSpec =  data ;
}

void CCmd_JobSpec::PostProcess()
{
}


void CCmd_JobSpec::GetSpec( CString &spec )
{
	spec = *m_pSpec;
}
