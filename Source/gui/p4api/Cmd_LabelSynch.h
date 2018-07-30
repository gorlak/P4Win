//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_LabelSynch.h  
//
//

#include "P4Command.h"


class CCmd_LabelSynch : public CP4Command
{
    // Construction
public:
    CCmd_LabelSynch(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_LabelSynch)

    BOOL Run(LPCTSTR labelName, CStringList *files, 
				    BOOL whatIf, BOOL removeFromLabel, BOOL noDeletes);

    CStringList *GetList() { return &m_StrListOut; }
	BOOL GetPreview() { return m_Preview; }

    // Attributes	
protected:

	BOOL m_Preview;

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
};









