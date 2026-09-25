# Bundled alarm sound

The default sound is **notify4.wav** by **Mihacappy**, from [Freesound sound 850178](https://freesound.org/people/Mihacappy/sounds/850178/).

- License: Creative Commons Zero (CC0 1.0)
- Duration: 7.221 seconds
- Format at source: 44.1 kHz, 16-bit, stereo WAV

The source tree includes `notify4.wav`. CMake embeds it into the `cttimer` executable. Verify the file with:

```bash
./scripts/check-notify4.sh
```

If the WAV is missing from a checkout, download the original from Freesound and save it as `assets/sounds/notify4.wav`, then reconfigure and rebuild.
