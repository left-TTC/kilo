# macOS Icon & Assets Guide

This directory contains everything needed to generate macOS app icons and asset catalogs for the Kilo browser.

## Directory Structure

```
kilo/mac/
├── help.md                          # This file
├── Assets.car                       # Compiled asset catalog (final output)
├── app.icns                         # macOS app icon (.icns format)
├── Assets.xcassets/                 # Xcode asset catalog source
│   ├── Contents.json
│   ├── AppIcon.appiconset/          # App icon set (for .app bundle)
│   │   ├── Contents.json
│   │   ├── icon_16x16.png
│   │   ├── icon_16x16@2x.png
│   │   ├── icon_32x32.png
│   │   ├── icon_32x32@2x.png
│   │   ├── icon_128x128.png
│   │   ├── icon_128x128@2x.png
│   │   ├── icon_256x256.png
│   │   ├── icon_256x256@2x.png
│   │   ├── icon_512x512.png
│   │   └── icon_512x512@2x.png
│   └── Icon.iconset/               # Legacy .icns icon set
│       ├── icon_16x16.png
│       ├── icon_16x16@2x.png
│       ├── icon_32x32.png
│       ├── icon_32x32@2x.png
│       ├── icon_128x128.png
│       ├── icon_128x128@2x.png
│       ├── icon_256x256.png
│       ├── icon_256x256@2x.png
│       ├── icon_512x512.png
│       └── icon_512x512@2x.png
└── AppIcon.icon/                   # macOS 11+ Big Sur icon (iconutil input)
    ├── icon.json
    └── Assets/
        └── kilo_1024 3.png
```

## Related Directories

| Directory | Description |
|-----------|-------------|
| `kilo/PNG/` | Source PNG images (common, beta, dev, nightly) |
| `kilo/PNG/common/icns/` | Scripts for generating .icns input images |
| `kilo/svg/` | SVG source files |
| `kilo/ico/` | Windows ICO generation |
| `kilo/icns/` | Pre-built .icns files for all channels |
| `kilo/word/` | Wordmark logo assets |
| `resource_img/app/theme/brave/mac/` | Final Assets.car & app.icns for all channels |

## Icon Channels

The Kilo browser has multiple release channels, each with its own icon set:

| Channel | Directory | ICO File |
|---------|-----------|----------|
| **Common (stable)** | `kilo/PNG/common/` | `kilo-256.ico` |
| **Beta** | `kilo/PNG/beta/` | `kilobeta-256.ico` |
| **Dev** | `kilo/PNG/dev/` | `kilodev-256.ico` |
| **Nightly** | `kilo/PNG/nightly/` | `kilonightly-256.ico` |

## Workflow

### 1. Source Images

Source PNG images are stored in `kilo/PNG/` organized by channel:

- `kilo/PNG/common/kilo.png` — Main source image (1024×1024)
- `kilo/PNG/common/kilo_1024.png` — 1024×1024 variant
- `kilo/PNG/common/kilo_256.png` — 256×256 variant
- etc.

Use `kilo/PNG/scale_img.py` to generate resized versions:

```bash
# Generate all standard icon sizes from a source image
python3 kilo/PNG/scale_img.py --in kilo/PNG/common/kilo.png

# Scale to specific dimensions
python3 kilo/PNG/scale_img.py --in input.png --out output.png --w 256 --h 256
```

### 2. Generate macOS .icns Input Images

The script `kilo/PNG/common/icns/get_white.py` processes source images for .icns generation:

- Reads PNGs from `kilo/PNG/common/icns/input/`
- Scales them to 82% of original size
- Centers them on a transparent background (preserving original dimensions)
- Outputs to `kilo/PNG/common/icns/output/`

```bash
cd kilo/PNG/common/icns
python3 get_white.py
```

### 3. Generate macOS .icns

Use `iconutil` (bundled with Xcode) to compile an iconset into .icns:

```bash
# From Icon.iconset (legacy format)
iconutil -c icns kilo/mac/Assets.xcassets/Icon.iconset -o kilo/mac/app.icns

# From AppIcon.icon (macOS 11+ Big Sur style)
iconutil -c icns kilo/mac/AppIcon.icon -o kilo/mac/app.icns
```

Pre-built .icns files for all channels are in `kilo/icns/`:
- `kilo/icns/kilo.icns`
- `kilo/icns/beta/app.icns`
- `kilo/icns/dev/app.icns`
- `kilo/icns/developmemt/app.icns`
- `kilo/icns/night/app.icns`

### 4. Generate Assets.car (Compiled Asset Catalog)

`Assets.car` is the compiled binary asset catalog used by macOS apps.

**Reference:** https://chromium.googlesource.com/chromium/src.git/%2B/refs/heads/main/docs/mac/icons.md

**Compile using Chromium's script:**

```bash
python3 tools/mac/icons/compile_car.py \
    kilo/mac/Assets.xcassets \
    -o kilo/mac/Assets.car
```

The `Assets.xcassets` directory contains:
- `Contents.json` — Root asset catalog metadata
- `AppIcon.appiconset/` — App icon definitions with `Contents.json` and PNG files at all required sizes (16×16, 32×32, 128×128, 256×256, 512×512, each with @2x variants)

### 5. Copy to Resource Directory

After generating `Assets.car` and `app.icns`, copy them to the appropriate channel directory under `resource_img/app/theme/brave/mac/`:

```bash
# Stable
cp kilo/mac/Assets.car resource_img/app/theme/brave/mac/
cp kilo/mac/app.icns resource_img/app/theme/brave/mac/

# Beta
cp kilo/mac/Assets.car resource_img/app/theme/brave/mac/beta/
cp kilo/mac/app.icns resource_img/app/theme/brave/mac/beta/

# Dev
cp kilo/mac/Assets.car resource_img/app/theme/brave/mac/dev/
cp kilo/mac/app.icns resource_img/app/theme/brave/mac/dev/

# Development
cp kilo/mac/Assets.car resource_img/app/theme/brave/mac/development/
cp kilo/mac/app.icns resource_img/app/theme/brave/mac/development/

# Nightly
cp kilo/mac/Assets.car resource_img/app/theme/brave/mac/nightly/
cp kilo/mac/app.icns resource_img/app/theme/brave/mac/nightly/
```

### 6. Generate Windows ICO (Optional)

Use `kilo/ico/turn_ico.py` to convert PNG to Windows ICO format:

```bash
python3 kilo/ico/turn_ico.py --in kilo/PNG/common/kilo.png --out kilo/ico/kilo-256.ico
```

## SVG Source

SVG source files are in `kilo/svg/`:
- `kilo.svg` — Main Kilo logo
- `kilo-64.svg` through `kilo-256.svg` — Various sizes
- `base64` — Base64 encoded version

## String Replacement Utility

The `kilo/str/` directory contains utilities for string operations:

- `replace_single_string.py` — Replace strings in files (supports plain text and regex)
- `get_str_number.py` — Number duplicate URL strings in `url_constants.h`

## Notes

- All PNG icons should be generated from the SVG source for consistency
- The `AppIcon.icon` directory uses the modern macOS 11+ Big Sur icon format with `icon.json` defining appearance (gradient fill, shadow, translucency)
- `Assets.car` is the format used by Chromium-based browsers for their macOS app icons
- For any issues, refer to https://support.brave.app/ as this browser is based on Brave
