//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Opened.h   
//

#include "P4Command.h"

class CCmd_Opened : public CP4Command
{
    // Construction
public:
    CCmd_Opened(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Opened)
				    
    BOOL Run(BOOL allOpenFiles, BOOL filter=FALSE, int changeNumber= -1, CStringList *files=NULL);
	void SetDepotPath(CString &path) { m_DepotPath = path; }
	CString *GetDepotPath() { return &m_DepotPath; }

    CObList *GetList() { return &m_List; }			

    // Attributes	
protected:
    CObList m_List;
    int m_ChangeNumber;
	CString m_DepotPath;	// only used with File > Properties

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
};









