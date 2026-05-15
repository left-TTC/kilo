#!/usr/bin/env python3
import os
import stat

BUILD_ROOT = "./pgk/kilo-build"

# Core files requiring 0755 permissions (directories, or owner-readable/writable/executable; others-readable/executable)
# Note: Even though `chrome_sandbox` ultimately requires 4755 permissions, assigning 0755 during the packaging stage is sufficient;
# the actual 4755 permissions will be set by the `postinst` script on the target machine, running as root.
EXECUTABLES = [
    "DEBIAN",            
    "DEBIAN/preinst",
    "DEBIAN/postinst",
    "DEBIAN/prerm",
    "DEBIAN/postrm",
    "opt/kilo-browser/kilo",
    "opt/kilo-browser/kilo-browser-wrapper",
    "opt/kilo-browser/chrome_crashpad_handler",
    "opt/kilo-browser/chrome_sandbox"
]


STRICT_REGULAR_FILES = [
    "DEBIAN/control",
    "usr/share/applications/kilo-browser.desktop",
    "etc/apparmor.d/kilo-browser"
]

RESOURCE_EXTENSIONS = ('.png', '.pak', '.bin', '.dat', '.so', '.pak.info')

def set_permission(rel_path, mode, mode_name):
    """为指定路径设置权限，包含存在性检查"""
    full_path = os.path.join(BUILD_ROOT, rel_path)
    if os.path.exists(full_path):
        os.chmod(full_path, mode)
        print(f"[成功] {mode_name} -> {rel_path}")
    else:
        print(f"[忽略] 文件不存在，已跳过: {rel_path}")

def main():
    if not os.path.isdir(BUILD_ROOT):
        print(f"错误: 找不到打包根目录 '{BUILD_ROOT}'，请检查脚本运行位置。")
        return

    print("=== 开始设置 Kilo Browser 打包权限 ===")

    # 1. 设置可执行文件和目录 (0755)
    print("\n--- 设置可执行权限 (0755) ---")
    for item in EXECUTABLES:
        set_permission(item, 0o755, "0755")

    # 2. 设置严格的普通文件 (0644)
    print("\n--- 设置基础配置权限 (0644) ---")
    for item in STRICT_REGULAR_FILES:
        set_permission(item, 0o644, "0644")

    # 3. 自动遍历 /opt/ 目录，将所有资源文件设为 0644，将所有子目录设为 0755
    print("\n--- 自动处理底层资源文件 ---")
    opt_dir = os.path.join(BUILD_ROOT, "opt")
    if os.path.exists(opt_dir):
        for root, dirs, files in os.walk(opt_dir):
            # 将所有子目录设为 0755
            for d in dirs:
                dir_path = os.path.join(root, d)
                os.chmod(dir_path, 0o755)
            
            # 匹配后缀名的文件设为 0644
            for f in files:
                if f.endswith(RESOURCE_EXTENSIONS):
                    file_path = os.path.join(root, f)
                    os.chmod(file_path, 0o644)
        print(f"[成功] 已将 /opt 目录下所有的 {RESOURCE_EXTENSIONS} 文件设为 0644")
        print(f"[成功] 已将 /opt 目录下所有的子目录设为 0755")

    # 4. 如果 /usr/bin/kilo-browser 是软链接，无需改权限；如果是文件，提醒一下
    link_path = os.path.join(BUILD_ROOT, "usr/bin/kilo-browser")
    if os.path.exists(link_path):
        if os.path.islink(link_path):
            print("\n[检查] /usr/bin/kilo-browser 是合法的软链接，无需修改权限。")
        else:
            print("\n[警告] /usr/bin/kilo-browser 不是软链接！请检查之前的步骤。")

    print("\n=== 权限设置完成！现在可以安全执行 fakeroot dpkg-deb --build 了 ===")

if __name__ == "__main__":
    main()