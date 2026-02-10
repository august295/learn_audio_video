# 设置 vcpkg 配置
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    # VS 2022 17.6+ 内置 vcpkg 位置: <VS>\VC\vcpkg
    # VS 内置的 vcpkg 优先级高于环境变量
    # 自定义 vcpkg 位置需提前手动设置 VCPKG_ROOT
    if(NOT DEFINED VCPKG_ROOT)
        if(DEFINED ENV{VCPKG_ROOT})
            set(VCPKG_ROOT "$ENV{VCPKG_ROOT}")
        else()
            set(VCPKG_ROOT "C:/dev/vcpkg")
        endif()
    endif()

    # 设置工具链文件
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(VCPKG_TARGET_TRIPLET "x64-windows-static")
    set(VCPKG_MANIFEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
endif()
message(STATUS "VCPKG_ROOT: ${VCPKG_ROOT}")
message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")
message(STATUS "VCPKG_MANIFEST_DIR: ${VCPKG_MANIFEST_DIR}")

################################################################################
# 3RDPARTY
################################################################################
macro(VCPKG_LOAD_3RDPARTY)
    set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}")
    message(STATUS "Loading 3rd party libraries from vcpkg...")

    # FFMPEG
    find_package(FFMPEG REQUIRED)
    if(FFMPEG_FOUND)
        message(STATUS "FFMPEG includes: ${FFMPEG_INCLUDE_DIRS}")
        message(STATUS "FFMPEG libraries: ${FFMPEG_LIBRARIES}")
    else()
        message(FATAL_ERROR "FFMPEG not found")
    endif()

    # GLFW
    find_package(glfw3 CONFIG REQUIRED)
    if(glfw3_FOUND)
        message(STATUS "GLFW Found")
    else()
        message(FATAL_ERROR "GLFW not found")
    endif()

    # SDL2
    find_package(SDL2 CONFIG REQUIRED)
    if(SDL2_FOUND)
        message(STATUS "SDL2 includes: ${SDL2_INCLUDE_DIRS}")
        message(STATUS "SDL2 libraries: ${SDL2_LIBRARIES}")
    else()
        message(FATAL_ERROR "SDL2 not found")
    endif()
    find_package(SDL2_ttf CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
    find_package(SDL2_mixer CONFIG REQUIRED)

    # OpenGL
    find_package(OpenGL REQUIRED)
    if(OpenGL_FOUND)
        message(STATUS "OpenGL libraries: ${OPENGL_LIBRARIES}")
    else()
        message(FATAL_ERROR "OpenGL not found")
    endif()
endmacro()
