//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Users.h   
//

#include "P4Command.h"

class CCmd_Users : public CP4Command
{
    // Construction
public:
    CCmd_Users(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Users)
				    
    BOOL Run();
    CObList const *GetList() const { return &m_List; }			

    // Attributes	
protected:
    CObList m_List;

    // CP4Command overrides
    virtual BOOL PWDRequired() const { return FALSE; }
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
};









