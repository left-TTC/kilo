#!/bin/bash

set -e

source ./config.sh

# Load Apple ID credentials from .env file
if [ -f .env ]; then
    set -a
    source .env
    set +a
else
    echo "❌ .env file not found. Please create executable/mac/tools/.env with your Apple ID credentials."
    echo ""
    echo "Required variables:"
    echo "  APPLE_ID           - Your Apple ID email"
    echo "  APPLE_TEAM_ID      - Your Apple Team ID"
    echo "  APPLE_APP_PASSWORD - App-specific password for notarization"
    exit 1
fi

if [ -z "$APPLE_ID" ] || [ -z "$APPLE_APP_PASSWORD" ]; then
    echo "❌ APPLE_ID and APPLE_APP_PASSWORD must be set in .env"
    exit 1
fi

echo "=========================================="
echo "Submitting notarization..."

xcrun notarytool submit "$DMG_PATH" \
    --apple-id "$APPLE_ID" \
    --team-id "$APPLE_TEAM_ID" \
    --password "$APPLE_APP_PASSWORD" \
    --wait

echo "=========================================="
echo "Stapling..."

xcrun stapler staple "$DMG_PATH"

echo "=========================================="
echo "Validating notarization..."

set +e
SPCTL_OUT=$(spctl -a -t open -vv "$DMG_PATH" 2>&1)
SPCTL_RC=$?
set -e
echo "$SPCTL_OUT"
if [ $SPCTL_RC -ne 0 ]; then
    echo ""
    echo "ℹ️  spctl rejected DMG (expected - DMG must be mounted to evaluate)."
    echo "    Notarization was confirmed by notarytool + stapler above."
fi

echo "=========================================="
echo "✅ NOTARIZATION SUCCESS"
