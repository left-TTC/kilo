import os
import shutil
import sys
import json

# Default paths
PROJECT_DIR = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
CONFIG_PATH = os.path.join(PROJECT_DIR, "project_build", "config.json")
TARGET_DIR = os.path.join(PROJECT_DIR, "executable", "mac", "app")
APP_NAME = "Kilo Browser.app"


def load_config():
    """Load build config to get brave source path."""
    if not os.path.isfile(CONFIG_PATH):
        return None
    with open(CONFIG_PATH, "r", encoding="utf-8") as f:
        return json.load(f)


def get_default_source():
    """Get default source path from config.
    
    brave.path points to the brave source directory, e.g. /path/to/src/brave.
    The build output is at the parent's out/Release_arm64, i.e. /path/to/src/out/Release_arm64.
    """
    config = load_config()
    if config:
        brave_path = config.get("brave", {}).get("path", "")
        if brave_path:
            src_dir = os.path.dirname(brave_path)
            return os.path.join(src_dir, "out", "Release_arm64")
    return ""


def fail(msg):
    print(f"[ERROR] {msg}")
    sys.exit(1)


def main():
    # Determine source directory
    if len(sys.argv) > 1:
        source_dir = sys.argv[1]
    else:
        source_dir = get_default_source()

    if not source_dir:
        fail("No source directory specified. Usage: python getKilo.py [source_dir]")

    source_app = os.path.join(source_dir, APP_NAME)
    target_app = os.path.join(TARGET_DIR, APP_NAME)

    # Check if source exists
    if not os.path.isdir(source_app):
        fail(f"Source app not found: {source_app}")

    # If target already exists, skip
    if os.path.isdir(target_app):
        print(f"[SKIP] {APP_NAME} already exists at: {target_app}")
        return

    # Copy the app bundle
    os.makedirs(TARGET_DIR, exist_ok=True)
    print(f"Copying {APP_NAME} ...")
    print(f"  From: {source_app}")
    print(f"  To:   {target_app}")
    shutil.copytree(source_app, target_app, symlinks=True)
    print("[DONE] Copy completed.")


if __name__ == "__main__":
    main()
