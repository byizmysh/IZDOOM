# Installing IZDOOM 0.17.1

## Requirements

- A working [IZ2W / IZ2S Platform](https://github.com/byizmysh/IZ2S-Platform) installation and its supported NS2 console setup.
- Linux with Python 3, Clang with RISC-V target support, LLD, llvm-objcopy, glslangValidator and spirv-val. These tools run on the build computer.
- Your own legally obtained **DOOM1.WAD**, the original DOOM episode-one game-data file. This version is tested with DOOM1.WAD, not arbitrary DOOM/DOOM II IWADs.

## What DOOM1.WAD is for

The WAD contains the original game's maps, textures, sprites, palettes and other game resources. IZDOOM's source code is the engine/port; it does not include those resources. The engine source license and the original game-data permissions are separate.

Do not rename a different IWAD to DOOM1.WAD. The supplied file is read during the build and embedded into your local module. During play, the game reads this embedded data from console memory; the build tool does not stream the WAD or video from the PC.

## Build the module

1. Download `IZDOOM-v0.17.1-source.zip` or `IZDOOM-v0.17.1-source.tar.gz` from this release. These are **source archives**, not ready-to-load game modules.
2. Extract the archive and open a terminal in the extracted `IZDOOM` directory.
3. With the required tools installed, build using your own WAD:

```sh
python3 build.py --wad "/path/to/DOOM1.WAD" --output build
```

The output is `build/izdoom.izm`. The build embeds your supplied game data, so keep that output local unless you have separate permission to redistribute the data.

## Add it to IZ2W Panel

1. Put the built module at `<IZ2S Platform folder>/modules/izdoom/izdoom.izm`. Create the `izdoom` folder if it does not exist.
2. Open or refresh IZ2W Panel's module/app library and select **IZDOOM**.
3. Connect your console using the platform's normal instructions and load the module.
4. Open **IZDOOM** from the console launcher.

When replacing an installed copy, unload IZDOOM in the panel before replacing its package. No BlitBun module or audio host service is required. Sound effects and music are not yet implemented. The IZ2W platform still manages loading and the console session; fully PC-disconnected operation has not been verified.
