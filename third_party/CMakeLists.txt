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

include(FetchContent)

set(SPDLOG_USE_STD_FORMAT ON)
set(SPDLOG_SYSTEM_INCLUDES ON)
set(SPDLOG_ENABLE_PCH ON)
FetchContent_Declare(
    spdlog
    URL "${CMAKE_CURRENT_SOURCE_DIR}/spdlog-1.15.2.zip"
)

set(CLI11_PRECOMPILED ON)
FetchContent_Declare(
    cli11
    URL "${CMAKE_CURRENT_SOURCE_DIR}/cli11-2.5.0.zip"
)

FetchContent_Declare(
    glaze
    URL "${CMAKE_CURRENT_SOURCE_DIR}/glaze-5.1.2.zip"
)

FetchContent_MakeAvailable(spdlog cli11 glaze)

target_include_directories(ueberzugpp SYSTEM PRIVATE include/)
target_link_libraries(ueberzugpp PRIVATE spdlog::spdlog CLI11::CLI11 glaze::glaze)

# cmake-format: on
