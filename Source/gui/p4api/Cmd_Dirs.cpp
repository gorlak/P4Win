//
// 
// Copyright 1998, 1999 Perforce Software.  All rights reserved.
//
// This file is part of Perforce - the FAST SCM System.
//
//

// Cmd_Dirs.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_dirs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




const CString g_sClientViewOnly = _T("-C");
const CString g_ShowDeletedFiles = _T("-D");


IMPLEMENT_DYNCREATE(CCmd_Dirs, CP4Command)


/*
	_________________________________________________________________
*/

CCmd_Dirs::CCmd_Dirs(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg = WM_P4DIRS;
	m_TaskName = _T("Dirs");
}


/*
	_________________________________________________________________
*/

CCmd_Dirs::~CCmd_Dirs()
{
	
}


/*
	_________________________________________________________________
*/

BOOL CCmd_Dirs::Run( LPCTSTR spec, BOOL bShowEntireDepot )
{
    m_SpecList.RemoveAll();
    m_SpecList.AddHead( spec );
    return Run( &m_SpecList, bShowEntireDepot );
}

BOOL CCmd_Dirs::Run( CStringList *specList, BOOL bShowEntireDepot )
{
    ASSERT_KINDOF( CStringList, specList );
	ASSERT( specList->GetCount() );
	
	ClearArgs( );
	m_BaseArgs= AddArg( _T("dirs") );

	if ( ! bShowEntireDepot )
		m_BaseArgs= AddArg ( g_sClientViewOnly );

#ifndef SIM_EARLY_SERVER
	if ( GET_P4REGPTR()->ShowDeleted( ) && GET_SERVERLEVEL() > 4 )
#endif
		m_BaseArgs= AddArg( g_ShowDeletedFiles );

	m_posStrListIn= specList->GetHeadPosition();
	m_pStrListIn= specList;  
	
	// Put the first few files into the arg list
	NextListArgs();

	return CP4Command::Run( );
}


/*
	_________________________________________________________________

	it's okay not to have subdirectories in a tree. so ignore the 
	error that comes back from the server, but print it out when 
	debugging. (it's okay if it's hard-coded here, since i'm the only 
	one who's going to see it.)

	return all other errors to the base class's error handler.
	ignore completion messages.
	_________________________________________________________________
*/

BOOL CCmd_Dirs::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg )
{
	if ( StrStr(errBuf, _T("no such file" ))  
       || StrStr(errBuf, _T(" - file(s) not in client view")) 
       || StrStr(errBuf, _T(" - no mappings in client view.")))
	{
		if (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
			m_ErrorList.AddHead(errBuf);
		else
			TheApp()->StatusAdd( LoadStringResource(IDS_NO_SUBDIR_UNDER_FOLDER), SV_DEBUG );  
		return TRUE ; 
	}

	return ( StrStr(errBuf, _T("up-to-date." )) != 0 );
}


/*
	_________________________________________________________________
	
	one subdirectory at a time comes back from the server.
	i could post a message with each one:

			TCHAR *msg = _tcsdup( data );
			::PostMessage( m_ReplyWnd, m_ReplyMsg, (WPARAM) msg, RETURN_FROM_SERVER );

	but that would be a waste of time. so let's shove them into a nice
	cstringarray and post the message in postprocess.
	_________________________________________________________________
*/

void CCmd_Dirs::OnOutputInfo( char level, LPCTSTR data, LPCTSTR msg )
{
	if( APP_ABORTING( ) )
    {
		ReleaseServerLock();
        ExitThread(0);
    }

	m_StrListOut.AddHead ( data );
	//	TheApp()->StatusAdd( data, SV_DEBUG );//	fanny: uncomment only when desperate
}



