; This script uses Inno Setup Preprocessor (ISPP) by Alex Yackimoff.
; To download and install ISPP, get the Inno Setup QuickStart Pack from http://www.jrsoftware.org/isdl.php#qsp

#define _AppName          	"P4Win"
#define _AppMutex         	"Running P4Win Instance"
#define _AppPublisher     	"gorlak"
#define _AppPublisherURL  	"https://github.com/gorlak/P4Win"

#ifndef _AppVersionMajor
#define _AppVersionMajor	"0"
#endif

#ifndef _AppVersionMinor
#define _AppVersionMinor	"0"
#endif

#ifndef _AppVersionPatch
#define _AppVersionPatch	"0"
#endif

#define _AppVersion			_AppVersionMajor + "." + _AppVersionMinor + "." + _AppVersionPatch

#ifndef _BuildConfig
#define _BuildConfig		"Release"
#endif

#define _VersionInfoComments    "P4Win is a Windows client for Perforce (Helix VCS) built in MFC."
#define _VersionInfoCopyright   "Copyright (C) " + _AppPublisher
#define _VersionInfoDescription "P4WIn"
#define _VersionInfoVersion     _AppVersion

[Setup]
AllowNoIcons=yes
AlwaysShowDirOnReadyPage=yes
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{1068CD05-61D1-4371-96EC-FD6AA4BC9E27}
AppName={#_AppName}
AppMutex={#_AppMutex}
AppVerName={cm:NameAndVersion,{#_AppName},{#_AppVersion}}
AppPublisher={#_AppPublisher}
AppPublisherURL={#_AppPublisherURL}
AppSupportURL={#_AppPublisherURL}
AppUpdatesURL={#_AppPublisherURL}
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
Compression=lzma
CreateUninstallRegKey=yes
DefaultDirName={pf64}\{#_AppName}
DisableDirPage=no
DefaultGroupName={#_AppName}
OutputDir=.\
OutputBaseFilename=P4WinSetup
PrivilegesRequired=admin
SetupIconFile=Source\gui\res\P4win.ico
SolidCompression=yes
Uninstallable=yes
UninstallDisplayIcon={uninstallexe}
UsePreviousAppDir=yes
UsePreviousGroup=yes
UsePreviousTasks=yes
VersionInfoCompany={#_AppPublisher}
VersionInfoCopyright={#_AppName} {#_AppVersion}
VersionInfoDescription={#_VersionInfoDescription}
VersionInfoProductName={#_AppName}
VersionInfoProductVersion={#_VersionInfoVersion}
VersionInfoTextVersion={#_VersionInfoVersion}
VersionInfoVersion={#_VersionInfoVersion}

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: "Bin\{#_BuildConfig}\P4Win.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "Bin\{#_BuildConfig}\P4Win.pdb"; DestDir: "{app}"; Flags: ignoreversion
Source: "Bin\{#_BuildConfig}\P4.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "Bin\{#_BuildConfig}\P4.pdb"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#_AppName}"; Filename: "{app}\P4Win.exe"; Comment: {#_VersionInfoComments}
Name: "{group}\{cm:UninstallProgram,{#_AppName}}"; Filename: "{uninstallexe}"; Comment: {#_VersionInfoComments}
Name: "{commondesktop}\{#_AppName}"; Filename: "{app}\P4Win.exe"; Comment: {#_VersionInfoComments}; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#_AppName}"; Filename: "{app}\P4Win.exe"; Comment: {#_VersionInfoComments}; Tasks: quicklaunchicon

[Run]
Filename: "{app}\P4Win.exe"; Description: "Run {#_AppName}"; Flags: nowait postinstall shellexec

[Code]
/////////////////////////////////////////////////////////////////////
function GetUninstallString(): String;
var
  sUnInstPath: String;
  sUnInstallString: String;
begin
  sUnInstPath := ExpandConstant('Software\Microsoft\Windows\CurrentVersion\Uninstall\{#emit SetupSetting("AppId")}_is1');
  sUnInstallString := '';
  if not RegQueryStringValue(HKLM, sUnInstPath, 'UninstallString', sUnInstallString) then
    RegQueryStringValue(HKCU, sUnInstPath, 'UninstallString', sUnInstallString);
  Result := sUnInstallString;
end;

/////////////////////////////////////////////////////////////////////
function IsUpgrade(): Boolean;
begin
  Result := (GetUninstallString() <> '');
end;

/////////////////////////////////////////////////////////////////////
function UnInstallOldVersion(): Integer;
var
  sUnInstallString: String;
  iResultCode: Integer;
begin
// Return Values:
// 1 - uninstall string is empty
// 2 - error executing the UnInstallString
// 3 - successfully executed the UnInstallString

  // default return value
  Result := 0;

  // get the uninstall string of the old app
  sUnInstallString := GetUninstallString();
  if sUnInstallString <> '' then begin
    sUnInstallString := RemoveQuotes(sUnInstallString);
    if Exec(sUnInstallString, '/SILENT /NORESTART /SUPPRESSMSGBOXES','', SW_HIDE, ewWaitUntilTerminated, iResultCode) then
      Result := 3
    else
      Result := 2;
  end else
    Result := 1;
end;

/////////////////////////////////////////////////////////////////////
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if (CurStep=ssInstall) then
  begin
    if (IsUpgrade()) then
    begin
      UnInstallOldVersion();
    end;
  end;
end;