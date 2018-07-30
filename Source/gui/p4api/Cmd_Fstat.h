//
// 
// Copyright 1997,1999 Perforce Software.  All rights reserved.
//
// This file is part of Perforce - the FAST SCM System.
//
//

// Cmd_Fstat.h   
//

#include "P4Command.h"
#include "P4FileStats.h"

class CCmd_Fstat : public CP4Command
{
    // Construction
public:
    CCmd_Fstat(CGuiClient *client=NULL);
    ~CCmd_Fstat();
    DECLARE_DYNCREATE(CCmd_Fstat)

    void AddFstatRow(CP4FileStats *stats);
    CObList *GetFileList( );
	CStringList *GetErrors() { return &m_ErrorList; }

    void SetIncludeAddedFiles( BOOL addedOK ) { m_IncludeAddedFiles= addedOK; }
    BOOL Run( BOOL suppress, LPCTSTR spec, BOOL bShowEntireDepot, long minChange=0); 
    BOOL Run( BOOL suppress, CStringList *specList, BOOL bShowEntireDepot, long minChange=0, BOOL bWorking=FALSE, long wkChgNbr=-1, long maxOutput=0); 
											    
    int GetUpdateType() { return m_UpdateType; }
    BOOL GetFullUpdate() { return m_FullUpdate; }
    void SetUpdateType( int updateType ) { m_UpdateType= updateType; }
    void SetFullUpdate( BOOL fullUpdate ) { m_FullUpdate= fullUpdate; }


    // Attributes	
protected:
    CObList *m_pBatch;
    BOOL m_FullUpdate;
    int m_UpdateType;
    CStringList m_SpecList;
	CStringList m_ErrorList;	// Listing of the " - file(s) not in client view" errors
    BOOL m_IncludeAddedFiles;
	BOOL m_bWorking;

    // CP4Command overrides
    virtual void OnOutputStat( StrDict *varList );
    virtual BOOL IsQueueable() const { return TRUE; }
    virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);	
    virtual void PostProcess();
};


// The CFstatWrapper class is really just a struct
class CFstatWrapper 
{
public:
    
    CCmd_Fstat *pCmd;
	CObList *pList;
};




