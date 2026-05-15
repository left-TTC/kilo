# Package

```
kilo-build/
├── DEBIAN/                              # Debian 打包元数据和控制脚本
│   ├── control                          # 核心：包名、版本、依赖列表、描述
│   ├── postinst                         # 安装后脚本（设置沙箱权限、注册默认浏览器）
│   ├── postrm                           # 卸载后脚本（清理软链接、清理系统缓存）
│   ├── preinst                          # 安装前脚本（可选，通常用于清理旧版本残留）
│   └── prerm                            # 卸载前脚本（可选，通常用于停止正在运行的进程）
│
├── opt/                                 
│   └── kilo-browser/                    # 浏览器的核心运行目录（绝对主力）
│       ├── kilo                         # 主可执行二进制文件 (ELF)
│       ├── kilo-browser-wrapper         # 启动包装脚本 (设置环境变量、禁用崩溃弹窗等)
│       ├── chrome_sandbox               # SUID 沙箱二进制文件 (非常关键，权限必须是 4755)
│       ├── chrome_crashpad_handler      # 崩溃报告处理程序
│       ├── icudtl.dat                   # 国际化 (ICU) 数据文件
│       ├── v8_context_snapshot.bin      # V8 JavaScript 引擎启动快照 (加速启动)
│       ├── snapshot_blob.bin            # V8 引擎备用快照
│       ├── resources.pak                # 核心资源文件 (HTML, JS, CSS 内部 UI)
│       ├── chrome_100_percent.pak       # 100% 缩放比例的 UI 资源
│       ├── chrome_200_percent.pak       # 200% 缩放比例的 UI 资源 (用于高分屏)
│       ├── product_logo_*.png           # 供内部页面或安装程序使用的各尺寸 Logo
│       ├── locales/                     # 多语言包目录
│       │   ├── en-US.pak
│       │   ├── zh-CN.pak
│       │   └── ...
│       ├── swiftshader/                 # 软件渲染回退方案 (无 GPU 时使用)
│       │   ├── libGLESv2.so
│       │   └── libEGL.so
│       └── crashpad_handler             # 某些版本中崩溃处理程序的别名
│
└── usr/                                 # 系统级集成目录
    ├── bin/
    │   └── kilo-browser                 # 指向 /opt/your-browser/your-browser-wrapper 的软链接
    │
    └── share/
        ├── applications/
        │   └── kilo-browser.desktop     # XDG 桌面快捷方式文件
        │
        └── icons/
            └── hicolor/                 # 遵循 XDG 规范的系统级图标存放处
                ├── 16x16/apps/kilo_browser.png
                ├── 32x32/apps/kilo_browser.png
                ├── 48x48/apps/kilo_browser.png
                ├── 64x64/apps/kilo_browser.png
                ├── 128x128/apps/kilo_browser.png
                └── 256x256/apps/kilo_browser.png
```