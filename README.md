# Kilo Browser

## Overview
This is a browser based on Brave web browser

## Sync (Clone and initialize the repo)
```
git clone git@github.com:left-TTC/kilo-browser.git path-to-your-project-folder/src/brave/kilo
cd path-to-your-project-folder/src/brave/kilo

npm run brave
cd ..
npm install

# the Chromium source is downloaded, which has a large history (gigabytes of data)
# this might take really long to finish depending on internet speed

npm run init
```

brave-core based android builds should use `npm run init -- --target_os=android --target_arch=arm` (or whichever CPU type you want to build for)
brave-core based iOS builds should use `npm run init -- --target_os=ios`

You can also set the target_os and target_arch for init and build using:

```
npm config set target_os android
npm config set target_arch arm
```

## Build
As you have got the brave code, run
```
npm run applyPatch
```
to apply kilo patch to brave and chromium

Then there will be same as brave, just run 
```
npm run build
```
in brave floder to build kilo and 
```
npm run build Release
```
to build Kilo Release