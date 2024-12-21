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

#include "cli/cli.hpp"
#include "config.hpp"
#include "logging.hpp"

#include <CLI/Error.hpp>

#include <print>

/*
#include "application.hpp"

#include "context/wayland_ctx.hpp"
#include "context/x11_ctx.hpp"*/

auto main(int argc, char *argv[]) -> int
{
    ProgramConfig config;
    auto read_config = config.read_config_file();
    if (!read_config) {
        std::print(stderr, "{}\n", read_config.error().lmessage());
        return 1;
    }

    auto log_init = LoggingManager::init(config);
    if (!log_init) {
        std::print(stderr, "{}\n", log_init.error().lmessage());
        return 1;
    }

    CliManager cli;
    try {
        cli.app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return cli.app.exit(e);
    }

    /*
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
    }*/
    return 0;
}
