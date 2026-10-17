; Standard installer for QTetraMesher. Build with build_installer.ps1 (Inno Setup 6.3 or newer).

#ifndef AppVersion
  #define AppVersion "0.8.5"
#endif
#ifndef SourceDir
  #define SourceDir "..\QTetraMesher_Win64_VS2022_0.8.5"
#endif

[Setup]
AppId={{7D1C0E52-4B8A-4E43-9C5F-2A6B3D8E91F4}
AppName=QTetraMesher
AppVersion={#AppVersion}
AppPublisher=dennis2society
AppPublisherURL=https://qtm.dennis2society.de
DefaultDirName={autopf}\QTetraMesher
DisableProgramGroupPage=yes
LicenseFile={#SourceDir}\gpl.txt
OutputDir=output
OutputBaseFilename=QTetraMesher_Setup_{#AppVersion}
SetupIconFile=..\resources\qtetramesher.ico
UninstallDisplayIcon={app}\resources\qtetramesher.ico
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
MinVersion=10.0
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
; per-user install by default, the dialog offers an all-users install
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog

[Tasks]
Name: "startmenuicon"; Description: "Create a &Start Menu shortcut"; GroupDescription: "Shortcuts:"
Name: "desktopicon"; Description: "Create a &Desktop shortcut"; GroupDescription: "Shortcuts:"; Flags: unchecked

[Files]
Source: "{#SourceDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\resources\qtetramesher.ico"; DestDir: "{app}\resources"; Flags: ignoreversion

[Icons]
Name: "{autoprograms}\QTetraMesher"; Filename: "{app}\QTetraMesher.exe"; IconFilename: "{app}\resources\qtetramesher.ico"; Tasks: startmenuicon
Name: "{autodesktop}\QTetraMesher"; Filename: "{app}\QTetraMesher.exe"; IconFilename: "{app}\resources\qtetramesher.ico"; Tasks: desktopicon

[Run]
Filename: "{app}\QTetraMesher.exe"; Description: "Launch QTetraMesher"; Flags: nowait postinstall skipifsilent
