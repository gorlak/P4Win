//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Ostat.h  
//
//

#include "P4Command.h"
	

class CCmd_Ostat : public CP4Command
{
    // Construction
public:
    CCmd_Ostat(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Ostat)

    BOOL Run(BOOL allOpenFiles, int changeNumber= -1, CStringList *files=NULL);
    CObArray const *GetArray() const { return &m_OpenArray; }

    // Attributes	
protected:
    BOOL m_AllOpenFiles;
    int m_ChangeNumber;
    CStringList *m_pSpecList;
    CObArray m_OpenArray;
    CStringArray m_UnresolvedArray;
    CStringArray m_ResolvedArray;

    void SortResolved();
    void SortUnresolved();
    void SortOpened();

    // CP4Command overrides
    virtual void PreProcess(BOOL& done);
    virtual BOOL IsQueueable() const { return TRUE; }
};









