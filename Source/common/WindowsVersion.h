#ifndef __WINDOWSVERSION__
#define __WINDOWSVERSION__

#define PACKVERSION(major,minor) MAKELONG(minor,major)

class CWindowsVersion
{
    enum WinVer
    {
        WV_UNKNOWN,
        WV_32S, // Win32s on Win3.1
        WV_95,
        WV_98,
        WV_ME,
        WV_NT4,
        WV_2K,
    };
public:
	CWindowsVersion();
    DWORD MajorVersion() const { return m_osv.dwMajorVersion; }
    DWORD MinorVersion() const { return m_osv.dwMinorVersion; }
    DWORD BuildNumber() const { return IsNT() ? m_osv.dwBuildNumber : LOWORD(m_osv.dwBuildNumber); }
    DWORD PlatformId() const { return m_osv.dwPlatformId; }
    CString CSDVersion() const { return CString(m_osv.szCSDVersion); }
    CString GetVersionString() const;
    bool IsNT() const { return m_osv.dwPlatformId == VER_PLATFORM_WIN32_NT; }

    WinVer Version() const { return m_winVer; }
    
    LANGID GetUILanguage() const { return m_UILang; }

	static DWORD GetDllVersion(LPCTSTR dllName);
	DWORD GetComCtl32Version() const { return m_comCtl32Version; }
protected:
    WinVer m_winVer;
	OSVERSIONINFO m_osv;
    CString m_versionString;
    LANGID  m_UILang;
	DWORD m_comCtl32Version;
};

#endif __WINDOWSVERSION__