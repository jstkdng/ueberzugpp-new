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
#include "util/unix_socket.hpp"
#include "version.hpp"

using cli::Manager;

Manager::Manager()
    : program("Display images in the terminal", "ueberzugpp")
{
}

auto Manager::initialize(const std::span<char *> args) -> int
{
    program.set_version_flag("-V,--version", version_str, "Print version information");

    auto *query_win_command =
        program.add_subcommand("query_windows", "**UNUSED**, only present for backwards compatibility.");
    query_win_command->allow_extras();

    auto *tmux_command = program.add_subcommand("tmux", "Handle tmux hooks. Used internaly.");
    tmux_command->allow_extras();

    setup_cmd_subcommand();
    setup_layer_subcommand();

    // program.failure_message(CLI::FailureMessage::help);

    try {
        program.parse(static_cast<int>(args.size()), args.data());
    } catch (const CLI::ParseError &e) {
        return program.exit(e);
    }
    return 0;
}

void Manager::setup_cmd_subcommand()
{
    auto *cmd_comand = program.add_subcommand("cmd", "Send a command to a running ueberzugpp instance.");
    cmd_comand->add_option("-s,--socket", cmd.socket, "UNIX socket of running instance")->required();
    cmd_comand->add_option("-a,--action", cmd.action, "Action to send")
        ->check(CLI::IsMember({"add", "remove", "exit", "flush"}));
    cmd_comand->add_option("-i,--identifier", cmd.id, "Preview identifier");
    cmd_comand->add_option("-f,--file", cmd.file_path, "Path of image file");
    cmd_comand->add_option("-x,--xpos", cmd.x, "X position of preview");
    cmd_comand->add_option("-y,--ypos", cmd.y, "Y position of preview");
    cmd_comand->add_option("--max-width", cmd.max_width, "Max width of preview");
    cmd_comand->add_option("--max-height", cmd.max_height, "Max height of preview");
}

void Manager::setup_layer_subcommand()
{
    auto *layer_command = program.add_subcommand("layer", "Display images on the terminal.");
    layer_command->add_flag("-s,--silent", config->silent, "Print stderr to /dev/null.");
    layer_command
        ->add_flag("--use-escape-codes", config->use_escape_codes, "Use escape codes to get terminal capabilities.")
        ->default_val(false);
    layer_command->add_option("--pid-file", config->pid_file, "Output file where to write the daemon PID.");
    layer_command->add_flag("--no-stdin", config->no_stdin, "Do not listen on stdin for commands.")
        ->needs("--pid-file");
    layer_command->add_flag("--no-cache", config->no_cache, "Disable caching of resized images.");
    layer_command->add_flag("--no-opencv", config->no_opencv, "Do not use OpenCV, use Libvips instead.");
    layer_command->add_option("-o,--output", config->output, "Image output method")
        ->check(CLI::IsMember({"x11", "wayland", "sixel", "kitty", "iterm2", "chafa"}));
    layer_command->add_flag("--origin-center", config->origin_center, "Location of the origin wrt the image");
    layer_command->add_option("-p,--parser", nullptr, "**UNUSED**, only present for backwards compatibility.");
    layer_command->add_option("-l,--loader", nullptr, "**UNUSED**, only present for backwards compatibility.");
}
