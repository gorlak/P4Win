//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4Job.cpp

#include "stdafx.h"
#include "p4win.h"
#include "p4job.h"
#include "tokenstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CP4Job, CObject)


CP4Job::CP4Job()
{
	m_IdxJobDate = m_IdxUser = m_IdxStatusTxt = m_IdxDescription = -1;
	m_Initialized = m_OKbyColm = m_OKbyCode = FALSE;
}

CP4Job::~CP4Job()
{
	
}


// Eat a row of text from 'P4 jobs' that looks like:
// job000010 on 05/03/1996 by seiwald 'open of a file at non-head rev '
// job000014 on 05/03/1996 by seiwald *open* '"change" of pending change does'
// job000027 on 05/03/1996 by seiwald *suspended* 'integ opens file without adding'
//
// 5-24-99 BUGFIX
// If a custom spec is defined that doesnt include one or more of the default fields,
// a few jobs are added under that jobspec, then the jobspec edited to re-include
// the missing field, the jobs listing will show all fields for recent jobs, but
// may only show partial info for older jobs.  In an extreme case, the output of 
// 'p4 jobs' may look like:
// 
// job000010 on 05/03/1996 by seiwald 'open of a file at non-head rev '
// job000014 by seiwald *open* '"change" of pending change does'
// job000027 on 05/03/1996 *suspended* 'integ opens file without adding'
// job000028 on 05/04/1996 'integ opens file without adding'
// job000029 'integ opens file without adding'
// job000030
// job000031 by seiwald
// 
// The fix is to ignore missing fields. Note that there are contrived situations where 
// the fix may fail.  Consider an old job that has no Status but has a description 
// of '*dammit*'.  If we add the Status field back to the jobspec, the gui will think the
// old job has a status of 'dammit'.
 
BOOL CP4Job::Create(LPCTSTR jobsRow, CDWordArray *codes)
{
	CString nextToken;
    
	CTokenString str;
	str.Create(jobsRow);

    CString jobName = str.GetToken();	// Any jobname is valid
	CString jobDate;
	CString user;
	CString statusTxt = _T("closed");		// default needed for SERVERLEVEL==3
	CString description;

	// Put next token in temp
	nextToken= str.GetToken();
    if( HaveCode(codes, JOB_DATE_CODE) && !nextToken.IsEmpty() && _tcscmp(nextToken, _T("on"))==0 )
    {
	    jobDate=str.GetToken();
	    if(jobDate.Find(_T("/")) == -1)
		    { ASSERT(0); return FALSE; }
		nextToken= str.GetToken(); 
    }

    if( HaveCode(codes, JOB_USER_CODE) && !nextToken.IsEmpty() && _tcscmp(nextToken, _T("by"))==0 )
    {
	    user=str.GetToken();     // Any username is valid
		nextToken= str.GetToken(); 
    }

	// Test for the next token NOT starting with a single quote, since
	// when the job is closed and SERVERLEVEL==3 there will be no 'closed'
	// token to indicate status
    if( HaveCode(codes, JOB_STATUS_CODE) && !nextToken.IsEmpty() && nextToken[0] == _T('*') )
    {
	    statusTxt= nextToken;
		nextToken= str.GetToken(); 

        if(statusTxt == _T("*open*"))
	    {
	    	statusTxt="open";
	    }
	    else if(statusTxt == _T("*suspended*"))
	    {
		    statusTxt=_T("suspended");
	    }
	    else
        {
			// The older servers don't support custom statuses, nor do they send
			// a status value of '*closed*'
			ASSERT( GET_SERVERLEVEL() > 3 );

			if( GET_SERVERLEVEL() > 3 )
			{
				int len=statusTxt.GetLength();
				if( len > 2 && statusTxt[0]==_T('*') && statusTxt[len-1]==_T('*') )
					statusTxt=statusTxt.Mid( 1, len-2 );
			}
        }
    }

    if( HaveCode(codes, JOB_DESCRIPTION_CODE) )
    {
	    nextToken+= str.GetRemainder();
	    description=RemoveTabs(nextToken);
	
	    // Trim off the leading and trailing quotes, if any
	    if(description.GetLength() > 2)
    	{
	    	if(description[0] == _T('\'') && description[description.GetLength()-1]==_T('\'') )
		    	description= description.Mid(1, description.GetLength());
	    }
    }

	m_FieldData.Add(jobName);

	INT_PTR max = codes->GetSize();
	for (int i = 0; ++i < max; )
	{
		switch (codes->GetAt(i))
		{
		case JOB_STATUS_CODE:
			m_FieldData.SetAtGrow(i, statusTxt);
			m_IdxStatusTxt = i;
			break;
		case JOB_USER_CODE:
			m_FieldData.SetAtGrow(i, user);
			m_IdxUser = i;
			break;
		case JOB_DATE_CODE:
			m_FieldData.SetAtGrow(i, jobDate);
			m_IdxJobDate = i;
			break;
		case JOB_DESCRIPTION_CODE:
			m_FieldData.SetAtGrow(i, description);
			m_IdxDescription = i;
			break;
		}
	}
    
	m_Initialized=m_OKbyColm=TRUE;
	return TRUE;
}


// An alternate create, to make a CP4Job from an existing job 
// for use in Add Job Fix dialogbox
void CP4Job::Create(CP4Job *job)
{
	m_IdxJobDate = job->m_IdxJobDate;
	m_IdxUser = job->m_IdxUser;
	m_IdxStatusTxt = job->m_IdxStatusTxt;
	m_IdxDescription = job->m_IdxDescription;

	int i;
	for (i=0; i < job->m_FieldData.GetSize(); )
		m_FieldData.Add(job->m_FieldData.GetAt(i++));
	for (i=0; i < job->m_FieldCodes.GetSize(); )
		m_FieldCodes.Add(job->m_FieldCodes.GetAt(i++));

	m_Initialized = job->m_Initialized;
	m_OKbyColm = job->m_OKbyColm;
	m_OKbyCode = job->m_OKbyCode;
}


// An alternate create, to make a CP4Job from the spec dialog, 
// for use in a single row update to the jobview
void CP4Job::Create(CStringArray &names, CDWordArray &codes)
{
	m_FieldData.Copy(names);
	m_FieldCodes.Copy(codes);
	INT_PTR max = m_FieldCodes.GetSize();
	for (int i = 0; ++i < max; )
	{
		switch (m_FieldCodes.GetAt(i))
		{
		case JOB_STATUS_CODE:
			m_IdxStatusTxt = i;
			break;
		case JOB_USER_CODE:
			m_IdxUser = i;
			break;
		case JOB_DATE_CODE:
			m_IdxJobDate = i;
			break;
		case JOB_DESCRIPTION_CODE:
			m_IdxDescription = i;
			break;
		}
	}
	m_Initialized=m_OKbyCode=TRUE;
}

void CP4Job::ConvertToColumns(CArray<int, int> &colCodes, CStringArray &colNames, CStringArray &fieldNames)
{
	int code;
	CString txt;
	CStringArray newFieldData;
	CDWordArray  newFieldCodes;
	INT_PTR maxFlds = fieldNames.GetSize();
	INT_PTR maxCols = colNames.GetSize();
    for( int subItem=0; subItem < maxCols; subItem++ )
    {
		txt = _T("");
		code = 0;
		CString colName = colNames.GetAt(subItem);
		for (int j = -1; ++j < maxFlds; )
		{
			if (fieldNames.GetAt(j) == colName)
			{
				code = colCodes.GetAt(j);
				txt  = GetJobFieldByCode(code);
				break;
			}
		}
		newFieldData.Add(txt);
		newFieldCodes.Add(code);
	}
	m_FieldData.RemoveAll();
	m_FieldData.SetSize(0);
	m_FieldData.Copy(newFieldData);
	m_FieldCodes.RemoveAll();
	m_FieldCodes.SetSize(0);
	m_FieldCodes.Copy(newFieldCodes);

	INT_PTR max = m_FieldCodes.GetSize();
	for (int i = 0; ++i < max; )
	{
		switch (m_FieldCodes.GetAt(i))
		{
		case JOB_STATUS_CODE:
			m_IdxStatusTxt = i;
			break;
		case JOB_USER_CODE:
			m_IdxUser = i;
			break;
		case JOB_DATE_CODE:
			m_IdxJobDate = i;
			break;
		case JOB_DESCRIPTION_CODE:
			m_IdxDescription = i;
			break;
		}
	}

	m_Initialized=m_OKbyColm=TRUE;	// probably m_OKbyCode, too - not tested, tho
	m_OKbyCode=FALSE;
}

// Yet another Create, to do the task from TAGged output
//
BOOL CP4Job::Create(StrDict *varlist, CStringArray *fieldnames, CDWordArray *fieldCodes)
{
	int i;
	StrPtr *str;

	try
	{
		// Get the Fields to display in the Jobs ListView
		for(i = -1; ++i < fieldnames->GetSize(); )
		{
			CString name= fieldnames->GetAt(i);
			if (!name.IsEmpty())
			{
				str= varlist->GetVar( CharFromCString(name) );
				if (!str)
					m_FieldData.Add(_T(""));
				else
					m_FieldData.Add(CharToCString(str->Value()));
			}
			else
				m_FieldData.Add(_T(""));
		}
		m_FieldCodes.Copy(*fieldCodes);

		INT_PTR max = m_FieldCodes.GetSize();
		for (i = 0; ++i < max; )
		{
			switch (m_FieldCodes.GetAt(i))
			{
			case JOB_STATUS_CODE:
				m_IdxStatusTxt = i;
				break;
			case JOB_USER_CODE:
				m_IdxUser = i;
				break;
			case JOB_DATE_CODE:
				m_IdxJobDate = i;
				break;
			case JOB_DESCRIPTION_CODE:
				m_IdxDescription = i;
				break;
			}
		}

		m_Initialized=m_OKbyColm=TRUE;	// probably m_OKbyCode, too - not tested, tho
		return TRUE;
	}
	catch(...)
	{
		ASSERT(0);		// bad data in Tagged output will cause record to be lost
		return FALSE;
	}
}


void CP4Job::SetJobStatus(UINT status)
{
	ASSERT(m_Initialized);
	
	CString statusTxt;
	if (m_IdxStatusTxt != -1)
	{
		switch(status)
		{
		case JOB_CLOSED:
			statusTxt=_T("closed"); break;
		case JOB_OPEN:
			statusTxt=_T("open"); break;
		case JOB_SUSPENDED:
			statusTxt=_T("suspended"); break;
		default:
			ASSERT(0);
			statusTxt=_T("open");
		}
		m_FieldData.SetAt(m_IdxStatusTxt, statusTxt);
	}
	else ASSERT(0);
}

BOOL CP4Job::HaveCode( CDWordArray *codes, DWORD code )
{
	INT_PTR max = codes->GetSize();
    for( int i=-1; ++i < max; )
    {
        if( code == codes->GetAt(i) )
            return TRUE;
    }

    return FALSE;
}

LPCTSTR CP4Job::GetJobField(INT_PTR i) const
{
	ASSERT(m_OKbyColm);
	return ((i < m_FieldData.GetSize()) && (i >= 0)) ? m_FieldData.GetAt(i) : _T("???");
}

LPCTSTR CP4Job::GetJobFieldByCode(DWORD code) const
{
	ASSERT(m_OKbyCode);
	INT_PTR max = m_FieldCodes.GetSize();
	for (int i = -1; ++i < max; )
	{
		if (m_FieldCodes.GetAt(i) == code)
			return m_FieldData.GetAt(i);
	}
	return _T("???");
}
