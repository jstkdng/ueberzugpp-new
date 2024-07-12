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

#include "application.hpp"
#include "config.hpp"
#include "os/signal.hpp"
#include "sub_commands/cmd.hpp"

#include <print>
#include <span>

#include "cli.hpp"

auto main(const int argc, char *argv[]) -> int
{
    signal_manager::setup_signals();

    const auto config = Config::instance();
    const auto config_read = config->read_config_file();
    if (!config_read) {
        std::print(stderr, "{}\n", config_read.error());
        return 1;
    }

    // CLI::App program("Display images in the terminal", "ueberzugpp");
    cli::Manager manager;
    return manager.initialize(std::span(argv, argc));

    // try {
    //     program.parse(argc, argv);
    // } catch (const CLI::ParseError &e) {
    //     return program.exit(e);
    // }

    // if (query_win_command->parsed()) {
    //     return 0;
    // }
    //
    // if (config->print_version) {
    //     std::print("{}\n", Application::get_version());
    //     return 0;
    // }
    //
    // if (!layer_command->parsed() && !tmux_command->parsed() && !cmd_comand->parsed()) {
    //     program.exit(CLI::CallForHelp());
    //     return 1;
    // }
    //
    // if (layer_command->parsed()) {
    //     Application application;
    //     const auto result = application.initialize();
    //     if (!result) {
    //         std::print(stderr, "{}\n", result.error());
    //         return 1;
    //     }
    //     application.run();
    // }
    //
    // if (cmd_comand->parsed()) {
    //     auto send_ok = cmd.send();
    //     if (!send_ok) {
    //         std::print(stderr, "{}\n", send_ok.error());
    //         return 1;
    //     }
    // }
    // return 0;
}
