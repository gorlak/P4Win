#ifndef __STRINGUTIL__
#define __STRINGUTIL__

inline int GetANumber(CString &str)
{
	int num=0;
	int comma=str.Find(_T(','));

	if(comma != -1)
	{
		num= _ttol(str.Left(comma));
		str=str.Mid(comma+1);
	}
	else
	{
		num= _ttol(str);
		str.Empty();
	}

	return num;
}

inline int GetPositiveNumber(CString &str)
{
    int value = GetANumber(str);
    if(value < 0)
        return 0;
    else
        return value;
}

inline CString LoadStringResource(UINT nID)
{
    // detect missing string resources
    CString result;
    if(!result.LoadString(nID))
    {
    	TRACE1("Error: failed to load string resource 0x%04x.\n", nID);
        // Avoid an annoying assert when displaying a message
        // box to say that we couldn't load the resource dll
    	ASSERT(AfxGetResourceHandle() == AfxGetInstanceHandle());
    }
    return result;
}

inline CString TruncateString(LPCTSTR str, int limit)
{
	// truncate a string so that it has at most limit characters
	// don't truncate mid-character
	LPCTSTR pEnd = str;
	while(limit-- && *pEnd)
		pEnd = CharNext(pEnd);
	return CString(str, pEnd - str);
}

inline void ConvertDates( CString &date1, CString &date2 )
{
	ASSERT( date1[ 2 ] == _T('/') || date1[ 4 ] == _T('/') );

	if( date1[ 2 ] == _T('/') )
	{
		ASSERT( date1[ 5 ] == _T('/') );
		date1 = date1.Mid( 6 ) + date1.Left( 2 ) + date1.Mid( 3,2 );
		date2 = date2.Mid( 6 ) + date2.Left( 2 ) + date2.Mid( 3,2 );  
	}
}

// a handy macro for determining # of bytes or wchars in static strings
#define STRLEN(str) (sizeof(str)/sizeof(TCHAR) - 1)

// provide more familiar sounding names for some generic string functions
// some of these are provided by the Shell Lightweight API
#include <shlwapi.h>
//#define StrNCmp _tcsncmp
//#define StrCmp  _tcscmp
//#define StrStr  _tcsstr
//#define StrChr  _tcschr
#define StrLen  lstrlen
//#define StrCSpn _tcscspn
#endif // __STRINGUTIL__