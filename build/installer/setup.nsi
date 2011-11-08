# the turnMeOn NSIS setup script

# the project name
name "turnMeOn"

# Name of the installer
outFile "turnMeOn_setup.exe"

# Installation directory (default)
#installDir $DESKTOP\turnMeOn
installDir $ProgramFiles\turnMeOn
DirText "This will install turnMeOn (a wake on LAN packet generator) on your computer."

#Install section (default)
section

# define which files need to be copied and where
setOutPath $INSTDIR
# Target executable
file turnMeOn.exe
# MINGW deps
file $%QTDIR%\bin\mingwm10.dll
file $%QTDIR%\bin\libgcc_s_dw2-1.dll
# Qt deps
file $%QTDIR%\bin\QtCore4.dll
file $%QTDIR%\bin\QtNetwork4.dll
file $%QTDIR%\bin\QtGui4.dll
file $%QTDIR%\bin\QtSql4.dll
setOutPath $INSTDIR\sqldrivers
file $%QTDIR%\bin\..\plugins\sqldrivers\qsqlite4.dll
# MAC database
setOutPath $INSTDIR\res
file oui.db

# Start menu link
setOutPath $INSTDIR #Program must be started from its root
createDirectory $SMPROGRAMS\turnMeOn
createShortCut $SMPROGRAMS\turnMeOn\turnMeOn.lnk $INSTDIR\turnMeOn.exe

# Create the uninstaller
writeUninstaller "$INSTDIR\uninstall.exe"

# Let windows know about us
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\turnMeOn" \
                 "DisplayName" "turnMeOn -- a wake on LAN packet generator"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\turnMeOn" \
                 "UninstallString" "$\"$INSTDIR\uninstall.exe$\""

sectionEnd

# Uninstall section
section "uninstall"

# Uninstall the program
delete "$INSTDIR\uninstall.exe"
delete "$INSTDIR\turnMeOn.exe"
delete "$INSTDIR\*.dll"
delete "$INSTDIR\sqldrivers\*.dll"
rmdir "$INSTDIR\sqldrivers"
delete "$INSTDIR\res\oui.db"
rmdir "$INSTDIR\res"
rmdir "$INSTDIR"

# Remove the start menu link
delete "$SMPROGRAMS\turnMeOn\turnMeOn.lnk"
rmdir "$SMPROGRAMS\turnMeOn"

# Remove us from the add/remove list
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\turnMeOn"

sectionEnd
