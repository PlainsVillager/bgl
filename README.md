# Bgl Minecraft Launcher

## Brief Introduction

A cli Minecraft launcher. Designed very ugly and with a bad code smell.

I'm learning to make projects with cpp. Don't use as your main launcher

## License

Licensed under MIT. Check [LICENSE](./LICENSE)

## Dependencies

[nlohmann-json](https://github.com/nlohmann/json), [cpr](https://github.com/libcpr/cpr), [vog/sha1](https://github.com/vog/sha1)

## Build Prerequisites

- Ninja as CMake *generator*
- CMake as *meta* build system ( cmake is not a build-system :D )
- Vcpkg as package manager
- \[Optional\] Clangd, clang-tidy and clang-format is recommended for enhanced coding experience

## Build Project From Source

1. Run `git clone https://github.com/PlainsVillager/bgl.git`
2. Run `cd bgl`
3. For linux, mingw or msys2: run `cmake --preset default` to configure project. For msvc, run `cmake --preset x64-release-windows` to configure project. Vcpkg packages will be installed automatically
4. For linux, mingw or msys2: run `cmake --build build --config Release`. For msvc, run `cmake --build out/build/x64-release-windows` to build binary executable file

## Additional notice

- For Visual Studio Code user: open `bgl.code-workspace` to import as a workspace

- This project was written on Windows when it starts, but now migrated to Linux now, and the program *should* work both on Windows and Linux

- WARNING: log4j2 fix not included in program
