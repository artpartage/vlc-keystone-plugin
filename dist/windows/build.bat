@echo off
REM ============================================================================
REM VLC Keystone Plugin - Build Script (Windows)
REM
REM Compiles keystone.c into libkeystone_plugin.dll using MinGW-w64 gcc.
REM
REM Prerequisites:
REM   - MinGW-w64 gcc (x86_64) in PATH or set GCC_PATH below
REM   - VLC 3.0.x SDK (download vlc-3.0.x-win64.7z from videolan.org,
REM     extract it, and point VLC_SDK to the sdk/ folder inside)
REM
REM Usage: build.bat [VLC_SDK_PATH] [GCC_EXE_PATH]
REM   Example: build.bat C:\vlc-3.0.21\sdk C:\mingw64\bin\gcc.exe
REM ============================================================================

setlocal

REM --- Configuration ---
set "SCRIPT_DIR=%~dp0"
set "SRC_DIR=%SCRIPT_DIR%..\..\src"
set "OUT_DIR=%SCRIPT_DIR%"
set "PLUGIN_NAME=libkeystone_plugin.dll"

REM VLC SDK path (override with first argument)
if not "%~1"=="" (
    set "VLC_SDK=%~1"
) else (
    set "VLC_SDK=%SCRIPT_DIR%vlc-sdk"
)

REM GCC path (override with second argument)
if not "%~2"=="" (
    set "GCC=%~2"
) else (
    where gcc >NUL 2>&1
    if errorlevel 1 (
        echo Erreur: gcc non trouve dans le PATH.
        echo Error: gcc not found in PATH.
        echo.
        echo Installez MinGW-w64 ou specifiez le chemin:
        echo   build.bat [VLC_SDK_PATH] [GCC_PATH]
        pause
        exit /b 1
    )
    set "GCC=gcc"
)

REM Verify VLC SDK
if not exist "%VLC_SDK%\include\vlc\plugins\vlc_common.h" (
    echo Erreur: VLC SDK non trouve a: %VLC_SDK%
    echo Error: VLC SDK not found at: %VLC_SDK%
    echo.
    echo Telechargez vlc-3.0.x-win64.7z depuis https://get.videolan.org/vlc/
    echo Extrayez et pointez vers le dossier sdk/:
    echo   build.bat C:\path\to\vlc-3.0.21\sdk
    pause
    exit /b 1
)

REM Verify source
if not exist "%SRC_DIR%\keystone.c" (
    echo Erreur: keystone.c non trouve dans %SRC_DIR%
    echo Error: keystone.c not found in %SRC_DIR%
    pause
    exit /b 1
)

echo === VLC Keystone Plugin - Build ===
echo.
echo Source:  %SRC_DIR%\keystone.c
echo VLC SDK: %VLC_SDK%
echo GCC:     %GCC%
echo Output:  %OUT_DIR%%PLUGIN_NAME%
echo.

echo Compilation...
"%GCC%" -shared -o "%OUT_DIR%%PLUGIN_NAME%" ^
    -O2 -Wall -Wno-int-to-pointer-cast ^
    -D__PLUGIN__ -DMODULE_STRING=\"keystone\" -DN_(x)=x ^
    -I"%VLC_SDK%\include\vlc\plugins" ^
    -I"%SRC_DIR%" ^
    "%SRC_DIR%\keystone.c" ^
    -L"%VLC_SDK%\lib" ^
    -lvlccore -lm

if errorlevel 1 (
    echo.
    echo Erreur de compilation.
    echo Compilation error.
    pause
    exit /b 1
)

echo.
echo === Compilation reussie! ===
echo.
echo Le plugin a ete compile: %OUT_DIR%%PLUGIN_NAME%
echo.
echo Pour l'installer, executez install.bat en tant qu'administrateur.
echo.
pause
