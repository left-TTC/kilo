import * as fs from "fs";
import * as path from "path";
import os from "os";
import chalk from "chalk";
import { loadConfig } from "../config/loadConfig";
import { getBrave } from "./getBrave";

/**
 * 确保目录存在
 */
function ensureDir(dir: string) {
    if (!fs.existsSync(dir)) {
        fs.mkdirSync(dir, { recursive: true });
    }
}

/**
 * 拷贝文件（覆盖）
 */
function copyFile(src: string, dest: string) {
    if (!fs.existsSync(src)) {
        throw new Error(`File not found: ${src}`);
    }
    fs.copyFileSync(src, dest);
    console.log(chalk.gray(`  copied: ${path.basename(src)}`));
}

/**
 * Windows 打包逻辑
 */
function packWin(srcRoot: string) {
    const releaseDir = path.resolve(srcRoot, "out", "Release");
    const outputDir = path.resolve(__dirname, "../../../executable/win");

    const mainExe = "kilo.exe";
    const requiredDlls = [
        "chrome_elf.dll",
    ];

    console.log(chalk.green("Packing for Windows"));
    console.log(chalk.gray("Release dir:"), releaseDir);
    console.log(chalk.gray("Output dir:"), outputDir);

    if (!fs.existsSync(releaseDir)) {
        throw new Error(`Release directory not found: ${releaseDir}`);
    }

    ensureDir(outputDir);

    // 拷贝主程序
    copyFile(
        path.join(releaseDir, mainExe),
        path.join(outputDir, mainExe)
    );

    // 拷贝必需 DLL
    for (const dll of requiredDlls) {
        copyFile(
            path.join(releaseDir, dll),
            path.join(outputDir, dll)
        );
    }

    console.log(chalk.green("Windows package complete"));
}

/**
 * 入口函数
 */
export function packExe() {
    const platform = os.platform();
    let config = loadConfig();
    let bravePath = config.brave.path;

    if (!bravePath || bravePath.length === 0) {
        console.log(chalk.blue("No brave workspace, loading...\n"));
        getBrave();
        config = loadConfig();
        bravePath = config.brave.path;
    }

    if (!bravePath) {
        throw new Error("Brave path not configured");
    }

    const srcRoot = path.resolve(bravePath, "..");
    console.log(chalk.green("Loaded brave environment:"), srcRoot);

    switch (platform) {
        case "win32":
            packWin(srcRoot);
            break;
        default:
            throw new Error(`Unsupported platform: ${platform}`);
    }
}

if (require.main === module) {
    packExe();
}
