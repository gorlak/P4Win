/*
 * Copyright 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */


// Cmd_Password.h   
//

#include "P4Command.h"

class CCmd_Password : public CP4Command
{
    // Construction
public:
    CCmd_Password(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Password)
				    
    BOOL Run(LPCTSTR oldPwd, LPCTSTR newPwd);

    // test a password string for invalid characters,
    // quote empty string, and return true if valid
    static bool PrepPassword( CString &pwd );

protected:
	CString m_OldPwd;
	CString m_NewPwd;

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
	virtual void OnPrompt( const StrPtr &msg, StrBuf &rsp, int noEcho, Error *e );
    virtual BOOL PWDRequired() const { return FALSE; }
};









