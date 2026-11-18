#!/usr/bin/env bash
# builds the levipack: manifest + .so, zipped the way LeviLauncher expects
set -e

PKG=build/pkg
rm -rf "$PKG"
mkdir -p "$PKG/lib"

cp build/libOutcome.so "$PKG/lib/"

cat > "$PKG/manifest.json" <<EOF
{
  "name": "Outcome",
  "version": "1.0.0",
  "author": "he",
  "entry": "lib/libOutcome.so",
  "target": "arm64-v8a",
  "game": ">=1.21.120"
}
EOF

cd build
zip -r Outcome.levipack pkg/manifest.json pkg/lib/libOutcome.so
echo "packed build/Outcome.levipack"
