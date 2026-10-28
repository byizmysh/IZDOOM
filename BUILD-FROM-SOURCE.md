# IZDOOM 0.17.1 — сборка из исходников

1. Скачайте архив **Исходники** из этого релиза и распакуйте его.
2. На Linux установите Python 3, Clang с поддержкой RISC-V, LLD, llvm-objcopy, glslangValidator и spirv-val.
3. Откройте терминал в распакованной папке `IZDOOM`.
4. Для сборки нужны игровые данные первого эпизода. Они уже включены в готовый `.izm` из этого же релиза. Его можно распаковать как ZIP:

```sh
python3 -m zipfile -e "/path/to/IZDOOM-v0.17.1-IZ2S.izm" package-data
python3 build.py --wad "package-data/game-data/DOOM1.WAD" --output build
```

Либо используйте имеющийся оригинальный DOOM1.WAD:

```sh
python3 build.py --wad "/path/to/DOOM1.WAD" --output build
```

5. Готовый модуль: **build/izdoom.izm**. Установите его по [инструкции для IZ2S](INSTALL-IZ2S.md).

Архив исходников содержит движок, модификации, шейдеры, инструменты сборки, иконку и лицензии. Игровые данные лежат в отдельной готовой сборке. Проверенная версия данных — DOOM shareware v1.9; произвольные DOOM/DOOM II IWAD не поддерживаются этим релизом.

# Build from source — English

1. Download the **Исходники / Sources** archive and extract it.
2. Install Python 3, Clang with RISC-V support, LLD, llvm-objcopy, glslangValidator and spirv-val on Linux.
3. Open a terminal in the extracted `IZDOOM` folder.
4. Extract the original episode-one data from the prebuilt module attached to this release and build:

```sh
python3 -m zipfile -e "/path/to/IZDOOM-v0.17.1-IZ2S.izm" package-data
python3 build.py --wad "package-data/game-data/DOOM1.WAD" --output build
```

Alternatively, pass your own original DOOM1.WAD with `--wad`. The output is **build/izdoom.izm**; follow [INSTALL-IZ2S.md](INSTALL-IZ2S.md).

The source archive excludes the game data. The prebuilt package preserves the original DOOM shareware v1.9 IWAD and its separate license; this is not a GPL grant over the game data. No retail DOOM IWAD is included.
