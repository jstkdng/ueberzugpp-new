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
    target_compile_options(ueberzugpp PRIVATE -Wno-builtin-macro-redefined)
endif ()

# reproducible builds
string(TIMESTAMP BUILD_DATE UTC)
target_compile_options(ueberzugpp PRIVATE "-fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/=")

configure_file(include/buildconfig.hpp.in buildconfig.hpp @ONLY)
target_include_directories(ueberzugpp SYSTEM PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_compile_options(ueberzugpp PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_compile_definitions(ueberzugpp PRIVATE $<$<CONFIG:Debug>:DEBUG>)

option(ENABLE_X11 "Enable X11 support" ON)
option(ENABLE_WAYLAND "Enable wayland support" ON)
option(ENABLE_OPENCV "Enable OpenCV image processing" ON)
option(ENABLE_OPENGL "Enable OpenGL image rendering" OFF)

add_subdirectory(third_party/)
target_compile_definitions(ueberzugpp PRIVATE
    $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE>
    $<$<CONFIG:Release>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>
)

find_package(ECM REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

if (ENABLE_X11)
    find_package(XCB REQUIRED COMPONENTS XCB SHM IMAGE RES)
    target_link_libraries(ueberzugpp PRIVATE XCB::XCB XCB::SHM XCB::IMAGE XCB::RES)
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
endif ()

target_sources(
    ueberzugpp
    PRIVATE
        src/main.cpp

    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS include/
    FILES
        include/result.hpp
)
target_link_libraries(ueberzugpp PRIVATE CLI11::CLI11 spdlog::spdlog nlohmann_json::nlohmann_json)

# cmake-format: on
