//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Resolved.h  
//

#include "P4Command.h"


class CCmd_Resolved : public CP4Command
{
    // Construction
public:
    CCmd_Resolved(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Resolved)

    BOOL Run(LPCTSTR filespec=NULL);
    CObArray const *GetArray() const { return &m_ResolvedArray; }

    // Attributes	
protected:
    CObArray m_ResolvedArray;

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
 	virtual void OnOutputStat( StrDict *varList );
	virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
    virtual void PostProcess();
};









