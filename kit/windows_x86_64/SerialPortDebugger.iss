[Setup]
AppName=SerialPortDebugger
AppVersion=2.0.0
DefaultDirName={commonpf}\SerialPortDebugger
DefaultGroupName=SerialPortDebugger
OutputDir=C:\Users\Administrator\Desktop
OutputBaseFilename=SerialPortDebugger_Setup
SetupIconFile=D:\Code\QtProjects\SerialPortDebugger\application.ico
UninstallDisplayIcon={app}\SerialPortDebugger.exe
Compression=lzma
SolidCompression=yes
PrivilegesRequired=none
ArchitecturesInstallIn64BitMode=x64

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\Users\Administrator\Desktop\temp_build\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\SerialPortDebugger"; Filename: "{app}\SerialPortDebugger.exe"
Name: "{commondesktop}\SerialPortDebugger"; Filename: "{app}\SerialPortDebugger.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\SerialPortDebugger.exe"; Description: "{cm:LaunchProgram,SerialPortDebugger}"; Flags: nowait postinstall skipifsilent