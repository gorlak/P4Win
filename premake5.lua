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

workspace "P4Win"

-- global options

architecture( _OPTIONS[ "architecture" ] )

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

configuration {}

-- projects

project "P4API"

	kind "StaticLib"
	language "C++"
	characterset "MBCS"
	-- staticruntime "On"

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
		"USE_SSL",
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
		"Dependencies/p4/sslstub/**",
		"Dependencies/p4/sys/fileiovms.*",
		"Dependencies/p4/sys/macfile.*",
	}

project "P4Win"

	kind "WindowedApp"
	language "C++"
	characterset "Unicode"
	-- staticruntime "On"

	flags
	{
		"MFC",
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

	local platform = "x64"
	if _OPTIONS[ "architecture" ] == "x86" then
		platform = "Win32"
	end

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
		"P4API",
		"ssleay32",
		"libeay32",
		"dbghelp",
		"ws2_32",
		"wininet",
		"winmm",
	}

	configuration "Debug"
		libdirs
		{
			"Dependencies/openssl-install/lib/vstudio-$(VisualStudioVersion)/" .. platform .. "/mdd"
		}

	configuration "not Debug"
		libdirs
		{
			"Dependencies/openssl-install/lib/vstudio-$(VisualStudioVersion)/" .. platform .. "/md"
		}

project "P4"

	kind "ConsoleApp"
	language "C++"
	characterset "MBCS"
	-- staticruntime "On"

	disablewarnings
	{
		"4091", -- dbghelp.dll antics
		"4996", -- deprecation
	}

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
		"USE_SSL",
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

	local platform = "x64"
	if _OPTIONS[ "architecture" ] == "x86" then
		platform = "Win32"
	end

	files
	{
		"Dependencies/p4/client/clientmain.*",
		"Source/p4/**"
	}

	links
	{
		"P4API",
		"ssleay32",
		"libeay32",
		"dbghelp",
		"ws2_32",
		"wininet",
		"setargv.obj",
	}

	configuration "Debug"
		libdirs
		{
			"Dependencies/openssl-install/lib/vstudio-$(VisualStudioVersion)/" .. platform .. "/mdd"
		}

	configuration "not Debug"
		libdirs
		{
			"Dependencies/openssl-install/lib/vstudio-$(VisualStudioVersion)/" .. platform .. "/md"
		}
