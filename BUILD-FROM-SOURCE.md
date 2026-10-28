# IZDOOM 0.17.1 — Build from source

The **Source code** archive includes all corresponding engine, port, shader and build sources, the icon, license notices and original episode-one data at **game-data/DOOM1.WAD**. No separate game-data download or purchase is required for this release.

1. Download and extract the source archive.
2. Install Python 3, Clang with RISC-V support, LLD, llvm-objcopy, glslangValidator and spirv-val on Linux. The compiler/toolchain itself is not bundled.
3. In the extracted `IZDOOM` directory, run:

```sh
python3 build.py --output build
```

The builder uses the bundled `game-data/DOOM1.WAD`. The output is **build/izdoom.izm**; follow [INSTALL-IZ2S.md](INSTALL-IZ2S.md). The original DOOM shareware v1.9 data retains its separate license in LICENSES/DOOM1-SHAREWARE.txt. No retail DOOM IWAD is included.
