//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Refresh.h   
//

#include "P4Command.h"

class CCmd_Refresh : public CP4Command
{
    // Construction
public:
    CCmd_Refresh(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Refresh)
				    
    BOOL Run(CStringList *files);
    // Attributes	
protected:

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
};









