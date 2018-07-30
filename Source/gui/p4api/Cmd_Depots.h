//
// 
// Copyright 1999,2001 Perforce Software.  All rights reserved.
//
// This file is part of Perforce - the FAST SCM System.
//
//

// Cmd_Depots.h   

// Runs "p4 depots" to get lists of local and remote depots
// Lists will be filtered by client view, if in client-view-only
// mode.
//
// Also, sets app->m_bNoCRLF according to client settings.

#include "P4Command.h"


class CCmd_Depots : public CP4Command
{
public:
    // Construction
    CCmd_Depots( CGuiClient *client = NULL );
    ~CCmd_Depots( );
    DECLARE_DYNCREATE( CCmd_Depots )

    CStringList *GetLocalDepotList() { return &m_LocalDepotList; }
    CStringList *GetRemoteDepotList() { return &m_RemoteDepotList; }
    BOOL Run( ); 

protected:
    // Attributes	
    CStringList m_LocalDepotList;
    CStringList m_RemoteDepotList;

    // CP4Command overrides
    virtual void OnOutputInfo( char level, LPCTSTR data, LPCTSTR msg );
    virtual BOOL HandledCmdSpecificError( LPCTSTR errBuf, LPCTSTR errMsg );
    virtual void PostProcess();
    virtual BOOL IsQueueable() const { return TRUE; }

	BOOL m_GotDepot;
};
