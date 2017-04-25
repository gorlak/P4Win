//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4FileStats.h

#ifndef __TOKENSTRING__
#define __TOKENSTRING__

#define TOKENBUFFERSIZE 2048

class CTokenString 
{
public:
	CTokenString();
	~CTokenString();
	void Create(LPCTSTR str);
    void Reset();
	LPTSTR GetToken(BOOL acceptQuoted = FALSE, BOOL acceptGtLt = FALSE);
	LPTSTR GetRemainder();

	inline void PrepareParse() { ASSERT(m_Initialized); m_Token[0]=_T('\0'); m_pNext= m_pStr; }


protected:
	LPTSTR m_pStr;
	int m_Len;
	BOOL m_Initialized;
	LPTSTR m_pNext;
	TCHAR m_Token[TOKENBUFFERSIZE];
	
};

#endif //__TOKENSTRING__
