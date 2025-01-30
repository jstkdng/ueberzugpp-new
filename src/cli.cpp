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

#include "cli.hpp"
#include "buildconfig.hpp"

#include <CLI/CLI.hpp>

namespace upp::subcommands
{

auto cmd::get_json_string() const -> std::string
{
    if (action == "exit" || action == "flush") {
        return std::format(
            R"({{"action":"{}"}}
)",
            action);
    }
    if (action == "remove") {
        return std::format(
            R"({{"action":"remove","identifier":"{}"}}
)",
            identifier);
    }
    if (action == "add") {
        return std::format(
            R"({{"action":"add","identifier":"{}","width":{},"height":{},"x":{},"y":{},"path":"{}","scaler":"{}"}}
)",
            identifier, width, height, x, y, file_path, scaler);
    }

    return {};
}

} // namespace upp::subcommands

namespace upp
{

Cli::Cli()
{
    program.set_version_flag("-V,--version", full_version_str);
    program.allow_extras(false);
    program.require_subcommand(1);

    setup_layer_subcommand();
    setup_cmd_subcommand();

    auto *query_win_command =
        program.add_subcommand("query_windows", "**UNUSED**, only present for backwards compatibility");
    query_win_command->allow_extras();
}

void Cli::setup_layer_subcommand()
{
    layer_command->add_flag("-s,--silent", layer.silent, "Print stderr to /dev/null")->default_val(false);
    layer_command
        ->add_flag("--use-escape-codes", layer.use_escape_codes, "Use escape codes to get terminal capabilities")
        ->default_val(false);
    layer_command->add_option("--pid-file", layer.pid_file, "Output file where to write the daemon PID");
    layer_command->add_flag("--no-stdin", layer.no_stdin, "Do not listen on stdin for commands")
        ->default_val(false)
        ->needs("--pid-file");
    layer_command->add_flag("--no-cache", layer.no_cache, "Disable caching of resized images")->default_val(false);
    layer_command->add_option("-o,--output", layer.output, "Image output method")
        ->check(CLI::IsMember({"x11", "wayland", "sixel", "kitty", "iterm2", "chafa"}));
    layer_command->add_flag("--origin-center", layer.origin_center, "Location of the origin wrt the image")
        ->default_val(false);
    layer_command->add_option("-p,--parser", layer.parser, "Command parser to use")
        ->check(CLI::IsMember({"json", "bash", "simple"}))
        ->default_str("json");
    layer_command->add_option("-l,--loader", nullptr, "**UNUSED**, only present for backwards compatibility");
}

void Cli::setup_cmd_subcommand()
{
    cmd_command->add_option("-s,--socket", cmd.socket, "unix socket of running instance")->required();
    cmd_command->add_option("-a,--action", cmd.action, "action to send")->required();
    cmd_command->add_option("-i,--identifier", cmd.identifier, "preview identifier");
    cmd_command->add_option("-f,--file", cmd.file_path, "path of image file");
    cmd_command->add_option("-x,--xpos", cmd.x, "x position of preview");
    cmd_command->add_option("-y,--ypos", cmd.y, "y position of preview");
    cmd_command->add_option("--max-width", cmd.width, "max width of preview");
    cmd_command->add_option("--max-height", cmd.height, "max height of preview");
    cmd_command->add_option("--scaler", cmd.scaler, "scaler to use")->default_str("contain");
}

} // namespace upp
