import os
from PIL import Image, UnidentifiedImageError

def scale_image(input_path, output_path, width, height, keep_aspect=True):
    """
    Scale image to width x height while keeping aspect ratio.
    """
    try:
        with Image.open(input_path) as img:
            # Convert to RGBA if needed
            if img.mode not in ("RGB", "RGBA"):
                img = img.convert("RGBA")

            # Resize keeping aspect ratio
            img.thumbnail((width, height), Image.LANCZOS)

            # Save as PNG
            img.save(output_path, format="PNG", optimize=True)
            print(f"Saved {output_path} ({img.width}x{img.height})")
    except UnidentifiedImageError:
        print(f"ERROR: Invalid image file: {input_path}")
    except Exception as e:
        print(f"ERROR: {e}")

def generate_scaled_pngs(folder_path):
    """
    Look for kilo-432.png in folder_path and generate PNGs at 108, 162, 216, 324
    """
    folder_path = os.path.abspath(folder_path)
    input_file = os.path.join(folder_path, "kilo-432.png")
    
    if not os.path.isfile(input_file):
        raise FileNotFoundError(f"Input image not found: {input_file}")

    sizes = [108, 162, 216, 324]
    for size in sizes:
        base_name = f"kilo-{size}.png"
        output_file = os.path.join(folder_path, base_name)
        scale_image(input_file, output_file, size, size)

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Generate scaled PNGs from kilo-432.png")
    parser.add_argument("folder", help="Folder containing kilo-432.png")
    args = parser.parse_args()

    generate_scaled_pngs(args.folder)
