# VLC Keystone Plugin (Corner Pin)

**[Français](#français) | [English](#english)**

---

## Français

### Description

Plugin VLC qui ajoute un filtre vidéo **Keystone** (correction de perspective) interactif. Permet de déplacer indépendamment les 4 coins de la vidéo en temps réel avec la souris, idéal pour :

- Correction de projection (projecteur en angle)
- Mapping vidéo simple
- Effets de perspective créatifs

### Fonctionnalités

- Déformation perspective complète (transformation homographique)
- Contrôle interactif à la souris : cliquer-glisser les coins directement sur la vidéo
- Indicateur orange au survol d'un coin, rouge lors du déplacement
- Persistance des positions lors de la répétition/boucle d'une vidéo
- Interpolation bilinéaire pour une qualité d'image optimale

### Installation (macOS)

> **IMPORTANT :** Fermez VLC **complètement** avant l'installation. VLC ne doit pas tourner, sinon le plugin ne sera pas reconnu.

1. Fermez VLC complètement
2. Téléchargez `libkeystone_plugin.dylib` depuis les [Releases](https://github.com/artpartage/vlc-keystone-plugin/releases)
3. Copiez le fichier dans : `/Applications/VLC.app/Contents/MacOS/plugins/`
4. Relancez VLC

Ou avec le Terminal :
```bash
chmod +x install.sh
./install.sh
```

Ou manuellement :
```bash
cp libkeystone_plugin.dylib /Applications/VLC.app/Contents/MacOS/plugins/
codesign --sign - --force --deep /Applications/VLC.app
```

### Utilisation

#### Méthode 1 - Préférences VLC (recommandée) :
1. Ouvrez VLC
2. Allez dans **Outils > Préférences** (ou VLC > Préférences sur macOS)
3. En bas à gauche, sélectionnez **« Tout afficher »** (c'est important !)
4. Naviguez vers **Vidéo > Filtres**
5. Cochez **Keystone** dans la liste
6. Cliquez **Enregistrer**
7. Redémarrez VLC
8. Lancez une vidéo — vous pouvez maintenant déplacer les coins avec la souris !

#### Méthode 2 - Ligne de commande :
```bash
open /Applications/VLC.app --args --video-filter=keystone --codec=avcodec video.mp4
```

### Contrôles souris

| Action | Résultat |
|--------|----------|
| Survoler un coin | Indicateur orange apparaît |
| Cliquer-glisser un coin | Indicateur rouge, déformation en temps réel |
| Relâcher | Le coin reste en position |

### Options en ligne de commande

Toutes les valeurs vont de -1.0 à 1.0 (0.0 = position par défaut).

| Option | Description |
|--------|-------------|
| `--keystone-tl-x` | Coin haut-gauche, décalage horizontal |
| `--keystone-tl-y` | Coin haut-gauche, décalage vertical |
| `--keystone-tr-x` | Coin haut-droit, décalage horizontal |
| `--keystone-tr-y` | Coin haut-droit, décalage vertical |
| `--keystone-bl-x` | Coin bas-gauche, décalage horizontal |
| `--keystone-bl-y` | Coin bas-gauche, décalage vertical |
| `--keystone-br-x` | Coin bas-droit, décalage horizontal |
| `--keystone-br-y` | Coin bas-droit, décalage vertical |
| `--no-keystone-show-handles` | Cacher les poignées interactives |

### Installation (Windows)

> **IMPORTANT :** Fermez VLC **complètement** avant l'installation (vérifiez aussi la zone de notification). VLC ne doit pas tourner, sinon le plugin ne sera pas reconnu.

1. Fermez VLC complètement
2. Téléchargez `libkeystone_plugin.dll` depuis les [Releases](https://github.com/artpartage/vlc-keystone-plugin/releases)
3. Copiez le fichier dans : `C:\Program Files\VideoLAN\VLC\plugins\video_filter\`
4. Relancez VLC

Ou utilisez `install.bat` : placez le DLL dans le dossier `dist/windows/` et faites un clic droit sur `install.bat` > **Exécuter en tant qu'administrateur**

### Utilisation (Windows)

Les mêmes étapes que ci-dessus (voir [Utilisation](#utilisation)).

### Note macOS

L'option `--codec=avcodec` est recommandée sur macOS pour éviter des incompatibilités avec le décodeur VideoToolbox par défaut.

---

## English

### Description

VLC plugin that adds an interactive **Keystone** (perspective correction) video filter. Move each of the 4 video corners independently in real time using the mouse. Ideal for:

- Projection correction (angled projector)
- Simple video mapping
- Creative perspective effects

### Features

- Full perspective deformation (homography transformation)
- Interactive mouse control: click and drag corners directly on the video
- Orange hover indicator when mouse approaches a corner, red when dragging
- Position persistence when looping the same video
- Bilinear interpolation for optimal image quality

### Installation (macOS)

> **IMPORTANT:** Close VLC **completely** before installing the plugin. VLC must not be running during installation, otherwise the plugin will not be recognized.

1. Close VLC completely
2. Download `libkeystone_plugin.dylib` from [Releases](https://github.com/artpartage/vlc-keystone-plugin/releases)
3. Copy the file to: `/Applications/VLC.app/Contents/MacOS/plugins/`
4. Restart VLC

Or using Terminal:
```bash
chmod +x install.sh
./install.sh
```

Or manually:
```bash
cp libkeystone_plugin.dylib /Applications/VLC.app/Contents/MacOS/plugins/
codesign --sign - --force --deep /Applications/VLC.app
```

### Usage

#### Method 1 - VLC Preferences (recommended):
1. Open VLC
2. Go to **Tools > Preferences**
3. At the bottom left, select **"Show settings: All"** (this is important!)
4. Navigate to **Video > Filters**
5. Check **Keystone** in the list
6. Click **Save**
7. Restart VLC
8. Play a video — you can now drag the corners with your mouse!

#### Method 2 - Command line:
```bash
open /Applications/VLC.app --args --video-filter=keystone --codec=avcodec video.mp4
```

### Mouse Controls

| Action | Result |
|--------|--------|
| Hover near a corner | Orange indicator appears |
| Click and drag a corner | Red indicator, real-time deformation |
| Release | Corner stays in position |

### Command Line Options

All values range from -1.0 to 1.0 (0.0 = default position).

| Option | Description |
|--------|-------------|
| `--keystone-tl-x` | Top-left corner, horizontal offset |
| `--keystone-tl-y` | Top-left corner, vertical offset |
| `--keystone-tr-x` | Top-right corner, horizontal offset |
| `--keystone-tr-y` | Top-right corner, vertical offset |
| `--keystone-bl-x` | Bottom-left corner, horizontal offset |
| `--keystone-bl-y` | Bottom-left corner, vertical offset |
| `--keystone-br-x` | Bottom-right corner, horizontal offset |
| `--keystone-br-y` | Bottom-right corner, vertical offset |
| `--no-keystone-show-handles` | Hide interactive handles |

### Installation (Windows)

> **IMPORTANT:** Close VLC **completely** before installing (check your system tray too). VLC must not be running during installation, otherwise the plugin will not be recognized.

1. Close VLC completely
2. Download `libkeystone_plugin.dll` from [Releases](https://github.com/artpartage/vlc-keystone-plugin/releases)
3. Copy the file to: `C:\Program Files\VideoLAN\VLC\plugins\video_filter\`
4. Restart VLC

Or use `install.bat`: place the DLL in the `dist/windows/` folder and right-click `install.bat` > **Run as administrator**

### Usage (Windows)

Same steps as above (see [Usage](#usage)).

### macOS Note

The `--codec=avcodec` option is recommended on macOS to avoid incompatibilities with the default VideoToolbox decoder.

---

## Compatibility

| Platform | Status |
|----------|--------|
| macOS (Apple Silicon / arm64) | Available |
| Windows (x86_64) | Available |
| macOS (Intel / x86_64) | Not yet compiled |
| Linux | Not yet compiled |

Built for **VLC 3.0.x**. Minor updates (3.0.x to 3.0.y) should remain compatible. A major VLC version change (e.g., 4.0) may require recompilation.

## Building from Source

### macOS

Requires the VLC source tree and build environment.

```bash
# From the VLC source root:
make -C modules video_filter/keystone.lo
make -C modules libkeystone_plugin.la

# The compiled plugin will be in modules/.libs/libkeystone_plugin.dylib
```

### Windows

Requires MinGW-w64 (gcc) and the VLC 3.0.x SDK.

1. Download VLC 3.0.x win64 archive from https://get.videolan.org/vlc/ and extract the `sdk/` folder
2. Install MinGW-w64 (x86_64)
3. Run:
```cmd
cd dist\windows
build.bat C:\path\to\vlc-3.0.21\sdk C:\path\to\mingw64\bin\gcc.exe
```

Or compile manually:
```bash
gcc -shared -o libkeystone_plugin.dll \
    -O2 -Wall -Wno-int-to-pointer-cast \
    -D__PLUGIN__ -DMODULE_STRING=\"keystone\" -DN_(x)=x \
    -I<VLC_SDK>/include/vlc/plugins \
    -Isrc \
    src/keystone.c \
    -L<VLC_SDK>/lib \
    -lvlccore -lm
```

## License

[GNU Lesser General Public License v2.1](LICENSE) (same as VLC)

## Author

**Art Partage** - Rad Gagnon
