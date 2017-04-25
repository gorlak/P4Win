//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// P4Fix.h

#ifndef __P4FIX__
#define __P4FIX__

// Class CP4Fix - a simple wrapper to parse and store info returned by 'P4 fixes'

class CP4Fix : public CObject
{
public:
	CP4Fix();
	DECLARE_DYNCREATE(CP4Fix)
	
protected:
	CString m_JobName;
	CString m_FixDate;
	CString m_User;
	long m_ChangeNum;

	BOOL m_Initialized;
	
public:
	BOOL Create(LPCTSTR fixRow);  // char * as returned by 'p4 fixes'
	BOOL CreateMyFix(LPCTSTR fixesRow);  // char * as returned by 'p4 fix'
	BOOL CreateFromUnfix(LPCTSTR unfixRow);
	
	LPCTSTR GetJobName() {ASSERT(m_Initialized); return m_JobName; }
	LPCTSTR GetFixDate() {ASSERT(m_Initialized); return m_FixDate; }
	LPCTSTR GetUser() {ASSERT(m_Initialized); return m_User; }
	long GetChangeNum() {ASSERT(m_Initialized); return m_ChangeNum; }
	
	~CP4Fix();

};

#endif //__P4FIX__


