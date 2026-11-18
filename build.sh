#!/usr/bin/env bash
set -e

NDK=${NDK:-$HOME/android-ndk-r28c}
OUT=build

rm -rf "$OUT"
mkdir -p "$OUT"

"$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android28-clang++" \
  -std=c++20 -O2 -fPIC -shared -fvisibility=hidden \
  -fno-exceptions -fno-rtti \
  -Iinclude -Ivendor/imgui -Ivendor/imgui/backends \
  -o "$OUT/libOutcome.so" \
  $(find src -name '*.cpp') \
  vendor/imgui/*.cpp \
  vendor/imgui/backends/imgui_impl_opengl3.cpp \
  -Lvendor/lib -lAnd64InlineHook -llog -landroid -lEGL -lGLESv3 -ldl

"$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip" \
  --strip-unneeded "$OUT/libOutcome.so"

echo "built $(du -h "$OUT/libOutcome.so" | cut -f1)"
