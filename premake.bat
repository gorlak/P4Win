@echo off

for /f %%f in ( '"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -property catalog_productLineVersion' ) do set VSVERSION=%%f

pushd %~dp0Dependencies\premake
if not exist bin\release\premake5.exe nmake -f Bootstrap.mak MSDEV=vs%VSVERSION% windows-msbuild
popd

"%~dp0Dependencies\premake\bin\release\premake5.exe" %*
