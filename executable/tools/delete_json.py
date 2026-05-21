import os
import shutil

# -----------------------------
# 配置
# -----------------------------
TARGET_DIR = r"D:\Release"  # Release 输出目录
BACKUP_DIR = os.path.join(TARGET_DIR, "_json_cleanup_backup")

# 需要保留的 JSON 文件（大小写忽略）
KEEP_JSON_PREFIX = ["VkICD", "VkLayer"]

# 创建回退目录
os.makedirs(BACKUP_DIR, exist_ok=True)

# -----------------------------
# 扫描 JSON 文件
# -----------------------------
deletable_json = []

for root, dirs, files in os.walk(TARGET_DIR):
    for f in files:
        if f.lower().endswith(".json"):
            if any(f.lower().startswith(prefix.lower()) for prefix in KEEP_JSON_PREFIX):
                continue  # 保留 GPU/Vulkan JSON
            full_path = os.path.join(root, f)
            rel_path = os.path.relpath(full_path, TARGET_DIR)
            deletable_json.append(rel_path)

# -----------------------------
# 输出可删除列表
# -----------------------------
print("==== 可删除 JSON 文件列表 ====")
for path in deletable_json:
    print(path)

confirm = input("\n是否移动这些 JSON 文件到回退目录？(y/n) ")
if confirm.lower() == "y":
    for rel_path in deletable_json:
        src_path = os.path.join(TARGET_DIR, rel_path)
        dst_path = os.path.join(BACKUP_DIR, rel_path)
        os.makedirs(os.path.dirname(dst_path), exist_ok=True)
        try:
            shutil.move(src_path, dst_path)
            print(f"[移动成功] {rel_path}")
        except Exception as e:
            print(f"[移动失败] {rel_path} -> {e}")
    print(f"\n操作完成，回退目录: {BACKUP_DIR}")
else:
    print("未执行移动操作。")
