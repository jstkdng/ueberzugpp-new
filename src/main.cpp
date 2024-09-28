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

#include "application.hpp"
#include "config/config.hpp"
#include "cli/cli.hpp"

auto main(int argc, char *argv[]) -> int
{
    Config config;
    auto config_ok = config.read_config_file();
    if (!config_ok) {
        std::cerr << config_ok.error().lmessage() << '\n';
        return 1;
    }

    CliManager cli;
    try {
        cli.app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return cli.app.exit(e);
    }


    /*
    auto *cmd_command = program.add_subcommand("cmd", "Send a command to a running ueberzugpp instance");
    cmd_command->add_option("-s,--socket", config.cmd.socket, "UNIX socket of running instance")->required();
    cmd_command->add_option("-a,--action", config.cmd.action, "Action to send")
        ->check(CLI::IsMember({"add", "remove", "exit", "flush"}));
    cmd_command->add_option("-i,--identifier", config.cmd.id, "Preview identifier");
    cmd_command->add_option("-f,--file", cmd.file_path, "Path of image file");
    cmd_command->add_option("-x,--xpos", cmd.x, "X position of preview");
    cmd_command->add_option("-y,--ypos", cmd.y, "Y position of preview");
    cmd_command->add_option("--max-width", cmd.max_width, "Max width of preview");
    cmd_command->add_option("--max-height", cmd.max_height, "Max height of preview");
    cmd_command->allow_extras(false);*/


    Application app;

    auto app_ok = app.init();
    if (!app_ok) {
        std::cerr << app_ok.error().lmessage() << '\n';
        return 1;
    }
    return 0;
}
