//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Fixes.h   
//

#include "P4Command.h"

class CCmd_Fixes : public CP4Command
{
    // Construction
    public:
    CCmd_Fixes(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Fixes)
				    
    BOOL Run(int changeNum=0, HTREEITEM change=NULL, BOOL bJob=FALSE, LPCTSTR jobname=NULL);

    CObList *GetList() { return &m_List; }			
    HTREEITEM GetFixedChange() { return m_ChangeItem; }
    int GetFixedChangeNumber() const { return m_ChangeNumber; }

    // Attributes	
protected:
    CObList m_List;
    HTREEITEM m_ChangeItem;
    int m_ChangeNumber;

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual BOOL IsQueueable() const { return TRUE; }
};









