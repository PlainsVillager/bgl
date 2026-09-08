## Bgl Minecraft Launcher

### Brief Introduction

A cli Minecraft launcher. Designed very ugly and with a bad code smell.

I'm learning to make projects with cpp. Don't use as your main launcher

### Build Environment

- Windows 11 Professional x64 
- Mingw-w64 ucrt64 gcc 16.2.0
- CMake 4.4.2
- Visual Studio Code
- Dependencies: [nlohmann-json](https://github.com/nlohmann/json), [cpr](https://github.com/libcpr/cpr), [sha1](https://github.com/vog/sha1)
- \[Optional\] Clangd, clang-tidy and clang-format is recommended for enhanced coding experience

### Notice of using vcpkg as package manager

Please use manifest mode and `x64-mingw-static` triplet(I've configured in CMakePresets.json)