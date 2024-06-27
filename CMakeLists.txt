# cmake-format: off

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

cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

project(ueberzugpp-new LANGUAGES CXX VERSION 1.0.0)
add_executable(ueberzugpp)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type.")
set_target_properties(ueberzugpp PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    EXPORT_COMPILE_COMMANDS ON
    COMPILE_WARNING_AS_ERROR ON
)

option(ENABLE_OPENCV "Enable OpenCV image processing." ON)

include(FetchContent)
include(GNUInstallDirs)
include(CheckCXXSymbolExists)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED COMPONENTS Crypto)
find_package(TBB REQUIRED)

# check if <execution> is available
set(CMAKE_REQUIRED_LIBRARIES TBB::tbb)
check_cxx_symbol_exists(std::execution::par_unseq execution
    HAVE_STD_EXECUTION_H)
if (HAVE_STD_EXECUTION_H)
    target_compile_definitions(ueberzugpp PRIVATE HAVE_STD_EXECUTION_H)
endif ()

set(SPDLOG_USE_STD_FORMAT ON)
find_package(spdlog CONFIG QUIET)
if (NOT spdlog_FOUND)
    FetchContent_Declare(
        spdlog
        URL https://github.com/gabime/spdlog/archive/refs/tags/v1.14.1.tar.gz)
    list(APPEND FC_LIBS spdlog)
endif ()

if (FC_LIBS)
    FetchContent_MakeAvailable(${FC_LIBS})
endif ()

if (ENABLE_OPENCV)
    target_compile_definitions(ueberzugpp PRIVATE ENABLE_OPENCV)
    find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs videoio)
    target_link_libraries(ueberzugpp PRIVATE
        opencv_core
        opencv_imgproc
        opencv_imgcodecs
        opencv_videoio
    )
endif ()

target_compile_options(ueberzugpp PRIVATE -Wall -Wextra -Wpedantic)
target_compile_definitions(ueberzugpp PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_compile_definitions(ueberzugpp PRIVATE $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE>)
target_compile_definitions(ueberzugpp PRIVATE $<$<CONFIG:Release>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>)

target_include_directories(ueberzugpp PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}"
)
target_link_libraries(ueberzugpp PRIVATE
    OpenSSL::Crypto
    TBB::tbb
    Threads::Threads
    spdlog::spdlog
)
target_sources(ueberzugpp PRIVATE
    src/main.cpp
    include/util/ptr.hpp

    include/crypto.hpp
    src/crypto.cpp

    include/application.hpp
    src/application.cpp

    include/os.hpp
    src/os.cpp

    include/command.hpp
    src/command.cpp

    include/unix_socket.hpp
    src/unix_socket/server.cpp
    src/unix_socket/client.cpp
    src/unix_socket/util.cpp
)

configure_file("include/version.hpp.in" version.hpp)
file(CREATE_LINK ueberzugpp "${PROJECT_BINARY_DIR}/ueberzug" SYMBOLIC)

# cmake-format: on
