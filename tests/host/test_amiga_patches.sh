#!/bin/sh
# The Amiga patches on the host (upstream fetched at the locked commit, or
# MJ=): IRandom seeds without std::random_device (irandom_test.cpp), and
# main() asks SDL2_image.module for its own 68k decoder on AmigaOS only,
# before SDL_Init and before the pictures are read.
#   sh tests/host/test_amiga_patches.sh
# Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Mojotron.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$HERE/../.." && pwd)
. "$ROOT/package/mojotron.lock"
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
git -C "$T" init -q mj
if [ -n "${MJ:-}" ]; then
    cp "$MJ"/*.cpp "$MJ"/*.hh "$MJ"/*.h "$T/mj/" 2>/dev/null || true
else
    git -C "$T/mj" fetch -q --depth 1 "$MOJOTRON_URL" "$MOJOTRON_COMMIT"
    git -C "$T/mj" checkout -q FETCH_HEAD
fi
for p in "$ROOT"/amiga/patches/*.patch; do (cd "$T/mj" && patch -s -p1 < "$p"); done
fail=0
check() { if [ "$2" = 1 ]; then echo "pass  $1"; else echo "FAIL  $1"; fail=1; fi; }

${CXX:-c++} -std=c++17 -Wall -D__amigaos__ -I"$HERE/sdlstub" -I"$T/mj" \
    -o "$T/irandom" "$HERE/irandom_test.cpp" "$T/mj/IRandom.cpp"
"$T/irandom" || fail=1
check "no std::random_device on AmigaOS" \
    "$(${CXX:-c++} -std=c++17 -E -P -D__amigaos__ -I"$HERE/sdlstub" -I"$T/mj" "$T/mj/IRandom.cpp" | sed -n '/IRandom::init/,$p' | grep -c 'random_device' | grep -qx 0 && echo 1 || echo 0)"

F="$T/mj/main.cpp"
hint=$(grep -n 'SDL_SetHint("SDL_IMAGE_DECODER", "cpu");' "$F" | cut -d: -f1)
init=$(grep -n 'SDL_Init(SDL_INIT_VIDEO' "$F" | head -n 1 | cut -d: -f1)
sprites=$(grep -n 'loadSprites()' "$F" | head -n 1 | cut -d: -f1)
check "decoder hint set once" "$([ "$(echo "$hint" | wc -w)" = 1 ] && echo 1 || echo 0)"
check "before SDL_Init" "$([ -n "$hint" ] && [ "$hint" -lt "$init" ] && echo 1 || echo 0)"
check "before the pictures are read" "$([ -n "$hint" ] && [ "$hint" -lt "$sprites" ] && echo 1 || echo 0)"
open=$(awk -v h="$hint" 'NR<h && /^#/ {l=$0} END {print l}' "$F")
close=$(awk -v h="$hint" 'NR>h && /^#/ {print; exit}' "$F")
check "only on AmigaOS" "$([ "$open" = "#ifdef __amigaos__" ] && [ "$close" = "#endif" ] && echo 1 || echo 0)"
exit $fail
