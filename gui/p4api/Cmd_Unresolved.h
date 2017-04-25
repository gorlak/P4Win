//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Unresolved.h   
//

#include "P4Command.h"

class CCmd_Unresolved : public CP4Command
{
    // Construction
public:
    CCmd_Unresolved(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Unresolved)
				    
    BOOL Run();
    CObArray const *GetArray() const { return &m_UnresolvedArray; }

    // Attributes	
protected:
    CObArray m_UnresolvedArray;

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
	virtual void OnOutputStat( StrDict *varList );
	virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
    virtual void PostProcess();
};









