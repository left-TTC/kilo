import fs from "fs";
import path from "path";
import chalk from "chalk";
import readline from "readline";
import { loadConfig } from "../config/loadConfig";

function replaceInFiles(dir: string, oldName: string, newName: string) {
    const entries = fs.readdirSync(dir, { withFileTypes: true });

    for (const entry of entries) {
        const fullPath = path.join(dir, entry.name);

        if (entry.isDirectory()) {
            replaceInFiles(fullPath, oldName, newName); 
        }else if (entry.isFile() && entry.name !== "config.json"){

            let content = fs.readFileSync(fullPath, "utf-8");
            const regex = new RegExp(oldName, "g"); 
            const replaced = content.replace(regex, newName);

            if (replaced !== content) {
                fs.writeFileSync(fullPath, replaced, "utf-8");
                console.log((`Updated file: ${fullPath}`));
            }
        }
    }
}

export async function rebrand() {
    const browser_string = "./resource_browser_string"
    const configPath = path.join(browser_string, "config.json");

    console.log(chalk.blue("read config: ", configPath), "\n")

    if (!fs.existsSync(configPath)) {
        console.error(chalk.red(`config.json not found in ${browser_string}`));
        process.exit(1);
    }

    const config = loadConfig()
    const name = config.project.name

    if(name.length === 0){
        console.log(chalk.red("rebrand your name in ", chalk.bgYellow("//project_build/config.json")))
    }

    const browser_config = JSON.parse(fs.readFileSync(configPath, "utf-8"));
    const oldName = browser_config.currentName;

    if (!oldName) {
        console.error(chalk.red("currentName not defined in config.json"));
        process.exit(1);
    }

    console.log("rename ", chalk.yellow(oldName), " to ", chalk.yellow(name))

    replaceInFiles(browser_string, oldName, name);

    browser_config.currentName = name;
    fs.writeFileSync(configPath, JSON.stringify(browser_config, null, 2), "utf-8");
    console.log()
    console.log(chalk.green(`Updated config.json with currentName = "${name}"`));

    // need update the build file
}

if (require.main === module) {
    rebrand();
}