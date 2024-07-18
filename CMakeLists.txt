# cmake-format: off

# Display images inside a terminal
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

cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

project(ueberzugpp-new LANGUAGES CXX VERSION 1.0.0)
add_executable(ueberzugpp)
set_target_properties(ueberzugpp PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    CXX_SCAN_FOR_MODULES OFF
    EXPORT_COMPILE_COMMANDS ON
    COMPILE_WARNING_AS_ERROR ON
)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type.")
option(ENABLE_OPENCV "Enable OpenCV image processing." ON)
option(ENABLE_X11 "Enable X11 canvas." ON)
option(ENABLE_XCB_ERRORS "Enable useful logging of XCB errors." OFF)
option(ENABLE_LIBCXX "Enable linking to libc++" OFF)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_definitions(ueberzugpp PRIVATE __cpp_concepts=202002L)
    target_compile_options(ueberzugpp PRIVATE -Wno-builtin-macro-redefined)
endif ()

if (ENABLE_LIBCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fexperimental-library")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
endif ()

include(FetchContent)
include(GNUInstallDirs)
include(CheckCXXSymbolExists)
include(CheckSourceCompiles)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED COMPONENTS Crypto)
find_package(TBB REQUIRED)

# check if <execution> is available
set(CMAKE_REQUIRED_LIBRARIES TBB::tbb Threads::Threads)
set(CMAKE_REQUIRED_FLAGS -std=c++23)
check_cxx_symbol_exists(std::execution::par_unseq execution HAVE_STD_EXECUTION_H)
if (HAVE_STD_EXECUTION_H)
    target_compile_definitions(ueberzugpp PRIVATE HAVE_STD_EXECUTION_H)
endif ()

check_source_compiles(CXX "
#include <thread>
int main(void) {auto t = std::jthread([] {}); return 0;}
" HAVE_STD_JTHREAD)
if (HAVE_STD_JTHREAD)
    target_compile_definitions(ueberzugpp PRIVATE HAVE_STD_JTHREAD)
endif ()

find_package(spdlog CONFIG QUIET)
if (NOT spdlog_FOUND)
    set(SPDLOG_USE_STD_FORMAT ON)
    FetchContent_Declare(
        spdlog
        URL https://github.com/gabime/spdlog/archive/refs/tags/v1.14.1.tar.gz)
    list(APPEND FC_LIBS spdlog)
endif ()

find_package(nlohmann_json CONFIG QUIET)
if (NOT nlohmann_json_FOUND)
    FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
    list(APPEND FC_LIBS nlohmann_json)
endif ()

if (FC_LIBS)
    FetchContent_MakeAvailable(${FC_LIBS})
endif ()

if (ENABLE_X11)
    target_compile_definitions(ueberzugpp PRIVATE ENABLE_X11)
    pkg_check_modules(XCB REQUIRED IMPORTED_TARGET xcb)
    pkg_check_modules(XCB_IMAGE REQUIRED IMPORTED_TARGET xcb-image)
    pkg_check_modules(XCB_RES REQUIRED IMPORTED_TARGET xcb-res)
    target_sources(ueberzugpp PRIVATE
        include/canvas/x11/x11_canvas.hpp
        include/canvas/x11/x11_window.hpp
        include/canvas/x11/x11_preview.hpp
        src/canvas/x11/x11_canvas.cpp
        src/canvas/x11/x11_window.cpp
        src/canvas/x11/x11_preview.cpp
    )
    target_link_libraries(ueberzugpp PRIVATE PkgConfig::XCB PkgConfig::XCB_IMAGE PkgConfig::XCB_RES)

    #    if(ENABLE_OPENGL)
    #        list(APPEND UEBERZUG_SOURCES "src/canvas/x11/window/x11egl.cpp")
    #    endif()

    if (ENABLE_XCB_ERRORS)
        target_compile_definitions(ueberzugpp PRIVATE ENABLE_XCB_ERRORS)
        pkg_check_modules(XCB_ERRORS REQUIRED IMPORTED_TARGET xcb-errors)
        target_link_libraries(ueberzugpp PRIVATE PkgConfig::XCB_ERRORS)
    endif ()
endif ()

if (ENABLE_OPENCV)
    target_compile_definitions(ueberzugpp PRIVATE ENABLE_OPENCV)
    find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs videoio)
    target_sources(ueberzugpp PRIVATE
        include/image/opencv_image.hpp
        src/image/opencv_image.cpp
    )
    target_link_libraries(ueberzugpp PRIVATE
        opencv_core
        opencv_imgproc
        opencv_imgcodecs
        opencv_videoio
    )
endif ()

target_compile_options(ueberzugpp PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(ueberzugpp PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined>)
target_link_options(ueberzugpp PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined>)
target_compile_definitions(ueberzugpp PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_compile_definitions(ueberzugpp PRIVATE $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE>)
target_compile_definitions(ueberzugpp PRIVATE $<$<CONFIG:Release>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>)

configure_file("include/version.hpp.in" version.hpp)
target_include_directories(ueberzugpp PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}"
)
target_include_directories(ueberzugpp SYSTEM PRIVATE "${CMAKE_SOURCE_DIR}/third_party")

pkg_check_modules(VIPS REQUIRED IMPORTED_TARGET vips-cpp)
pkg_check_modules(SIXEL REQUIRED IMPORTED_TARGET libsixel)
pkg_check_modules(CHAFA REQUIRED IMPORTED_TARGET chafa>=1.6)

if (CMAKE_HOST_SYSTEM_VERSION MATCHES "^.*microsoft.*$")
    target_compile_definitions(ueberzug PRIVATE WSL)
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(ueberzugpp PRIVATE
        src/os/linux/process.cpp
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_sources(ueberzugpp PRIVATE
        src/os/apple/process.cpp
    )
endif ()

target_sources(ueberzugpp PRIVATE
    src/main.cpp

    include/canvas/canvas.hpp
    src/canvas/canvas.cpp

    include/image/image.hpp
    src/image/image.cpp
    include/image/libvips_image.hpp
    src/image/libvips_image.cpp

    include/cli.hpp
    src/cli.cpp

    include/util/str_map.hpp
    include/util/ptr.hpp
    include/util/crypto.hpp
    include/util/util.hpp
    include/util/unix_socket.hpp
    include/util/tmux.hpp
    src/util/tmux.cpp
    src/util/crypto.cpp
    src/util/util.cpp
    src/util/unix/socket_server.cpp
    src/util/unix/socket_client.cpp

    include/os/signal.hpp
    include/os/process.hpp
    include/os/os.hpp
    src/os/os.cpp
    src/os/signal.cpp

    include/application.hpp
    src/application.cpp

    include/command.hpp
    src/command.cpp

    include/terminal.hpp
    include/terminal_info.hpp
    src/terminal.cpp
    src/terminal_info.cpp

    include/config.hpp
    src/config.cpp
)

target_link_libraries(ueberzugpp PRIVATE
    OpenSSL::Crypto
    TBB::tbb
    Threads::Threads
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    PkgConfig::VIPS
    PkgConfig::SIXEL
    PkgConfig::CHAFA
)

file(CREATE_LINK ueberzugpp "${PROJECT_BINARY_DIR}/ueberzug" SYMBOLIC)

# cmake-format: on
