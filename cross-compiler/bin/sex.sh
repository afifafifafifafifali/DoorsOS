#!/usr/bin/env bash
set -euo pipefail

DIR="${1:-.}"

OLD="x86_64-elf-"
NEW="x86_64-doorsos-"

echo "📦 Renaming ALL binaries in: $DIR"
echo "🔁 $OLD → $NEW"
echo ""

shopt -s nullglob

for file in "$DIR"/${OLD}*; do
    base="$(basename "$file")"
    newname="${base/$OLD/$NEW}"

    if [[ "$base" != "$newname" ]]; then
        mv "$file" "$DIR/$newname"
        echo "✔ $base → $newname"
    fi
done

echo ""
echo "✅ Done."
