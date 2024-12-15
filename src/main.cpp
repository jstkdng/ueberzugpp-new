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

#include <print>

#include <CLI/Error.hpp>

#include "application.hpp"
#include "cli/cli.hpp"

#include "context/wayland_ctx.hpp"
#include "context/x11_ctx.hpp"

auto main(int argc, char *argv[]) -> int
{
    CliManager cli;
    try {
        cli.app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return cli.app.exit(e);
    }

    WlContext ctx;
    X11Context ctx2;

    Application app(&cli);
    auto run_ok = app.run();
    if (!run_ok) {
        auto msg = run_ok.error().lmessage();
        if (msg.back() != '\n') {
            msg.push_back('\n');
        }
        std::print(stderr, "{}", msg);
        return 1;
    }
    return 0;
}
