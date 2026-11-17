#!/bin/sh
# amiga/xmlreader.cpp against Expat on the host: the same events for
# tests/host/samples.xml and for Mojotron's own res.xml and robotwar.xml
# (upstream fetched at the locked commit, or MJ=).
#   sh tests/host/test_xmlreader.sh
# Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Mojotron.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$HERE/../.." && pwd)
. "$ROOT/package/mojotron.lock"
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
if [ -z "${MJ:-}" ]; then
    git -C "$T" init -q mj
    git -C "$T/mj" fetch -q --depth 1 "$MOJOTRON_URL" "$MOJOTRON_COMMIT"
    git -C "$T/mj" checkout -q FETCH_HEAD
    MJ="$T/mj"
fi
CXX=${CXX:-c++}
$CXX -std=c++17 -I"$MJ" -o "$T/with-expat" "$HERE/xml_events.cpp" "$MJ/expatcpp.cpp" -lexpat
$CXX -std=c++17 -I"$ROOT/amiga/include" -I"$MJ" -o "$T/with-ours" "$HERE/xml_events.cpp" "$ROOT/amiga/xmlreader.cpp"
fail=0
for f in "$HERE/samples.xml" "$MJ/res.xml" "$MJ/robotwar.xml"; do
    "$T/with-expat" "$f" > "$T/a"; "$T/with-ours" "$f" > "$T/b"
    if cmp -s "$T/a" "$T/b" && [ -s "$T/a" ]; then
        echo "ok   $(basename "$f"): $(wc -l < "$T/a") events the same"
    else
        echo "FAIL $(basename "$f")"; diff "$T/a" "$T/b" | head -20; fail=1
    fi
done
[ $fail = 0 ] && echo PASS || { echo FAIL; exit 1; }
