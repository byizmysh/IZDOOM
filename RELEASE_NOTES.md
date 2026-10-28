## IZ2S build

[**Download IZDOOM-v0.17.1-IZ2S.izm**](https://github.com/byizmysh/IZDOOM/releases/download/v0.17.1/IZDOOM-v0.17.1-IZ2S.izm)

1. Install and configure [IZ2S Platform](https://github.com/byizmysh/IZ2S-Platform).
2. Create `modules/izdoom` inside the platform installation if needed.
3. Copy the downloaded module there as `izdoom.izm`: `<IZ2S folder>/modules/izdoom/izdoom.izm`.
4. Refresh the platform's app library, select IZDOOM and load it onto your connected console.
5. Open IZDOOM in the console launcher.

The ready-to-load module includes the first-episode game data. No compiler or separate data file is required.

[**Installation guide**](https://github.com/byizmysh/IZDOOM/releases/download/v0.17.1/INSTALL-IZ2S.md)

## Source code

[**Download source code (.zip)**](https://github.com/byizmysh/IZDOOM/releases/download/v0.17.1/IZDOOM-v0.17.1-source.zip) · [**Download source code (.tar.gz)**](https://github.com/byizmysh/IZDOOM/releases/download/v0.17.1/IZDOOM-v0.17.1-source.tar.gz)

The archive includes all engine, port, shader and build sources, the icon, licenses and original episode-one data at `game-data/DOOM1.WAD`. No separate game-data download or purchase is required.

To build on Linux, install Python 3, Clang with RISC-V support, LLD, llvm-objcopy, glslangValidator and spirv-val. In the extracted `IZDOOM` directory, run:

```sh
python3 build.py --output build
```

The output is `build/izdoom.izm`. For regular installation, use the prebuilt module above.

[**Build-from-source guide**](https://github.com/byizmysh/IZDOOM/releases/download/v0.17.1/BUILD-FROM-SOURCE.md)
