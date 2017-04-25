//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Info.h  

//	Runs "p4 info" and fills CP4Info member with the results.

#include "P4Command.h"
#include "P4Info.h"



class CCmd_Info : public CP4Command
{
    // Construction
public:
    CCmd_Info(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Info)

    BOOL Run();

    CP4Info const & GetInfo() const { return m_Info; }
protected:
    // Attributes	
    CP4Info m_Info;

    // CP4Command overrides
    virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg );	
    virtual BOOL PWDRequired() const { return FALSE; }
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual void PreProcess(BOOL& done);
};









