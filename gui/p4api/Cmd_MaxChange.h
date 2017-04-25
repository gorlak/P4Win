//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_MaxChange.h   
//

#include "P4Command.h"

class CCmd_MaxChange : public CP4Command
{
    // Construction
public:
    CCmd_MaxChange(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_MaxChange)

    BOOL Run(  );

    int GetMaxChange() const { return m_MaxChange; }

protected:
    // Attributes	
    int m_MaxChange;
    BOOL m_StartedNoServerLevel;
	    
    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
};









