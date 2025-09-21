cmake_minimum_required(VERSION 3.16)
project(autopattern LANGUAGES C CXX)

# ---- Languages / Standards ----------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ---- SDL2 detection -----------------------------------------------------------
find_package(SDL2 CONFIG QUIET)
if(NOT SDL2_FOUND)
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    add_library(SDL2::SDL2 INTERFACE IMPORTED)
    target_include_directories(SDL2::SDL2 INTERFACE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(SDL2::SDL2 INTERFACE ${SDL2_LIBRARIES})
  else()
    message(FATAL_ERROR "SDL2 not found. Install it with vcpkg/brew/apt etc.")
  endif()
endif()

set(_need_sdl2main OFF)
if(WIN32 AND TARGET SDL2::SDL2main)
  set(_need_sdl2main ON)
endif()

# ---- bgfx subproject ----------------------------------------------------------
add_subdirectory(external/bgfx.cmake)

# ---- Library ------------------------------------------------------------------
add_library(atpt STATIC
  src/panel.cpp
  src/panel_set.cpp
  src/noise.cpp
  src/bad_noise.cpp
)

target_link_libraries(atpt PRIVATE
  SDL2::SDL2
  bgfx bimg bx
)

target_include_directories(atpt PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)

# ---- Executable ---------------------------------------------------------------
add_executable(autopattern
  src/main.cpp
)

target_link_libraries(autopattern PRIVATE
  SDL2::SDL2
  bgfx bimg bx
  atpt
)

target_compile_definitions(autopattern PRIVATE SDL_MAIN_HANDLED)

if(UNIX AND NOT APPLE)
  find_package(Threads REQUIRED)
  target_link_libraries(autopattern PRIVATE Threads::Threads dl)
endif()

# ---- Shader build (cross-platform shaderc) -----------------------------------
set(SHADER_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(SHADER_OUT ${CMAKE_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${SHADER_OUT})

set(SHADERS
  vs_fullscreen.sc
  fs_texture.sc
)

# platform/profile detection (shared for VS/FS)
if(WIN32)
  set(BGFX_PLATFORM windows)
  set(BGFX_PROFILE s_5_0)       # HLSL Shader Model 5.0
elseif(APPLE)
  set(BGFX_PLATFORM osx)
  set(BGFX_PROFILE metal)       # Metal shading language
else()
  set(BGFX_PLATFORM linux)
  set(BGFX_PROFILE 150)         # GLSL 1.50
endif()

set(BGFX_SHADER_INCLUDE ${CMAKE_SOURCE_DIR}/external/bgfx.cmake/bgfx/src)

set(SHADER_BINARIES "")
foreach(SH ${SHADERS})
  get_filename_component(SH_NAME ${SH} NAME_WE)

  if(SH_NAME MATCHES "^vs_")
    set(_stype v)
  elseif(SH_NAME MATCHES "^fs_")
    set(_stype f)
  else()
    message(FATAL_ERROR "Unsupported shader filename prefix in ${SH_NAME}")
  endif()

  add_custom_command(
    OUTPUT ${SHADER_OUT}/${SH_NAME}.bin
    COMMAND "$<TARGET_FILE:shaderc>"
            -f "${SHADER_DIR}/${SH}"
            -o "${SHADER_OUT}/${SH_NAME}.bin"
            --type ${_stype}
            --platform ${BGFX_PLATFORM}
            --profile ${BGFX_PROFILE}
            -i "${BGFX_SHADER_INCLUDE}"
            --varyingdef "${SHADER_DIR}/varying.def.sc"
    DEPENDS ${SHADER_DIR}/${SH} shaderc
    VERBATIM
  )
  list(APPEND SHADER_BINARIES ${SHADER_OUT}/${SH_NAME}.bin)
endforeach()

add_custom_target(shaders ALL DEPENDS ${SHADER_BINARIES})
add_dependencies(autopattern shaders)

add_custom_command(TARGET autopattern POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:autopattern>/shaders
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_OUT} $<TARGET_FILE_DIR:autopattern>/shaders
)

# ---- macOS RPATH --------------------------------------------------------------
if(APPLE)
  set_target_properties(autopattern PROPERTIES
    BUILD_RPATH "@loader_path"
    INSTALL_RPATH "@loader_path"
  )
endif()

# ---- Install -----------------------------------------------------------------
include(GNUInstallDirs)
install(TARGETS autopattern RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${SHADER_OUT}/ DESTINATION ${CMAKE_INSTALL_BINDIR}/shaders)
install(TARGETS atpt ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
