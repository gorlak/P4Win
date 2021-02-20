newoption
{
	trigger	= "client",
	description	= "Build the p4 client",
}

newoption
{
	trigger	= "japanese",
	description	= "Bundle japanese resources",
}

newoption
{
	trigger = "architecture",
	description = "Specify architecture (see premake 'architecture' action for choices)",
	default = (function() if os.is64bit() then return 'x86_64' else return 'x86' end end)(),
}

-- global options

architecture( _OPTIONS[ "architecture" ] )

flags
{
	"FatalWarnings",
	"NoMinimalRebuild",
}

defines
{
	-- win32
	"WIN32",
	"_WIN32",
	"_WINSOCK_DEPRECATED_NO_WARNINGS",

	-- crt
	"_CRT_SECURE_NO_DEPRECATE",
	"_CRT_NON_CONFORMING_SWPRINTFS",
	
	-- p4 libs
	"OS_NT",
	"ID_OS=\"NTX64\"",
	"ID_REL=\"2018.2\"",
	"ID_PATCH=\"gorlak\"",
	"ID_Y=\"0\"",
	"ID_M=\"0\"",
	"ID_D=\"0\"",
	"USE_SSL",
	"Z_PREFIX",
}

buildoptions
{
	"/MP",
	"/Zm256",
	"/d2Zi+", -- http://randomascii.wordpress.com/2013/09/11/debugging-optimized-codenew-in-visual-studio-2012/
}

linkoptions
{
	"/ignore:4221", -- disable warning about linking .obj files with not symbols defined (conditionally compiled away)
}

configurations
{
	"Debug",
	"Release",
}

location "Build"
objdir "Build"

-- global configurations

configuration "Debug"
	targetdir( "Bin/Debug/" )
	libdirs { "Bin/Debug/" }

configuration "Release"
	targetdir( "Bin/Release/" )
	libdirs { "Bin/Release/" }

configuration "Debug"
	defines
	{
		"_DEBUG",
	}
	symbols "On"
	buildoptions
	{
		"/Ob0",
	}

configuration "Release"
	defines
	{
		"NDEBUG",
	}
	symbols "On"
	optimize "Speed"
	editandcontinue "Off"
	--omitframepointer "On"
	buildoptions
	{
		"/Ob2",
		"/Oi",
	}

configuration {"Debug", "x86"}
	includedirs
	{
		"Dependencies/openssl-Win32-mdd/include",
	}
	libdirs
	{
		"Dependencies/openssl-Win32-mdd/lib"
	}

configuration {"not Debug", "x86"}
	includedirs
	{
		"Dependencies/openssl-Win32-md/include",
	}
	libdirs
	{
		"Dependencies/openssl-Win32-md/lib"
	}

configuration {"Debug", "x86_64"}
	includedirs
	{
		"Dependencies/openssl-x64-mdd/include",
	}
	libdirs
	{
		"Dependencies/openssl-x64-mdd/lib"
	}

configuration {"not Debug", "x86_64"}
	includedirs
	{
		"Dependencies/openssl-x64-md/include",
	}
	libdirs
	{
		"Dependencies/openssl-x64-md/lib"
	}

configuration {}

-- workspace

workspace "P4Win"

-- projects

project "librpc"

	kind "StaticLib"
	language "C++"
	characterset "MBCS"
	-- staticruntime "On"

	disablewarnings
	{
		"4018", -- sized/unsigned mismatched
		"4099", -- class vs. struct
		"4244", -- int conversion possible loss of data
		"4267", -- size_t conversion possible loss of data
		"4996", -- deprecation
	}

	includedirs
	{
		"Dependencies/p4/net",
		"Dependencies/p4/msgs",
		"Dependencies/p4/rpc",
		"Dependencies/p4/support",
		"Dependencies/p4/sys",
		"Dependencies/p4/zlib",
	}

	files
	{
		"Dependencies/p4/net/**.c",
		"Dependencies/p4/net/**.cc",
		"Dependencies/p4/net/**.h",
		"Dependencies/p4/rpc/**.c",
		"Dependencies/p4/rpc/**.cc",
		"Dependencies/p4/rpc/**.h",
	}

	excludes
	{
	}

project "libsupp"

	kind "StaticLib"
	language "C++"
	characterset "MBCS"
	-- staticruntime "On"

	disablewarnings
	{
		"4018", -- sized/unsigned mismatched
		"4099", -- class vs. struct
		"4101", -- unreferenced local
		"4244", -- int conversion possible loss of data
		"4267", -- size_t conversion possible loss of data
		"4309", -- truncation of constant value
		"4996", -- deprecation
	}

	includedirs
	{
		"Dependencies/p4/diff",
		"Dependencies/p4/i18n",
		"Dependencies/p4/map",
		"Dependencies/p4/msgs",
		"Dependencies/p4/net",
		"Dependencies/p4/script",
		"Dependencies/p4/support",
		"Dependencies/p4/sys",
		"Dependencies/p4/zlib",
	}

	files
	{
		"Dependencies/p4/diff/**.c",
		"Dependencies/p4/diff/**.cc",
		"Dependencies/p4/diff/**.h",
		"Dependencies/p4/i18n/**.c",
		"Dependencies/p4/i18n/**.cc",
		"Dependencies/p4/i18n/**.h",
		"Dependencies/p4/map/**.c",
		"Dependencies/p4/map/**.cc",
		"Dependencies/p4/map/**.h",
		"Dependencies/p4/msgs/**.c",
		"Dependencies/p4/msgs/**.cc",
		"Dependencies/p4/msgs/**.h",
		"Dependencies/p4/support/**.c",
		"Dependencies/p4/support/**.cc",
		"Dependencies/p4/support/**.h",
		"Dependencies/p4/sys/**.c",
		"Dependencies/p4/sys/**.cc",
		"Dependencies/p4/sys/**.h",
		"Dependencies/p4/zlib/**.c",
		"Dependencies/p4/zlib/**.cc",
		"Dependencies/p4/zlib/**.h",
	}

	excludes
	{
		"Dependencies/p4/sys/fileiovms.*",
		"Dependencies/p4/sys/macfile.*",
	}

project "libscript-sqlite"

	kind "StaticLib"
	language "C++"
	characterset "MBCS"
	-- staticruntime "On"

	defines
	{
		"SQLITE_THREADSAFE=0",
		"SQLITE_DEFAULT_MEMSTATUS=0",
		"SQLITE_DEFAULT_WAL_SYNCHRONOUS=1",
		"SQLITE_LIKE_DOESNT_MATCH_BLOBS",
		"SQLITE_OMIT_SHARED_CACHE",
		"SQLITE_DEFAULT_FILE_PERMISSIONS=0600",
		"SQLITE_ENABLE_API_ARMOR",
		"SQLITE_ENABLE_JSON1",
	}

	files
	{
		"Dependencies/p4/script/sqlite3.*",
	}

project "libscript-curl"

	kind "StaticLib"
	language "C++"
	characterset "MBCS"
	-- staticruntime "On"

	defines
	{
		"CURL_STATICLIB",
		"BUILDING_LIBCURL",
		"CURL_DISABLE_FTP",
		"CURL_DISABLE_LDAP",
		"CURL_DISABLE_TELNET",
		"CURL_DISABLE_DICT",
		"CURL_DISABLE_FILE",
		"CURL_DISABLE_TFTP",
		"CURL_DISABLE_IMAP",
		"CURL_DISABLE_POP3",
		"USE_OPENSSL",
		"HAVE_ZLIB_H",
		"HAVE_ZLIB",
		"HAVE_LIBZ",
		"USE_IPV6",
		"CURL_DISABLE_NTLM",
		"CURL_DISABLE_GOPHER",
		"CURL_DISABLE_RTSP",
	}

	includedirs
	{
		"Dependencies/p4/script",
		"Dependencies/p4/script/libs/cURL",
		"Dependencies/p4/script/lua-5.3",
		"Dependencies/p4/zlib",
	}

	files
	{
		"Dependencies/p4/script/libs/cURL/**.c",
		"Dependencies/p4/script/libs/cURL/**.h",
	}

project "libscript"

	kind "StaticLib"
	language "C++"
	characterset "MBCS"
	-- staticruntime "On"

	disablewarnings
	{
		"4018", -- sized/unsigned mismatched
		"4099", -- class vs. struct
		"4101", -- unreferenced local
		"4244", -- int conversion possible loss of data
		"4267", -- size_t conversion possible loss of data
		"4302", -- type conversion truncation
		"4311", -- truncation from pointer to long
		"4996", -- deprecation
	}

	includedirs
	{
		"Dependencies/p4/client",
		"Dependencies/p4/diff",
		"Dependencies/p4/i18n",
		"Dependencies/p4/map",
		"Dependencies/p4/msgs",
		"Dependencies/p4/net",
		"Dependencies/p4/script",
		"Dependencies/p4/script/lua-5.3",
		"Dependencies/p4/support",
		"Dependencies/p4/sys",
		"Dependencies/p4/zlib",
	}

	files
	{
		"Dependencies/p4/script/libs/cjson/lua_cjson.cc",
		"Dependencies/p4/script/*.c",
		"Dependencies/p4/script/*.cc",
		"Dependencies/p4/script/*.h",
	}

	excludes
	{
		"Dependencies/p4/script/sqlite3.*",
	}

project "libclient"

	kind "StaticLib"
	language "C++"
	characterset "MBCS"
	-- staticruntime "On"

	disablewarnings
	{
		"4005", -- macro redefinition, see WIN32_LEAN_AND_MEAN below
		"4018", -- sized/unsigned mismatched
		"4099", -- class vs. struct
		"4244", -- int conversion possible loss of data
		"4267", -- size_t conversion possible loss of data
		"4996", -- deprecation
	}

	defines
	{
		"WIN32_LEAN_AND_MEAN", -- necessary for <rpc.h> debacle
	}

	includedirs
	{
		"Dependencies/p4/client",
		"Dependencies/p4/diff",
		"Dependencies/p4/i18n",
		"Dependencies/p4/map",
		"Dependencies/p4/msgs",
		"Dependencies/p4/net",
		"Dependencies/p4/rpc",
		"Dependencies/p4/script",
		"Dependencies/p4/support",
		"Dependencies/p4/sys",
		"Dependencies/p4/zlib",
	}

	files
	{
		"Dependencies/p4/client/**.c",
		"Dependencies/p4/client/**.cc",
		"Dependencies/p4/client/**.h",
		"Dependencies/p4/web/**.c",
		"Dependencies/p4/web/**.cc",
		"Dependencies/p4/web/**.h",
	}

	excludes
	{
		"Dependencies/p4/client/clientmain.*",
	}

project "p4"

	kind "ConsoleApp"
	language "C++"
	characterset "MBCS"
	-- staticruntime "On"

	disablewarnings
	{
		"4005", -- macro redefinition, see WIN32_LEAN_AND_MEAN below
		"4018", -- sized/unsigned mismatched
		"4091", -- dbghelp.dll antics
		"4101", -- unreferenced local
		"4244", -- int conversion possible loss of data
		"4996", -- deprecation
	}

	defines
	{
		"WIN32_LEAN_AND_MEAN", -- necessary for <rpc.h> debacle
	}

	includedirs
	{
		"Dependencies/openssl-install/include",
		"Dependencies/p4/client",
		"Dependencies/p4/diff",
		"Dependencies/p4/i18n",
		"Dependencies/p4/map",
		"Dependencies/p4/msgs",
		"Dependencies/p4/net",
		"Dependencies/p4/rpc",
		"Dependencies/p4/script",
		"Dependencies/p4/support",
		"Dependencies/p4/sys",
		"Dependencies/p4/zlib",
	}

	files
	{
		"Dependencies/p4/client/clientmain.*",
		"Source/p4/**"
	}

	links
	{
		"librpc",
		"libsupp",
		"libscript",
		"libscript-curl",
		"libscript-sqlite",
		"libclient",
		"ssleay32",
		"libeay32",
		"dbghelp",
		"ws2_32",
		"wininet",
		"setargv.obj",
	}

project "P4Win"

	kind "WindowedApp"
	language "C++"
	characterset "Unicode"
	-- staticruntime "On"

	flags
	{
		"MFC",
	}

	disablewarnings
	{
		"4091", -- dbghelp.dll antics
		"4996", -- deprecation
	}

	defines
	{
		"UNICODE=1",
	}

	includedirs
	{
		"Dependencies/p4/client",
		"Dependencies/p4/diff",
		"Dependencies/p4/i18n",
		"Dependencies/p4/map",
		"Dependencies/p4/msgs",
		"Dependencies/p4/net",
		"Dependencies/p4/script",
		"Dependencies/p4/support",
		"Dependencies/p4/sys",
		"Dependencies/p4/zlib",
		"Source/common",
		"Source/gui",
		"Source/gui/p4api",
		"Source/gui/spec-dlgs",
		"Source/gui/OptionsDlg",
	}

	files
	{
		"Source/common/**.cpp",
		"Source/common/**.h",
		"Source/gui/**.cpp",
		"Source/gui/**.h",
		"Source/gui/res/P4Win.manifest",
	}

	if _OPTIONS["japanese"] then
		files
		{
			"Source/gui/P4Win411/P4Win411.rc",
		}
	else
		files
		{
			"Source/gui/P4Win409/P4Win409.rc",
		}
	end

	pchheader( "StdAfx.h" )
	pchsource( "Source/gui/StdAfx.cpp" )

	linkoptions
	{
		"/manifestuac:no"
	}

	links
	{
		"librpc",
		"libsupp",
		"libscript",
		"libscript-curl",
		"libscript-sqlite",
		"libclient",
		"ssleay32",
		"libeay32",
		"dbghelp",
		"ws2_32",
		"wininet",
		"winmm",
	}
