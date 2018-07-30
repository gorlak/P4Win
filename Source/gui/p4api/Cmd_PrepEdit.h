//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_PrepEdit.h  
//
//

#include "P4Command.h"


class CCmd_PrepEdit : public CP4Command
{
    // Construction
public:
    CCmd_PrepEdit(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_PrepEdit)

    BOOL Run(LPCTSTR filespec, BOOL getHead, BOOL addFile);
	void SetWarnIfLocked( BOOL b ) { m_WarnIfLocked = b; }

protected:
    // Attributes	
    CString m_FileName;
    BOOL m_GetHead;
	BOOL m_AddFile;
	BOOL m_WarnIfLocked;

    // CP4Command overrides
    virtual void PreProcess(BOOL& done);
};









