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

1. Fermez VLC
2. Ouvrez le Terminal
3. Naviguez vers le dossier `dist/macos/`
4. Exécutez :
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

Lancez VLC avec le filtre :
```bash
open /Applications/VLC.app --args --video-filter=keystone --codec=avcodec video.mp4
```

Ou activez-le dans : VLC > Préférences > Tout afficher > Vidéo > Filtres > cochez **Keystone**

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

1. Fermez VLC
2. Téléchargez `libkeystone_plugin.dll` depuis les [Releases](https://github.com/artpartage/vlc-keystone-plugin/releases)
3. Placez le fichier dans le dossier `dist/windows/`
4. Faites un clic droit sur `install.bat` > **Exécuter en tant qu'administrateur**

Ou manuellement, copiez le DLL dans :
```
C:\Program Files\VideoLAN\VLC\plugins\video_filter\
```

### Utilisation (Windows)

```cmd
vlc --video-filter=keystone video.mp4
```

Ou dans VLC > Outils > Préférences > Tout afficher > Vidéo > Filtres > cochez **Keystone**

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

1. Close VLC
2. Open Terminal
3. Navigate to the `dist/macos/` folder
4. Run:
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

Launch VLC with the filter:
```bash
open /Applications/VLC.app --args --video-filter=keystone --codec=avcodec video.mp4
```

Or enable it in: VLC > Preferences > Show All > Video > Filters > check **Keystone**

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

1. Close VLC
2. Download `libkeystone_plugin.dll` from [Releases](https://github.com/artpartage/vlc-keystone-plugin/releases)
3. Place the file in the `dist/windows/` folder
4. Right-click `install.bat` > **Run as administrator**

Or manually copy the DLL to:
```
C:\Program Files\VideoLAN\VLC\plugins\video_filter\
```

### Usage (Windows)

```cmd
vlc --video-filter=keystone video.mp4
```

Or enable it in: VLC > Tools > Preferences > Show All > Video > Filters > check **Keystone**

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
