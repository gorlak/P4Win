@echo off

setlocal

:: https://github.com/gorlak/PowerMateTray
for /f "delims=" %%f in ( 'git remote get-url origin' ) do set ORIGIN=%%f

:: remove .git from the path if it's there (.git is the module url but not the source code url)
SET ORIGIN=%ORIGIN:.git=%

:: 297d3d60af11c8749ab8515eec6ab188da3df317
for /f "delims=" %%f in ( 'git rev-parse head' ) do set COMMIT=%%f

:: /gorlak/PowerMateTray/297d3d60af11c8749ab8515eec6ab188da3df317/
SET URL=%ORIGIN:https://github.com=%/%COMMIT%

:: C:\Users\geoff\Projects\gorlak-PowerMateTray\.gitignore*.gitignore 
for /f "delims=" %%f in ( 'git ls-files -- *.c *.C *.cc *.cp *.cpp *.cxx *.CPP *.h *.hh *.impl *.inl' ) do (call :PROCESS_FILE "%~dp0source_index.txt" "%%f")

endlocal

exit /b 0

::
:: This is necessary to do some subsitutions that aren't possible with a for /f param
::

:PROCESS_FILE
 set TXT=%1
 set FILE=%2
 set FILE_NOQUOTE=%FILE:"=%
 echo %CD%\%FILE_NOQUOTE:/=\%*%URL%/%FILE_NOQUOTE% >> %TXT%
 GOTO :eof