#!/bin/bash
OUT=${1:-deps}
DEPS_FILE=${2:-deps.json}

set -e

mkdir -p "$OUT" >/dev/null 2>&1

if [[ ! -f "$DEPS_FILE" ]]; then
    printf "  ERROR\tFile '%s' not found.\n" "$DEPS_FILE" >&2
    exit 1
fi

download_file() {
    local url=$1
    local output=$2

    if command -v wget &> /dev/null; then
        wget "$url" -O "$output" >/dev/null 2>&1
    elif command -v curl &> /dev/null; then
        curl -L "$url" -o "$output" >/dev/null 2>&1
    else
        printf "  ERROR\tNeither wget nor curl is available for downloading.\n" >&2
        exit 1
    fi
}

cat "$DEPS_FILE" | jq -c '.[]' | while IFS= read -r dep; do
    name=$(echo "$dep" | jq -r '.name')
    url=$(echo "$dep" | jq -r '.url')
    output_zip="$OUT/$name.zip"
    output_dir="$OUT/$name"

    if [[ -d "$output_dir" ]]; then
        printf "  INFO\tSkipping %s, already downloaded.\n" "$name"
        continue
    fi

    printf "  INFO\tDownloading %s...\n" "$name"
    download_file "$url" "$output_zip"

    mkdir -p "$output_dir" >/dev/null 2>&1

    unzip -q -j "$output_zip" -d "$output_dir" >/dev/null 2>&1
    rm "$output_zip" >/dev/null 2>&1

    printf "  INFO\tDownloaded and extracted %s.\n" "$name"
done
