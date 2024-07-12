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

#include "cli.hpp"
#include "config.hpp"
#include "os/signal.hpp"

#include <iostream>
#include <span>

auto main(const int argc, char *argv[]) -> int
{
    signal_manager::setup_signals();

    const auto config = Config::instance();
    const auto config_read = config->read_config_file();
    if (!config_read) {
        std::cerr << config_read.error() << '\n';
        return 1;
    }

    cli::Manager manager;
    auto init_ok = manager.initialize(std::span(argv, argc));
    if (!init_ok) {
        return init_ok.error();
    }

    auto cli_handled = manager.handle_layer_subcommand()
                           .or_else([&manager](auto) { return manager.handle_cmd_subcommand(); })
                           .or_else([&manager](auto) { return manager.handle_tmux_subcommand(); });
    if (!cli_handled) {
        const auto &err = cli_handled.error();
        if (!err.empty()) {
            std::cerr << err << '\n';
        }
        return 1;
    }

    return 0;
}
