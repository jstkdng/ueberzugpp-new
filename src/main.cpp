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

#include <iostream>

#include <CLI11.hpp>

#include "application.hpp"
#include "config/build.hpp"

auto main(int argc, char *argv[]) -> int
{
    CLI::App program("Display images in the terminal", "ueberzugpp");
    //auto *layer_command = program.add_subcommand("layer", "Display images on the terminal");

    program.set_version_flag("-V,--version", version_str);
    program.allow_extras(false);
    program.require_subcommand(1);

    auto *query_win_command =
        program.add_subcommand("query_windows", "**UNUSED**, only present for backwards compatibility");
    query_win_command->allow_extras();

    try {
        program.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return program.exit(e);
    }

    Application app;

    auto res = app.init();
    if (!res) {
        std::cerr << res.error().lmessage() << '\n';
        return 1;
    }
    return 0;
}
