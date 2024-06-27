# Display images inside a terminal
# Copyright (C) 2024  JustKidding
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <https://www.gnu.org/licenses/>.

# // clang-format off
cmake_minimum_required(VERSION 3.22...3.28 FATAL_ERROR)

set(UEBERZUGPP_VERSION 1.0.0)
project(ueberzugpp LANGUAGES CXX C VERSION ${UEBERZUGPP_VERSION})
add_executable(ueberzugpp)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type.")
set_target_properties(ueberzugpp PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    EXPORT_COMPILE_COMMANDS ON
)

include(FetchContent)
include(GNUInstallDirs)
include(CheckCXXSymbolExists)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED COMPONENTS Crypto)
find_package(TBB REQUIRED)

configure_file("include/version.hpp.in" version.hpp)

set(PROJECT_WARNINGS_CXX -Wall -Wextra -Wpedantic -Werror)
target_compile_options(
    ueberzugpp PRIVATE $<$<CONFIG:Debug>:
    $<$<COMPILE_LANGUAGE:CXX>:${PROJECT_WARNINGS_CXX}> >)
target_compile_definitions(ueberzugpp PRIVATE $<$<CONFIG:Debug>: DEBUG >)
target_include_directories(ueberzugpp PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}"
)
target_link_libraries(ueberzugpp PRIVATE
    OpenSSL::Crypto
    TBB::tbb
    Threads::Threads
)
target_sources(ueberzugpp PRIVATE
    include/util/ptr.hpp
    include/crypto.hpp
    src/main.cpp
    src/crypto.cpp
)

file(CREATE_LINK ueberzugpp "${PROJECT_BINARY_DIR}/ueberzug" SYMBOLIC)