## Bgl Minecraft Launcher

### Brief Introduction

A cli Minecraft launcher. Designed very ugly and with a bad code smell.

I'm learning to make projects with cpp. Don't use as your main launcher

### Build Environment

- Windows 11
- mingw gcc 16.2
- CMake
- Dependencies: [nlohmann-json](https://github.com/nlohmann/json), [cpr](https://github.com/libcpr/cpr), [sha1](https://github.com/vog/sha1)
- \[Optional\] Clangd, clang-tidy and clang-format is recommended for enhanced coding experience

### Build Project

1. Clone this repository and navigate to root directory
2. Run `vcpkg new --application` to deploy vcpkg manifest mode
3. Run `cmake --preset default` to configure project
4. Run `cmake --build build --config Release` to build release binary executable file
