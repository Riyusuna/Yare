![YareZoEngineGraphic](https://i.imgur.com/y3som7P.png)

# YareZo [![Build status](https://ci.appveyor.com/api/projects/status/ibi6um09v5j03068?svg=true)](https://ci.appveyor.com/project/Resoona/yare)
A Vulkan Rendering Engine

Cross platform support for Linux/Windows (sometimes)

### Dependencies
- Vulkan SDK - https://www.lunarg.com/vulkan-sdk/
- C++17 compiler (MSVC, MinGW-w64, GCC)
- CMake

### External Libraries
See LICENCE
- GLM
- GLFW
- Spdlog
- IMGUI
- TinyObjLoader
- STB (stb_image)

### Build
Windows - build.bat - Requires Ninja, vcvarsall.bat, and CMake in system path.
build.bat release run regen

Linux - buildlinux.sh - Requires g++ compiler, Ninja, and CMake.
./buildlinux.sh release run regen

### Screenshots
![05.23.2020](/Screenshots/08.18.2021-sandbox.jpg "Multiple models rendered with a skybox pipeline")
