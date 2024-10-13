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

add_library(ueberzugpp-libs STATIC)

set_target_properties(ueberzugpp-libs PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    CXX_SCAN_FOR_MODULES OFF
)

target_include_directories(ueberzugpp-libs SYSTEM PUBLIC "${CMAKE_SOURCE_DIR}/third_party/")

# spdlog
file(GLOB SPDLOG_SRC spdlog/*.cpp)
target_sources(ueberzugpp-libs PRIVATE ${SPDLOG_SRC})
target_compile_definitions(ueberzugpp-libs PUBLIC SPDLOG_COMPILED_LIB SPDLOG_USE_STD_FORMAT)

# jsoncpp
set(JSONCPP_SRC jsoncpp.cpp)
target_sources(ueberzugpp-libs PRIVATE ${JSONCPP_SRC})

# CLI11
target_sources(ueberzugpp-libs PRIVATE CLI11/Precompile.cpp)
target_compile_definitions(ueberzugpp-libs PUBLIC CLI11_COMPILE)

# cmake-format: on
