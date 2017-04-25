//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_AutoResolve.h   
//

#include "P4Command.h"

class CCmd_AutoResolve : public CP4Command
{
    // Construction
public:
    CCmd_AutoResolve(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_AutoResolve)
				    
    BOOL Run(CStringList *files, int type, BOOL preview, BOOL force, BOOL textmerge, int whtSp);
	    
    CStringList *GetList() { return &m_StrListOut; }
    BOOL IsPreview() { return m_Preview; }

    // Attributes	
protected:
    BOOL m_Preview;
    BOOL IsValidMergeMessage(CString const & data);

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
};









