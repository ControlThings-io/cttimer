# cttimer (ControlThings Timer)

A small KDE Plasma countdown timer built with Qt 6 and Kirigami.

**App ID:** `io.controlthings.cttimer`  
**License:** GPL-3.0-or-later  
**Default alarm:** `notify4.wav` by Mihacappy, CC0 1.0

## Behavior

- Start timers only from the CLI: `cttimer 10m`, `cttimer 1h30m`, `cttimer 25m "Lab"`
- Each invocation is a separate process and window.
- Starts immediately.
- Progress ring shows relative time remaining.
- Shows the wall-clock time at which the timer will expire.
- Click inside the ring or press **Space** to pause/resume.
- Press **+** / **=** to add the configured increment.
- Press **-** to subtract the configured increment, clamped to a minimum of **10 seconds**.
- Default increment: 1 minute.
- **Ctrl+,** opens Settings.
- Windows default to always-on-top.
- Closing the window terminates that timer and stops its audio.
- At zero, the window remains open at `00:00`, optionally shows a desktop notification, and plays the selected sound.
- `cttimer` with no duration, or `cttimer --settings`, opens Settings.

## Native build (Fedora/Aurora)

Install build dependencies appropriate for your development environment. The application requires Qt 6 Core/Gui/QML/Quick/QuickControls2/Multimedia/DBus plus the Kirigami QML module.

Typical build:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/cttimer 10m
```

Install:

```bash
sudo cmake --install build
```

On an immutable Aurora/bootc system, build in a toolbox/distrobox or as part of the image build rather than layering development packages onto the host.

## Default sound

Freesound requires login to download the original `notify4.wav`, so it is intentionally not included in this generated archive. Download the CC0 original from:

https://freesound.org/people/Mihacappy/sounds/850178/

Save it to:

```text
assets/sounds/notify4.wav
```

CMake detects the file automatically and embeds it into the executable via Qt resources. Verify it with:

```bash
./scripts/check-notify4.sh
```

Reconfigure/rebuild after adding the WAV.

## Flatpak

The local-development manifest uses the KDE 6.9 runtime:

```bash
flatpak install --user flathub org.kde.Platform//6.9 org.kde.Sdk//6.9
flatpak-builder --user --install --force-clean build-flatpak packaging/flatpak/io.controlthings.cttimer.yml
flatpak run io.controlthings.cttimer 10m
```

Because the manifest source is a local directory, it is intended for development/ControlThings builds. A Flathub submission should replace the local `dir` source with a tagged release archive and checksum.

The sandbox requests only Wayland/X11 fallback, audio output, and the notification D-Bus service. It does not request network access.

## Configuration

Native installs store settings at `~/.config/cttimer/cttimer.conf` (respecting `XDG_CONFIG_HOME`). Inside Flatpak, the same logical path is sandboxed under the app's per-user config directory.

## Source layout

```text
src/                    C++ backend and embedded resources
qml/                    Kirigami UI
assets/icons/           application icon
assets/sounds/          CC0 alarm asset location
packaging/flatpak/      Flatpak manifest
data/                   desktop entry and AppStream metadata
scripts/                helper scripts
tests/                  duration parser tests
```
