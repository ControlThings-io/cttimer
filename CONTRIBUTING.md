# Building and testing cttimer

cttimer uses CMake and Ninja. It requires Qt 6.8 or newer with Core, Gui, Qml, Quick, QuickControls2, Multimedia, DBus, and Test for tests, plus the Kirigami QML module.

## Native build

Install the dependencies in your development environment, then run:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/cttimer 10m
```

To install the native build:

```bash
sudo cmake --install build
```

On an immutable Aurora/bootc system, build in a toolbox or distrobox, or as part of the image build.

## VS Code Dev Container with Podman

The checked-in Dev Container provides the Fedora C++ toolchain and Qt 6 and Kirigami dependencies. Install the VS Code Dev Containers extension and set Podman as its container command in your user settings:

```json
{
  "dev.containers.dockerPath": "podman"
}
```

Run **Dev Containers: Reopen in Container** from the VS Code command palette. CMake Tools uses Ninja and `build-devcontainer`. To build and test manually in the integrated terminal:

```bash
cmake -S . -B build-devcontainer -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build-devcontainer
ctest --test-dir build-devcontainer --output-on-failure
```

Running the graphical app from the Dev Container also requires forwarding the host's Wayland and audio sockets. Use the Flatpak workflow below to exercise the sandboxed GUI.

## Default sound

The source tree includes `assets/sounds/notify4.wav`. CMake embeds it into the executable as a Qt resource. Check the file with:

```bash
./scripts/check-notify4.sh
```

If the file is absent from your checkout, download the CC0 original from [Freesound](https://freesound.org/people/Mihacappy/sounds/850178/) and save it at `assets/sounds/notify4.wav`. Reconfigure and rebuild after adding it.

## Flatpak

The local development manifest uses the KDE 6.11 runtime. Install the Platform and SDK into the system Flatpak installation, then build a local bundle and install it system-wide:

```bash
flatpak install --system flathub org.kde.Platform//6.11 org.kde.Sdk//6.11
flatpak-builder --force-clean --repo=build-flatpak-repo build-flatpak packaging/flatpak/io.controlthings.cttimer.yml
flatpak build-bundle build-flatpak-repo cttimer.flatpak io.controlthings.cttimer
sudo flatpak install --system --reinstall ./cttimer.flatpak
flatpak run --system io.controlthings.cttimer --version
flatpak run --system io.controlthings.cttimer
```

The `--reinstall` flag replaces an existing system installation, and `run --system` avoids launching an older per-user installation with the same app ID. To check for a per-user copy, run `flatpak info --user io.controlthings.cttimer`. Run `flatpak run --system io.controlthings.cttimer --settings` to verify the Settings page.

Qt also keeps a compiled QML cache in the app's user cache directory, separate from the Flatpak build cache. This can survive uninstalling the app. The manifest disables this disk cache because Flatpak normalizes resource timestamps, which can allow older cached UI code to remain valid after a rebuild. To test an existing installation without using its QML disk cache, run:

```bash
flatpak run --system --env=QML_DISABLE_DISK_CACHE=1 io.controlthings.cttimer --settings
flatpak run --system --env=QML_DISABLE_DISK_CACHE=1 io.controlthings.cttimer
```

The manifest uses a local directory as its source. A Flathub submission should use a tagged release archive and checksum. The sandbox requests Wayland or X11 fallback, audio output, and the notification D-Bus service; it does not request network access.

## Source layout

```text
src/                    C++ backend and embedded resources
qml/                    Kirigami UI
assets/icons/           application icon
assets/sounds/          CC0 alarm asset
packaging/flatpak/      Flatpak manifest
data/                   desktop entry and AppStream metadata
scripts/                helper scripts
tests/                  duration parser tests
```
