newoption
{
	trigger	= "japanese",
	description	= "Bundle japanese resources",
}

workspace "P4Win"

-- global options

architecture "x86_64"
floatingpoint "Fast"

flags
{
	"NoMinimalRebuild",
}

defines
{
	"WIN32",
	"_WIN32",
	"_CRT_SECURE_NO_DEPRECATE",
	"_CRT_NON_CONFORMING_SWPRINTFS",
	"_WINSOCK_DEPRECATED_NO_WARNINGS",
	"_AFXDLL=1",
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
	omitframepointer "On"
	buildoptions
	{
		"/Ob2",
		"/Oi",
	}

configuration {}

-- projects

project "P4"

	kind "StaticLib"
	language "C++"
	characterset "MBCS"

	defines
	{
		"WIN32_LEAN_AND_MEAN", -- necessary for <rpc.h> debacle
		"OS_NT",
		"ID_OS=\"NTX64\"",
		"ID_REL=\"2018.1\"",
		"ID_PATCH=\"gorlak\"",
		"ID_Y=\"0\"",
		"ID_M=\"0\"",
		"ID_D=\"0\"",
		"Z_PREFIX",
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
		"Dependencies/p4/support",
		"Dependencies/p4/sys",
		"Dependencies/p4/zlib",
	}

	files
	{
		"Dependencies/p4/**.c",
		"Dependencies/p4/**.cc",
		"Dependencies/p4/**.h",
	}

	excludes
	{
		"Dependencies/p4/api/p4dvcsapi.*",
		"Dependencies/p4/client/clientmain.*",
		"Dependencies/p4/sys/fileiovms.*",
		"Dependencies/p4/sys/macfile.*",
	}

project "P4Win"

	kind "WindowedApp"
	language "C++"
	characterset "Unicode"

	flags
	{
		"FatalWarnings",
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
		"Dependencies/openssl-install/include",
		"Dependencies/p4/client",
		"Dependencies/p4/diff",
		"Dependencies/p4/i18n",
		"Dependencies/p4/map",
		"Dependencies/p4/msgs",
		"Dependencies/p4/net",
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
		"Source/**.cpp",
		"Source/**.h",
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

	links
	{
		"P4",
		"dbghelp",
		"ws2_32",
		"wininet",
		"winmm",
	}
