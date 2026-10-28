# IZDOOM 0.17.1 — сборка из исходников

Архив **«Исходники»** содержит весь код порта и движка, шейдеры, инструменты сборки, иконку, лицензии и игровые данные первого эпизода. Отдельно скачивать или покупать данные для этой версии не нужно. Они лежат в **game-data/DOOM1.WAD**.

1. Скачайте и распакуйте архив **«Исходники»** из этого релиза.
2. На Linux установите Python 3, Clang с поддержкой RISC-V, LLD, llvm-objcopy, glslangValidator и spirv-val. Эти обычные инструменты сборки в архив не включены.
3. Откройте терминал в распакованной папке `IZDOOM`.
4. Выполните:

```sh
python3 build.py --output build
```

По умолчанию используются уже включённые данные `game-data/DOOM1.WAD`. Явный эквивалент команды:

```sh
python3 build.py --wad game-data/DOOM1.WAD --output build
```

5. Получится **build/izdoom.izm**. Установите его по [инструкции для IZ2S](INSTALL-IZ2S.md).

Для обычной игры сборка из исходников не нужна: скачайте готовый `.izm` из этого же релиза. Релиз содержит первый эпизод DOOM shareware v1.9. Лицензия игровых данных сохранена в `LICENSES/DOOM1-SHAREWARE.txt`; GPL движка не распространяется автоматически на эти данные.

# Build from source — English

The **Исходники / Sources** archive includes all corresponding engine, port, shader and build sources, the icon, license notices and original episode-one data at **game-data/DOOM1.WAD**. No separate game-data download or purchase is required for this release.

1. Download and extract the source archive.
2. Install Python 3, Clang with RISC-V support, LLD, llvm-objcopy, glslangValidator and spirv-val on Linux. The compiler/toolchain itself is not bundled.
3. In the extracted `IZDOOM` directory, run:

```sh
python3 build.py --output build
```

The builder uses the bundled `game-data/DOOM1.WAD`. The output is **build/izdoom.izm**; follow [INSTALL-IZ2S.md](INSTALL-IZ2S.md). The original DOOM shareware v1.9 data retains its separate license in LICENSES/DOOM1-SHAREWARE.txt. No retail DOOM IWAD is included.
