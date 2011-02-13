; skoobeditor.nsi
;
; This script is perhaps one of the simplest NSIs you can make. All of the
; optional settings are left to their default settings. The installer simply 
; prompts the user asking them where to install, and drops a copy of example1.nsi
; there. 

;--------------------------------

; The name of the installer
Name "SkoobEditorInstaller"

; The file to write
OutFile "bin\SkoobEditorInstaller.exe"

; The default installation directory
InstallDir $PROGRAMFILES\SkoobEditor

; Request application privileges for Windows Vista
RequestExecutionLevel none

;--------------------------------

; Pages

Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "SkoobEditor (required)"

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File bin\Release\skoobeditor.exe
  File $%QTDIR%\qt\lib\qscintilla2.dll
  File $%QTDIR%\qt\bin\QtCore4.dll
  File $%QTDIR%\qt\bin\QtGui4.dll
  File $%QTDIR%\qt\bin\mingwm10.dll
  File $%QTDIR%\qt\bin\libgcc_s_dw2-1.dll
  
  ;C:\Qt\2010.05\mingw\bin;C:\Qt\2010.05\qt\bin;
  WriteUninstaller "uninstall.exe"
  
SectionEnd ; end the section

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\SkoobEditor"
  CreateShortCut "$SMPROGRAMS\SkoobEditor\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\SkoobEditor\SkoobEditor.lnk" "$INSTDIR\SkoobEditor.exe" "" "$INSTDIR\SkoobEditor.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKCU  "Software\Lip6\SkoobEditor"

  ; Remove files and uninstaller
  Delete $INSTDIR\*.exe
  Delete $INSTDIR\*.dll

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\SkoobEditor\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\SkoobEditor"
  RMDir "$INSTDIR"

SectionEnd

