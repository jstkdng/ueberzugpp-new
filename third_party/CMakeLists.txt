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

set(SPDLOG_USE_STD_FORMAT ON)
set(SPDLOG_SYSTEM_INCLUDES ON)
set(SPDLOG_ENABLE_PCH ON)
add_subdirectory(spdlog/)

set(CLI11_PRECOMPILED ON)
add_subdirectory(CLI11-2.4.2/)

target_link_libraries(ueberzugpp PRIVATE spdlog::spdlog CLI11::CLI11)
target_include_directories(ueberzugpp SYSTEM PRIVATE include/)

# cmake-format: on
