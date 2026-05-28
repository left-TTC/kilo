#!/bin/bash

set -e

source ./config.sh

echo "=========================================="
echo "Signing app..."

if [ ! -d "$APP_PATH" ]; then
    echo "❌ App not found"
    exit 1
fi

# =========================================================

# SIGN MACH-O FILES

# =========================================================

while IFS= read -r file; do
    if file "$file" | grep -q "Mach-O"; then
        echo "Signing: $file"
        codesign \
            --force \
            --timestamp \
            --options runtime \
            --entitlements "$ENTITLEMENTS" \
            --sign "$SIGN_IDENTITY" \
            "$file"
    fi
done < <(find "$APP_PATH" -type f)

# =========================================================

# SIGN NESTED BUNDLES

# =========================================================

while IFS= read -r bundle; do
    echo "Signing bundle: $bundle"
    codesign \
        --force \
        --timestamp \
        --options runtime \
        --entitlements "$ENTITLEMENTS" \
        --sign "$SIGN_IDENTITY" \
        "$bundle"
done < <(find "$APP_PATH" \( \
    -name "*.app" \
    -o -name "*.framework" \
    -o -name "*.xpc" \
    -o -name "*.appex" \
    -o -name "*.bundle" \
\))

# =========================================================

# SIGN MAIN APP

# =========================================================

echo "Signing main app..."
codesign \
    --force \
    --deep \
    --timestamp \
    --options runtime \
    --entitlements "$ENTITLEMENTS" \
    --sign "$SIGN_IDENTITY" \
    "$APP_PATH"

echo "=========================================="
echo "✅ SIGN SUCCESS"
