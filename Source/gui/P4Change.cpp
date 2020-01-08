//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4Change.cpp

#include "stdafx.h"
#include "p4change.h"
#include "tokenstring.h"
#include "p4win.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CP4Change, CObject)


CP4Change::CP4Change()
{
	m_Initialized=FALSE;
}

CP4Change::~CP4Change()
{
	
}

// Eat a row of text from 'P4 changes' that looks like:
// Change 2698 on 02/01/1997 by NIRIAS@ELWOOD *pending* 'Another change 
BOOL CP4Change::Create(LPCTSTR changesRow)
{
	CTokenString str;
	str.Create(changesRow);

	VERIFY(lstrcmp(str.GetToken(), _T("Change"))==0);   // Skip first word, 'Change'

	m_ChangeNumber=_ttol(str.GetToken());
	ASSERT(m_ChangeNumber);

	VERIFY(lstrcmp(str.GetToken(), _T("on"))==0);   // Skip over word, 'on'
	m_ChangeDate=str.GetToken();
	ASSERT(m_ChangeDate.Find(_T("/")));

	CString temp=str.GetToken();
	if (lstrcmp(temp, _T("by"))!=0 && _istdigit(temp.GetAt(0)) && temp.Find(_T(':')) != -1)
	{
		m_ChangeDate += _T(' ') + temp;
		temp=str.GetToken();
	}

	VERIFY(lstrcmp(temp, _T("by"))==0);   // Skip over word, 'by'
	m_UserAtClient=str.GetToken();

	m_MyChange=FALSE;
	int at= m_UserAtClient.Find(_T("@"));
	if( at == -1)
		ASSERT(0);
	else if( Compare( m_UserAtClient.Mid( at+1 ), GET_P4REGPTR()->GetP4Client() ) == 0 )
		m_MyChange=TRUE;
	
	LPTSTR token=str.GetToken();
	if(lstrcmp(token, _T("*pending*"))==0)
	{
		m_Pending=TRUE;
		token=str.GetToken();
	}
	else
		m_Pending=FALSE;


	// Get the description, and expand '\n' characters
	temp= token;
	temp+= str.GetRemainder();
	LPTSTR out=m_Description.GetBuffer(temp.GetLength() * 2);
	LPCTSTR in=temp.GetBuffer(temp.GetLength()+1);
	while(*in != _T('\0'))
	{
		if(*in == _T('\n'))
		{
			*out= _T('\r');
			out++;
		}
		*out++= *in++;
	}
	*out=_T('\0');
	m_Description.ReleaseBuffer();

	// Trim off the leading and trailing quotes, if any
	if(m_Description.GetLength() > 2)
	{
		if(m_Description[0] == _T('\'') && m_Description[m_Description.GetLength()-1]==_T('\'') )
			m_Description= m_Description.Mid(1, m_Description.GetLength()-2);
	}
	m_Initialized=TRUE;
    return TRUE;
}

static void TimestampToFormattedTime( long changeTime, CString *pFormatted )
{
	// There's some weird negative number math going on in CP4FileStats::GetFormattedHeadTime()
	ASSERT(changeTime >= 0);

	struct tm *t = _localtime32( (const __time32_t *)&changeTime ); 
	pFormatted->Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), 
		t->tm_year+1900, t->tm_mon+1, t->tm_mday, 
		t->tm_hour, t->tm_min, t->tm_sec);
}

BOOL CP4Change::Create(StrDict *varlist)
{
 	StrPtr *str;

 	VERIFY( str = varlist->GetVar( "change" ) );
	m_ChangeNumber = atol(!str ? "" : str->Value());
	ASSERT(m_ChangeNumber);

 	VERIFY( str = varlist->GetVar( "time" ) );
	TimestampToFormattedTime( atol(!str ? "0" : str->Value()), &m_ChangeDate );

 	VERIFY( str = varlist->GetVar( "user" ) );
	m_UserAtClient = CharToCString(!str ? "" : str->Value());

	m_UserAtClient.AppendChar( L'@' );

 	VERIFY( str = varlist->GetVar( "client" ) );
	m_UserAtClient.Append( CharToCString(!str ? "" : str->Value()) );

	CString sClient = !str ? "" : str->Value();
	m_MyChange = ( Compare( sClient, GET_P4REGPTR()->GetP4Client() ) == 0 );
	
 	VERIFY( str = varlist->GetVar( "status" ) );
	m_Pending = (strcmp(!str ? "" : str->Value(), "pending")==0);

 	str = varlist->GetVar( "shelved" );
	m_Shelved = (str != NULL);

 	VERIFY( str = varlist->GetVar( "desc" ) );
	m_Description = !str ? "" : str->Value();

	m_Initialized=TRUE;
    return TRUE;
}

CString CP4Change::GetFormattedChange(BOOL showChangeDesc, BOOL sortByUser) const
{
	ASSERT(m_Initialized);

	int lgth = GET_SERVERLEVEL() >= 19 ? GET_P4REGPTR()->GetUseLongChglistDesc() : 31;
	CString desc = TruncateString(m_Description, max(31, lgth));
	BOOL bTrunc  = m_Description.GetLength() >= lgth;
	CString txt;

	if(showChangeDesc && lgth > 31)
	{
		CString txtout;
		int len=desc.GetLength();
		LPTSTR ptr=desc.GetBuffer(len);
		LPTSTR ptrout=txtout.GetBuffer(len);
		for(int i=0; i<len; i++)
		{
			if(*ptr==_T('\r'))
			{
				*ptrout=_T(' ');
				ptrout++;
			}
			else if(*ptr!=_T('\n') && *ptr!=_T('\t'))
			{
				*ptrout=*ptr;
				ptrout++;
			}
			ptr++;
		}
		*ptrout=_T('\0');
		desc = txtout;
		txtout.ReleaseBuffer();
	}

	if( Compare( m_UserAtClient, GET_P4REGPTR()->GetMyID() ) == 0 )
	{
		if(showChangeDesc)
		{
            txt.FormatMessage(!bTrunc ? IDS_CHANGE_n_s : IDS_CHANGE_n_s_TRUNC, 
						m_ChangeNumber, desc);
		}
		else
			txt.FormatMessage(IDS_CHANGE_n, m_ChangeNumber);
	}
	else
	{
		if(sortByUser)
		{
            if(showChangeDesc)
				txt.FormatMessage(!bTrunc ? IDS_CHANGE_s_n_s : IDS_CHANGE_s_n_s_TRUNC, 
						m_UserAtClient, m_ChangeNumber, desc);
			else
				txt.FormatMessage(IDS_CHANGE_USER_s_n, m_UserAtClient, m_ChangeNumber);
		}
		else
		{
			if(showChangeDesc)
				txt.FormatMessage(!bTrunc ? IDS_CHANGE_n_s_s : IDS_CHANGE_n_s_s_TRUNC, 
						m_ChangeNumber, m_UserAtClient, desc);
			else
				txt.FormatMessage(IDS_CHANGE_n_s, m_ChangeNumber, m_UserAtClient);
		}
	}
	


	return txt;
}
