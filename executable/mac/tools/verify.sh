#!/bin/bash

set -e

source ./config.sh

echo "=========================================="
echo "Verifying codesign..."

codesign \
    --verify \
    --deep \
    --strict \
    --verbose=4 \
    "$APP_PATH"

echo "=========================================="
echo "Checking Gatekeeper..."

set +e
spctl -a -t exec -vv "$APP_PATH"
SPCTL_RC=$?
set -e
if [ $SPCTL_RC -ne 0 ]; then
    echo ""
    echo "ℹ️  Gatekeeper rejected (expected before notarization)."
    echo "    This will be resolved by notarize.sh later in the pipeline."
fi

echo "=========================================="
echo "Displaying entitlements..."

codesign -d --entitlements :- "$APP_PATH"

echo "=========================================="
echo "Displaying signature info..."

codesign -dv --verbose=4 "$APP_PATH"

echo "=========================================="
echo "✅ VERIFY SUCCESS"
