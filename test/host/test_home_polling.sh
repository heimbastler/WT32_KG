#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
tmp=$(mktemp -d)
trap 'rm -f -- "$tmp/home.js" "$tmp/home-state.json" "$tmp/inputs.json" "$tmp/edit.js" "$tmp/web-escaping.json"; rmdir "$tmp"' EXIT

HOME_POLLING_FIXTURE_DIR="$tmp" sh "$root/test/host/test_home_streaming.sh"
/System/Library/Frameworks/JavaScriptCore.framework/Versions/A/Helpers/jsc \
  "$root/test/host/test_home_polling.js" -- "$tmp"
