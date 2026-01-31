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
    URL "file://./spdlog-v1.16.0.tar.gz"
)

block()
set(CMAKE_CXX_STANDARD 11)
set(CLI11_PRECOMPILED ON)
FetchContent_Declare(
    CLI11
    URL "file://./CLI11-v2.6.1.tar.gz"
)
endblock()

FetchContent_Declare(
    glaze
    URL "file://./glaze-v6.1.0.tar.gz"
)

FetchContent_MakeAvailable(spdlog CLI11 glaze)

target_link_libraries(ueberzugpp PRIVATE spdlog::spdlog CLI11::CLI11 glaze::glaze)

# cmake-format: on

