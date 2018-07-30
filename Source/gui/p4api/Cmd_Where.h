//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Where.h   
//

#include "P4Command.h"

class CCmd_Where : public CP4Command
{
    // Construction
public:
    CCmd_Where(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Where)
				    
    BOOL Run(CString &filespec);
    BOOL Run(CStringList *filelist);
    CString GetDepotSyntax() { ASSERT(m_Depot.GetCount() && 1); return m_Depot.GetTail(); }
    CString GetClientSyntax() { ASSERT(m_Client.GetCount() && 1); return m_Client.GetTail(); }
    CString GetLocalSyntax() { ASSERT(m_Local.GetCount() && 1); return m_Local.GetTail(); }
	CStringList *GetDepotFiles() { return &m_Depot; }
	CStringList *GetClientFiles() { return &m_Client; }
	CStringList *GetLocalFiles() { return &m_Local; }

    // Attributes	
protected:
    CStringList m_Depot;
	CStringList m_Client;
	CStringList m_Local;

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual BOOL IsQueueable() const { return TRUE; }
	virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
};
