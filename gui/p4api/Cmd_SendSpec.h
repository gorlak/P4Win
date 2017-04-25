/*
 * Copyright 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */


// Cmd_SendSpec.h  
//
// Use to send a spec to the server
//

#include "P4Command.h"

	

class CCmd_SendSpec : public CP4Command
{
    // Construction
public:
    CCmd_SendSpec(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_SendSpec)

    BOOL Run( int specType, LPCTSTR specText, BOOL submit, BOOL force=FALSE, BOOL reopen=FALSE, int unchangedFlag=0, BOOL uFlag=FALSE );
    LPCTSTR GetNewJobName() const { return m_NewJobName; }
    int GetNewChangeNum() const { return m_NewChangeNum; }
    LPCTSTR GetSpecText() const { return m_SpecText; }

protected:
    // For changes and jobs, a new name may be assigned by server
    int m_NewChangeNum;
    CString m_NewJobName;

    // Attributes
    BOOL m_Submit;
    BOOL m_ForceEdit;
	BOOL m_Reopen;
	int m_UnchangedFlag;
    int m_SpecType;
    CString m_SpecText;
    bool m_SpecTextSent;
		    
    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
    virtual void OnInputData(StrBuf *strBuf, Error *e);
};









