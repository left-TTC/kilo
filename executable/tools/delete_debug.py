import os
import glob
import shutil

# -----------------------------
# 配置部分
# -----------------------------
RELEASE_DIR = r"/Users/left/Documents/project/kilo/mac-pack"  # 你的 Release 输出目录

# 需要保留的文件夹
KEEP_FOLDERS = ["resources", "locales", "WidevineCdm"]

# 主程序保留
KEEP_FILES = ["brave.exe", "icudtl.dat"]

# 可以安全删除的文件类型/模式
DELETE_PATTERNS = [
    "*.pdb",    # 调试符号
    "*.log",    # 日志
    "*.dmp",    # 崩溃转储
    "*.obj",    # 编译中间文件
    "*.lib",
    "*.exp",
    "README*",
    "LICENSE*",
    "*.md"
]

# -----------------------------
# 删除文件函数
# -----------------------------
def delete_safe_files(base_dir):
    # 遍历 Release 目录
    for root, dirs, files in os.walk(base_dir):
        # 排除核心文件夹
        dirs[:] = [d for d in dirs if d not in KEEP_FOLDERS]

        for f in files:
            file_path = os.path.join(root, f)
            # 保留核心文件
            if f in KEEP_FILES:
                continue
            # 匹配可删除模式
            for pattern in DELETE_PATTERNS:
                if glob.fnmatch.fnmatch(f, pattern):
                    try:
                        os.remove(file_path)
                        print(f"[删除] {file_path}")
                    except Exception as e:
                        print(f"[错误] 删除 {file_path} 失败: {e}")
                    break  # 匹配一次就删除

# -----------------------------
# 执行
# -----------------------------
if __name__ == "__main__":
    print(f"开始清理 Release 目录: {RELEASE_DIR}")
    delete_safe_files(RELEASE_DIR)
    print("清理完成！")
