#!/bin/bash

for file in "$@"; do
    [[ ! -f "$file" ]] && continue
    type=$(file -b "$file")

    case "$type" in
        *gzip*)       gunzip "$file" ;;
        *bzip2*)      bunzip2 "$file" ;;
        *XZ*)         unxz "$file" ;;
        *Zip*)        unzip -o "$file" >/dev/null ;;
        *tar*)        tar -xf "$file" ;;
        *7-zip*)      7z x -y "$file" >/dev/null ;;
        *RAR*)        unrar x -o+ "$file" >/dev/null ;;
        *)            echo "skip: $file" >&2 ;;
    esac
done