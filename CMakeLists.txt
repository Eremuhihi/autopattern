cmake_minimum_required(VERSION 3.16)
project(autopattern LANGUAGES C CXX)

# ---- Languages / Standards ----------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ---- SDL2 detection (Config or pkg-config fallback) --------------------------
# Try config packages first (vcpkg, brew, etc.)
find_package(SDL2 QUIET CONFIG)
if(NOT SDL2_FOUND)
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    add_library(SDL2::SDL2 INTERFACE IMPORTED)
    target_include_directories(SDL2::SDL2 INTERFACE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(SDL2::SDL2 INTERFACE ${SDL2_LIBRARIES})
  else()
    message(FATAL_ERROR "SDL2 not found: install SDL2 (vcpkg/brew/apt) or enable pkg-config.")
  endif()
endif()

# On Windows (MSVC/MinGW), SDL2main may be provided by the config package
set(_need_sdl2main OFF)
if(WIN32 AND TARGET SDL2::SDL2main)
  set(_need_sdl2main ON)
endif()

# ---- bgfx (subproject) --------------------------------------------------------
# Assumes external/bgfx.cmake is present in the tree.
add_subdirectory(external/bgfx.cmake)

# ---- Executable ---------------------------------------------------------------
add_executable(cellauto
  src/main.cpp
)

# Link SDL2 and bgfx stack
target_link_libraries(cellauto PRIVATE
  SDL2::SDL2
  bgfx bimg bx
)

if(_need_sdl2main)
  target_link_libraries(cellauto PRIVATE SDL2::SDL2main)
endif()

# Linux-specific system libs
if(UNIX AND NOT APPLE)
  find_package(Threads REQUIRED)
  target_link_libraries(cellauto PRIVATE Threads::Threads dl)
endif()

# ---- Shader build (cross-platform shaderc) -----------------------------------
set(SHADER_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(SHADER_OUT ${CMAKE_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${SHADER_OUT})

# Shaders you actually have in shaders/
set(SHADERS
  vs_fullscreen.sc
  fs_texture.sc
)

# Choose bgfx platform/profile per OS
# platform: linux / windows / osx
# profile : GLSL (150) / HLSL (s_5) / Metal (metal)
set(BGFX_PLATFORM
  $<IF:$<BOOL:${APPLE}>,osx,$<IF:$<BOOL:${WIN32}>,windows,linux>>
)
set(BGFX_PROFILE
  $<IF:$<BOOL:${APPLE}>,metal,$<IF:$<BOOL:${WIN32}>,s_5,150>>
)

# The include path to bgfx shader includes
set(BGFX_SHADER_INCLUDE ${CMAKE_SOURCE_DIR}/external/bgfx.cmake/bgfx/src)

set(SHADER_BINARIES "")
foreach(SH ${SHADERS})
  get_filename_component(SH_NAME ${SH} NAME_WE)

  # Infer shader type from the filename prefix (vs_/fs_/cs_/gs_/ps_)
  if(SH_NAME MATCHES "^vs_")
    set(_stype v)
  elseif(SH_NAME MATCHES "^fs_")
    set(_stype f)
  else()
    message(FATAL_ERROR "Unsupported shader filename prefix in ${SH_NAME}. Use vs_*, fs_* etc.")
  endif()

  add_custom_command(
    OUTPUT ${SHADER_OUT}/${SH_NAME}.bin
    COMMAND $<TARGET_FILE:shaderc>
            -f ${SHADER_DIR}/${SH}
            -o ${SHADER_OUT}/${SH_NAME}.bin
            --type ${_stype}
            --platform ${BGFX_PLATFORM}
            --profile ${BGFX_PROFILE}
            -i ${BGFX_SHADER_INCLUDE}
            --varyingdef ${SHADER_DIR}/varying.def.sc
    DEPENDS ${SHADER_DIR}/${SH} shaderc
    VERBATIM
  )
  list(APPEND SHADER_BINARIES ${SHADER_OUT}/${SH_NAME}.bin)
endforeach()

add_custom_target(shaders ALL DEPENDS ${SHADER_BINARIES})
add_dependencies(cellauto shaders)

# Copy compiled shaders next to the executable
add_custom_command(TARGET cellauto POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:cellauto>/shaders
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_OUT} $<TARGET_FILE_DIR:cellauto>/shaders
)

# ---- macOS runtime path (so bundled dylibs are found) -------------------------
if(APPLE)
  set_target_properties(cellauto PROPERTIES
    BUILD_RPATH "@loader_path"
    INSTALL_RPATH "@loader_path"
  )
endif()

# ---- Install (optional, simple layout) ---------------------------------------
include(GNUInstallDirs)
install(TARGETS cellauto RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${SHADER_OUT}/ DESTINATION ${CMAKE_INSTALL_BINDIR}/shaders)

