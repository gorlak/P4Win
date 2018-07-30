//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Labels.h   
//

#include "P4Command.h"

class CCmd_Labels : public CP4Command
{
    // Construction
public:
    CCmd_Labels(CGuiClient *client=NULL);
    ~CCmd_Labels();
    DECLARE_DYNCREATE(CCmd_Labels)
				    
    BOOL Run( CStringList *viewSpec );

    CObList const *GetList() const { return m_pList; }			

    // Attributes	
protected:
    CObList *m_pList;
    CObArray m_Labels;

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
	virtual void OnOutputStat( StrDict *varList );
    virtual void PostProcess();
};









