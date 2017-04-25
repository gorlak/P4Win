/*
 * Copyright 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */


// Cmd_Logout.h   
//

#include "P4Command.h"

class CCmd_Logout : public CP4Command
{
    // Construction
public:
    CCmd_Logout(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Logout)
				    
    BOOL Run(BOOL bInvalidating);

protected:

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
};
