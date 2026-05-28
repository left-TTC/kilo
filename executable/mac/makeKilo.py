#!/usr/bin/env python3

"""
Kilo Browser macOS build orchestrator.

Orchestrates the full build pipeline:
  1. getKilo.py  - Copy built app from output directory
  2. sign.sh     - Code sign the app bundle
  3. verify.sh   - Verify code signature and Gatekeeper
  4. package.sh  - Create and sign DMG
  5. notarize.sh - Submit for Apple notarization

Each step's output is parsed to determine success/failure.
After the pipeline, spctl is run to confirm notarization status.
If not notarized, possible reasons are analyzed and reported.
"""

import os
import subprocess
import sys
import shlex

# ── Paths ──────────────────────────────────────────────────────────────────

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
TOOLS_DIR = os.path.join(SCRIPT_DIR, "tools")
APP_DIR = os.path.join(SCRIPT_DIR, "app")
APP_NAME = "Kilo Browser.app"
APP_PATH = os.path.join(APP_DIR, APP_NAME)
DMG_NAME = "Kilo Browser.dmg"
DMG_PATH = os.path.join(APP_DIR, DMG_NAME)

# ── Utilities ──────────────────────────────────────────────────────────────


def run(cmd, cwd=None, env=None):
    """Run a command, streaming output in real-time, and return (returncode, stdout_lines, stderr_lines)."""
    print(f"\n{'='*60}")
    print(f"▶ Running: {' '.join(shlex.quote(str(a)) for a in cmd)}")
    print(f"{'='*60}")

    process = subprocess.Popen(
        cmd,
        cwd=cwd or TOOLS_DIR,
        env={**os.environ, **(env or {})},
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        bufsize=1,
    )

    stdout_lines = []
    stderr_lines = []

    def read_stream(stream, lines_list, label=""):
        for line in iter(stream.readline, ""):
            lines_list.append(line.rstrip("\n"))
            print(line, end="", flush=True)
        stream.close()

    import threading

    t_out = threading.Thread(target=read_stream, args=(process.stdout, stdout_lines))
    t_err = threading.Thread(target=read_stream, args=(process.stderr, stderr_lines))
    t_out.start()
    t_err.start()
    t_out.join()
    t_err.join()

    process.wait()
    return process.returncode, stdout_lines, stderr_lines


def check_success(returncode, stdout_lines, stderr_lines, step_name):
    """Check if a step succeeded based on return code and output."""
    if returncode != 0:
        print(f"\n❌ [{step_name}] FAILED (exit code {returncode})")
        return False

    # Also check for success markers in output
    success_markers = [
        "✅ SIGN SUCCESS",
        "✅ PACKAGE SUCCESS",
        "✅ NOTARIZATION SUCCESS",
        "✅ VERIFY SUCCESS",
        "[DONE]",
        "[SKIP]",
    ]
    for line in stdout_lines + stderr_lines:
        for marker in success_markers:
            if marker in line:
                print(f"\n✅ [{step_name}] SUCCESS")
                return True

    # If no explicit success marker but exit code is 0, still consider success
    print(f"\n✅ [{step_name}] SUCCESS (exit code 0)")
    return True


def check_notarization():
    """Verify notarization by mounting the DMG and checking the app inside."""
    print(f"\n{'='*60}")
    print("▶ Checking notarization status...")
    print(f"{'='*60}")

    if not os.path.isfile(DMG_PATH):
        print("❌ DMG not found, cannot check notarization.")
        return False

    # Mount DMG to check the app inside (ticket is stapled to DMG)
    result = subprocess.run(
        ["hdiutil", "attach", "-nobrowse", "-plist", DMG_PATH],
        capture_output=True, text=True,
    )
    if result.returncode != 0:
        print("❌ Failed to mount DMG for verification.")
        return False

    import plistlib
    pl = plistlib.loads(result.stdout.encode())
    mount_pt = None
    for e in pl.get("system-entities", []):
        mp = e.get("mount-point")
        if mp:
            mount_pt = mp
            break

    if not mount_pt:
        print("❌ Could not find mount point.")
        return False

    # Find the app inside the mounted DMG
    app_in_dmg = os.path.join(mount_pt, APP_NAME)
    if not os.path.isdir(app_in_dmg):
        print(f"❌ App not found inside DMG: {app_in_dmg}")
        subprocess.run(["hdiutil", "detach", mount_pt, "-force"], capture_output=True)
        return False

    # Check notarization of app inside DMG
    result = subprocess.run(
        ["spctl", "-a", "-t", "exec", "-vv", app_in_dmg],
        capture_output=True, text=True,
    )
    output = result.stdout + result.stderr
    print(output)

    # Detach
    subprocess.run(["hdiutil", "detach", mount_pt, "-force"], capture_output=True)

    notarized_indicators = [
        "accepted",
        "notarized",
        "source=Notarized",
    ]
    not_notarized_indicators = [
        "rejected",
        "denied",
        "CSSMERR_TP_CERT_REVOKED",
        "code object is not signed at all",
    ]

    is_notarized = any(ind in output for ind in notarized_indicators)
    is_rejected = any(ind in output for ind in not_notarized_indicators)

    if is_notarized and not is_rejected:
        print("\n✅ NOTARIZATION CONFIRMED: App is notarized and trusted by Gatekeeper.")
        return True
    else:
        print("\n❌ NOTARIZATION CHECK FAILED.")
        return False


def analyze_notarization_failure():
    """Analyze and report possible reasons for notarization failure."""
    print(f"\n{'='*60}")
    print("🔍 Analyzing possible notarization failure reasons...")
    print(f"{'='*60}")

    issues = []

    # 1. Check if app exists
    if not os.path.isdir(APP_PATH):
        issues.append("❌ App bundle not found at: " + APP_PATH)
    else:
        # 2. Check code signature
        result = subprocess.run(
            ["codesign", "-dv", "--verbose=2", APP_PATH],
            capture_output=True, text=True,
        )
        sig_output = result.stdout + result.stderr
        if result.returncode != 0:
            issues.append("❌ App is not signed or signature is invalid.")
        else:
            print("  ✓ Code signature present")
            # Check for runtime option
            if "runtime" not in sig_output and "flags" not in sig_output:
                issues.append("⚠️  App may be missing the 'runtime' entitlement (hardened runtime).")

        # 3. Check DMG exists
        if not os.path.isfile(DMG_PATH):
            issues.append("❌ DMG not found at: " + DMG_PATH)
        else:
            # Check DMG signature
            result = subprocess.run(
                ["codesign", "-dv", DMG_PATH],
                capture_output=True, text=True,
            )
            if result.returncode != 0:
                issues.append("❌ DMG is not signed.")

        # 4. Check entitlements
        entitlements_path = os.path.join(SCRIPT_DIR, "app", "entitlements.plist")
        if os.path.isfile(entitlements_path):
            print("  ✓ Entitlements file found")
        else:
            issues.append("❌ Entitlements file not found.")

        # 5. Check if stapler validate fails
        result = subprocess.run(
            ["xcrun", "stapler", "validate", DMG_PATH],
            capture_output=True, text=True,
        )
        staple_output = result.stdout + result.stderr
        if result.returncode != 0:
            issues.append("❌ Stapler validation failed (notarization ticket not attached).")
            if "The validate action failed" in staple_output:
                issues.append("  → The notarization ticket was not stapled to the DMG.")
        else:
            print("  ✓ Stapler validation passed")

        # 6. Check network / Apple service status
        result = subprocess.run(
            ["xcrun", "notarytool", "history", "--apple-id", "test@test.com",
             "--team-id", "test", "--password", "test"],
            capture_output=True, text=True,
        )
        err_output = result.stderr + result.stdout
        if "Could not communicate with the notary service" in err_output:
            issues.append("⚠️  Could not connect to Apple notary service (network issue or service down).")

    # Print analysis
    if issues:
        print(f"\n{'='*60}")
        print("📋 NOTARIZATION FAILURE ANALYSIS:")
        print(f"{'='*60}")
        for issue in issues:
            print(f"  {issue}")
        print()
        print("💡 Suggested fixes:")
        print("  1. Ensure the app is properly signed with hardened runtime (`--options runtime`)")
        print("  2. Verify the DMG is also signed")
        print("  3. Check that .env contains valid Apple ID credentials")
        print("  4. Generate a new app-specific password at: https://appleid.apple.com/account/manage")
        print("  5. Try running notarize.sh manually to see detailed error output")
        print("  6. Check Apple System Status: https://developer.apple.com/system-status/")
    else:
        print("\n  ✓ No obvious issues found. The notarization may have failed due to")
        print("    Apple server issues or credential problems. Check the notarize.sh output above.")

    return issues


# ── Main Pipeline ──────────────────────────────────────────────────────────


def main():
    print(f"{'='*60}")
    print("🚀 Kilo Browser macOS Build Pipeline")
    print(f"{'='*60}")
    print(f"  App:  {APP_PATH}")
    print(f"  DMG:  {DMG_PATH}")
    print(f"{'='*60}")

    steps = [
        ("getKilo.py", ["python3", "getKilo.py"]),
        ("sign.sh",    ["bash", "sign.sh"]),
        ("verify.sh",  ["bash", "verify.sh"]),
        ("package.sh", ["bash", "package.sh"]),
        ("notarize.sh",["bash", "notarize.sh"]),
    ]

    for step_name, cmd in steps:
        returncode, stdout, stderr = run(cmd)
        if not check_success(returncode, stdout, stderr, step_name):
            print(f"\n⛔ Pipeline aborted at step: {step_name}")
            sys.exit(1)

    # ── Post-pipeline: Verify notarization ──
    print(f"\n{'='*60}")
    print("🔍 Post-pipeline notarization verification")
    print(f"{'='*60}")

    notarized = check_notarization()
    if not notarized:
        analyze_notarization_failure()
        print(f"\n{'='*60}")
        print("⚠️  PIPELINE COMPLETED BUT NOTARIZATION NOT CONFIRMED")
        print(f"{'='*60}")
        sys.exit(1)
    else:
        print(f"\n{'='*60}")
        print("🎉 ALL CHECKS PASSED - APP IS READY FOR DISTRIBUTION")
        print(f"{'='*60}")


if __name__ == "__main__":
    main()
