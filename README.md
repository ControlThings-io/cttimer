# cttimer (ControlThings Timer)

A small KDE Plasma countdown timer built with Qt 6 and Kirigami. Start a timer from the command line; each timer opens its own window and begins immediately.

## Use

```bash
cttimer 10m
cttimer 1h30m
cttimer 25m "Lab"
cttimer --settings
```

A duration can also be a number of minutes (`cttimer 10`) or clock notation (`cttimer 25:00`). Running `cttimer` without a duration opens Settings.

- Click inside the progress ring or press **Space** to pause or resume.
- Press **+** or **=** to add the configured increment; press **-** to subtract it, down to a minimum of 10 seconds.
- Press **Ctrl+,** to open Settings.
- The window shows the time when the timer will finish and stays on top by default. Closing it stops the timer and its audio.
- At zero, the window remains open at `00:00` and can play an alarm and show a desktop notification. Both can be configured in Settings.

The default alarm is `notify4.wav` by Mihacappy, licensed CC0 1.0. You can select a different sound in Settings.

## KDE Plasma shortcuts

You can assign a custom global shortcut to each timer you use often. For example, set **Meta+Shift+2** to run `cttimer 20m "Lab"` and start a 20-minute Lab timer with one shortcut. For a Flatpak install, use `flatpak run io.controlthings.cttimer 20m "Lab"` as the command.

## Installation

Build and install from source using [CONTRIBUTING.md](CONTRIBUTING.md). The project also includes a Flatpak manifest for local builds.

Native settings are stored at `~/.config/cttimer/cttimer.conf` (or under `XDG_CONFIG_HOME`). Flatpak keeps settings in the app's sandboxed configuration directory.

## License

GPL-3.0-or-later. See [LICENSE](LICENSE). App ID: `io.controlthings.cttimer`.
