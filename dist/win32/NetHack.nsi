Name "Ne.W.T. for NetHack"
OutFile "newt-1.3.0-nethack_win32-1.exe"
InstallDir "$PROGRAMFILES\NeWT\NetHack"

;--------------------------------

Page directory
Page instfiles

;--------------------------------

Section ""
  SetOutPath $INSTDIR
  File ..\..\nethack\binary\*

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NeWT-NetHack" "DisplayName" "Ne.W.T. for NetHack"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NeWT-NetHack" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NeWT-NetHack" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NeWT-NetHack" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
SectionEnd

Section "Desktop icon" SecDesktop
  SetOutPath $INSTDIR
  CreateShortcut "$DESKTOP\Ne.W.T. for NetHack.lnk" "$INSTDIR\NeWT.exe" "" "$INSTDIR\NeWT.exe" 0
SectionEnd

Section "Start Menu Shortcuts"
  SetOutPath $INSTDIR
  CreateDirectory "$SMPROGRAMS\Ne.W.T. for NetHack"
  CreateShortCut "$SMPROGRAMS\Ne.W.T. for NetHack\Play Ne.W.T. for NetHack.lnk" "$INSTDIR\NeWT.exe" "" "$INSTDIR\NeWT.exe" 0
  CreateShortCut "$SMPROGRAMS\Ne.W.T. for NetHack\Manual.lnk" "$INSTDIR\manual\index.html" "" "$INSTDIR\manual\index.html" 0
  CreateShortCut "$SMPROGRAMS\Ne.W.T. for NetHack\Darkarts Studios.lnk" "http://www.darkarts.co.za" "" "http://www.darkarts.co.za" 0
  CreateShortCut "$SMPROGRAMS\Ne.W.T. for NetHack\Uninstall Ne.W.T. for NetHack.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd

Section "Uninstall"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NeWT-NetHack"
  DeleteRegKey HKLM SOFTWARE\NeWT-NetHack
  Delete $INSTDIR\*
  RMDir "$INSTDIR"
  Delete "$SMPROGRAMS\Ne.W.T. for NetHack\Play Ne.W.T. for NetHack.lnk"
  Delete "$SMPROGRAMS\Ne.W.T. for NetHack\Manual.lnk"
  Delete "$SMPROGRAMS\Ne.W.T. for NetHack\Darkarts Studios.lnk"
  Delete "$SMPROGRAMS\Ne.W.T. for NetHack\Uninstall Ne.W.T. for NetHack.lnk"
  RMDir "$SMPROGRAMS\Ne.W.T. for NetHack"
  Delete "$DESKTOP\Ne.W.T. for NetHack.lnk"
SectionEnd
