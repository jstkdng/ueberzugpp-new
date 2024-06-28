// Display images inside a terminal
// Copyright (C) 2024  JustKidding
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "command.hpp"

#include <iostream>
#include <spdlog/spdlog.h>

auto main() -> int
{
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::trace);

    // CommandManager cmd("bruh");
    // cmd.wait_for_input_on_stdin();
}
