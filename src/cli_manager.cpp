// Display images inside a terminal
// Copyright (C) 2024  JustKidding
//
// This file is part of ueberzugpp.
//
// ueberzugpp is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ueberzugpp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ueberzugpp.  If not, see <https://www.gnu.org/licenses/>.

#include "cli_manager.hpp"
#include "config/build.hpp"

auto CliManager::init(const std::span<char *> args) -> Result<void>
{
    program.set_version_flag("-V,--version", version_str);
    program.allow_extras(false);
    program.require_subcommand(1);

    setup_layer_command();

    auto *query_win_command =
        program.add_subcommand("query_windows", "**UNUSED**, only present for backwards compatibility");
    query_win_command->allow_extras();

    try {
        program.parse(static_cast<int>(args.size()), args.data());
    } catch (const CLI::ParseError &e) {
        return Err(e.what());
    }
    return {};
}

void CliManager::setup_layer_command()
{
}
