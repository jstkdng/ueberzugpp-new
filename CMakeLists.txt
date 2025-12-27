# cmake-format: off

# Display images in the terminal
# Copyright (C) 2025  JustKidding
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
set_target_properties(ueberzugpp
PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    CXX_SCAN_FOR_MODULES OFF
    EXPORT_COMPILE_COMMANDS ON
)

find_package(PkgConfig REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(spdlog REQUIRED)

pkg_check_modules(VIPS REQUIRED IMPORTED_TARGET vips)

# reproducible builds
string(TIMESTAMP BUILD_DATE UTC)
configure_file(include/buildconfig.hpp.in buildconfig.hpp @ONLY)
target_include_directories(ueberzugpp SYSTEM PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_compile_options(ueberzugpp PRIVATE "-ffile-prefix-map=${CMAKE_SOURCE_DIR}/=/")
target_compile_options(ueberzugpp PRIVATE $<$<CONFIG:Debug>:-Wall -Wextra -Wpedantic -Werror>)
target_compile_definitions(ueberzugpp PRIVATE $<$<CONFIG:Debug>:DEBUG SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE>)

target_sources(ueberzugpp
PRIVATE
    src/main.cpp
    src/application.cpp
    src/os.cpp
    src/exceptions.cpp
    src/command/layer.cpp
    src/command/dispatch.cpp
PRIVATE
FILE_SET HEADERS
BASE_DIRS include/
FILES
    include/application.hpp
    include/os.hpp
    include/exceptions.hpp
    include/util/ptr.hpp
    include/util/log.hpp
    include/command/layer.hpp
    include/command/dispatch.hpp
)

target_link_libraries(ueberzugpp PRIVATE
    CLI11::CLI11
    spdlog::spdlog
    PkgConfig::VIPS
)

file(CREATE_LINK ueberzugpp "${PROJECT_BINARY_DIR}/ueberzug" SYMBOLIC)

install(TARGETS ueberzugpp RUNTIME)
install(FILES "${PROJECT_BINARY_DIR}/ueberzug" TYPE BIN)

# cmake-format: on
