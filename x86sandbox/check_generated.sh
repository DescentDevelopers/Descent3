#!/usr/bin/env bash
# Checks that x86sandbox/generated is what x86sandbox/regenerate.sh makes from the
# current guest sources.
#
# The generated C depends on the compiler that built the guest, so this needs the
# clang that x86sandbox/generated/README.txt names (CLANG, WASM_LD to choose it).
# WABT_DIR may name a WABT checkout at regenerate.sh's WABT_COMMIT; otherwise the
# script fetches that commit into a temporary directory.
#
# Usage: x86sandbox/check_generated.sh

set -euo pipefail

here="$(cd "$(dirname "$0")" && pwd)"
root="$(cd "$here/.." && pwd)"
CLANG="${CLANG:-clang}"
WASM_LD="${WASM_LD:-wasm-ld}"

readme="$(cat "$here/generated/README.txt")"
want_clang="$(sed -n 's/^clang: //p' <<<"$readme")"
have_clang="$("$CLANG" --version)"
have_clang="${have_clang%%$'\n'*}"
if [[ "$have_clang" != "$want_clang" ]]; then
  echo "check_generated.sh: x86sandbox/generated was made with \"$want_clang\"," >&2
  echo "but $CLANG is \"$have_clang\". Set CLANG and WASM_LD to that compiler." >&2
  exit 1
fi

script="$(cat "$here/regenerate.sh")"
wabt_commit="$(sed -n 's/^WABT_COMMIT=//p' <<<"$script")"
if [[ -z "$wabt_commit" ]]; then
  echo "check_generated.sh: no WABT_COMMIT in regenerate.sh" >&2
  exit 1
fi

work=""
if [[ -z "${WABT_DIR:-}" ]]; then
  work="$(mktemp -d)"
  trap 'rm -rf "$work"' EXIT
  WABT_DIR="$work/wabt"
  git init -q "$WABT_DIR"
  git -C "$WABT_DIR" fetch -q --depth 1 https://github.com/WebAssembly/wabt "$wabt_commit"
  git -C "$WABT_DIR" checkout -q FETCH_HEAD
  # WABT uses OpenSSL for SHA-256 if it is installed, and this submodule otherwise.
  git -C "$WABT_DIR" submodule update -q --init --depth 1 third_party/picosha2
fi

CLANG="$CLANG" WASM_LD="$WASM_LD" WABT_DIR="$WABT_DIR" "$here/regenerate.sh"

if ! git -C "$root" diff --exit-code --stat -- x86sandbox/generated; then
  echo "check_generated.sh: x86sandbox/generated does not match the guest sources." >&2
  echo "Run x86sandbox/regenerate.sh with \"$want_clang\" and commit the result." >&2
  exit 1
fi
if [[ -n "$(git -C "$root" status --porcelain -- x86sandbox/generated)" ]]; then
  echo "check_generated.sh: regenerate.sh made files in x86sandbox/generated that are not committed:" >&2
  git -C "$root" status --porcelain -- x86sandbox/generated >&2
  exit 1
fi
echo "x86sandbox/generated matches the guest sources"
