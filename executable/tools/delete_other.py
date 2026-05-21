import os
import shutil

TARGET_DIR = r"D:\Release"
BACKUP_DIR = os.path.join(TARGET_DIR, "_cleanup_backup")

KEEP_FILES = ["kilo.exe", "brave.exe", "icudtl.dat"]
KEEP_FOLDERS = ["resources", "locales", "WidevineCdm"]

DELETE_KEYWORDS = [
    "build.ninja",
    ".runtime_deps",
    "generator.exe",
    "plugin.exe",
    "bench",
    "fuzzer",
    "exercise",
    "ghost_rules",
    "flatc.exe",
    "mksnapshot.exe",
    "protoc",
    "toolchain.ninja",
    "torque.exe",
    "v8_context_snapshot_generator.exe",
    "dbgcore.dll",
    "dbghelp.dll",
    "gpu_benchmark",
    "leveldb_db_bench",
    "devtools_protocol_encoding_json_fuzzer",
    "exercise_2_solution",
    "args.gn",
    "args_generated.gni",
    "gn_args_page_graph.json",
    "gn_logs.txt",
    "environment.x64",
    "environment.x86",
    "first run",
    "version",
]

os.makedirs(BACKUP_DIR, exist_ok=True)

deletable = []

# 扫描可删除列表
for root, dirs, files in os.walk(TARGET_DIR, topdown=True):
    dirs[:] = [d for d in dirs if d != os.path.basename(BACKUP_DIR)]

    for d in dirs:
        if d in KEEP_FOLDERS:
            continue
        if any(kw.lower() in d.lower() for kw in DELETE_KEYWORDS):
            rel_dir = os.path.relpath(os.path.join(root, d), TARGET_DIR)
            deletable.append(("dir", rel_dir))

    for f in files:
        if f in KEEP_FILES:
            continue
        if any(kw.lower() in f.lower() for kw in DELETE_KEYWORDS):
            rel_file = os.path.relpath(os.path.join(root, f), TARGET_DIR)
            deletable.append(("file", rel_file))

# 输出列表
print("==== 可删除列表 ====")
for typ, path in deletable:
    print(f"{typ}: {path}")

confirm = input("\n是否移动这些文件/文件夹到回退目录？(y/n) ")
if confirm.lower() == "y":
    for typ, path in deletable:
        src_path = os.path.join(TARGET_DIR, path)
        dst_path = os.path.join(BACKUP_DIR, path)

        # 1️⃣ 源文件/目录存在才处理
        if not os.path.exists(src_path):
            print(f"[跳过] 文件/目录不存在: {path}")
            continue

        # 2️⃣ 确保目标目录存在
        os.makedirs(os.path.dirname(dst_path), exist_ok=True)

        # 3️⃣ 移动或复制
        try:
            if typ == "file":
                shutil.move(src_path, dst_path)
            else:
                # 目录移动，Windows 上深层目录确保安全
                shutil.move(src_path, dst_path)
            print(f"[移动成功] {path}")
        except Exception as e:
            print(f"[移动失败] {path} -> {e}")

print(f"\n操作完成，可回退目录: {BACKUP_DIR}")
