## Installation

1. Install and configure [IZ2W / IZ2S Platform](https://github.com/byizmysh/IZ2S-Platform).
2. Download and extract one of the source archives below. Install Python 3, Clang/LLD with RISC-V support, llvm-objcopy, glslangValidator and spirv-val.
3. Supply your own **DOOM1.WAD** and run this from the extracted `IZDOOM` folder:

```sh
python3 build.py --wad "/path/to/DOOM1.WAD" --output build
```

4. Copy `build/izdoom.izm` to `<IZ2S Platform folder>/modules/izdoom/izdoom.izm`.
5. Refresh IZ2W Panel, select and load IZDOOM, then open it in the console launcher.

[Full installation instructions](https://github.com/byizmysh/IZDOOM/blob/main/INSTALL.md) are also attached below as `INSTALL.md`.

**About DOOM1.WAD:** it contains the original game's maps, textures, sprites and other resources. The release includes the port's source code, not those game-data files. The WAD is embedded into your module during the build and read from console memory during play.
