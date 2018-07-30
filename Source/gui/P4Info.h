//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//


// P4Info.h

#ifndef __P4INFO__
#define __P4INFO__


// Class CP4Info - a simple wrapper to parse and store info returned by 'P4 info'

class CP4Info
{

public:
	CP4Info();

	void SetPort(LPCTSTR port);
	void SetClient(LPCTSTR client);
	void SetUser(LPCTSTR user);

// Attributes
public:
	CString m_UserName;
	CString m_ClientName;
	CString m_ClientHost;
	CString m_ClientRoot;
	CString m_CurrentDirectory;
	CString m_ClientAddress;
    CString m_ClientP4Charset;
    CString m_ClientAnsiCodePage;
	CString m_ServerAddress;
	CString m_ServerRoot;
	CString m_ServerDate;
	CString m_ServerUptime;
	CString m_ServerVersion;
	CString m_ServerLicense;
	CString m_ProxyVersion;
	CString m_UnicodeMode;
	CString m_Error;
};

#endif //__P4INFO__
