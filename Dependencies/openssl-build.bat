@echo off

setlocal

if "%VisualStudioVersion%" equ "" echo Please run this script from a Visual Studio command prompt for the compiler you want to use
if "%VisualStudioVersion%" equ "" exit /b 1

:: make perl.exe available via %PATH%
set PATH=%PATH%;%ProgramFiles%\EShell Menu\StrawberryPerl\perl\bin

if "%VSCMD_ARG_TGT_ARCH%" equ "x64" (
    set PLATFORM=x64
    set CONFIGURE=VC-WIN64A
    set SETUP=ms\do_win64a.bat
) else (
    set PLATFORM=Win32
    set CONFIGURE=VC-WIN32
    set SETUP=ms\do_ms.bat
)

pushd %~dp0openssl

::
:: static crt
::

perl Configure %CONFIGURE% no-asm --prefix=%~dp0\%PLATFORM%-mt
call %SETUP%
nmake -f ms\nt.mak
nmake -f ms\nt.mak install
copy /y tmp32\lib.pdb %~dp0\%PLATFORM%-mt\lib\
nmake -f ms\nt.mak clean

perl Configure debug-%CONFIGURE% no-asm --prefix=%~dp0\%PLATFORM%-mtd
call %SETUP%
nmake -f ms\nt.mak
nmake -f ms\nt.mak install
copy /y tmp32.dbg\lib.pdb %~dp0\%PLATFORM%-mtd\lib\
nmake -f ms\nt.mak clean

::
:: dll crt
::

copy /y ms\nt.mak ms\nt.mak.orig

perl Configure %CONFIGURE% no-asm --prefix=%~dp0\%PLATFORM%-md
call %SETUP%
copy /y ms\nt.mak ms\nt.mak.unhacked
perl -p -e "s/\/MT/\/MD/g" ms\nt.mak.unhacked > ms\nt.mak
nmake -f ms\nt.mak
nmake -f ms\nt.mak install
copy /y tmp32\lib.pdb %~dp0\%PLATFORM%-md\lib\
nmake -f ms\nt.mak clean

perl Configure debug-%CONFIGURE% no-asm --prefix=%~dp0\%PLATFORM%-mdd
call %SETUP%
copy /y ms\nt.mak ms\nt.mak.unhacked
perl -p -e "s/\/MT/\/MD/g" ms\nt.mak.unhacked > ms\nt.mak
nmake -f ms\nt.mak
nmake -f ms\nt.mak install
copy /y tmp32.dbg\lib.pdb %~dp0\%PLATFORM%-mdd\lib\
nmake -f ms\nt.mak clean

copy /y ms\nt.mak.orig ms\nt.mak

::
:: Tidy up workspace
::

git checkout .
git clean -fdx

popd

pushd %~dp0

if exist openssl-install attrib -r openssl-install\*.* /s
if exist openssl-install rmdir /s /q openssl-install

ren %PLATFORM%-mt\lib mt
ren %PLATFORM%-mtd\lib mtd
ren %PLATFORM%-md\lib md
ren %PLATFORM%-mdd\lib mdd

ren %PLATFORM%-mt openssl-install
mkdir openssl-install\lib\vstudio-%VisualStudioVersion%\%PLATFORM%
move openssl-install\mt openssl-install\lib\vstudio-%VisualStudioVersion%\%PLATFORM%\
move %PLATFORM%-mtd\mtd openssl-install\lib\vstudio-%VisualStudioVersion%\%PLATFORM%\
move %PLATFORM%-md\md openssl-install\lib\vstudio-%VisualStudioVersion%\%PLATFORM%\
move %PLATFORM%-mdd\mdd openssl-install\lib\vstudio-%VisualStudioVersion%\%PLATFORM%\

rmdir /s /q %PLATFORM%-mtd
rmdir /s /q %PLATFORM%-md
rmdir /s /q %PLATFORM%-mdd

popd

endlocal
