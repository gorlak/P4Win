#include "stdafx.h"
#include "WindowsVersion.h"
#include "RegKeyEx.h"
#include "commonres.h"


DWORD CWindowsVersion::GetDllVersion(LPCTSTR lpszDllName)
{
	HINSTANCE hInstDll = LoadLibrary(lpszDllName);
	if(!hInstDll)
		return 0;

	DWORD dwVersion = 0;
	DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC) GetProcAddress(hInstDll, "DllGetVersion");

	// Because some DLLs might not implement this function, you
	// must test for it explicitly. Depending on the particular 
	// DLL, the lack of a DllGetVersion function can be a useful
	// indicator of the version.
	if(pDllGetVersion)
	{
		DLLVERSIONINFO dvi;
		HRESULT hr;

		ZeroMemory(&dvi, sizeof(dvi));
		dvi.cbSize = sizeof(dvi);

		hr = (*pDllGetVersion)(&dvi);

		if(SUCCEEDED(hr))
		{
			dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
		}
	}
        
	FreeLibrary(hInstDll);
    return dwVersion;
}

CWindowsVersion::CWindowsVersion()
{
	m_osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  
    if(!GetVersionEx(&m_osv))
    {
        ASSERT(0);
        // make sure nothing bad will happen trying to interpret OSVERSIONINFO
        memset((void*)&m_osv, 0, sizeof(m_osv));
    }
    else
    {
        switch(m_osv.dwPlatformId)
        {
        case VER_PLATFORM_WIN32s: //Win32s on Windows 3.1. 
            m_winVer = WV_32S;
            break;

        case VER_PLATFORM_WIN32_WINDOWS: //WIN32 on 95 or 98 or ME
            if(m_osv.dwMinorVersion == 0)
            {
                m_winVer = WV_95;
            }
            else if(m_osv.dwMinorVersion == 10)
            {
                m_winVer = WV_98;
            }
            if(m_osv.dwMinorVersion == 90)
            {
                m_winVer = WV_ME;
            }
            break;

        case VER_PLATFORM_WIN32_NT: //Win32 on Windows NT. 

            if(m_osv.dwMajorVersion == 4) 
            {
                m_winVer = WV_NT4;
            }
            else if(m_osv.dwMajorVersion >= 5)
            {
                m_winVer = WV_2K;
            }
            break;

        default: 
            m_winVer = WV_UNKNOWN;
            break;
        }
    }

    // get the language
    m_UILang = 0;
    switch(m_winVer)
    {
    case WV_2K:
        {
        // Disable this section to emulate Windows NT before Windows 2000, when testing
        // on Windows 2000
        // Use GetUserDefaultUILanguage to find the user's prefered UI language

        // Declare function pointer
        LANGID (WINAPI *pfnGetUserDefaultUILanguage) () = NULL ;

        HMODULE hMKernel32 = LoadLibraryW(L"kernel32.dll") ;
        
        pfnGetUserDefaultUILanguage = 
            (unsigned short (WINAPI *)(void)) 
                GetProcAddress(hMKernel32, "GetUserDefaultUILanguage") ;

        if(NULL != pfnGetUserDefaultUILanguage)
            m_UILang = pfnGetUserDefaultUILanguage() ;
        }
        break;
    case WV_NT4:
        {
            // Running on Windows NT 4.0 or earlier. Get UI language
            // from locale of .default user in registry:
            // HKEY_USERS\.DEFAULT\Control Panel\International\Locale
            CRegKeyEx key;
            if(ERROR_SUCCESS == key.Open(HKEY_USERS, _T(".DEFAULT\\Control Panel\\International"), KEY_READ))
            {
                CString value = key.QueryValueString(_T("Locale"));
                m_UILang = (LANGID)_tcstol(value,NULL,16);
            };
            
        }
        break;
    case WV_95:
    case WV_98:
    case WV_ME:
        {
            // Running on Windows 9x. Get the system UI language from registry:
            CRegKeyEx key;
            if(ERROR_SUCCESS == key.Open(HKEY_USERS, _T(".Default\\Control Panel\\desktop\\ResourceLocale"), KEY_READ))
            {
                CString value = key.QueryValueString(_T(""));
                m_UILang = (LANGID)_tcstol(value, 0, 16);
            };
            
        }
        break;
    }
	// get the common controls dll version
	m_comCtl32Version = GetDllVersion(_T("comctl32.dll"));
}

CString CWindowsVersion::GetVersionString() const
{
    CString buildNum;
    buildNum.FormatMessage(IDS_BUILD_NUMBER_n, BuildNumber());
    CString str;

    switch(m_osv.dwPlatformId)
    {
    case VER_PLATFORM_WIN32s: //Win32s on Windows 3.1. 
        str = _T("Microsoft® Windows 3.1(TM)");
        break;

    case VER_PLATFORM_WIN32_WINDOWS: //WIN32 on 95 or 98				 
        //determine if Win95 or Win98
        if(m_osv.dwMinorVersion == 0)
        {
            str = _T("Microsoft® Windows 95(TM), ") 
                + buildNum;
            if(lstrlen(m_osv.szCSDVersion))
                str += _T(", ") + CString(m_osv.szCSDVersion);
        }
        else if(m_osv.dwMinorVersion == 10)
        {
            str = _T("Microsoft® Windows 98(TM), ") 
                + buildNum;
            if(lstrlen(m_osv.szCSDVersion))
                str += _T(", ") + CString(m_osv.szCSDVersion);
        }
        else if(m_osv.dwMinorVersion == 90)
        {
            str = _T("Microsoft® Windows Millenium(TM), ") 
                + buildNum;
            if(lstrlen(m_osv.szCSDVersion))
                str += _T(", ") + CString(m_osv.szCSDVersion);
        }
        else
        {
            str.Format(_T("Microsoft® Windows %d.%d"), 
                m_osv.dwMajorVersion, m_osv.dwMinorVersion);
        }
        break;

    case VER_PLATFORM_WIN32_NT: //Win32 on Windows NT. 

        if(m_osv.dwMajorVersion == 4) 
        {
            str = _T("Microsoft® Windows NT(TM), ")  
                + buildNum;
            if(lstrlen(m_osv.szCSDVersion))
                str += _T(", ") + CString(m_osv.szCSDVersion);
        }
        else if(m_osv.dwMajorVersion == 5)
        {
			switch(m_osv.dwMinorVersion)
			{
			case 0:
	            str = _T("Microsoft® Windows 2000(TM), ") + buildNum;
				break;
			case 1:
	            str = _T("Microsoft® Windows XP(TM), ") + buildNum;
				break;
			default:
	            str.Format(_T("Microsoft® Windows %d.%d"), 
					m_osv.dwMajorVersion, m_osv.dwMinorVersion);
				break;
			}
            if(lstrlen(m_osv.szCSDVersion))
                str += _T(", ") + CString(m_osv.szCSDVersion);
        }
        else
        {
            str.Format(_T("Microsoft® Windows %d.%d"), 
                m_osv.dwMajorVersion, m_osv.dwMinorVersion);
        }
        break;

    default: 
        break;
    }
    return str;
}//end if

