//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// GuiClientUser.cpp   
//
// CGuiClientUser is a P4 ClientUser that overrides all members which want
// to use printf, stdout, stdin, etc
//
#include "stdafx.h"
#include "P4Win.h"
#include "GuiClientUser.h"
#include "Cmd_Diff.h"
#include "Cmd_SendSpec.h"
#include "msgserver.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CGuiClientUser::CGuiClientUser()
{
}

void CGuiClientUser::PushCommandPtr(CP4Command *cmd) 
{ 
    ASSERT_KINDOF(CP4Command, cmd); 
    m_command.AddHead(cmd); 
}

void CGuiClientUser::PopCommandPtr(CP4Command *cmd) 
{ 
    ASSERT(m_command.GetHead() == cmd); 
    m_command.RemoveHead(); 
}

CGuiClientUser::~CGuiClientUser()
{
    // calls to PushCommandPtr and PopCommandPtr should be matched
    ASSERT(m_command.IsEmpty());
}

void CGuiClientUser::Diff( FileSys *f1, FileSys *f2, int doPage, char *diffFlags, Error *e )
{
    CCmd_Diff *pDiffCmd = dynamic_cast<CCmd_Diff*>(m_command.GetHead());
    if(pDiffCmd)
		pDiffCmd->Diff(f1, f2, doPage, diffFlags, e);
	else
		ASSERT(0);
}

void CGuiClientUser::OutputInfo( char level, const char *data )
{
    CGuiClient *client = m_command.GetHead()->GetClient();
    ASSERT(client);
    client->getServerInfo();

	CString sData = CharToCString(data);
	m_command.GetHead()->OnOutputInfo( level, sData, sData );
}

void CGuiClientUser::OutputStat( StrDict *varList )
{
	m_command.GetHead()->OnOutputStat( varList );
}

void CGuiClientUser::OutputError( const char *errBuf )
{
    CGuiClient *client = m_command.GetHead()->GetClient();
    ASSERT(client);
    client->getServerInfo();

	CString sData = CharToCString(errBuf);
	m_command.GetHead()->OnOutputError( 0x7F, sData, sData );
}

void CGuiClientUser::OutputText( const char *data, int length )
{
	CString sData = CharToCString(data);
	m_command.GetHead()->OnOutputText( sData, sData.GetLength() );
}

void CGuiClientUser::OutputBinary( const char *data, int length )
{
	m_command.GetHead()->OnOutputText( CString(data), length );
}

void CGuiClientUser::Message( Error *err )
{
	CGuiClient *client = m_command.GetHead()->GetClient();
	ASSERT(client);
	client->getServerInfo();

	StrBuf  buf,  bufl;

	// Errors have to end with \n; info not.

	int nlflag = err->IsInfo() ? EF_PLAIN : EF_NEWLINE;
	CString sBuf = FormatError(err, EF_NOXLATE | nlflag);
	CString sBufl = "";

	StrPtr lang = client->GetLanguage();
	if (lang.Length())
		sBufl = FormatError(err, nlflag);

	char level = (char)(err->GetGeneric() + '0');
	if( err->IsInfo() )
	{
		// Trigger output?
		if ( err->CheckId( MsgServer::TriggerOutput ) )
		{
			TheApp()->StatusAdd( sBufl.GetLength() ? sBufl : CString(sBuf), SV_MSG );
			return;
		}
		// Info
		m_command.GetHead()->OnOutputInfo( level, sBuf, 
						sBufl.GetLength() ? sBufl : CString(sBuf) );
	}
	else
	{
		// Trigger output?
		ErrorId *eid;
		for (int i = -1; (eid = err->GetId(++i)) != NULL; )
		{
			if (eid->code == MsgServer::TriggerFailed.code 
			 || eid->code == MsgServer::TriggersFailed.code)
			{
				TheApp()->StatusAdd( sBufl.GetLength() ? sBufl : CString(sBuf), 
					err->IsError() ? SV_ERROR : SV_MSG, err->IsError() ? true : false );
				if (err->IsError())
					m_command.GetHead()->SetTriggerError();
				return;
			}
		}
		// warn, failed, fatal
		m_command.GetHead()->OnOutputError( level, sBuf, 
						sBufl.GetLength() ? sBufl : CString(sBuf) );
	}
}

void CGuiClientUser::ErrorPause( char *errBuf, Error *e )
{
	m_command.GetHead()->OnErrorPause( CString(errBuf), e );
}

void CGuiClientUser::InputData( StrBuf *strBuf, Error *e )
{
    m_command.GetHead()->OnInputData(strBuf, e);
}

int CGuiClientUser::Resolve( ClientMerge *m, Error *e )
{
    return m_command.GetHead()->OnResolve(m, e);
}

void CGuiClientUser::Prompt( const StrPtr &msg, StrBuf &rsp, int noEcho, Error *e )
{
	m_command.GetHead()->OnPrompt( msg, rsp, noEcho, e );
}

// The following member functions are not used by the gui, so the
// server should not be invoking them
#ifdef _DEBUG
	void	CGuiClientUser::Merge( FileSys *base, FileSys *leg1, FileSys *leg2, 
						   FileSys *result, Error *e )
		{ ASSERT(0); }
	void	CGuiClientUser::Edit( FileSys *f1, Error *e )
		{ ASSERT(0); }
	void	CGuiClientUser::Help( char **help )
		{ ASSERT(0); }
#endif
