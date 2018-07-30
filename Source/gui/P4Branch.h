//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// P4Branch.h

#ifndef __P4BRANCH__
#define __P4BRANCH__

// Class CP4Branch - a simple wrapper to parse and store info returned by 'P4 branchs'

class CP4Branch : public CObject
{
public:
	CP4Branch();
	DECLARE_DYNCREATE(CP4Branch)
	
protected:
	CString m_BranchName;
	CString m_Owner;
	CString m_Options;
	CString m_Date;
	CString m_Description;
	
	BOOL m_Initialized;
	
public:
	void Create(LPCTSTR name, LPCTSTR owner, LPCTSTR options, LPCTSTR date, LPCTSTR desc);
	BOOL Create(LPCTSTR branchRow);
	BOOL Create(StrDict *varlist);

	LPCTSTR GetBranchName() const {ASSERT(m_Initialized); return LPCTSTR(m_BranchName); }
	LPCTSTR GetOwner() const {ASSERT(m_Initialized); return m_Owner; }
  	LPCTSTR GetOptions() const {ASSERT(m_Initialized); return m_Options; }
	LPCTSTR GetDate() const {ASSERT(m_Initialized); return LPCTSTR(m_Date); }
	LPCTSTR GetDescription() const {ASSERT(m_Initialized); return LPCTSTR(m_Description); }
		
	~CP4Branch();

};

#endif //__P4BRANCH__


