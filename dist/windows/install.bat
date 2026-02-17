@echo off
REM ============================================================================
REM VLC Keystone Plugin - Installation Script (Windows)
REM
REM This script installs the keystone (corner pin) video filter plugin for VLC.
REM It copies the plugin DLL to VLC's video_filter plugin directory.
REM
REM Usage: Right-click install.bat > Run as administrator
REM        (admin rights needed to write to Program Files)
REM ============================================================================

setlocal

set "PLUGIN_NAME=libkeystone_plugin.dll"
set "SCRIPT_DIR=%~dp0"
set "PLUGIN_SRC=%SCRIPT_DIR%%PLUGIN_NAME%"

REM Detect VLC installation path
set "VLC_DIR=C:\Program Files\VideoLAN\VLC"
if not exist "%VLC_DIR%\vlc.exe" (
    set "VLC_DIR=C:\Program Files (x86)\VideoLAN\VLC"
)
if not exist "%VLC_DIR%\vlc.exe" (
    echo Erreur: VLC n'est pas installe dans C:\Program Files\VideoLAN\VLC
    echo Error: VLC is not installed at C:\Program Files\VideoLAN\VLC
    echo.
    echo Veuillez specifier le chemin de VLC:
    echo Please specify the VLC installation path:
    set /p "VLC_DIR=VLC path: "
)
if not exist "%VLC_DIR%\vlc.exe" (
    echo Erreur: vlc.exe non trouve dans "%VLC_DIR%"
    echo Error: vlc.exe not found in "%VLC_DIR%"
    pause
    exit /b 1
)

set "VLC_PLUGINS=%VLC_DIR%\plugins\video_filter"

if not exist "%PLUGIN_SRC%" (
    echo Erreur: %PLUGIN_NAME% non trouve dans le dossier courant.
    echo Error: %PLUGIN_NAME% not found in the current directory.
    echo Cherche: %PLUGIN_SRC%
    pause
    exit /b 1
)

echo === VLC Keystone Plugin - Installation ===
echo.
echo Plugin:      %PLUGIN_SRC%
echo Destination: %VLC_PLUGINS%\
echo.

REM Check if VLC is running
tasklist /FI "IMAGENAME eq vlc.exe" 2>NUL | find /I /N "vlc.exe" >NUL
if "%ERRORLEVEL%"=="0" (
    echo VLC est en cours d'execution. Fermez VLC avant d'installer.
    echo VLC is currently running. Please close VLC before installing.
    pause
    exit /b 1
)

REM Create destination directory if needed
if not exist "%VLC_PLUGINS%" (
    mkdir "%VLC_PLUGINS%"
)

REM Copy plugin
echo [1/1] Copie du plugin...
copy /Y "%PLUGIN_SRC%" "%VLC_PLUGINS%\" >NUL
if errorlevel 1 (
    echo.
    echo Erreur: impossible de copier le fichier. Executez ce script en tant qu'administrateur.
    echo Error: could not copy the file. Run this script as administrator.
    pause
    exit /b 1
)
echo       OK: %PLUGIN_NAME% copie.

echo.
echo === Installation terminee! ===
echo.
echo Pour utiliser le filtre keystone, lancez VLC avec:
echo.
echo   vlc --video-filter=keystone video.mp4
echo.
echo Ou dans VLC ^> Preferences ^> Video ^> Filters, cochez 'Keystone'.
echo.
echo Les coins se deplacent avec la souris (cliquer-glisser).
echo Le carre orange apparait quand la souris approche un coin.
echo.
echo Options de ligne de commande:
echo   --keystone-tl-x=0.05   Decaler le coin haut-gauche horizontalement
echo   --keystone-tl-y=0.05   Decaler le coin haut-gauche verticalement
echo   --keystone-tr-x, --keystone-tr-y   Coin haut-droit
echo   --keystone-bl-x, --keystone-bl-y   Coin bas-gauche
echo   --keystone-br-x, --keystone-br-y   Coin bas-droit
echo   --no-keystone-show-handles          Cacher les poignees interactives
echo.
pause
