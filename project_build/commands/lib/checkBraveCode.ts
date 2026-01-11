
import * as fs from "fs";
import * as path from "path";
import * as readline from "readline";
import { Config } from "../config/Config";


export enum BraveState {
    fine,
    pathError,
    readError,
    packageJsonMissing,
    versionDismatched,
    noChromium,
}

export function checkBrave(
    bravePath: string,
    config: Config,
): BraveState {

    const resolved = path.resolve(bravePath);
    console.log("brave path: ", resolved)

    if (!fs.existsSync(resolved)) return BraveState.pathError

    const pkgPath = path.join(resolved, "package.json");
    if (!fs.existsSync(pkgPath)) {
        return BraveState.packageJsonMissing;
    }

    const pkgRaw = fs.readFileSync(pkgPath, "utf-8");
    const pkg = JSON.parse(pkgRaw);

    if (!pkg.version || typeof pkg.version !== "string") {
        return BraveState.readError;
    }

    if (pkg.version != config.brave.version) {
        return BraveState.versionDismatched;
    }

    const chromiumGn = path.join(
        resolved,
        "..",
        "build",
        "BUILD.gn"
    );

    if (!fs.existsSync(chromiumGn)) {
        return BraveState.noChromium;
    }

    return BraveState.fine;
}   