import os
import shutil

# -----------------------------
# 配置
# -----------------------------
TARGET_DIR = r"D:\Release"  # 要清理的目录
TEMP_BACKUP = os.path.join(TARGET_DIR, "_test_backup")  # 临时隔离目录

KEEP_FOLDERS = ["resources", "locales", "WidevineCdm"]
KEEP_FILES = ["brave.exe", "icudtl.dat"]

os.makedirs(TEMP_BACKUP, exist_ok=True)

# -----------------------------
# 遍历目录
# -----------------------------
for root, dirs, files in os.walk(TARGET_DIR, topdown=True):
    # 排除隔离目录本身
    dirs[:] = [d for d in dirs if d != os.path.basename(TEMP_BACKUP)]

    # 处理文件夹名包含 test
    for d in dirs[:]:
        if "test" in d.lower() and d not in KEEP_FOLDERS:
            src_dir = os.path.join(root, d)
            rel_dir = os.path.relpath(src_dir, TARGET_DIR)
            dst_dir = os.path.join(TEMP_BACKUP, rel_dir)
            os.makedirs(os.path.dirname(dst_dir), exist_ok=True)
            shutil.move(src_dir, dst_dir)
            print(f"[隔离文件夹] {rel_dir}")
            dirs.remove(d)  # 避免递归重复

    # 处理文件名包含 test
    for f in files:
        if "test" in f.lower() and f not in KEEP_FILES:
            src_file = os.path.join(root, f)
            rel_file = os.path.relpath(src_file, TARGET_DIR)
            dst_file = os.path.join(TEMP_BACKUP, rel_file)
            os.makedirs(os.path.dirname(dst_file), exist_ok=True)
            shutil.move(src_file, dst_file)
            print(f"[隔离文件] {rel_file}")

print("清理完成！")
print(f"所有包含 test 的文件/文件夹已移动到: {TEMP_BACKUP}")
