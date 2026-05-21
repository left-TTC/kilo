#!/usr/bin/env python3
"""
【正式午夜版】
安排今天深夜 12 点（次日 00:00）自动打开终端 UI 运行任务，并在运行完后彻底自焚清理。
"""

import os
import sys
import plistlib
import subprocess

# --- 硬编码配置 ---
BRAVE_DIR = "/Users/left/Documents/project/kilo/ios/src/brave"
LAUNCH_AGENTS_DIR = os.path.expanduser("~/Library/LaunchAgents")
PLIST_LABEL = "com.kilo.ios.init"
PLIST_PATH = os.path.join(LAUNCH_AGENTS_DIR, f"{PLIST_LABEL}.plist")
RUN_SCRIPT_PATH = os.path.expanduser("~/kilo_init.sh")


def create_midnight_run_script(work_dir):
    """在用户家目录生成实际在终端里跑的 sh 脚本"""
    # 核心修改：业务跑完后，由终端自己去把 launchd 里的定时器注销并删除文件。
    script_content = f"""#!/bin/zsh
echo "=== [SUCCESS] 午夜 12 点已到，正在为您初始化 iOS 编译环境... ==="
export NVM_DIR="$HOME/.nvm"
[ -s "$NVM_DIR/nvm.sh" ] && \\. "$NVM_DIR/nvm.sh"

cd "{work_dir}"
echo "当前工作目录: $(pwd)"

# 1. 执行你的核心下载和构建业务
nvm use 24 && npm run init -- --target_os=ios

echo "\\n=== [SUCCESS] 业务执行完毕，正在清理定时任务残留... ==="
# 2. 功成身退：在这里安全地注销定时器并删除任务文件
launchctl unload "{PLIST_PATH}"
rm -f "{PLIST_PATH}"
rm -f "{RUN_SCRIPT_PATH}"
echo "=== [DONE] 所有临时定时器已清理干净！ ==="
"""
    with open(RUN_SCRIPT_PATH, "w", encoding="utf-8") as f:
        f.write(script_content)
    
    # 赋予执行权限
    os.chmod(RUN_SCRIPT_PATH, 0o755)


def create_midnight_plist():
    # 后台只管一件事情：半夜 12 点一到，用 AppleScript 呼出终端跑脚本。
    # 后面没有任何多余的命令，防止提前自焚。
    applescript_cmd = f'tell application "Terminal" to do script "{RUN_SCRIPT_PATH}"'
    shell_command = f"osascript -e '{applescript_cmd}'\n"

    plist_data = {
        "Label": PLIST_LABEL,
        "ProgramArguments": [
            "/bin/zsh", "-c", shell_command
        ],
        # 核心修改：改为精准日历触发，每天的 00:00（即深夜 12 点）
        "StartCalendarInterval": {
            "Hour": 0,
            "Minute": 0
        },
        "RunAtLoad": False,
    }

    os.makedirs(LAUNCH_AGENTS_DIR, exist_ok=True)
    with open(PLIST_PATH, "wb") as f:
        plistlib.dump(plist_data, f)


def main():
    if not os.path.isdir(BRAVE_DIR):
        print(f"[ERROR] 找不到硬编码的 Brave 目录: {BRAVE_DIR}")
        sys.exit(1)

    print(f"Brave 目标目录: {BRAVE_DIR}")
    print(f"1. 正在生成本地实体业务脚本（含自焚逻辑）: {RUN_SCRIPT_PATH}")
    create_midnight_run_script(BRAVE_DIR)

    print(f"2. 正在配置午夜闹钟，定点在 00:00 AM 触发...")
    # 清理残余
    if os.path.exists(PLIST_PATH):
        subprocess.run(["launchctl", "unload", PLIST_PATH], capture_output=True)

    create_midnight_plist()

    # 投递任务，静候午夜
    subprocess.run(["launchctl", "load", PLIST_PATH], check=True)

    print(f"\n[部署成功] 明灯已挂起！")
    print(f"  任务将在今天深夜 12 点（00:00 AM）自动弹窗执行。")
    print(f"  检查当前排队状态命令: launchctl list | grep kilo")
    print(f"  手动取消命令: launchctl unload {PLIST_PATH}")


if __name__ == "__main__":
    main()