#!/bin/bash
#
# VLC Keystone Plugin - Uninstallation Script (macOS)
#

set -e

VLC_APP="/Applications/VLC.app"
VLC_PLUGINS="$VLC_APP/Contents/MacOS/plugins"
PLUGIN_NAME="libkeystone_plugin.dylib"

if [ ! -f "$VLC_PLUGINS/$PLUGIN_NAME" ]; then
    echo "Le plugin keystone n'est pas installe."
    echo "The keystone plugin is not installed."
    exit 0
fi

# Check if VLC is running
if pgrep -x "VLC" > /dev/null 2>&1; then
    echo "VLC est en cours d'execution. Fermez VLC avant de desinstaller."
    echo "VLC is currently running. Please close VLC before uninstalling."
    exit 1
fi

echo "=== VLC Keystone Plugin - Desinstallation ==="
echo ""

rm "$VLC_PLUGINS/$PLUGIN_NAME"
echo "[1/2] Plugin supprime."

codesign --sign - --force --deep "$VLC_APP"
echo "[2/2] VLC.app re-signe."

echo ""
echo "=== Desinstallation terminee! ==="
echo ""
