#!/usr/bin/env bash

set -Eeuo pipefail

source ./config.sh

echo "=========================================="
echo "Building DMG..."

#
# Validate
#
[ -d "$APP_PATH" ] || {
    echo "❌ App not found: $APP_PATH"
    exit 1
}

[ -f "$VOLUME_ICON" ] || {
    echo "❌ Volume icon not found: $VOLUME_ICON"
    exit 1
}

mkdir -p "$(dirname "$DMG_PATH")"

#
# Cleanup old dmg and shadow
#
rm -f "$DMG_PATH" "${DMG_PATH}.shadow"

#
# Temporary staging directory
#
STAGING_DIR="$(mktemp -d)"

cleanup() {
    rm -rf "$STAGING_DIR"
    # Keep shadow file if DMG build failed - useful for debugging
}
trap cleanup EXIT

echo "Using staging directory:"
echo "$STAGING_DIR"

#
# Copy app
#
echo "Copying app..."

cp -R "$APP_PATH" "$STAGING_DIR/"

#
# Build dmg (create-dmg handles UDRW→UDZO conversion)
#
echo "Creating DMG..."
echo "VOL ICON = $VOLUME_ICON"
ls -lh "$VOLUME_ICON"

./create-dmg \
    --volname "${APP_NAME} Installer" \
    --volicon "$VOLUME_ICON" \
    --window-pos 200 120 \
    --window-size 800 500 \
    --icon-size 120 \
    --icon "${APP_NAME}.app" 200 250 \
    --hide-extension "${APP_NAME}.app" \
    --app-drop-link 600 250 \
    --no-internet-enable \
    "$DMG_PATH" \
    "$STAGING_DIR"

echo "=========================================="
echo "DMG built: $DMG_PATH"

echo "=========================================="
echo "DMG created successfully"

echo "Output:"
echo "  $DMG_PATH"

echo "Size:"
du -h "$DMG_PATH"

echo "=========================================="
echo "✅ PACKAGE SUCCESS"
