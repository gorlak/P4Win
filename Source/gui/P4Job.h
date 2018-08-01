//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// P4Job.h

#ifndef __P4JOB__
#define __P4JOB__

#define JOB_NAME_CODE           101
#define JOB_STATUS_CODE         102
#define JOB_USER_CODE           103
#define JOB_DATE_CODE           104
#define JOB_DESCRIPTION_CODE    105

#define	MAX_JOBS_FIELDS			10

enum JobStatus
{
	JOB_CLOSED,
	JOB_OPEN,
	JOB_SUSPENDED
};

// Class CP4Job - a simple wrapper to parse and store info returned by 'P4 jobs'

class CP4Job : public CObject
{
public:
	CP4Job();
	DECLARE_DYNCREATE(CP4Job)
	
protected:
	int m_IdxJobDate;
	int m_IdxUser;
	int m_IdxStatusTxt;
	int m_IdxDescription;
	CStringArray m_FieldData;
	CDWordArray  m_FieldCodes;

	BOOL m_Initialized;
	BOOL m_OKbyColm;
	BOOL m_OKbyCode;

    BOOL HaveCode( CDWordArray *codes, DWORD code ); 
    	
public:
	BOOL Create(StrDict *varlist, CStringArray *fieldnames, CDWordArray *fieldCodes); // tagged output from 'p4 -Ztag jobs'
	BOOL Create(LPCTSTR jobsRow, CDWordArray *codes);	// char * as returned by 'p4 jobs'
	void Create(CStringArray &names, CDWordArray &codes);
	void ConvertToColumns(CArray<int, int> &colCodes, CStringArray &colNames, CStringArray &fieldNames);
	void Create(CP4Job *job);

	LPCTSTR GetJobName() const {ASSERT(m_Initialized); return m_FieldData.GetAt(0); }
	LPCTSTR GetJobDate() const {ASSERT(m_Initialized); return m_IdxJobDate == -1 ? _T("") : m_FieldData.GetAt(m_IdxJobDate); }
	LPCTSTR GetUser() const {ASSERT(m_Initialized); return m_IdxUser == -1 ? _T("") : m_FieldData.GetAt(m_IdxUser); }
	LPCTSTR GetStatusText() const {ASSERT(m_Initialized); return m_IdxStatusTxt == -1 ? _T("") : m_FieldData.GetAt(m_IdxStatusTxt); }
	LPCTSTR GetDescription() const {ASSERT(m_Initialized); return m_IdxDescription == -1 ? _T("") : m_FieldData.GetAt(m_IdxDescription); }

	LPCTSTR GetJobField(INT_PTR i) const;
	LPCTSTR GetJobFieldByCode(DWORD code) const;

	void SetJobStatus(UINT status);
	void SetJobName(LPCTSTR jobName) {ASSERT(m_Initialized); m_FieldData.SetAt(0, jobName); }
	~CP4Job();

};

#endif //__P4JOB__
