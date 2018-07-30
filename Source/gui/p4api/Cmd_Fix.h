//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Fix.h   
//

#include "P4Command.h"

class CCmd_Fix : public CP4Command
{
    // Construction
public:
    CCmd_Fix(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Fix)
				    
    BOOL Run(CStringList *jobs, long changeNum, BOOL unfix=FALSE, LPCTSTR jobstatus=NULL);

    CObList *GetList() { return &m_List; }
    BOOL IsUnfixing() const { return m_Unfixing; }
    BOOL IsNewStatus() const { return m_IsNewStatus; }

    // Attributes	
protected:
    BOOL m_Unfixing;
	BOOL m_IsNewStatus;
    CObList m_List;

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
};









