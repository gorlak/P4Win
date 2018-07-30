#include "stdafx.h"
#include "p4win.h"
#include "GuiClient.h"
#include "GuiClientUser.h"
#include "p4command.h"

#include "MainFrm.h"
#include "Cmd_SendSpec.h"
#include "cmd_describe.h"

extern FileSysType LookupType( const StrPtr *type );	// stolen from clientservice.cc

//		the value when calling Client::SetProtocol( "wingui", ...
//		See //depot/main/p4-doc/codes/CONVERT
//
static LPCSTR sWinGUI_ProtocolLevel = "9";

CGuiClient::CGuiClient()
{
    m_ui = new CGuiClientUser();
    SetProtocol("wingui", const_cast<char*>(sWinGUI_ProtocolLevel) );

    // Record the permanent p4port if we havent already, then
    // set the port to the active port
    //
    CString port= ( GetPort() ).Text();
    CString permport= GET_P4REGPTR()->GetP4Port(TRUE);
    if( permport.GetLength() == 0)
	    GET_P4REGPTR()->SetP4Port( port, FALSE, TRUE, FALSE );

    port= GET_P4REGPTR()->GetP4Port();
    SetPort( port );
	SetTrans();
}

void CGuiClient::SetPort(LPCTSTR port) 
{ 
	m_aPort = CharFromCString(port); 
	ClientApi::SetPort(m_aPort); 
#ifdef UNICODE
	// assume to begin with that the new server is unicode
	SET_UNICODE(TRUE);
	SetTrans();
#endif
}

void CGuiClient::SetTrans()
{
	static	BOOL	firstTime = TRUE;
	CString msg;

	CString charset = GetCharset().Text();
    CString permcharset = GET_P4REGPTR()->GetP4Charset(TRUE);
    if( permcharset != charset )
	{
		CString tempcharset = GET_P4REGPTR()->GetP4Charset();
		GET_P4REGPTR()->SetP4Charset( charset, firstTime, TRUE, FALSE );
		if ((tempcharset != charset && !firstTime) || (firstTime && !tempcharset.IsEmpty()))
			GET_P4REGPTR()->SetP4Charset( tempcharset, TRUE, FALSE, FALSE );
	}

#ifndef UNICODE
	// for non-UNICODE builds, set unicode mode based on P4CHARSET
	if(charset.IsEmpty())
		SET_UNICODE(FALSE);
	else
		SET_UNICODE(TRUE);
#endif
    charset= GET_P4REGPTR()->GetP4Charset();

#ifdef	_DEBUG
	static CString oldCharset = _T("");
	if (charset != oldCharset && !firstTime)
	{
	    msg.Format(_T("P4CHARSET=%s"), oldCharset = charset);
		TheApp()->StatusAdd(msg, SV_DEBUG);
	}
#endif

	if(charset.GetLength())
    {
        int p4Charset = CharSetCvt::Lookup(CharFromCString(charset));
        switch(p4Charset)
        {
        case -1:
			::PostMessage(MainFrame()->m_hWnd, WM_PERFORCE_OPTIONS, 
				IDS_PAGE_CONNECTION, IDS_PAGE_CONNECTION);
            msg.FormatMessage(IDS_MISSING_OR_INVALID_P4CHARSET, charset);
            TheApp()->StatusAdd(msg, SV_ERROR);
			GET_P4REGPTR()->SetP4Charset( _T(""), TRUE, TRUE, TRUE );
            break;
		default:
        case CharSetCvt::EUCJP:
        case CharSetCvt::WIN_US_OEM:
		case CharSetCvt::MACOS_ROMAN:
		case CharSetCvt::ISO8859_5:
		case CharSetCvt::KOI8_R:
			::PostMessage(MainFrame()->m_hWnd, WM_PERFORCE_OPTIONS, 
				IDS_PAGE_CONNECTION, IDS_PAGE_CONNECTION);
            msg.FormatMessage(IDS_INCOMPATIBLE_P4CHARSET, charset);
            TheApp()->StatusAdd(msg, SV_ERROR);
			GET_P4REGPTR()->SetP4Charset( _T(""), TRUE, TRUE, TRUE );
            break;
        case CharSetCvt::ISO8859_1:
        case CharSetCvt::ISO8859_15:
		case CharSetCvt::WIN_CP_1251:
        case CharSetCvt::UTF_8:
		case CharSetCvt::UTF_8_BOM:
		case CharSetCvt::UTF_16:
		case CharSetCvt::UTF_16_LE:
		case CharSetCvt::UTF_16_BE:
		case CharSetCvt::UTF_16_BOM:
		case CharSetCvt::UTF_16_LE_BOM:
		case CharSetCvt::UTF_16_BE_BOM:
		case CharSetCvt::UTF_32_LE:
		case CharSetCvt::UTF_32_BE:
		case CharSetCvt::UTF_32_LE_BOM:
		case CharSetCvt::UTF_32_BE_BOM: 
		case CharSetCvt::UTF_32_BOM:
        case CharSetCvt::SHIFTJIS:
        case CharSetCvt::WIN_US_ANSI:
        case CharSetCvt::NONE:
            break;
        }

#ifdef UNICODE
		if(IS_UNICODE())
		{
			// use UTF-8 for all but file content
			ClientApi::SetTrans(CharSetCvt::UTF_8, p4Charset, CharSetCvt::UTF_8, CharSetCvt::UTF_8);
		}
		else
		{
			// turn off all translations
			ClientApi::SetTrans(CharSetCvt::NONE,CharSetCvt::NONE,CharSetCvt::NONE,CharSetCvt::NONE);
		}
#else
        int uiCharset = CharSetCvt::WIN_US_ANSI;
        switch(GetACP())
        {
        case 932:
            uiCharset = CharSetCvt::SHIFTJIS;
            break;
        case 1252:
            uiCharset = CharSetCvt::WIN_US_ANSI;
            break;
        default:
            ASSERT("Unsupported system ANSI code-page detected");
            break;
        }
        ClientApi::SetTrans(uiCharset, p4Charset, p4Charset, uiCharset);
#endif
    }
#ifdef UNICODE
	else
	{
		// turn off all translations
		// if server is in unicode mode it won't work, but
		// we can't guess which charset to use.
		// TODO: prompt user to select a charset
		SET_UNICODE(FALSE);
		ClientApi::SetTrans(CharSetCvt::NONE,CharSetCvt::NONE,CharSetCvt::NONE,CharSetCvt::NONE);
	}
#endif
	firstTime = FALSE;
}

CGuiClient::~CGuiClient()
{
    delete m_ui;
}

void CGuiClient::Run( const char *func) 
{ 
    ClientApi::Run(func, m_ui); 
}

void CGuiClient::RunTag( const char *func) 
{ 
    ClientApi::RunTag(func, m_ui); 
}

void CGuiClient::WaitTag() 
{ 
    ClientApi::WaitTag(m_ui); 
}

void CGuiClient::PushCommandPtr(CP4Command *cmd)
{
    m_ui->PushCommandPtr(cmd);
}

void CGuiClient::PopCommandPtr(CP4Command *cmd)
{
    m_ui->PopCommandPtr(cmd);
}

void CGuiClient::UseTaggedProtocol()
{
    SetProtocol( "tag", "yes"); 
}

void CGuiClient::UseSpecdefsProtocol()
{
    SetProtocol( "specstring", "yes"); 
}

CString CGuiClient::GetUserStr()
{
    ClientApi client;
    client.SetProtocol( "wingui", sWinGUI_ProtocolLevel );
#ifdef UNICODE
	if(IS_UNICODE())
		client.SetTrans(CharSetCvt::UTF_8, CharSetCvt::UTF_8, CharSetCvt::UTF_8, CharSetCvt::UTF_8);
#endif
    Error e;
	e.Clear();
	return CharToCString(client.GetUser().Text());
}

CString CGuiClient::GetClientStr()
{
    ClientApi client;
    client.SetProtocol( "wingui", sWinGUI_ProtocolLevel );
#ifdef UNICODE
	if(IS_UNICODE())
		client.SetTrans(CharSetCvt::UTF_8, CharSetCvt::UTF_8, CharSetCvt::UTF_8, CharSetCvt::UTF_8);
#endif
    Error e;
	e.Clear();
    return CharToCString(client.GetClient().Text());
}

bool CGuiClient::DefinePort(LPCTSTR port)
{
    ClientApi client;
    client.SetProtocol( "wingui", sWinGUI_ProtocolLevel );
#ifdef UNICODE
	if(IS_UNICODE())
		client.SetTrans(CharSetCvt::UTF_8, CharSetCvt::UTF_8, CharSetCvt::UTF_8, CharSetCvt::UTF_8);
#endif
    Error e;
	e.Clear();
    client.DefinePort( CharFromCString(port), &e);
	return (!e.Test());
}

bool CGuiClient::DefineCharset(LPCTSTR charset)
{
    ClientApi client;
    client.SetProtocol( "wingui", sWinGUI_ProtocolLevel );
#ifdef UNICODE
	if(IS_UNICODE())
		client.SetTrans(CharSetCvt::UTF_8, CharSetCvt::UTF_8, CharSetCvt::UTF_8, CharSetCvt::UTF_8);
#endif
    Error e;
	e.Clear();
    client.DefineCharset( CharFromCString(charset), &e);
	return (!e.Test());
}

bool CGuiClient::DefineUser(LPCTSTR user)
{
    ClientApi client;
    client.SetProtocol( "wingui", sWinGUI_ProtocolLevel );
#ifdef UNICODE
	if(IS_UNICODE())
		client.SetTrans(CharSetCvt::UTF_8, CharSetCvt::UTF_8, CharSetCvt::UTF_8, CharSetCvt::UTF_8);
#endif
    Error e;
	e.Clear();
    client.DefineUser( CharFromCString(user), &e);
	return (!e.Test());
}

bool CGuiClient::DefineClient(LPCTSTR clientName)
{
    ClientApi client;
    client.SetProtocol( "wingui", sWinGUI_ProtocolLevel );
#ifdef UNICODE
	if(IS_UNICODE())
		client.SetTrans(CharSetCvt::UTF_8, CharSetCvt::UTF_8, CharSetCvt::UTF_8, CharSetCvt::UTF_8);
#endif
    Error e;
	e.Clear();
    client.DefineClient( CharFromCString(clientName), &e);
	return (!e.Test());
}

bool CGuiClient::DefinePassword(LPCTSTR password)
{
    ClientApi client;
    client.SetProtocol( "wingui", sWinGUI_ProtocolLevel );
#ifdef UNICODE
	if(IS_UNICODE())
		client.SetTrans(CharSetCvt::UTF_8, CharSetCvt::UTF_8, CharSetCvt::UTF_8, CharSetCvt::UTF_8);
#endif
    Error e;
	e.Clear();
    client.DefinePassword( CharFromCString(password), &e);
	return (!e.Test());
}

void CGuiClient::getServerInfo()
{
	int i=0;
	StrPtr *x;

	if( ( x = GetProtocol( "server2" ) ) != 0 ||
		( x = GetProtocol( "server" ) ) != 0 )
			i = x->Atoi();

	if( i )
	{
		//		Make a note of the current server protocol
		//		bad compatibility between 98 servers and 97 guis.
		//		test for two protocols
		//
		SET_SERVERLEVEL( i );

		if( ( x = GetProtocol( "security" ) ) != 0 )
			SET_SECURITYLEVEL( x->Atoi() );

        if((x= GetProtocol( "nocase" )) != NULL)
			SET_NOCASE(TRUE);
		else
			SET_NOCASE(FALSE);
	}

	//		get the spec def type in this server
	//		no need to check the server level for it.
	//		if nothing is returned, just use the built in ones
	//		in p4spec.h
	//
    CCmd_Describe *pCmd = dynamic_cast<CCmd_Describe*>(m_ui->GetCommandPtr());
    if(pCmd && (x=GetVar("specdef")) != NULL )
	{
		pCmd->SetSpecStr(CharToCString(x->Value()));
	}
}

