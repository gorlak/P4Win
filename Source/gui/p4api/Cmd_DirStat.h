//
// Copyright 1998 Perforce Software.  All rights reserved.
//
//

// Cmd_DirStat.h  
//
//

#include "P4Command.h"
	

class CCmd_DirStat : public CP4Command
{
    // Construction
public:
    CCmd_DirStat(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_DirStat)

    BOOL Run( CStringList *specs, BOOL entireDepot );
    CObList const *GetFiles() const { return &m_Files; }
    CStringList *GetDirs() { return &m_Dirs; }
    CStringList *GetSpecList() { ASSERT(m_pSpecList != NULL); return m_pSpecList; }

    // Attributes	
protected:
	BOOL m_ShowOnlyNotInDepot;
    BOOL m_EntireDepot;
    CStringList *m_pSpecList;

    CStringList m_Dirs;
	CStringList m_NotMapped;
    CObList m_Files;

    // CP4Command overrides
    virtual void PreProcess(BOOL& done);
    virtual BOOL IsQueueable() const { return TRUE; }
};









