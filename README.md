# IZDOOM 0.17.1

<img src="assets/izdoom-icon.png" width="128" alt="IZDOOM — IZMYSH">

**IZDOOM is a homebrew port of DOOM for NS2 (Nintendo Switch 2).**

An unofficial DOOM-derived GPU port for the [IZ2W / IZ2S Platform](https://github.com/byizmysh/IZ2S-Platform), by IZMYSH contributors.

This release restores the preserved silent 0.17 gameplay baseline, including the smoother GPU level transition. It retains the accepted movement fixes, reduces analog movement/turn sensitivity by 20%, removes the central crosshair, and uses the IZDOOM icon with a small IZMYSH credit.

Game logic runs in an RV32 interpreter on the console GPU; shaders render the world, sprites, HUD and transitions. This module has **no sound**, host audio service, BlitBun dependency or PC audio processing. The ordinary IZ2W platform is still required to load and run the module.

## Build

Requirements: Python 3, Clang with RISC-V target and LLD, llvm-objcopy, glslangValidator and spirv-val. No network access is required by the build. Supply your own legally obtained DOOM1.WAD:

```sh
python3 build.py --wad /path/to/DOOM1.WAD --output build
```

The resulting `build/izdoom.izm` can be imported into IZ2W Panel. This source release includes the original pinned PureDOOM header, modified engine, patch, all guest/shader sources, SDK interface headers and build/asset preparation tools.

## Licensing and data

IZDOOM remains a DOOM-derived port, not a clean-room original game. The combined source is distributed under GPL version 2; component notices and additional grants are described in LICENSE-SCOPE.md, NOTICE.md and docs/LICENSING.md. A standalone GitHub repository may be used with the required notices and complete corresponding source; GitHub's Fork button is not a license requirement.

**No IWAD, extracted game art, memory image or playable module containing game data is included in the public source archive.** Local build outputs embed your supplied data; separate redistribution rights are required before publishing those outputs.

This is an unofficial project, not endorsed by id Software.
