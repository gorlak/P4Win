//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// P4Client.h

#ifndef __P4CLIENT__
#define __P4CLIENT__

// Class CP4Client - a simple wrapper to parse and store info returned by 'P4 clients'

class CP4Client : public CObject
{
public:
	CP4Client();
	DECLARE_DYNCREATE(CP4Client)
	
protected:
	CString m_ClientName;
	CString m_Owner;
	CString m_Host;
	CString m_Date;
	CString m_Root;
	CString m_Description;
	
		
	BOOL m_Initialized;
	
public:
	void Create(LPCTSTR name, LPCTSTR owner, LPCTSTR host, LPCTSTR date, LPCTSTR root, LPCTSTR desc);
	BOOL Create(LPCTSTR clientRow);
	BOOL Create(StrDict *varlist);

	LPCTSTR GetClientName() const {ASSERT(m_Initialized); return LPCTSTR(m_ClientName); }
	LPCTSTR GetOwner() const {ASSERT(m_Initialized); return LPCTSTR(m_Owner); }
	LPCTSTR GetHost() const {ASSERT(m_Initialized); return LPCTSTR(m_Host); }
	LPCTSTR GetDate() const {ASSERT(m_Initialized); return LPCTSTR(m_Date); }
	LPCTSTR GetRoot() const {ASSERT(m_Initialized); return LPCTSTR(m_Root); }
	LPCTSTR GetDescription() const {ASSERT(m_Initialized); return LPCTSTR(m_Description); }
		
	~CP4Client();

};

#endif //__P4CLIENT__
