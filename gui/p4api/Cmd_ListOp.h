//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_ListOp.h   
//

#include "P4Command.h"

class CCmd_ListOp : public CP4Command
{
    // Construction
public:
    CCmd_ListOp(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_ListOp)

    // newStatus is P4EDIT, P4DELETE, P4LOCK, P4UNLOCK, P4REVERT
    BOOL Run(CStringList *files, int command, long changeNum=0, LPCTSTR newType= NULL);

    int GetCommand() const { return m_Command; }
    CStringList *GetList() { return &m_StrListOut; }
    BOOL GetOutputErrFlag() const { return m_OutputError; }
    void SetChkForSyncs( BOOL b ) { m_ChkForSyncs = b; }
    CStringList *GetSyncList() { return &m_StrListSync; }
	void SetWarnIfLocked( BOOL b ) { m_WarnIfLocked = b; }

    // Attributes	
protected:
    int m_Command;
    BOOL m_OutputError;
    BOOL m_ChkForSyncs;
	BOOL m_WarnIfLocked;
    CStringList m_StrListSync;

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
};









