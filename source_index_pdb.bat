@echo off

setlocal
pushd %~dp0

::
:: Generate the script section file for pdbstr.exe
::

echo:
echo Writing source_index.txt header...

echo SRCSRV: ini ------------------------------------------------ > source_index.txt
echo VERSION=2 >> source_index.txt
echo VERCTRL=http >> source_index.txt
echo SRCSRV: variables ------------------------------------------ >> source_index.txt
echo HTTP_ALIAS=https://raw.githubusercontent.com >> source_index.txt
echo HTTP_EXTRACT_TARGET=%%HTTP_ALIAS%%%%var2%% >> source_index.txt
echo SRCSRVTRG=%%HTTP_EXTRACT_TARGET%% >> source_index.txt
echo SRCSRV: source files --------------------------------------- >> source_index.txt

echo:
echo List Root
call source_index_repo.bat

echo:
echo List Submodules
git submodule foreach --recursive cmd //c "%~dp0source_index_repo.bat"

echo:
echo Writing footer...
echo SRCSRV: end ------------------------------------------------ >> source_index.txt

::
:: Update the pdb file with the script section
::

echo:
echo Adding srcsrv section to %1...
"%ProgramFiles(x86)%\Windows Kits\10\Debuggers\x64\srcsrv\pdbstr.exe" -w -p:%1 -s:srcsrv -i:source_index.txt

popd
endlocal

if %ERRORLEVEL% neq 0 exit /b 1
exit /b 0
