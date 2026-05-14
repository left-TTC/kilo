
; Kilo.iss
#define ReleaseDir "D:\brave\src\out\Release"
#define OutPutKilo "D:\kiloInstaller"
#define IcoPath "D:\Project\Kilo-browser\kilo\ico\kilo-256.ico"

[Setup]
AppName=Kilo Browser
AppVersion=0.0.2
AppPublisher=github/@left-TTC
; company
AppCopyright=© 2026 Kilo

DefaultDirName={pf}\Kilo Browser
DefaultGroupName=Kilo Browser
OutputDir={#OutPutKilo}
OutputBaseFilename=Kilo Installer
Compression=lzma
SolidCompression=yes

;There is no such for the time being
AppId={{KiloBrowser-6A3C8A7F-9B2C-4D1A-ABCD-112233445566}}
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
CloseApplications=yes
RestartApplications=no

SetupIconFile={#IcoPath}
UninstallDisplayIcon={app}\kilo.exe

[Files]
; 主程序
Source: "{#ReleaseDir}\kilo.exe"; DestDir: "{app}"

; Core Chromium
Source: "{#ReleaseDir}\chrome.dll"; DestDir: "{app}"
Source: "{#ReleaseDir}\chrome_elf.dll"; DestDir: "{app}"
Source: "{#ReleaseDir}\chrome_proxy.exe"; DestDir: "{app}"
Source: "{#ReleaseDir}\chrome_pwa_launcher.exe"; DestDir: "{app}"
Source: "{#ReleaseDir}\chrome_wer.dll"; DestDir: "{app}"
Source: "{#ReleaseDir}\icudtl.dat"; DestDir: "{app}"
Source: "{#ReleaseDir}\snapshot_blob.bin"; DestDir: "{app}"
Source: "{#ReleaseDir}\v8_context_snapshot.bin"; DestDir: "{app}"

; Brave / Chromium资源文件
Source: "{#ReleaseDir}\resources.pak"; DestDir: "{app}"
Source: "{#ReleaseDir}\brave_resources.pak"; DestDir: "{app}"
Source: "{#ReleaseDir}\chrome_100_percent.pak"; DestDir: "{app}"
Source: "{#ReleaseDir}\chrome_200_percent.pak"; DestDir: "{app}"
Source: "{#ReleaseDir}\brave_100_percent.pak"; DestDir: "{app}"
Source: "{#ReleaseDir}\brave_200_percent.pak"; DestDir: "{app}"
Source: "{#ReleaseDir}\headless_command_resources.pak"; DestDir: "{app}"
Source: "{#ReleaseDir}\headless_lib_data.pak"; DestDir: "{app}"
Source: "{#ReleaseDir}\headless_lib_strings.pak"; DestDir: "{app}"

; GPU / ANGLE
Source: "{#ReleaseDir}\libEGL.dll"; DestDir: "{app}"
Source: "{#ReleaseDir}\libGLESv2.dll"; DestDir: "{app}"
Source: "{#ReleaseDir}\vulkan-1.dll"; DestDir: "{app}"
Source: "{#ReleaseDir}\vk_swiftshader.dll"; DestDir: "{app}"
Source: "{#ReleaseDir}\VkICD_mock_icd.dll"; DestDir: "{app}"
Source: "{#ReleaseDir}\VkLayer_khronos_validation.dll"; DestDir: "{app}"
Source: "{#ReleaseDir}\d3dcompiler_47.dll"; DestDir: "{app}"
Source: "{#ReleaseDir}\dxcompiler.dll"; DestDir: "{app}"
Source: "{#ReleaseDir}\dxil.dll"; DestDir: "{app}"

; ANGLE / locale / 子文件夹递归
Source: "{#ReleaseDir}\angledata\*"; DestDir: "{app}\angledata"; Flags: recursesubdirs createallsubdirs
Source: "{#ReleaseDir}\locales\*"; DestDir: "{app}\locales"; Flags: recursesubdirs createallsubdirs; Excludes: "*.info"
Source: "{#ReleaseDir}\MEIPreload\*"; DestDir: "{app}\MEIPreload"; Flags: recursesubdirs createallsubdirs
Source: "{#ReleaseDir}\PrivacySandboxAttestationsPreloaded\*"; DestDir: "{app}\PrivacySandboxAttestationsPreloaded"; Flags: recursesubdirs createallsubdirs

; Manifest
Source: "{#ReleaseDir}\143.1.87.0.manifest"; DestDir: "{app}"
Source: "{#ReleaseDir}\chrome.VisualElementsManifest.xml"; DestDir: "{app}"
Source: "{#ReleaseDir}\mobileprovisions.filelist"; DestDir: "{app}"

[Icons]
Name: "{group}\Kilo Browser"; Filename: "{app}\kilo.exe"
Name: "{commondesktop}\Kilo Browser"; Filename: "{app}\kilo.exe"; WorkingDir: "{app}"
