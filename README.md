# autopattern
This is a repository for creating and storing cell-based interactive auto patterns.

# install
##Prerequisites
- Git (2.30+)
- CMake (3.16+)
- C++20 compiler
 - Linux: GCC 11+ or Clang 14+
 - Windows: MSVC 2019+ (Visual Studio or Build Tools)
 - macOS: Xcode 14+ (Apple Clang)
- (Recommended) Ninja build system
- vcpkg (used as the CMake toolchain for SDL2 and bgfx deps)

bgfx sources are expected under external/bgfx.cmake (already committed or added as a submodule).

## Windows
```
git clone https://github.com/eremuhihi/autopattern autopattern
cd autopattern
git submodule update --init --recursive
```

```
cmake -S . -B build -G "Ninja" `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_TOOLCHAIN_FILE=[path of vcpkg.cmake]

cmake --build build -v
```

```
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_TOOLCHAIN_FILE=$toolchain
cmake --build build --config Release
```

## MacOS

# sample
## noise
[noise pattern](docs/assets/videos/noise.gif)
