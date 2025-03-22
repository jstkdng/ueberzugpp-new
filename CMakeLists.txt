# cmake-format: off

# Display images in the terminal
# Copyright (C) 2024  JustKidding
#
# This file is part of ueberzugpp.
#
# ueberzugpp is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ueberzugpp is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with ueberzugpp.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.23...3.29 FATAL_ERROR)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type.")
set(CMAKE_COLOR_DIAGNOSTICS ON)

project(ueberzugpp LANGUAGES CXX C VERSION 0.0.1)

add_executable(ueberzugpp)
set_target_properties(
    ueberzugpp
    PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        CXX_SCAN_FOR_MODULES OFF
        EXPORT_COMPILE_COMMANDS ON
)

option(ENABLE_X11 "Enable X11 support" ON)
option(ENABLE_WAYLAND "Enable wayland support" ON)
option(ENABLE_LIBVIPS "Enable image loading through libvips" ON)
option(ENABLE_OPENCL "Enable OpenCL image processing" OFF)
option(ENABLE_DEBUG_LOGGING "Enable debug logging on non debug builds" OFF)
option(USE_BUNDLED_LIBRARIES "Use bundled libraries" ON)
option(USE_LIBCXX "Link against libc++" OFF)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_definitions(ueberzugpp PRIVATE __cpp_concepts=202002L)
    target_compile_options(ueberzugpp PRIVATE -Wno-builtin-macro-redefined)
endif ()

if (USE_LIBCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fexperimental-library")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
endif ()

add_subdirectory(third_party/ EXCLUDE_FROM_ALL)

include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)
find_package(TBB REQUIRED)
find_package(OpenSSL REQUIRED COMPONENTS Crypto)
find_package(Threads REQUIRED)

if (ENABLE_X11 OR ENABLE_WAYLAND)
    find_package(ECM REQUIRED NO_MODULE)
    list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
endif ()

if (ENABLE_LIBVIPS)
    pkg_check_modules(VIPS REQUIRED IMPORTED_TARGET vips)
    target_link_libraries(ueberzugpp PRIVATE PkgConfig::VIPS)

    target_sources(
        ueberzugpp
        PRIVATE
            src/image/vips.cpp

        PRIVATE
        FILE_SET HEADERS
        BASE_DIRS include/
        FILES
            include/image/vips.hpp
    )
endif ()

if (ENABLE_X11)
    pkg_check_modules(XCBERRORS REQUIRED IMPORTED_TARGET xcb-errors)
    find_package(XCB REQUIRED COMPONENTS XCB SHM IMAGE RES)
    target_link_libraries(ueberzugpp PRIVATE XCB::XCB XCB::SHM XCB::IMAGE XCB::RES PkgConfig::XCBERRORS)

    target_sources(
        ueberzugpp
        PRIVATE
            src/x11/canvas.cpp
            src/x11/context.cpp
            src/x11/window.cpp
            src/x11/types/window.cpp

        PRIVATE
        FILE_SET HEADERS
        BASE_DIRS include/
        FILES
            include/x11/canvas.hpp
            include/x11/context.hpp
            include/x11/types.hpp
            include/x11/window.hpp
    )
endif ()

if (ENABLE_WAYLAND)
    add_library(wayland-protocols STATIC)

    find_package(Wayland REQUIRED COMPONENTS Client)
    find_package(WaylandProtocols REQUIRED)
    find_package(WaylandScanner REQUIRED)

    ecm_add_wayland_client_protocol(wayland-protocols
        PROTOCOL "${WaylandProtocols_DATADIR}/stable/xdg-shell/xdg-shell.xml"
        BASENAME "xdg-shell")

    target_link_libraries(ueberzugpp PRIVATE Wayland::Client wayland-protocols)
    target_sources(
        ueberzugpp
        PRIVATE
            src/wayland/canvas.cpp
            src/wayland/window.cpp
            src/wayland/shm.cpp
            src/wayland/socket/socket.cpp
            src/wayland/socket/hyprland.cpp

        PRIVATE
        FILE_SET HEADERS
        BASE_DIRS include/
        FILES
            include/wayland/canvas.hpp
            include/wayland/types.hpp
            include/wayland/window.hpp
            include/wayland/shm.hpp
            include/wayland/socket/socket.hpp
            include/wayland/socket/hyprland.hpp
            include/wayland/socket/dummy.hpp
    )
endif ()

if (ENABLE_OPENCL)
    find_package(OpenCL REQUIRED)
    find_package(OpenCLHeaders REQUIRED)
    find_package(OpenCLHeadersCpp REQUIRED)
    target_link_libraries(ueberzugpp PRIVATE OpenCL::OpenCL OpenCL::Headers OpenCL::HeadersCpp)
endif ()

target_compile_options(ueberzugpp PRIVATE $<$<CONFIG:Debug>:-Wall -Wextra -Wpedantic -Werror>)
target_compile_definitions(ueberzugpp PRIVATE SPDLOG_NO_SOURCE_LOC)
target_compile_definitions(ueberzugpp PRIVATE $<$<CONFIG:Debug>:DEBUG SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE>)
if (ENABLE_DEBUG_LOGGING)
    target_compile_definitions(ueberzugpp PRIVATE $<$<NOT:$<CONFIG:Debug>>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE>)
else ()
    target_compile_definitions(ueberzugpp PRIVATE $<$<NOT:$<CONFIG:Debug>>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>)
endif ()

# reproducible builds
string(TIMESTAMP BUILD_DATE UTC)
configure_file(include/buildconfig.hpp.in buildconfig.hpp @ONLY)
target_include_directories(ueberzugpp SYSTEM PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_sources(
    ueberzugpp
    PRIVATE
        src/main.cpp
        src/application/application.cpp
        src/application/context.cpp
        src/cli.cpp
        src/unix/fd.cpp
        src/unix/socket/client.cpp
        src/unix/socket/server.cpp
        src/os/os.cpp
        src/os/process.cpp
        src/terminal.cpp
        src/command/command.cpp
        src/command/listener.cpp
        src/util/util.cpp
        src/util/crypto.cpp
        src/canvas.cpp
        src/image/scalers.cpp

    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS include/
    FILES
        include/application/application.hpp
        include/application/context.hpp
        include/cli.hpp
        include/log.hpp
        include/terminal.hpp
        include/command/command.hpp
        include/command/listener.hpp
        include/image/scalers.hpp
        include/util/result.hpp
        include/util/concurrent_deque.hpp
        include/util/ptr.hpp
        include/util/util.hpp
        include/util/str_map.hpp
        include/util/crypto.hpp
        include/unix/fd.hpp
        include/unix/socket.hpp
        include/os/os.hpp
        include/base/canvas.hpp
)
target_link_libraries(ueberzugpp PRIVATE
    TBB::tbb
    Threads::Threads
    OpenSSL::Crypto
)

file(CREATE_LINK ueberzugpp "${PROJECT_BINARY_DIR}/ueberzug" SYMBOLIC)

install(TARGETS ueberzugpp RUNTIME)
install(FILES "${PROJECT_BINARY_DIR}/ueberzug" TYPE BIN)
# cmake-format: on
