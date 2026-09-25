#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
AUDIO="$ROOT/assets/sounds/notify4.wav"
if [[ ! -s "$AUDIO" ]]; then
    echo "Missing: $AUDIO" >&2
    echo "Download the CC0 original from https://freesound.org/people/Mihacappy/sounds/850178/" >&2
    exit 1
fi
file "$AUDIO"
echo "notify4.wav is present and will be embedded automatically by CMake."
