//
// 
// Copyright 1999,2001 Perforce Software.  All rights reserved.
//
// This file is part of Perforce - the FAST SCM System.
//
//

// Cmd_Depots.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_depots.h"
#include "cmd_describe.h"
#include "tokenstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNCREATE(CCmd_Depots, CP4Command)


CCmd_Depots::CCmd_Depots(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg = WM_P4DEPOTS;
	m_TaskName = _T("Depots");
	m_GotDepot = FALSE;
}


//-------------------------------------------------------------------

CCmd_Depots::~CCmd_Depots()
{
	
}


//-------------------------------------------------------------------

BOOL CCmd_Depots::Run( )											
{
	ClearArgs( );
	AddArg( _T("depots") );

	return CP4Command::Run( );
}


//-------------------------------------------------------------------

BOOL CCmd_Depots::HandledCmdSpecificError( LPCTSTR errBuf, LPCTSTR errMsg )
{
	if ( StrStr(errBuf, _T("no such file") ) )
	{
		TheApp()->StatusAdd( LoadStringResource(IDS_NO_SUBDIR_UNDER_FOLDER), SV_DEBUG );  
		return TRUE ; 
	}
	
	BOOL b = (StrStr(errBuf, _T("up-to-date.") ) != 0);

	if (!b && !m_GotDepot)
		m_FatalError = TRUE;	// have to set this in case error comes from the broker
								// else we hang.
	return ( b );
}


//-------------------------------------------------------------------
//	One depot at a time comes back from the server, something like:  
//
//	Depot beyond 1999/01/21 remote beyond:1666 //... 'Created by laura. '
//	Depot depot 1999/01/18 local subdir depot/... 'Created by seiwald. '
//	Depot usr 1999/01/26 remote perforce:1951 //usr/... 'Created by seiwald. '
//-------------------------------------------------------------------

void CCmd_Depots::OnOutputInfo( char level, LPCTSTR data, LPCTSTR msg )
{
	if( APP_ABORTING( ) && m_Asynchronous )
    {
		ReleaseServerLock();	
        ExitThread(0);
    }

	if ( StrNCmp(data, _T("Depot "), 6) ==0 )
		m_GotDepot = TRUE;
	else
		ASSERT(0);
	CTokenString str;
	str.Create( data + 6);

	CString depotName;
	depotName.Format(_T("//%s"), str.GetToken()); 
	CString date= str.GetToken();
	ASSERT(StrStr(date, _T("/")));
	
	CString depotType= str.GetToken();
	if( depotType.CompareNoCase(_T("local")) == 0)
		m_LocalDepotList.AddHead( depotName );
	else if( depotType.CompareNoCase(_T("stream")) == 0)
		m_LocalDepotList.AddHead( depotName );
	else if( depotType.CompareNoCase(_T("remote")) == 0)
		m_RemoteDepotList.AddHead( depotName );
	else if( depotType.CompareNoCase(_T("archive")) == 0)
		m_RemoteDepotList.AddHead( depotName );
	else if( depotType.CompareNoCase(_T("spec")) == 0)
	{
		if (GET_SERVERLEVEL() >= 18)
			m_RemoteDepotList.AddHead( depotName );
	}
	else
		ASSERT(0);
}

//-------------------------------------------------------------------

static BOOL IsLFonly(LPCTSTR desc)
{
	CString lineend= TheApp()->GetClientSpecField( _T("LineEnd"), desc );
	if (!lineend.IsEmpty())
		return lineend == _T("unix");
	CString options= TheApp()->GetClientSpecField( _T("Options"), desc );
	return ( options.Find(_T("nocrlf")) != -1 ) ? TRUE : FALSE;
}

//-------------------------------------------------------------------
// extract depot names from client spec text, and put into depotList
// formatted as '//depot/ ', and separated by newlines

static BOOL GetViewDepots( LPCTSTR spectext, CString &depotList)
{
	CString line;
	BOOL hasPlusMapping= FALSE;
    BOOL inView= FALSE;
    int start, end, i;

    CString spec= spectext;
    
    for( start=end=0; end < spec.GetLength()-2; end++)
    {
        if( spec[end] == '\n' )
        {
			line= spec.Mid( start, end-start+1 );
			start= end+1;

			if( line.Find(_T("View")) == 0 )
				inView= TRUE;
			else if( inView )
			{
				line.TrimLeft(_T(" \t\""));	// space, TAB & dbl-quote
				if (line.GetAt(0) == _T('+'))
					hasPlusMapping = TRUE;
				line.TrimLeft(_T(" \t\"+"));	// space, TAB, dbl-quote & plus-sign
				if ( (i = line.Find(_T("//"))) == 0 )
				{
					for(i+=2; i < line.GetLength(); i++)
					{
						if( line[i] == _T('/') )
						{
							line= line.Left( i+1 ) + _T(" ");
							line.TrimLeft(_T('\"'));
							break;
						}
					}
					if( depotList.Find(line) == -1 )
						depotList += line;
				}
				else if (((i=line.Find(_T("-"))) == 0) || ((i==1) && (line.GetAt(0) == _T('\"'))))
				{
				}
				else
					inView= FALSE;
			}
		}
	}

	if( inView && start < spec.GetLength() -1 )
	{
		// Catch a straggler in case there is no trailing '\n'
		line= spec.Mid( start );
		line.TrimLeft(_T(" \t\""));	// space, TAB & dbl-quote
		if (line.GetAt(0) == _T('+'))
			hasPlusMapping = TRUE;
		line.TrimLeft(_T(" \t\"+"));	// space, TAB, dbl-quote & plus-sign
		if ( ((i = line.Find(_T("//"))) == 0) || ((i == 1) && (line[0] == _T('\"'))) )
		{
			for(i+=2; i < line.GetLength(); i++)
			{
				if( line[i] == _T('/') )
				{
					line= line.Left( i+1 ) + _T(" ");
					line.TrimLeft(_T('\"'));
					break;
				}
			}
			
			if( depotList.Find(line) == -1 )
				depotList += line;
		}
	}
	return hasPlusMapping;
}

// Check to see if client view has plus mappings
static BOOL DoesClientViewHavePlusMapping(LPCTSTR spectext)
{
	BOOL inView= FALSE;
	int start, end;
	CString line;
	CString spec= spectext;
	for( start=end=0; end < spec.GetLength()-2; end++)
	{
		if( spec[end] == '\n' )
		{
			 line= spec.Mid( start, end-start+1 );
			start= end+1;

			if( line.Find(_T("View")) == 0 )
				inView= TRUE;
			else if( inView )
			{
				line.TrimLeft(_T(" \t\""));	// space, TAB & dbl-quote
				if (line.GetAt(0) == _T('+'))
				{
					return TRUE;
					break;
				}
			}
		}
	}
	if( inView && start < spec.GetLength()-1 )
	{
		line= spec.Mid( start );
		line.TrimLeft(_T(" \t\""));	// space, TAB & dbl-quote
		if (line.GetAt(0) == _T('+'))
			return TRUE;
	}
	return FALSE;
}

//-------------------------------------------------------------------

void CCmd_Depots::PostProcess()
{
    // The 'p4 depots' command may produce no output, in which case 
    // GET_SERVERLEVEL() will come up dry.  So the first order of 
    // business is to run ANY command that is guaranteed to produce 
    // output.  I selected 'p4 client' cuz we already have the header 
    // included and unlike commands like 'p4 info', it wont take a year 
    // and a day on a slow network

	BOOL hasPlusMapping = FALSE;

    CCmd_Describe cmd(m_pClient);
    cmd.Init( NULL, RUN_SYNC );
    BOOL cmdStarted= cmd.Run( P4CLIENT_SPEC, GET_P4REGPTR()->GetP4Client() );
    if(cmdStarted && !cmd.GetError())
    {
        // get client spec, and set NOCRLF flag and Client root in app
        TheApp()->m_bNoCRLF = IsLFonly(cmd.GetDescription());
        TheApp()->Set_m_ClientRoot(TheApp()->GetClientSpecField( _T("Root"), cmd.GetDescription()));
		if (GET_SERVERLEVEL() >= 22)
	        TheApp()->Set_m_ClientSubOpts(TheApp()->GetClientSpecField( _T("SubmitOptions"), cmd.GetDescription()));
    }
    else
    {
		TheApp()->m_ClientRoot.Empty();
        m_ErrorTxt= LoadStringResource(IDS_UNABLE_TO_GET_CLIENT_DESCRIPTION);
        m_FatalError=TRUE;
		return;
    }

    INT_PTR depotCount= m_LocalDepotList.GetCount() + m_RemoteDepotList.GetCount(); 

	if (GET_P4REGPTR()->ShowEntireDepot() > SDF_DEPOT)
	{
		m_LocalDepotList.RemoveAll();
		m_LocalDepotList.AddHead(TheApp()->m_ClientRoot);
		hasPlusMapping = DoesClientViewHavePlusMapping(cmd.GetDescription());
	}
    // If more than one depot returned and in client-view-only mode,
    // get the client spec and remove any depots that are outside the clientview
    //
    else if( depotCount > 1 && !(GET_P4REGPTR()->ShowEntireDepot() == SDF_DEPOT) )
    {
        if(!m_FatalError)
        {
            CString depot;
            CString view;
            hasPlusMapping = GetViewDepots( cmd.GetDescription(), view );
            if(IS_NOCASE())
                view.MakeLower();

            POSITION pos1, pos2;
            for( pos1= m_LocalDepotList.GetHeadPosition(); (pos2=pos1) != NULL; )
            {
                depot.Format(_T("%s/"), m_LocalDepotList.GetNext(pos1));
                if(IS_NOCASE())
                    depot.MakeLower();
                if( view.Find( depot ) == -1 )
                {
                    XTRACE(_T("Removed local depot %s because it is outside client view"), depot );
                    m_LocalDepotList.RemoveAt( pos2 );
                }
			}
			for( pos1= m_RemoteDepotList.GetHeadPosition(); (pos2=pos1) != NULL; )
			{
				depot.Format(_T("%s/"), m_RemoteDepotList.GetNext(pos1));
				if(IS_NOCASE())
					depot.MakeLower();
				if( view.Find( depot ) == -1 )
				{
					XTRACE(_T("Removed remote depot %s because it is outside client view"), depot );
					m_RemoteDepotList.RemoveAt( pos2 );
				}
			}
		}
	}
	// Else if no depots returned, just add the single remote depot 'depot' 
	// since there is ALWAYS a default depot called 'depot' when no depots have
	// been defined
	//
	else
	{
		hasPlusMapping = DoesClientViewHavePlusMapping(cmd.GetDescription());
		if ( depotCount == 0 )
			m_LocalDepotList.AddHead( _T("//depot") );
	}

	TheApp()->m_HasPlusMapping = hasPlusMapping;
}
