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

if (USE_BUNDLED_LIBRARIES)
    set(SPDLOG_USE_STD_FORMAT ON)
    set(SPDLOG_SYSTEM_INCLUDES ON)
    set(SPDLOG_ENABLE_PCH ON)
    add_subdirectory(spdlog/)

    block()
    set(CMAKE_CXX_STANDARD 11)
    set(CLI11_PRECOMPILED ON)
    add_subdirectory(CLI11/)
    endblock()

    add_subdirectory(glaze/)
else ()
    find_package(spdlog REQUIRED)
    find_package(CLI11 REQUIRED)
    find_package(glaze REQUIRED)
endif ()

target_include_directories(ueberzugpp SYSTEM PRIVATE include/)
target_link_libraries(ueberzugpp PRIVATE spdlog::spdlog CLI11::CLI11 glaze::glaze)

# cmake-format: on
