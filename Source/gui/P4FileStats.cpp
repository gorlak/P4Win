//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4FileStats.cpp
#include "stdafx.h"
#include "P4Win.h"
#include "MainFrm.h"
#include "P4FileStats.h"
#include "GuiClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// File actions, MUST match the enums in P4FileStats.h

static LPCTSTR actions[]=
{
	_T("none"),  // not a Perforce type, just padding to match the enum in header
	_T("unknown"),
	_T("add"),
	_T("edit"),
	_T("delete"),
	_T("branch"),
	_T("integrate"),
	_T("import"),
	_T("move/add"),
	_T("move/delete"),
	_T("no action"),
	0
};

// File types, MUST match the enums in P4FileStats.h

static LPCTSTR types[]=
{
	_T("unknown"),
	_T("text"),
	_T("ctext"),
	_T("cxtext"),
	_T("ltext"),
	_T("ktext"),
	_T("ttext"),
	_T("xtext"),
	_T("xltext"),
	_T("kxtext"),
	_T("binary"),
	_T("tbinary"),
	_T("ubinary"),
	_T("xbinary"),
	_T("symlink"),
	_T("resource"),
	_T("tempobj"),
	_T("xtempobj"),
	_T("unicode"),
	_T("xunicode"),
	_T("utf16"),
	0
};


IMPLEMENT_DYNCREATE(CP4FileStats, CObject)

CP4FileStats::CP4FileStats()
{
	Clear();
}

void CP4FileStats::Clear()
{
	m_MyOpenAction= m_OtherOpenAction= 0;
	m_MyLock= m_OtherLock= FALSE;
	m_OtherUserMyClient= FALSE;
	m_HeadRev= m_HaveRev= 0;
	m_HeadAction= 0;
	m_HeadType= m_Type= _T("unknown");
	m_HeadTime=0;
	m_Unresolved=FALSE;
	m_Resolved=FALSE;
	m_OpenChangeNum=m_HeadChangeNum=0;
	m_OtherOpens=0;
	m_UserParam=0;
	m_NotInDepot=FALSE;
	m_FileSize=0;

	m_DepotPath=_T("");
	m_ClientPath=_T("");
	m_OtherUsers=_T("");
	m_Digest=_T("");
}

void CP4FileStats::Create( CP4FileStats *st )
{
	m_MyOpenAction= st->m_MyOpenAction;
	m_OtherOpenAction= st->m_OtherOpenAction;
	m_OtherUserMyClient= st->m_OtherUserMyClient;
	m_MyLock= st->m_MyLock; 
	m_OtherLock= st->m_OtherLock;
	m_HeadRev= st->m_HeadRev;
	m_HaveRev= st->m_HaveRev;
	m_HeadTime= st->m_HeadTime;
	m_HeadAction= st->m_HeadAction;
	m_Type= st->m_Type;
	m_HeadType= st->m_HeadType;
	m_Unresolved= st->m_Unresolved;
	m_Resolved= st->m_Resolved;
	m_OpenChangeNum= st->m_OpenChangeNum;
	m_HeadChangeNum= st->m_HeadChangeNum;
	m_OtherOpens= st->m_OtherOpens;
	m_UserParam= st->m_UserParam;
	m_NotInDepot= st->m_NotInDepot;

	m_DepotPath= st->m_DepotPath;
	m_ClientPath= st->m_ClientPath;
	m_ClientPath.Replace(_T('/'), _T('\\'));
	m_OtherUsers= st->m_OtherUsers;
	m_Digest= st->m_Digest;
	m_FileSize= st->m_FileSize;
}


CP4FileStats::~CP4FileStats()
{
	
}


// Create from an fstat result set.
BOOL CP4FileStats::Create(StrDict *client)
{
	int i;
	StrPtr *str;
	Error err;

	// Get the depot name
	str= client->GetVar( "depotFile", &err);		// name in depot
    ASSERT(str || err.Test());
    if(err.Test())
        goto badFile;
	m_DepotPath = CharToCString(str->Value());

	// If the client path exists, note that file is in client view
	str= client->GetVar( "clientFile" );
    if(str)
	{
		m_ClientPath = CharToCString(str->Value());
		m_ClientPath.Replace(_T('/'), _T('\\'));
	}
	else
    {
        // need to determine if the client path doesn't exist or doesn't translate
        // we can't handle the no translation case.
		CString txt = FormatError(&err);
		if(txt.Find(_T("No Translation")) == 0)
            goto badFile;

        // there is no client path
        m_ClientPath=_T("");
    }

	// Concatenate a list of all other users with the file open
    {
        char varName[] = "otherOpen   ";
	    char varNam2[] = "otherAction   ";
	    for(m_OtherOpens=m_OtherOpenAction=0; m_OtherOpens < 100; m_OtherOpens++)
	    {
		    itoa(m_OtherOpens, varName+9, 10);
		    if( (str=client->GetVar( varName )) == 0 )
			    break;
		    else
		    {
			    if(m_OtherOpens==0)
				    m_OtherUsers = CharToCString(str->Value());
			    else
			    {
				    m_OtherUsers+=_T("/");
				    m_OtherUsers+=CharToCString(str->Value());
			    }
			    if (m_OtherOpenAction != F_DELETE)
			    {
				    itoa(m_OtherOpens, varNam2+11, 10);
				    if ( (str=client->GetVar( varNam2 )) != 0)
				    {
					    for(i=F_UNKNOWNACTION; actions[i]; i++)
					    {
						    if(_tcscmp(actions[i], CharToCString(str->Value()))==0)
						    {
							    m_OtherOpenAction=(BYTE) i;
							    break;
						    }
					    }
				    }
			    }
		    }
	    }
    }

	if(	(str= client->GetVar( "headRev" )) != NULL)
		m_HeadRev=atol(str->Value());
	if( (str= client->GetVar( "haveRev" )) != NULL)
		m_HaveRev=atol(str->Value());
	if( (str= client->GetVar( "change" )) != NULL)
		m_OpenChangeNum=atol(str->Value());
	if( (str= client->GetVar( "headChange" )) != NULL)
		m_HeadChangeNum=atol(str->Value());
	if( (str= client->GetVar( "headTime" )) != NULL)
		m_HeadTime=atol(str->Value());
	
	if( (str= client->GetVar( "ourLock" )) != NULL)
		m_MyLock=TRUE;
	
	if( (str= client->GetVar( "otherLock" )) != NULL)
		m_OtherLock=TRUE;
	

	if( (str= client->GetVar( "type" )) != NULL)
		m_Type= CharToCString(str->Value());

	if( (str= client->GetVar( "headType" )) != NULL)
		m_HeadType= CharToCString(str->Value());

	if( (str= client->GetVar( "headAction" )) != NULL)
	{
		for(i=F_UNKNOWNACTION; actions[i]; i++)
		{
			if(_tcscmp(actions[i], CharToCString(str->Value()))==0)
			{
				m_HeadAction=(BYTE) i;
				break;
			}
		}
	}
	ASSERT(client->GetVar("headAction")==NULL || m_HeadAction);

	if( (str= client->GetVar( "action" )) != NULL)
	{
		for(i=F_UNKNOWNACTION; actions[i]; i++)
		{
			if(_tcscmp(actions[i], CharToCString(str->Value()))==0)
			{
				m_MyOpenAction= (BYTE) i;
				break;
			}
		}
	}
	ASSERT(client->GetVar("action")==NULL || m_MyOpenAction);
	if (!m_HaveRev && !m_HeadRev && (m_MyOpenAction == F_ADD || m_MyOpenAction == F_BRANCH))
		m_HaveRev = 1;
	
	if( (str= client->GetVar( "unresolved" )) != NULL)
		m_Unresolved=TRUE;

	str= client->GetVar( "actionOwner" );
    if(str)
	{
		m_ActionOwner = CharToCString(str->Value());
		if (Compare( m_ActionOwner, GET_P4REGPTR()->GetP4User() ) !=0)
		{
			m_OtherUserMyClient = TRUE;
			m_OtherUsers = m_ActionOwner + _T('@') + GET_P4REGPTR()->GetP4Client();
		}
	}

	str= client->GetVar( "digest" );
    if(str)
		m_Digest = CharToCString(str->Value());

	if(	(str= client->GetVar( "fileSize" )) != NULL)
		m_FileSize=atol(str->Value());

	// In release builds, these values may be zero for an unrecognized
	// file type or action, which maps to F_UNKNOWNFILETYPE or F_UNKNOWNACTION
//	ASSERT(client->GetVar("headType")== NULL || m_HeadType);	// commented out as useless and irritating in debug version - leighb 99/11/30
	ASSERT(client->GetVar("headAction")==NULL || m_HeadAction);
	ASSERT(client->GetVar("action")==NULL || m_MyOpenAction);


	return TRUE;
badFile:
    // most likely a translation failure.  Nothing to do but ignore this file.
    return FALSE;
}

// This verion of Create() used to process info returned by P4 ADD
// looks like :   //depot/x_win32/samples/rpc/README.TXT#1 - opened for add
BOOL CP4FileStats::Create(LPCTSTR depotName, long changeNumber)
{
	CString line=depotName;
	int pound=m_DepotPath.ReverseFind(_T('#'));

	ASSERT(pound != -1);
	if(pound == 0)  
		{ ASSERT(0); return FALSE; }  // not a line from P4 add

	m_DepotPath= line.Left(pound);

	// File revision
	m_HaveRev=_ttoi(depotName+pound+1);
	ASSERT(m_HaveRev==1);

	m_HeadRev=0;  // Not in depot yet
	m_MyOpenAction=F_ADD;
	m_HeadAction=F_ADD;
	m_OpenChangeNum=changeNumber;
	m_HeadType=m_Type=types[F_UNKNOWNFILETYPE];

	return TRUE;
}


// This version of Create() is only needed till open gets frobbed to
// provide data in the fstat format.
//
// Parses a row returned by P4 opened, of the form:
//	"//depot/dir/subdir/fname#9 - edit change 25 (text) by user@machine *locked*"
//  "//depot/x_win32/samples/rpc/README.TXT#1 - add default change (text)"
//  "//depot/x_win32/embedded - dash/README.TXT#1 - add default change (text)"
BOOL CP4FileStats::Create(LPCTSTR openRow)
{
	// Find the revision delimiter '#', and then scan subsequent fileRow 
	// characters for the separator, " - ".  We need to look for the
	// rev number first, because " - " may be embedded within the filename.

	CString line=openRow;
	int pound= line.Find(_T('#'));   
	if(pound == -1)
		{ ASSERT(0); return FALSE; }		// doesnt look like a fileRow

	int separator= pound+1;
	int len= line.GetLength();
	for( ; separator < len ; separator++)
	{
		if(line[separator]==_T(' ') && line[separator+1]==_T('-') && line[separator+2]==_T(' '))
			break;
	}
	
	if(separator == len)
		{ ASSERT(0); return FALSE; }		// doesnt look like a fileRow
			
	m_DepotPath=line.Left(pound);

	// File revision - note that this is stored under haveRev, no matter which user has the
	// file.
	//		note, too that rev can be 0, if the opened command returns a version #null
	//		(so remove the assert that used to be here)
	//
	long rev=_ttol(openRow+pound+1);
	m_HaveRev=rev;

	CString info=line.Mid(separator+3);
	CString ModeText=info.Left(info.Find(_T(" ")));
	
	// File open action
	int openAction=F_UNKNOWNACTION;
	for(int i=F_UNKNOWNACTION; actions[i]; i++)
	{
		if(_tcscmp(actions[i], ModeText)==0)
		{
			openAction=i;
			break;
		}
	}

	// File change number
	info=info.Mid(ModeText.GetLength()+1);
	if(info.Find(_T("default"))==0)
	{
		m_OpenChangeNum=0;		// default change
	}	
	else
	{
		if(info.Find(_T("change"))==0)
		{
			m_OpenChangeNum=_ttoi(info.Mid(7));
		}
	}
		
	
	// File type
	info=info.Mid(info.Find(_T("("))+1);
	CString TypeText=info.Left(info.Find(_T(")")));
	m_HeadType = m_Type = TypeText;
	
	info=info.Mid( min( info.GetLength()-1, TypeText.GetLength() + 2));
	int byStart, userLen;
	if( (byStart=info.Find(_T("by"))) == 0)
	{
		info=info.Mid(byStart+3);	// Skip over "by "
		userLen=info.Find(_T(" "));  
		if(userLen == -1)
			m_OtherUsers=info;
		else
			m_OtherUsers=info.Left(userLen);

		if( Compare( m_OtherUsers, GET_P4REGPTR()->GetMyID()) ==0 )
		{
			m_OtherUsers.Empty();
			m_OtherOpens=0;
			m_MyOpenAction= (BYTE) openAction;
			m_HaveRev= rev;
			if(info.Find(_T("locked")) > 0)
				m_MyLock=TRUE;
		}
		else
		{
			// See if its on my client
			int at= m_OtherUsers.Find(_T('@'));
			if( at != -1 && ++at < m_OtherUsers.GetLength() )
			{
				if( Compare( m_OtherUsers.Mid(at), GET_P4REGPTR()->GetP4Client()) ==0 )
					m_OtherUserMyClient= TRUE;
			}
			else
				// Why didnt we find client name
				ASSERT(0);
			
			// Update locked and open action info
			m_OtherOpens=1;
			if(info.Find(_T("locked")) > 0)
				m_OtherLock=TRUE;
		
			m_OtherOpenAction= (BYTE) openAction;
		}
	} 
	else
	{
		// didnt find "by", so its my open file
		m_OtherOpens=0;
		m_MyOpenAction= (BYTE) openAction;
		m_HaveRev= rev;
		if(info.Find(_T("locked")) > 0)
			m_MyLock=TRUE;
	}
	
	return TRUE;
}


// This version of Create is used to create an entry
// for a file that is not under Perforce control
BOOL CP4FileStats::Create( LPCTSTR localsyntax, LPCTSTR depotsyntax )
{
	Clear();
	m_DepotPath = depotsyntax;
	m_ClientPath= localsyntax;
	m_ClientPath.Replace(_T('/'), _T('\\'));
	m_NotInDepot= TRUE;
	return TRUE;
}


////////////////////////////////////
// Functions to allow updates to file status info


void CP4FileStats::SetClosed()
{
	SetOpenAction(0, FALSE);
	SetOpenAction(0, TRUE);
}

void CP4FileStats::SetLocked(BOOL locked, BOOL otherUser)
{
	if(otherUser)
		m_OtherLock= (BYTE)locked;
	else
		m_MyLock= (BYTE)locked;
}


void CP4FileStats::SetOpenAction(int action, BOOL otherUser)
{
	ASSERT(action >= 0 && action < F_MAXACTION);

	if(otherUser)
	{
		m_OtherOpenAction= (BYTE) action;
		if(action == 0)
		{
			m_Unresolved=FALSE;
			m_OtherLock= FALSE;
			m_OtherOpens=0;
			m_OtherUserMyClient=FALSE;
		}
	}
	else
	{
		m_MyOpenAction= (BYTE) action;
		if(action == 0)
		{
			m_Unresolved=FALSE;
			m_MyLock= FALSE;
		}
	}
}

void CP4FileStats::SetOtherOpens(int num)
{
	m_OtherOpens=num; 
	if(num==0)
	{
		m_OtherOpenAction=0;
		m_OtherUserMyClient=FALSE;
		SetLocked(FALSE, TRUE);
	}
}

void CP4FileStats::SetHeadAction(int action)
{
	ASSERT(action >= 0 && action < F_MAXACTION);
	m_HeadAction= (BYTE) action;
}

void CP4FileStats::SetHeadType(int type)
{
	ASSERT(type >= 0 && type < F_MAXTYPE);
	m_HeadType= types[type];
}

void CP4FileStats::SetHeadType(LPCTSTR txttype)
{
	ASSERT(txttype != NULL);
	ASSERT(_tcslen(txttype) > 0);

	m_HeadType= txttype;
}

void CP4FileStats::SetType(int type)
{
	ASSERT(type >= 0 && type < F_MAXTYPE);
	m_Type= types[type];
}

void CP4FileStats::SetType(LPCTSTR txttype)
{
	ASSERT(txttype != NULL);
	ASSERT(_tcslen(txttype) > 0);

	m_Type= txttype;
}

void CP4FileStats::SetHaveRev(long rev)
{
	ASSERT(rev >= 0);

	m_HaveRev=rev; 
	if(m_HeadRev != 0 && m_HeadRev < rev)
		m_HeadRev= rev;
}

void CP4FileStats::SetDepotPath(LPCTSTR path)
{
	ASSERT(_tcslen(path)==0 || _tcsncmp(path, _T("//"), 2) == 0 );
	m_DepotPath= path;

	// If the depot path just got cleared, free the buffer
	if(path[0]==_T('\0'))
		m_DepotPath.FreeExtra();
}

void CP4FileStats::SetClientPath(LPCTSTR path)
{
	ASSERT(_tcslen(path)==0 || path[1]==_T(':'));
	m_ClientPath=path;
	m_ClientPath.Replace(_T('/'), _T('\\'));
}

// User list looks like: swine@cow/pig@vermin/spion@goon
void CP4FileStats::SetOtherUsers(LPCTSTR userlist)
{
	m_OtherUsers=userlist;
	m_OtherOpens=0;

	// If the list just got cleared, free the buffer
	if(userlist[0]==_T('\0'))
	{
		m_OtherUsers.FreeExtra();
		return;
	}


	int hitSlash=TRUE;
	for(int i=0; i< m_OtherUsers.GetLength(); i++)
	{
		if(m_OtherUsers[i] == _T('/'))
		{
			ASSERT(!hitSlash);  // two slashes without '@' in between
			hitSlash=TRUE;
		}

		if(m_OtherUsers[i] == _T('@'))
		{
			ASSERT(hitSlash);
			hitSlash=FALSE;
			m_OtherOpens++;
		}
	}
}

///////////////////////////////////////////////////////
// Data access members

CString CP4FileStats::GetActionStr(int action) const
{
	ASSERT(action >=0 && action < F_MAXACTION);
	
	return CString(actions[action]);
}

CString CP4FileStats::GetDepotDir() const
{
	int slash=m_DepotPath.ReverseFind(_T('/'));
	if(slash != -1)
		return m_DepotPath.Left(slash+1);
	else
		{ ASSERT(0); return CString(); }
}

CString CP4FileStats::GetClientDir() const
{
	int slash=m_ClientPath.ReverseFind(_T('\\'));
	if(slash != -1)
		return m_ClientPath.Left(slash+1);
	else
		{ ASSERT(0); return CString(); }
}


CString CP4FileStats::GetDepotFilename() const
{
	int slash=m_DepotPath.ReverseFind(_T('/'));
	
	if(slash != -1)
		return m_DepotPath.Mid(slash+1);
	else
		{ ASSERT(0); return CString(); }
}

CString CP4FileStats::GetClientFilename() const
{
	int slash=m_ClientPath.ReverseFind(_T('\\'));
	
	if(slash != -1)
		return m_ClientPath.Mid(slash+1);
	else
		{ ASSERT(0); return CString(); }
}


CString CP4FileStats::GetFormattedFilename(BOOL showFileType) const
{
	CString filename = GET_P4REGPTR( )->ShowEntireDepot( ) <= SDF_DEPOT
		             ? GetDepotFilename() : GetClientFilename();

	// Format name + haveRev+headRev for display
	CString temp;

	if(m_HeadAction == F_DELETE)
	{
		// If the user has the file at < headrev, let the user know
		if( m_HaveRev > 0 && m_HaveRev < m_HeadRev )
			temp.FormatMessage(IDS_FSTAT_s_n_n_s_HEAD_REV_DELETED, filename, m_HaveRev, m_HeadRev, m_HeadType);
		else if(showFileType)
			temp.FormatMessage(IDS_FSTAT_s_n_n_s_DELETED, filename, m_HaveRev, m_HeadRev, m_HeadType);
		else
			temp.FormatMessage(IDS_FSTAT_s_n_n_DELETED, filename, m_HaveRev, m_HeadRev);
	}
	else
	{
		if (!m_HeadRev && !m_HaveRev)
			temp = filename;
		else if(showFileType)
			temp.FormatMessage(IDS_FSTAT_s_n_n_s, filename, m_HaveRev, m_HeadRev, m_HeadType);
		else
			temp.FormatMessage(IDS_FSTAT_s_n_n, filename, m_HaveRev, m_HeadRev);
	}
	return temp;
}

CString CP4FileStats::GetFormattedChangeFile(BOOL showFileType, BOOL showOpenAction) const
{
	// Format name + haveRev+headRev for display
	CString temp;
	int openAction= m_MyOpenAction;

	if(showOpenAction && m_OtherOpens && !m_MyOpenAction)
	{
		openAction= m_OtherOpenAction;
	}

	if(showFileType)
	{
		CString type = (m_Type == _T("unknown")) ? m_HeadType : m_Type;
		if(showOpenAction)
			temp.FormatMessage(IDS_FSTAT_s_n_s_s, m_DepotPath, m_HaveRev, 
								type, actions[openAction]);
		else
			temp.FormatMessage(IDS_FSTAT_s_n_s, m_DepotPath, m_HaveRev, type);
	}
	else
	{
		if(showOpenAction)
			temp.FormatMessage(IDS_FSTAT_s_n_s, m_DepotPath, m_HaveRev, actions[openAction]);
		else
			temp.FormatMessage(IDS_FSTAT_s_n, m_DepotPath, m_HaveRev);
	}	
	return temp;
}

CString CP4FileStats::GetFormattedHeadTime()
{
	CString time;
	struct tm *t;

	if (!m_HeadTime && !m_HeadRev && !m_HaveRev)
	{
		time = _T("");
	}
	else
	{
		if(m_HeadTime < 0)
			m_HeadTime = -m_HeadTime;

		t = _localtime32( (const __time32_t *)&m_HeadTime ); 
		
		time.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), 
			t->tm_year+1900, t->tm_mon+1, t->tm_mday, 
			t->tm_hour, t->tm_min, t->tm_sec);
	}
	return CString(time);
}

BOOL CP4FileStats::IsTextFile() const
{
	CString type = m_HeadType.Find(_T("unknown")) == -1 ? m_HeadType : m_Type;
	return(	((type.Find(_T("text")) != -1) 
		  || (type.Find(_T("symlink")) != -1)
		  || (type.Find(_T("unicode")) != -1)
		  || (type.Find(_T("utf16")) != -1)) ? TRUE : FALSE ); 
}

BOOL CP4FileStats::IsOtherOpenExclusive() const
{
	if (IsOtherOpen())
	{
		int i;
		CString fileType = GetHeadType();
		if ((i = fileType.Find(_T('+'))) != -1)
		{
			if (fileType.Find(_T('l'),i) != -1)
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CP4FileStats::IsMyOpenExclusive() const
{
	if (IsMyOpen())
	{
		int i;
		CString fileType = GetHeadType();
		if ((i = fileType.Find(_T('+'))) != -1)
		{
			if (fileType.Find(_T('l'),i) != -1)
				return TRUE;
		}
	}
	return FALSE;
}