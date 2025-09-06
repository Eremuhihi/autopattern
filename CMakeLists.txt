cmake_minimum_required(VERSION 3.16)
project(autopattern LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg toolchain前提

find_package(SDL2 CONFIG REQUIRED)

add_subdirectory(external/bgfx.cmake)

add_executable(cellauto
  src/main.cpp
)

target_link_libraries(cellauto PRIVATE
  SDL2::SDL2 SDL2::SDL2main
  bgfx bimg bx
)

if (UNIX AND NOT APPLE)
  find_package(Threads REQUIRED)
  target_link_libraries(cellauto PRIVATE Threads::Threads dl)
endif()

# shader 自動ビルド
set(SHADER_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(SHADER_OUT ${CMAKE_BINARY_DIR}/shaders)

file(MAKE_DIRECTORY ${SHADER_OUT})

set(SHADERS
  vs_fullscreen.sc
  fs_texture.sc
)

foreach(SH ${SHADERS})
  get_filename_component(SH_NAME ${SH} NAME_WE)
  add_custom_command(
    OUTPUT ${SHADER_OUT}/${SH_NAME}.bin
    COMMAND $<TARGET_FILE:shaderc>
      -f ${SHADER_DIR}/${SH}
      -o ${SHADER_OUT}/${SH_NAME}.bin
      --type ${SH_NAME}
      --platform linux
      --profile 150
      -i ${CMAKE_SOURCE_DIR}/external/bgfx.cmake/bgfx/src
      --varyingdef ${SHADER_DIR}/varying.def.sc
    DEPENDS ${SHADER_DIR}/${SH} shaderc
  )
  list(APPEND SHADER_BINARIES ${SHADER_OUT}/${SH_NAME}.bin)
endforeach()

add_custom_target(shaders ALL DEPENDS ${SHADER_BINARIES})
add_dependencies(cellauto shaders)

# 実行時 shaders ディレクトリにコピー
add_custom_command(TARGET cellauto POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:cellauto>/shaders
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_OUT} $<TARGET_FILE_DIR:cellauto>/shaders
)

