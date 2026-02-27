import { execSync } from 'child_process';
import { existsSync } from 'fs';
import { join, dirname } from 'path';
import chalk from 'chalk';
import { loadConfig } from '../config/loadConfig';

// 检查是否已经打过补丁
function isPatchApplied(): boolean {
    try {
        const config = loadConfig();
        // 这里可以添加更复杂的检查逻辑
        // 例如检查某个标志文件或配置项
        console.log(chalk.blue(`Brave path in config: ${config.brave.path}`));
        return config.brave.path.length > 0;
    } catch (error) {
        return false;
    }
}

// 执行npm run getbrave
function runGetBrave(): void {
    console.log(chalk.blue('Running npm run getbrave...'));
    try {
        execSync('npm run getbrave', { stdio: 'inherit' });
        console.log(chalk.green('✓ npm run getbrave completed successfully'));
    } catch (error) {
        console.log(chalk.red('Error running npm run getbrave:'), error);
        process.exit(1);
    }
}

// 执行npm run applyPatch
function runApplyPatch(): void {
    console.log(chalk.blue('Running npm run applyPatch...'));
    try {
        execSync('npm run applyPatch', { stdio: 'inherit' });
        console.log(chalk.green('✓ npm run applyPatch completed successfully'));
    } catch (error) {
        console.log(chalk.red('Error running npm run applyPatch:'), error);
        process.exit(1);
    }
}

// 执行npm run build with suffix
function runBuildWithSuffix(suffix: string): void {
    console.log(chalk.blue(`Running npm run build with suffix: ${suffix}`));
    
    // 获取当前目录
    const currentDir = process.cwd();
    const parentDir = dirname(currentDir);
    
    console.log(chalk.blue(`Current directory: ${currentDir}`));
    console.log(chalk.blue(`Parent directory: ${parentDir}`));
    
    // 切换到上级目录
    process.chdir(parentDir);
    console.log(chalk.blue(`Changed to directory: ${process.cwd()}`));
    
    try {
        // 执行npm run build加上后缀
        const command = `npm run build ${suffix}`;
        console.log(chalk.blue(`Executing: ${command}`));
        execSync(command, { stdio: 'inherit' });
        console.log(chalk.green(`✓ npm run build ${suffix} completed successfully`));
    } catch (error) {
        console.log(chalk.red(`Error running npm run build ${suffix}:`), error);
        process.exit(1);
    } finally {
        // 切换回原始目录
        process.chdir(currentDir);
    }
}

// 主函数
export async function buildWithBrave() {
    const args = process.argv.slice(2);
    
    // 获取后缀参数
    const suffix = args.length > 0 ? args[0] : '';
    
    console.log(chalk.blue('Starting build with brave support...'));
    console.log(chalk.blue(`Build suffix: ${suffix || '(none)'}`));
    
    // 检查是否已经打过补丁
    if (!isPatchApplied()) {
        console.log(chalk.yellow('Patch not applied or brave path not configured.'));
        console.log(chalk.yellow('Running setup steps...'));
        
        // 执行npm run getbrave
        runGetBrave();
        
        // 执行npm run applyPatch
        runApplyPatch();
    } else {
        console.log(chalk.green('✓ Patch already applied, skipping setup steps'));
    }
    
    // 执行npm run build with suffix
    runBuildWithSuffix(suffix);
    
    console.log(chalk.green('\n✅ Build with brave support completed successfully!'));
}

if (require.main === module) {
    buildWithBrave();
}