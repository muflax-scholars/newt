Name "Ne.W.T. for Slash'EM"
OutFile "newt-1.3.0-slashem_win32-1.exe"
InstallDir "$PROGRAMFILES\NeWT\SlashEM"

;--------------------------------

Page directory
Page instfiles

;--------------------------------

Section ""
  SetOutPath $INSTDIR
  File ..\..\slashem\binary\*

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NeWT-SlashEM" "DisplayName" "Ne.W.T. for SlashEM"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NeWT-SlashEM" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NeWT-SlashEM" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NeWT-SlashEM" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
SectionEnd

Section "Desktop icon" SecDesktop
  SetOutPath $INSTDIR
  CreateShortcut "$DESKTOP\Ne.W.T. for SlashEM.lnk" "$INSTDIR\NeWT.exe" "" "$INSTDIR\NeWT.exe" 0
SectionEnd

Section "Start Menu Shortcuts"
  SetOutPath $INSTDIR
  CreateDirectory "$SMPROGRAMS\Ne.W.T. for SlashEM"
  CreateShortCut "$SMPROGRAMS\Ne.W.T. for SlashEM\Play Ne.W.T. for SlashEM.lnk" "$INSTDIR\NeWT.exe" "" "$INSTDIR\NeWT.exe" 0
  CreateShortCut "$SMPROGRAMS\Ne.W.T. for SlashEM\Manual.lnk" "$INSTDIR\Guidebook.txt" "" "$INSTDIR\Guidebook.txt" 0
  CreateShortCut "$SMPROGRAMS\Ne.W.T. for SlashEM\Darkarts Studios.lnk" "http://www.darkarts.co.za" "" "http://www.darkarts.co.za" 0
  CreateShortCut "$SMPROGRAMS\Ne.W.T. for SlashEM\Uninstall Ne.W.T. for SlashEM.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd

Section "Uninstall"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NeWT-SlashEM"
  DeleteRegKey HKLM SOFTWARE\NeWT-SlashEM
  Delete $INSTDIR\*
  RMDir "$INSTDIR"
  Delete "$SMPROGRAMS\Ne.W.T. for SlashEM\Play Ne.W.T. for SlashEM.lnk"
  Delete "$SMPROGRAMS\Ne.W.T. for SlashEM\Manual.lnk"
  Delete "$SMPROGRAMS\Ne.W.T. for SlashEM\Darkarts Studios.lnk"
  Delete "$SMPROGRAMS\Ne.W.T. for SlashEM\Uninstall Ne.W.T. for SlashEM.lnk"
  RMDir "$SMPROGRAMS\Ne.W.T. for SlashEM"
  Delete "$DESKTOP\Ne.W.T. for SlashEM.lnk"
SectionEnd
