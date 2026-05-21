import os
import shutil

# -----------------------------
# 配置部分
# -----------------------------
TARGET_DIR = r"/Users/left/Documents/project/kilo/mac-pack"  # 要清理的目录
TEMP_ISOLATE = os.path.join(TARGET_DIR, "_siso_backup")  # 隔离目录
RUN_BACKUP = True  # True 表示保留隔离文件，可手动恢复

# -----------------------------
# 创建隔离目录
# -----------------------------
os.makedirs(TEMP_ISOLATE, exist_ok=True)

# -----------------------------
# 遍历 TARGET_DIR
# -----------------------------
for root, dirs, files in os.walk(TARGET_DIR):
    # 排除隔离目录本身
    dirs[:] = [d for d in dirs if d != "_siso_backup"]

    for f in files:
        if "siso" in f.lower():  # 文件名包含 siso，忽略大小写
            src_path = os.path.join(root, f)
            rel_path = os.path.relpath(src_path, TARGET_DIR)
            dst_path = os.path.join(TEMP_ISOLATE, rel_path)

            # 创建目标目录
            os.makedirs(os.path.dirname(dst_path), exist_ok=True)

            # 移动文件到隔离目录
            shutil.move(src_path, dst_path)
            print(f"[隔离] {rel_path}")

print("清理完成！")
print(f"所有 siso 文件已移动到: {TEMP_ISOLATE}")
print("如果需要回退，可将隔离目录中的文件移回原位置。")
