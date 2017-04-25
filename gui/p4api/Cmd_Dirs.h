//
// 
// Copyright 1998, 1999 Perforce Software.  All rights reserved.
//
// This file is part of Perforce - the FAST SCM System.
//
//

// Cmd_Dirs.h   
//

#include "P4Command.h"
#include "P4FileStats.h"


class CCmd_Dirs : public CP4Command
{
public:
    CCmd_Dirs( CGuiClient *client = NULL );
    ~CCmd_Dirs( );
    DECLARE_DYNCREATE( CCmd_Dirs )

    CStringList *GetList() { return &m_StrListOut; }
    CStringList *GetErrors() { return &m_ErrorList; }
    BOOL Run( LPCTSTR spec, BOOL bShowEntireDepot);
    BOOL Run( CStringList *specList, BOOL bShowEntireDepot ); 

    int GetUpdateType() const { return m_UpdateType; }
    BOOL GetFullUpdate() const { return m_FullUpdate; }
    void SetUpdateType( int updateType ) { m_UpdateType= updateType; }
    void SetFullUpdate( BOOL fullUpdate ) { m_FullUpdate= fullUpdate; }

protected:
    BOOL m_FullUpdate;
    int m_UpdateType;
    CStringList m_SpecList;
	CStringList m_ErrorList;

    // CP4Command overrides
    virtual void OnOutputInfo( char level, LPCTSTR data, LPCTSTR msg );
    virtual BOOL HandledCmdSpecificError( LPCTSTR errBuf, LPCTSTR errMsg );
    virtual BOOL IsQueueable() const { return TRUE; }
};



