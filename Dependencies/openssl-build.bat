@echo off

setlocal

if "%VisualStudioVersion%" equ "" echo Please run this script from a Visual Studio command prompt for the compiler you want to use
if "%VisualStudioVersion%" equ "" exit /b 1

:: make perl.exe available via %PATH%
set PATH=%PATH%;%~dp0StrawberryPerl\perl\bin

if "%VSCMD_ARG_TGT_ARCH%" equ "x64" (
    set PLATFORM=x64
    set CONFIGURE=VC-WIN64A
    set SETUP=ms\do_win64a.bat
) else (
    set PLATFORM=Win32
    set CONFIGURE=VC-WIN32
    set SETUP=ms\do_ms.bat
)

if "%1" equ "" set BUILD_DEBUG=1
if "%1" equ "" set BUILD_RELEASE=1
if "%1" equ "debug" set BUILD_DEBUG=1
if "%1" equ "release" set BUILD_RELEASE=1

set RUNTIME=d

pushd %~dp0openssl

::
:: Backup
::

if "%RUNTIME%" equ "d" copy /y ms\nt.mak ms\nt.mak.orig

::
:: Release
::

if defined BUILD_RELEASE rmdir /s /q %~dp0\openssl-%PLATFORM%-m%RUNTIME%
if defined BUILD_RELEASE perl Configure %CONFIGURE% no-asm --prefix=%~dp0\openssl-%PLATFORM%-m%RUNTIME%
if defined BUILD_RELEASE call %SETUP%

if defined BUILD_RELEASE if "%RUNTIME%" equ "d" copy /y ms\nt.mak ms\nt.mak.unhacked
if defined BUILD_RELEASE if "%RUNTIME%" equ "d" perl -p -e "s/\/MT/\/MD/g" ms\nt.mak.unhacked > ms\nt.mak

if defined BUILD_RELEASE nmake -f ms\nt.mak
if defined BUILD_RELEASE nmake -f ms\nt.mak install
if defined BUILD_RELEASE copy /y tmp32\lib.pdb %~dp0\openssl-%PLATFORM%-m%RUNTIME%\lib\
if defined BUILD_RELEASE nmake -f ms\nt.mak clean

::
:: Debug
::

if defined BUILD_DEBUG rmdir /s /q %~dp0\openssl-%PLATFORM%-m%RUNTIME%d
if defined BUILD_DEBUG perl Configure debug-%CONFIGURE% no-asm --prefix=%~dp0\openssl-%PLATFORM%-m%RUNTIME%d
if defined BUILD_DEBUG call %SETUP%

if defined BUILD_DEBUG if "%RUNTIME%" equ "d" copy /y ms\nt.mak ms\nt.mak.unhacked
if defined BUILD_DEBUG if "%RUNTIME%" equ "d" perl -p -e "s/\/MT/\/MD/g" ms\nt.mak.unhacked > ms\nt.mak

if defined BUILD_DEBUG nmake -f ms\nt.mak
if defined BUILD_DEBUG nmake -f ms\nt.mak install
if defined BUILD_DEBUG copy /y tmp32.dbg\lib.pdb %~dp0\openssl-%PLATFORM%-m%RUNTIME%d\lib\
if defined BUILD_DEBUG nmake -f ms\nt.mak clean

::
:: Restore
::

if "%RUNTIME%" equ "d" copy /y ms\nt.mak.orig ms\nt.mak

::
:: Tidy
::

git checkout .
git clean -fdx

popd

endlocal
