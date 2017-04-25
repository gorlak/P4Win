#pragma once

#ifdef UNICODE

class CharString
{
	UINT m_cp;
	char * m_str;
protected:
	CharString(UINT cp)
	{
		m_str = 0;
		m_cp = cp;
	}
    void Assign(LPCTSTR s) 
    { 
        delete m_str;
        int size = WideCharToMultiByte(m_cp,0,s,-1,0,0,0,0);
        m_str = new char[size+1];
        WideCharToMultiByte(m_cp,0,s,-1,m_str,size,0,0);
		m_str[size] = 0;
    }
    void Assign(CharString const &s) 
    { 
        delete m_str;
        m_str = new char[strlen(s.m_str)+1];
        strcpy(m_str, s.m_str);
    }
    void Assign(CString const &src) 
    {
		Assign((LPCTSTR)src);
    }
public:
	CharString()
	{
		m_cp = 0;
		m_str = 0;
	}
	CharString(CharString const &s)
    {
		m_cp = s.m_cp;
        m_str = new char[strlen(s.m_str)+1];
        strcpy(m_str, s.m_str);
    }
	~CharString()
	{
		delete m_str;
	}
	CharString & operator =(CharString const & s)
	{
		Assign(s);
		return *this;
	}
    CharString & operator =(CString const & s) 
	{
		Assign(s);
		return *this;
	}
    CharString & operator =(LPCTSTR s) 
	{
		Assign(s);
		return *this;
	}
	operator const char *() const { return m_str; }
};

class AnsiString : public CharString
{
public:
	AnsiString() : CharString(CP_ACP)
    {
	}
	AnsiString(CString const &s) : CharString(CP_ACP)
	{
		this->CharString::operator = (s);
	}
	AnsiString(LPCTSTR s) : CharString(CP_ACP)
	{
		this->CharString::operator = (s);
	}
    AnsiString(AnsiString const &s) : CharString(s)
    {
    }
};

class UTF8String : public CharString
{
public:
	UTF8String() : CharString(CP_UTF8)
    {
	}
	UTF8String(CString const &s) : CharString(CP_UTF8)
	{
		this->CharString::operator = (s);
	}
	UTF8String(LPCTSTR s) : CharString(CP_UTF8)
	{
		this->CharString::operator = (s);
	}
    UTF8String(UTF8String const &s) : CharString(s)
    {
    }
};

class UCS2String
{
	WCHAR * ucs2;
public:
	UCS2String(bool utf8, const char *src)
	{
		UINT cp = utf8 ? CP_UTF8 : CP_ACP;
		int size = MultiByteToWideChar(cp,0,src,-1,0,0);
		ucs2 = new WCHAR[size+1];
		MultiByteToWideChar(cp,0,src,-1,ucs2, size);
		ucs2[size] = 0;
	}
	~UCS2String()
	{
		delete [] ucs2;
	}
	operator const WCHAR *() const { return ucs2; }
};
#else
#define CharString CString
#define AnsiString CString
#endif
