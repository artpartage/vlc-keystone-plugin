@echo off
REM ============================================================================
REM VLC Keystone Plugin - Uninstall Script (Windows)
REM
REM Removes the keystone plugin from VLC's plugin directory.
REM
REM Usage: Right-click uninstall.bat > Run as administrator
REM ============================================================================

setlocal

set "PLUGIN_NAME=libkeystone_plugin.dll"

REM Detect VLC installation path
set "VLC_DIR=C:\Program Files\VideoLAN\VLC"
if not exist "%VLC_DIR%\vlc.exe" (
    set "VLC_DIR=C:\Program Files (x86)\VideoLAN\VLC"
)
if not exist "%VLC_DIR%\vlc.exe" (
    echo Erreur: VLC n'est pas installe.
    echo Error: VLC is not installed.
    pause
    exit /b 1
)

set "PLUGIN_PATH=%VLC_DIR%\plugins\video_filter\%PLUGIN_NAME%"

if not exist "%PLUGIN_PATH%" (
    echo Le plugin n'est pas installe.
    echo The plugin is not installed.
    pause
    exit /b 0
)

REM Check if VLC is running
tasklist /FI "IMAGENAME eq vlc.exe" 2>NUL | find /I /N "vlc.exe" >NUL
if "%ERRORLEVEL%"=="0" (
    echo VLC est en cours d'execution. Fermez VLC avant de desinstaller.
    echo VLC is currently running. Please close VLC before uninstalling.
    pause
    exit /b 1
)

echo Suppression de %PLUGIN_PATH%...
del "%PLUGIN_PATH%"
if errorlevel 1 (
    echo Erreur: impossible de supprimer le fichier. Executez ce script en tant qu'administrateur.
    echo Error: could not delete the file. Run this script as administrator.
    pause
    exit /b 1
)

echo Plugin desinstalle avec succes.
echo Plugin uninstalled successfully.
pause
