#!/bin/bash

# Usage:
# ./compare_bins.sh <regions_root>
# Example:
# ./compare_bins.sh data/analysis/regions

BASE_DIR=$1

DUMP_DIR="$BASE_DIR/dump"
OFC_DIR="$BASE_DIR/official"

HEX_DUMP_DIR="data/hex/dump"
HEX_OFC_DIR="data/hex/official"
OUT_BASE="data/analysis/comparisons"

# ---------- Validation ----------
if [[ -z "$BASE_DIR" ]]; then
    echo "Usage: $0 <regions_root>"
    exit 1
fi

if [[ ! -d "$DUMP_DIR" || ! -d "$OFC_DIR" ]]; then
    echo "[!] dump/ or official/ folder not found inside $BASE_DIR"
    exit 1
fi

mkdir -p "$HEX_DUMP_DIR"
mkdir -p "$HEX_OFC_DIR"
mkdir -p "$OUT_BASE"

echo "[+] Starting batch analysis..."

# ---------- Loop through dump files ----------
for DMP_FILE in "$DUMP_DIR"/*.bin; do

    FILE_NAME=$(basename "$DMP_FILE")
    OFC_FILE="$OFC_DIR/$FILE_NAME"

    if [[ ! -f "$OFC_FILE" ]]; then
        echo "[!] Skipping $FILE_NAME (no matching official file)"
        continue
    fi

    NAME="${FILE_NAME%.bin}"
    OUTDIR="$OUT_BASE/${NAME}_comparison"

    mkdir -p "$OUTDIR"

    echo "[+] Processing: $NAME"

    # ---------- HEX ----------
    xxd "$DMP_FILE" > "$HEX_DUMP_DIR/${NAME}.hex"
    xxd "$OFC_FILE" > "$HEX_OFC_DIR/${NAME}.hex"

    # Copy to comparison folder
    cp "$HEX_DUMP_DIR/${NAME}.hex" "$OUTDIR/dump.hex"
    cp "$HEX_OFC_DIR/${NAME}.hex" "$OUTDIR/official.hex"

    # ---------- Size ----------
    DMP_SIZE=$(wc -c < "$DMP_FILE")
    OFC_SIZE=$(wc -c < "$OFC_FILE")

    {
        echo "Dump size: $DMP_SIZE bytes"
        echo "Official size: $OFC_SIZE bytes"
    } > "$OUTDIR/size.txt"

    # ---------- Strings ----------
    strings "$DMP_FILE" > "$OUTDIR/dump_strings.txt"
    strings "$OFC_FILE" > "$OUTDIR/official_strings.txt"

    # ---------- Diff (hex) ----------
    diff "$OUTDIR/dump.hex" "$OUTDIR/official.hex" > "$OUTDIR/hex_diff.txt"

    # ---------- Diff (strings) ----------
    diff "$OUTDIR/dump_strings.txt" "$OUTDIR/official_strings.txt" > "$OUTDIR/strings_diff.txt"

    # ---------- Hash ----------
    md5sum "$DMP_FILE" > "$OUTDIR/dump_md5.txt"
    md5sum "$OFC_FILE" > "$OUTDIR/official_md5.txt"

done

echo "[+] Done. Everything is in $OUT_BASE"