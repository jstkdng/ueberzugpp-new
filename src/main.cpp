// Display images in the terminal
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

#include "application/application.hpp"
#include "cli.hpp"

#include <CLI/CLI.hpp>

#include <print>

auto main(int argc, char *argv[]) -> int
{
    upp::Cli cli;
    try {
        cli.program.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return cli.program.exit(e);
    }

    upp::Application application(&cli);
    if (auto result = application.run(); !result) {
        auto msg = result.error().message();
        if (msg.back() == '\n') {
            msg.pop_back();
        }
        std::println(stderr, "{}", msg);
        return 1;
    }

    return 0;
}
