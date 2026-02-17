#!/bin/bash
#
# VLC Keystone Plugin - Installation Script (macOS)
#
# This script installs the keystone (corner pin) video filter plugin for VLC.
# It copies the plugin to VLC's plugin directory and re-signs the application
# bundle so macOS allows it to run.
#
# Usage: ./install.sh
#

set -e

PLUGIN_NAME="libkeystone_plugin.dylib"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PLUGIN_SRC="$SCRIPT_DIR/$PLUGIN_NAME"

# Detect VLC installation path
VLC_APP="/Applications/VLC.app"
if [ ! -d "$VLC_APP" ]; then
    echo "Erreur: VLC n'est pas installe dans /Applications/VLC.app"
    echo "Error: VLC is not installed at /Applications/VLC.app"
    exit 1
fi

VLC_PLUGINS="$VLC_APP/Contents/MacOS/plugins"

if [ ! -f "$PLUGIN_SRC" ]; then
    echo "Erreur: $PLUGIN_NAME non trouve dans le dossier courant."
    echo "Error: $PLUGIN_NAME not found in the current directory."
    exit 1
fi

echo "=== VLC Keystone Plugin - Installation ==="
echo ""
echo "Plugin:      $PLUGIN_SRC"
echo "Destination: $VLC_PLUGINS/"
echo ""

# Check if VLC is running
if pgrep -x "VLC" > /dev/null 2>&1; then
    echo "VLC est en cours d'execution. Fermez VLC avant d'installer."
    echo "VLC is currently running. Please close VLC before installing."
    exit 1
fi

# Copy plugin
echo "[1/2] Copie du plugin..."
cp "$PLUGIN_SRC" "$VLC_PLUGINS/"
echo "      OK: $PLUGIN_NAME copie."

# Re-sign VLC bundle (required on macOS after modifying the app bundle)
echo "[2/2] Re-signature de VLC.app (requis par macOS)..."
codesign --sign - --force --deep "$VLC_APP"
echo "      OK: VLC.app re-signe."

echo ""
echo "=== Installation terminee! ==="
echo ""
echo "Pour utiliser le filtre keystone, lancez VLC avec:"
echo ""
echo "  open /Applications/VLC.app --args --video-filter=keystone --codec=avcodec fichier.mp4"
echo ""
echo "Ou dans VLC > Preferences > Video > Filters, cochez 'Keystone'."
echo ""
echo "Les coins se deplacent avec la souris (cliquer-glisser)."
echo "Le carre orange apparait quand la souris approche un coin."
echo ""
echo "Options de ligne de commande:"
echo "  --keystone-tl-x=0.05   Decaler le coin haut-gauche horizontalement"
echo "  --keystone-tl-y=0.05   Decaler le coin haut-gauche verticalement"
echo "  --keystone-tr-x, --keystone-tr-y   Coin haut-droit"
echo "  --keystone-bl-x, --keystone-bl-y   Coin bas-gauche"
echo "  --keystone-br-x, --keystone-br-y   Coin bas-droit"
echo "  --no-keystone-show-handles          Cacher les poignees interactives"
echo ""
