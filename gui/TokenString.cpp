//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// TokenString.cpp
#include "stdafx.h"
#include "tokenstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CTokenString::CTokenString()
{
	m_Initialized=FALSE;
	m_pStr=NULL;
}

CTokenString::~CTokenString()
{
    Reset();
}

void CTokenString::Create(LPCTSTR str)
{
    ASSERT(!m_Initialized);
    ASSERT(!m_pStr);

	ASSERT(str != NULL);
	ASSERT(_tcslen(str) > 0);

	m_Len= _tcslen(str);
	m_pStr= new TCHAR[m_Len+1];
	_tcscpy(m_pStr, str);
	m_pNext= m_pStr;
	m_Initialized=TRUE;
}

void CTokenString::Reset()
{
    delete [] m_pStr;
    m_pStr = NULL;
    m_Initialized = FALSE;
}

LPTSTR CTokenString::GetToken(BOOL acceptQuoted /*=FALSE */, BOOL acceptGtLt /*=FALSE*/)
{
	ASSERT(m_Initialized);

	// Skip inital space
	while(*m_pNext == _T(' ') || *m_pNext == _T('\t') ||
			 *m_pNext == _T('\n') || *m_pNext == _T('\r') )
		m_pNext++;

	if(*m_pNext == _T('\0'))
		m_Token[0]=_T('\0');  // hit end of string, we're done
	else
	{
		int tokenLen=0;
		LPTSTR pTok=m_Token;

		// if it's a quoted string, look for ending quote
		if (acceptQuoted && (*m_pNext == _T('\"')))
		{
			do
			{
				*pTok++= *m_pNext++;
				tokenLen++;
			} while ((*m_pNext != _T('\"')) && *m_pNext && (tokenLen < TOKENBUFFERSIZE-1));
		}
		else if (acceptGtLt && (*m_pNext == _T('<')))
		{
			do
			{
				*pTok++= *m_pNext++;
				tokenLen++;
			} while ((*m_pNext != _T('>')) && *m_pNext && (tokenLen < TOKENBUFFERSIZE-1));
		}

		// Copy while not another space
		while(*m_pNext != _T('\0') && *m_pNext != _T('\t') &&
				*m_pNext != _T('\r') && *m_pNext != _T('\n') &&
				*m_pNext != _T(' ') && tokenLen < TOKENBUFFERSIZE-1)
		{
			*pTok++= *m_pNext++;
			tokenLen++;
		}
		ASSERT(tokenLen < TOKENBUFFERSIZE);
		*pTok= _T('\0');
	}
	
	return m_Token;
}

LPTSTR CTokenString::GetRemainder()
{
	ASSERT(m_Initialized);

	int tokenLen=0;
	LPTSTR pTok=m_Token;

	// Copy while not end of string
	while(*m_pNext != _T('\0') &&  tokenLen < TOKENBUFFERSIZE-1)
	{
		*pTok= *m_pNext;
		pTok++;
		m_pNext++;
		tokenLen++;
	}

	*pTok= _T('\0');
	ASSERT(tokenLen < TOKENBUFFERSIZE);
	return m_Token;
}