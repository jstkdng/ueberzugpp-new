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
cmake_policy(SET CMP0146 OLD)

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

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_definitions(ueberzugpp PRIVATE __cpp_concepts=202002L)
    target_compile_options(ueberzugpp PRIVATE -Wno-builtin-macro-redefined -Wno-unknown-warning-option)
endif ()

# reproducible builds
string(TIMESTAMP BUILD_DATE UTC)
target_compile_options(ueberzugpp PRIVATE "-fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/=")

target_include_directories(ueberzugpp SYSTEM PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_compile_options(ueberzugpp PRIVATE $<$<CONFIG:Debug>:-Wall -Wextra -Wpedantic -Werror>)
target_compile_definitions(ueberzugpp PRIVATE $<$<CONFIG:Debug>:DEBUG>)

option(ENABLE_X11 "Enable X11 support" ON)
option(ENABLE_WAYLAND "Enable wayland support" ON)
option(ENABLE_OPENCV "Enable OpenCV image processing" OFF)
option(ENABLE_OPENGL "Enable OpenGL image rendering" OFF)

add_subdirectory(libraries/)
add_subdirectory(third_party/)
target_compile_definitions(ueberzugpp PRIVATE
    $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE>
    $<$<CONFIG:Release>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>
)

include(GNUInstallDirs)
include(CheckCXXSymbolExists)
include(CheckSourceCompiles)

find_package(PkgConfig REQUIRED)
find_package(TBB REQUIRED)
find_package(Threads REQUIRED)
find_package(ECM REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

# check if <execution> is available
set(CMAKE_REQUIRED_LIBRARIES TBB::tbb Threads::Threads)
set(CMAKE_REQUIRED_FLAGS -std=c++23)
check_cxx_symbol_exists(std::execution::par_unseq execution HAVE_STD_EXECUTION)

# check if <jthread> is available
check_source_compiles(CXX "
#include <thread>
int main(void) {auto t = std::jthread([] {}); return 0;}
" HAVE_STD_JTHREAD)

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

    target_compile_options(ueberzugpp PRIVATE -Wno-subobject-linkage)
    target_link_libraries(ueberzugpp PRIVATE Wayland::Client wayland-protocols)
    target_sources(
        ueberzugpp
        PRIVATE
            src/wayland/canvas.cpp
            src/wayland/socket/socket.cpp
            src/wayland/socket/hyprland.cpp

        PRIVATE
        FILE_SET HEADERS
        BASE_DIRS include/
        FILES
            include/wayland/canvas.hpp
            include/wayland/types.hpp
    )
endif ()

pkg_check_modules(VIPS REQUIRED IMPORTED_TARGET vips-cpp)

configure_file(include/buildconfig.hpp.in buildconfig.hpp @ONLY)

target_sources(
    ueberzugpp
    PRIVATE
        src/main.cpp
        src/application/application.cpp
        src/application/context.cpp
        src/cli.cpp
        src/unix/fd.cpp
        src/unix/socket/client.cpp
        src/os/os.cpp
        src/os/process.cpp
        src/terminal.cpp
        src/command/command.cpp
        src/command/listener.cpp
        src/util/util.cpp
        src/canvas.cpp
        src/image.cpp
        src/image/vips.cpp

    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS include/
    FILES
        include/application/application.hpp
        include/application/context.hpp
        include/cli.hpp
        include/command.hpp
        include/terminal.hpp
        include/util/result.hpp
        include/util/thread.hpp
        include/util/concurrent_deque.hpp
        include/util/ptr.hpp
        include/util/util.hpp
        include/util/str_map.hpp
        include/unix/fd.hpp
        include/unix/socket.hpp
        include/os/os.hpp
        include/base/canvas.hpp
        include/base/image.hpp
        include/image/vips.hpp
)
target_link_libraries(ueberzugpp PRIVATE
    glaze::glaze
    TBB::tbb
    Threads::Threads
    PkgConfig::VIPS
)

file(CREATE_LINK ueberzugpp "${PROJECT_BINARY_DIR}/ueberzug" SYMBOLIC)

# cmake-format: on
