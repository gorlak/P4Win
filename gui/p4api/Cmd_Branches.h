//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Branches.h   
//

#include "P4Command.h"

class CCmd_Branches : public CP4Command
{
    // Construction
public:
    CCmd_Branches(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Branches)
				    
    BOOL Run();

    CObList *GetList() { return &m_List; }			

    // Attributes	
protected:
    CObList m_List;

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
	virtual void OnOutputStat( StrDict *varList );
};









