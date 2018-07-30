
#include <atlbase.h>

class CRegKeyEx : public CRegKey
{
public:
    // unhide versions of Create and Open from CRegKey that take strings
    using CRegKey::Create;
    using CRegKey::Open;

    LONG Open(HKEY hKeyParent, UINT nKeyNameID,
		REGSAM samDesired = KEY_ALL_ACCESS)
    {
        CString keyName;
        if(keyName.LoadString(nKeyNameID))
            return CRegKey::Open(hKeyParent, keyName, samDesired);
        else
            return 2;
    }
    
    LONG Create(HKEY hKeyParent, UINT nKeyNameID, LPTSTR lpszClass = REG_NONE, 
        DWORD dwOptions = REG_OPTION_NON_VOLATILE, 
        REGSAM samDesired = KEY_ALL_ACCESS, LPSECURITY_ATTRIBUTES lpSecAttr = NULL, 
        LPDWORD lpdwDisposition = NULL)
    {
        CString keyName;
        if(keyName.LoadString(nKeyNameID))
            return CRegKey::Create(hKeyParent, keyName, lpszClass, dwOptions, samDesired, lpSecAttr, lpdwDisposition);
        else
            return 2;
    }
    
    CString QueryValueString(LPCTSTR valueName, LPCTSTR defaultValue = _T(""), LONG *pSuccess = NULL)
    {
        ASSERT(valueName);
        ASSERT(defaultValue);

        CString result = defaultValue;
        DWORD bufLen = 0;
#if _MSC_VER >= 1300
        DWORD type;
        LONG success = CRegKey::QueryValue(valueName, &type, 0, &bufLen);
        if(ERROR_SUCCESS == success && type == REG_SZ)
#else
        LONG success = CRegKey::QueryValue(0, valueName, &bufLen);
        if(ERROR_SUCCESS == success)
#endif
        {
            CString value;
#if _MSC_VER >= 1300
            LONG success = CRegKey::QueryStringValue(valueName, value.GetBufferSetLength(bufLen/sizeof(TCHAR)-1), &bufLen);
#else
            LONG success = CRegKey::QueryValue(value.GetBufferSetLength(bufLen/sizeof(TCHAR)-1), valueName, &bufLen);
#endif
            if(pSuccess)
                *pSuccess = success;
            if(ERROR_SUCCESS == success)
                result = value;
        }
        else
        {
            if(pSuccess)
                *pSuccess = success;
        }
        return result;
    }
    
    CString QueryValueString(UINT nValueNameID, LPCTSTR defaultValue = _T(""), LONG *pSuccess = NULL)
    {
        CString valueName;
        if(valueName.LoadString(nValueNameID))
        {
            return QueryValueString(valueName, defaultValue, pSuccess);
        }
        else
        {
            // couldn't find string for value name in stringtable
            if(pSuccess)
                *pSuccess = 2;
            return defaultValue;
        }
    }

    LONG SetValueString(LPCTSTR value, LPCTSTR valueName)
    {
#if _MSC_VER >= 1300
        return SetStringValue(valueName, value);
#else
        return SetValue(value, valueName);
#endif
    }
    
    LONG SetValueString(LPCTSTR value, UINT nValueNameID)
    {
        CString valueName;
        if(valueName.LoadString(nValueNameID))
        {
            return SetValueString(value, valueName);
        }
        else
        {
            // couldn't find string for value name in stringtable
            return 2;
        }
    }

    DWORD QueryValueLong(LPCTSTR valueName, DWORD defaultValue = 0, LONG *pSuccess = NULL)
    {
        DWORD result = defaultValue;
#if _MSC_VER >= 1300
        LONG success = CRegKey::QueryDWORDValue(valueName, result);
#else
        LONG success = CRegKey::QueryValue(result, valueName);
#endif
        if(pSuccess)
            *pSuccess = success;
        return result;
    }
    
    DWORD QueryValueLong(UINT nValueNameID, DWORD defaultValue = 0, LONG *pSuccess = NULL)
    {
        CString valueName;
        if(valueName.LoadString(nValueNameID))
        {
            return QueryValueLong(valueName, defaultValue, pSuccess);
        }
        else
        {
            // couldn't find string for value name in stringtable
            if(pSuccess)
                *pSuccess = 2;
            return defaultValue;
        }
    }
    
    LONG SetValueLong(UINT value, LPCTSTR valueName)
    {
#if _MSC_VER >= 1300
        return SetDWORDValue(valueName, value);
#else
        return SetValue(value, valueName);
#endif
    }

    LONG SetValueLong(UINT value, UINT nValueNameID)
    {
        CString valueName;
        if(valueName.LoadString(nValueNameID))
        {
            return SetValueLong(value, valueName);
        }
        else
        {
            // couldn't find string for value name in stringtable
            return 2;
        }
    }


};

