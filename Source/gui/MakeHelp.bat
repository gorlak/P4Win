@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by P4WIN.HPJ. >"hlp\P4win.hm"
echo. >>"hlp\P4win.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\P4win.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\P4win.hm"
echo. >>"hlp\P4win.hm"
echo // Prompts (IDP_*) >>"hlp\P4win.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\P4win.hm"
echo. >>"hlp\P4win.hm"
echo // Resources (IDR_*) >>"hlp\P4win.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\P4win.hm"
echo. >>"hlp\P4win.hm"
echo // Dialogs (IDD_*) >>"hlp\P4win.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\P4win.hm"
echo . >>"hlp\P4win.hm"
echo // Frame Controls (IDW_*) >>"hlp\P4win.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\P4win.hm"
REM -- Make help for Project P4WIN


echo Building Win32 Help files
rem start /wait hcrtf -x "hlp\P4win.hpj"
echo.
if exist Debug\nul copy "hlp\P4win.hm" Debug
if exist Debug\nul copy "hlp\P4win.hlp" Debug
if exist Debug\nul copy "hlp\P4win.cnt" Debug
if exist Release\nul copy "hlp\P4win.hm" Release
if exist Release\nul copy "hlp\P4win.hlp" Release
if exist Release\nul copy "hlp\P4win.cnt" Release
echo.


