@echo off

setlocal

if "%VisualStudioVersion%" equ "" echo Please run this script from a Visual Studio command prompt for the compiler you want to use
if "%VisualStudioVersion%" equ "" exit /b 1

:: make perl.exe available via %PATH%
set PATH=%PATH%;%ProgramFiles%\EShell Menu\StrawberryPerl\perl\bin

pushd %~dp0openssl

perl Configure VC-WIN64A no-asm --prefix=%~dp0\x64-mt
call ms\do_win64a.bat
nmake -f ms\nt.mak
nmake -f ms\nt.mak install
copy /y tmp32\lib.pdb %~dp0\x64-mt\lib\
nmake -f ms\nt.mak clean

perl Configure debug-VC-WIN64A no-asm --prefix=%~dp0\x64-mtd
call ms\do_win64a.bat
nmake -f ms\nt.mak
nmake -f ms\nt.mak install
copy /y tmp32.dbg\lib.pdb %~dp0\x64-mtd\lib\
nmake -f ms\nt.mak clean

::
:: I'm sorry, but this is honestly the best answer @gorlak
::

copy /y ms\nt.mak ms\nt.mak.orig

perl Configure VC-WIN64A no-asm --prefix=%~dp0\x64-md
call ms\do_win64a.bat
copy /y ms\nt.mak ms\nt.mak.unhacked
perl -p -e "s/\/MT/\/MD/g" ms\nt.mak.unhacked > ms\nt.mak
nmake -f ms\nt.mak
nmake -f ms\nt.mak install
copy /y tmp32\lib.pdb %~dp0\x64-md\lib\
nmake -f ms\nt.mak clean

perl Configure debug-VC-WIN64A no-asm --prefix=%~dp0\x64-mdd
call ms\do_win64a.bat
copy /y ms\nt.mak ms\nt.mak.unhacked
perl -p -e "s/\/MT/\/MD/g" ms\nt.mak.unhacked > ms\nt.mak
nmake -f ms\nt.mak
nmake -f ms\nt.mak install
copy /y tmp32.dbg\lib.pdb %~dp0\x64-mdd\lib\
nmake -f ms\nt.mak clean

copy /y ms\nt.mak.orig ms\nt.mak

git checkout .
git clean -fdx

popd

pushd %~dp0

if exist openssl-install attrib -r openssl-install\*.* /s
if exist openssl-install rmdir /s /q openssl-install

ren x64-mt\lib mt
ren x64-mtd\lib mtd
ren x64-md\lib md
ren x64-mdd\lib mdd

ren x64-mt openssl-install
mkdir openssl-install\lib\vstudio-%VisualStudioVersion%\x64
move openssl-install\mt openssl-install\lib\vstudio-%VisualStudioVersion%\x64\
move x64-mtd\mtd openssl-install\lib\vstudio-%VisualStudioVersion%\x64\
move x64-md\md openssl-install\lib\vstudio-%VisualStudioVersion%\x64\
move x64-mdd\mdd openssl-install\lib\vstudio-%VisualStudioVersion%\x64\

rmdir /s /q x64-mtd
rmdir /s /q x64-md
rmdir /s /q x64-mdd

popd

endlocal
