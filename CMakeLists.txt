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

cmake_minimum_required(VERSION 3.23...3.29 FATAL_ERROR)
cmake_policy(SET CMP0146 OLD)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type.")
set(CMAKE_COLOR_DIAGNOSTICS ON)

project(ueberzugpp LANGUAGES CXX C VERSION 0.0.1)

add_executable(ueberzugpp)
set_target_properties(ueberzugpp PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    CXX_SCAN_FOR_MODULES OFF
    EXPORT_COMPILE_COMMANDS ON
)

option(ENABLE_X11 "Enable X11 support" ON)
option(ENABLE_WAYLAND "Enable wayland support" ON)
option(ENABLE_OPENCV "Enable OpenCV image processing" ON)
option(ENABLE_OPENGL "Enable OpenGL image rendering" OFF)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_definitions(ueberzugpp PRIVATE __cpp_concepts=202002L)
    target_compile_options(ueberzugpp PRIVATE -Wno-builtin-macro-redefined)
endif ()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

if (ENABLE_OPENGL)
    find_package(OpenGL REQUIRED)
    target_link_libraries(ueberzugpp PRIVATE OpenGL::OpenGL OpenGL::EGL)
endif ()

if (ENABLE_X11)
    pkg_check_modules(XCB REQUIRED IMPORTED_TARGET xcb)
    pkg_check_modules(XCBIMAGE REQUIRED IMPORTED_TARGET xcb-image)
    pkg_check_modules(XCBRES REQUIRED IMPORTED_TARGET xcb-res)

    target_link_libraries(ueberzugpp
        PRIVATE
            PkgConfig::XCB
            PkgConfig::XCBIMAGE
            PkgConfig::XCBRES
    )

    target_sources(ueberzugpp
        PRIVATE
            src/xcb/connection.cpp
            src/xcb/window.cpp
            src/context/x11_ctx.cpp

        PRIVATE
        FILE_SET HEADERS
        BASE_DIRS include/
        FILES
            include/xcb.hpp
            include/context/x11_ctx.hpp
    )
endif ()

if (ENABLE_WAYLAND)
    find_package(ECM REQUIRED NO_MODULE)
    list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

    find_package(Wayland REQUIRED COMPONENTS Client)
    find_package(WaylandProtocols REQUIRED)
    find_package(WaylandScanner REQUIRED)

    ecm_add_wayland_client_protocol(ueberzugpp
        PROTOCOL "${WaylandProtocols_DATADIR}/stable/xdg-shell/xdg-shell.xml"
        BASENAME "xdg-shell")

    target_link_libraries(ueberzugpp
        PRIVATE
            Wayland::Client
    )
    target_sources(ueberzugpp
        PRIVATE
            src/context/wayland_ctx.cpp

        PRIVATE
        FILE_SET HEADERS
        BASE_DIRS include/
        FILES
            include/context/wayland_ctx.hpp
    )
endif ()

if(ENABLE_OPENCV)
    find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs videoio)
    target_link_libraries(ueberzugpp PRIVATE ${OpenCV_LIBS})
endif()

target_compile_options(ueberzugpp PRIVATE -Wall -Wextra -Wpedantic -Werror)

# reproducible builds
string(TIMESTAMP BUILD_DATE UTC)
target_compile_options(ueberzugpp PRIVATE "-fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/=")

configure_file(include/buildconfig.hpp.in buildconfig.hpp @ONLY)
target_include_directories(ueberzugpp SYSTEM PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

# definitions
target_compile_definitions(ueberzugpp
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE>
        $<$<CONFIG:Release>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>
)

add_subdirectory(libraries)

target_sources(ueberzugpp
    PRIVATE
        src/main.cpp
        src/os/process.cpp
        src/os/fd.cpp
        src/os/os.cpp
        src/cli/command.cpp
        src/error.cpp
        src/terminal.cpp
        src/application.cpp
        src/config.cpp
        src/logging.cpp
        src/cli/cli.cpp

    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS include/
    FILES
        include/util/passkey.hpp
        include/util/ptr.hpp
        include/util/util.hpp
        include/cli/command.hpp
        include/os/process.hpp
        include/os/fd.hpp
        include/os/os.hpp
        include/config.hpp
        include/error.hpp
        include/logging.hpp
        include/terminal.hpp
        include/application.hpp
        include/cli/cli.hpp
        include/cli/subcommands.hpp
)

target_link_libraries(ueberzugpp
    PRIVATE
        Threads::Threads
        ueberzugpp-libs
)

# cmake-format: on
