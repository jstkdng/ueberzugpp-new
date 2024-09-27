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
#include "config/config.hpp"

auto main(int argc, char *argv[]) -> int
{
    Config config;
    auto config_ok = config.read_config_file();
    if (!config_ok) {
        std::cerr << config_ok.error().lmessage() << '\n';
        return 1;
    }

    CLI::App program("Display images in the terminal", "ueberzugpp");

    program.set_version_flag("-V,--version", version_str);
    program.allow_extras(false);
    program.require_subcommand(1);

    auto *query_win_command =
        program.add_subcommand("query_windows", "**UNUSED**, only present for backwards compatibility");
    query_win_command->allow_extras();

    auto *layer_command = program.add_subcommand("layer", "Display images on the terminal");
    layer_command->add_flag("-s,--silent", config.silent, "Print stderr to /dev/null");
    layer_command
        ->add_flag("--use-escape-codes", config.use_escape_codes, "Use escape codes to get terminal capabilities")
        ->default_val(false);
    layer_command->add_option("--pid-file", config.pid_file, "Output file where to write the daemon PID");
    layer_command->add_flag("--no-stdin", config.no_stdin, "Do not listen on stdin for commands")->needs("--pid-file");
    layer_command->add_flag("--no-cache", config.no_cache, "Disable caching of resized images");
    layer_command->add_flag("--no-opencv", config.no_opencv, "Do not use OpenCV, use Libvips instead");
    layer_command->add_option("-o,--output", config.output, "Image output method")
        ->check(CLI::IsMember({"x11", "wayland", "sixel", "kitty", "iterm2", "chafa"}));
    layer_command->add_flag("--origin-center", config.origin_center, "Location of the origin wrt the image");
    layer_command->add_option("-p,--parser", config.parser, "Command parser to use")
        ->check(CLI::IsMember({"json", "bash", "simple"}))
        ->default_val("json");
    layer_command->add_option("-l,--loader", nullptr, "**UNUSED**, only present for backwards compatibility");

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
    cmd_command->allow_extras(false);

    try {
        program.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return program.exit(e);
    }

    Application app;

    auto app_ok = app.init();
    if (!app_ok) {
        std::cerr << app_ok.error().lmessage() << '\n';
        return 1;
    }
    return 0;
}
