import { execSync } from 'child_process';
import { existsSync } from 'fs';
import { join, dirname, basename } from 'path';
import chalk from 'chalk';

function getParentFolderName(): string {
    const currentDir = process.cwd();
    const parentDir = dirname(currentDir);
    return basename(parentDir);
}

function checkAndCloneBraveCore(): void {
    const parentFolderName = getParentFolderName();
    
    console.log(chalk.blue(`Checking parent folder name: ${parentFolderName}`));
    
    if (parentFolderName.toLowerCase() !== 'brave') {
        console.log(chalk.red(`Error: Parent folder name is "${parentFolderName}", not "brave".`));
        console.log(chalk.yellow('Please ensure the current folder is inside a folder named "brave".'));
        process.exit(1);
    }
    
    console.log(chalk.green('✓ Parent folder name is "brave"'));
    
    // Get current directory and parent directory
    const currentDir = process.cwd();
    const parentDir = dirname(currentDir);
    const braveCoreDir = join(parentDir, 'brave-core');
    
    console.log(chalk.blue(`Current directory: ${currentDir}`));
    console.log(chalk.blue(`Parent directory: ${parentDir}`));
    console.log(chalk.blue(`Brave-core target directory: ${braveCoreDir}`));
    
    // Check if brave-core directory already exists
    if (existsSync(braveCoreDir)) {
        console.log(chalk.yellow(`Directory ${braveCoreDir} already exists.`));
        
        // Check if it's a git repository
        try {
            process.chdir(braveCoreDir);
            execSync('git status', { stdio: 'pipe' });
            console.log(chalk.green('✓ Directory is already a git repository'));
            
            // Fetch tags
            console.log(chalk.blue('Fetching tags...'));
            execSync('git fetch --tags', { stdio: 'inherit' });
            
            // Checkout v1.87.3
            console.log(chalk.blue('Checking out v1.87.3...'));
            try {
                execSync('git checkout -b v1.87.3 v1.87.3', { stdio: 'inherit' });
                console.log(chalk.green('✓ Successfully checked out v1.87.3'));
            } catch (error) {
                console.log(chalk.yellow('Branch v1.87.3 might already exist, trying to switch to it...'));
                execSync('git checkout v1.87.3', { stdio: 'inherit' });
                console.log(chalk.green('✓ Successfully switched to v1.87.3'));
            }
            
        } catch (error) {
            console.log(chalk.red('Error: Directory exists but is not a valid git repository.'));
            console.log(chalk.yellow('Please remove the directory manually and try again.'));
            process.exit(1);
        }
    } else {
        // Clone the repository
        console.log(chalk.blue('Cloning brave/brave-core.git...'));
        try {
            execSync(`git clone git@github.com:brave/brave-core.git "${braveCoreDir}"`, { stdio: 'inherit' });
            console.log(chalk.green('✓ Successfully cloned repository'));
            
            // Change to the cloned directory
            process.chdir(braveCoreDir);
            
            // Fetch tags
            console.log(chalk.blue('Fetching tags...'));
            execSync('git fetch --tags', { stdio: 'inherit' });
            
            // Checkout v1.87.3
            console.log(chalk.blue('Checking out v1.87.3...'));
            execSync('git checkout -b v1.87.3 v1.87.3', { stdio: 'inherit' });
            console.log(chalk.green('✓ Successfully checked out v1.87.3'));
            
        } catch (error) {
            console.log(chalk.red('Error during git operations:'));
            console.error(error);
            process.exit(1);
        }
    }
    
    console.log(chalk.green('\n✅ All operations completed successfully!'));
    console.log(chalk.blue(`Brave-core repository is now at: ${braveCoreDir}`));
    console.log(chalk.blue(`On branch: v1.87.3`));
}

export function braveCommand(): void {
    try {
        checkAndCloneBraveCore();
    } catch (error) {
        console.error(chalk.red('Unexpected error:'), error);
        process.exit(1);
    }
}

if (require.main === module) {
    braveCommand();
}