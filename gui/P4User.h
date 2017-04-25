//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// P4User.h

#ifndef __P4USER__
#define __P4USER__

// Class CP4User - a simple wrapper to parse and store info returned by 'P4 users'

class CP4User : public CObject
{
public:
	CP4User();
	DECLARE_DYNCREATE(CP4User)
	
protected:
	CString m_UserName;
	CString m_Email;
	CString m_FullName;
	CString m_LastAccessDate;
	
	BOOL m_Initialized;
	
public:
	void Create(LPCTSTR name, LPCTSTR email, LPCTSTR fullname, LPCTSTR accessdate);
	BOOL Create(LPCTSTR userRow);

	LPCTSTR GetUserName() const {ASSERT(m_Initialized); return m_UserName; }
	LPCTSTR GetEmail() const {ASSERT(m_Initialized); return m_Email; }
	LPCTSTR GetFullName() const {ASSERT(m_Initialized); return m_FullName; }
	LPCTSTR GetLastAccess() const {ASSERT(m_Initialized); return m_LastAccessDate; }
		
	~CP4User();

};

#endif //__P4USER__


