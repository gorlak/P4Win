//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Files.h   
//

#include "P4Command.h"

class CCmd_Files : public CP4Command
{
    // Construction
public:
    CCmd_Files(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Files)
				    
    BOOL Run(CString &filespec);
    CStringList *GetFileList() { return &m_List; }


    // Attributes	
protected:
    CStringList m_List;

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual BOOL IsQueueable() const { return TRUE; }
};
