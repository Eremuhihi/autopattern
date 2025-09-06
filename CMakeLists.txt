cmake_minimum_required(VERSION 3.24)
project(autopattern LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg は CMake ツールチェーンで指定している前提:
# -D CMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake

# SDL2（vcpkg）
find_package(SDL2 CONFIG REQUIRED)

# bgfx（submodule）
add_subdirectory(external/bgfx.cmake)
# これで targets: bx, bimg, bgfx が使えるようになる

add_executable(cellauto
  src/main.cpp
)

target_link_libraries(cellauto PRIVATE
  SDL2::SDL2 SDL2::SDL2main
  bgfx bimg bx
)

# Linux/WSL の補助リンク（必要環境のみ）
if (UNIX AND NOT APPLE)
  find_package(Threads REQUIRED)
  target_link_libraries(cellauto PRIVATE Threads::Threads dl)
endif()
