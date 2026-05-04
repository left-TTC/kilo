import os
import subprocess
from PIL import Image

input_dir = "./input"
output_dir = "./output"

scale = 0.82

os.makedirs(output_dir, exist_ok=True)

def run(cmd):
    subprocess.run(cmd, shell=True, check=True)

for file in os.listdir(input_dir):
    if not file.lower().endswith(".png"):
        continue

    in_path = os.path.join(input_dir, file)
    out_path = os.path.join(output_dir, file)

    # 1️⃣ 读取原尺寸（关键修复点）
    with Image.open(in_path) as img:
        w, h = img.size

    # 2️⃣ ImageMagick 正确命令
    cmd = f"""
    magick "{in_path}" \
    -alpha on \
    -resize {int(scale*100)}% \
    -background none \
    -gravity center \
    -extent {w}x{h} \
    "{out_path}"
    """

    run(cmd)

print("Done.")