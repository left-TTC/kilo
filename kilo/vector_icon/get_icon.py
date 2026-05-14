import os
import shutil

def collect_icon_files(src_folder, dst_folder, extension=".icon"):
    """
    遍历 src_folder 找到所有指定扩展名文件，并复制到 dst_folder，
    只有包含该扩展名文件的目录才创建目标目录。
    
    :param src_folder: 源文件夹
    :param dst_folder: 目标文件夹
    :param extension: 文件扩展名（默认 .icon）
    """
    src_folder = os.path.abspath(src_folder)
    dst_folder = os.path.abspath(dst_folder)

    for root, dirs, files in os.walk(src_folder):
        # 找出当前目录下符合条件的文件
        icon_files = [f for f in files if f.lower().endswith(extension.lower())]
        if not icon_files:
            continue  # 如果没有 .icon 文件，跳过这个目录

        # 计算相对路径
        rel_path = os.path.relpath(root, src_folder)
        target_dir = os.path.join(dst_folder, rel_path)
        os.makedirs(target_dir, exist_ok=True)

        for file in icon_files:
            src_file = os.path.join(root, file)
            dst_file = os.path.join(target_dir, file)
            shutil.copy2(src_file, dst_file)
            print(f"Copied: {src_file} -> {dst_file}")

if __name__ == "__main__":
    source_folder = "/home/f/myproject/Brave/src/brave"
    destination_folder = "./icon"

    collect_icon_files(source_folder, destination_folder)
    print("All .icon files collected successfully.")