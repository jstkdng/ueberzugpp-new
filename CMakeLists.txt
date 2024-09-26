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

cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type.")
project(ueberzugpp LANGUAGES CXX VERSION 0.0.1)

add_executable(ueberzugpp)
set_target_properties(ueberzugpp PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    CXX_SCAN_FOR_MODULES OFF
    EXPORT_COMPILE_COMMANDS ON
)

option(ENABLE_X11 "Enable X11 support" ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

if (ENABLE_X11)
    target_compile_definitions(ueberzugpp PRIVATE ENABLE_X11)
    pkg_check_modules(XCB REQUIRED IMPORTED_TARGET xcb)
    #pkg_check_modules(XCBIMAGE REQUIRED IMPORTED_TARGET xcb-image)
    #pkg_check_modules(XCBRES REQUIRED IMPORTED_TARGET xcb-res)

    target_link_libraries(ueberzugpp
        PRIVATE
            PkgConfig::XCB
    )

    target_sources(ueberzugpp
        PRIVATE
            src/xcb/xcb_connection.cpp
            src/xcb/xcb_window.cpp

        PRIVATE
        FILE_SET HEADERS
        BASE_DIRS include/
        FILES
            include/xcb.hpp
            include/xcb_fwd.hpp
    )
endif()

target_compile_options(ueberzugpp PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_include_directories(ueberzugpp SYSTEM PRIVATE third_party/)

configure_file(include/config.hpp.in config.hpp @ONLY)
target_include_directories(ueberzugpp SYSTEM PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

# spdlog
file(GLOB SPDLOG_SRC src/spdlog/*.cpp)
target_sources(ueberzugpp PRIVATE ${SPDLOG_SRC})
target_compile_definitions(ueberzugpp PRIVATE SPDLOG_COMPILED_LIB SPDLOG_USE_STD_FORMAT)

# definitions
target_compile_definitions(ueberzugpp
    PRIVATE
        JSON_DIAGNOSTICS=1
        $<$<CONFIG:Debug>:DEBUG SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE>
        $<$<CONFIG:Release>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>
)

target_sources(ueberzugpp
    PRIVATE
        src/main.cpp
        src/os/process.cpp
        src/os/fd.cpp
        src/os/os.cpp
        src/error.cpp
        src/terminal.cpp
        src/application.cpp

    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS include/
    FILES
        include/util/passkey.hpp
        include/os/process.hpp
        include/os/fd.hpp
        include/os/os.hpp
        include/error.hpp
        include/terminal.hpp
        include/application.hpp
)

target_link_libraries(ueberzugpp
    PRIVATE
        Threads::Threads
)

# cmake-format: on
