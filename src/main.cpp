// Display images inside a terminal
// Copyright (C) 2024  JustKidding
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "application.hpp"
#include "config.hpp"
#include "signal.hpp"

#include <iostream>

#include "CLI11.hpp"

auto main(const int argc, char *argv[]) -> int
{
    signal_manager::setup_signals();

    const auto config = Config::instance();
    const auto config_read = config->read_config_file();
    if (!config_read) {
        std::cerr << config_read.error() << '\n';
        return 1;
    }

    CLI::App program("Display images in the terminal", "ueberzug");
    program.add_flag("-V,--version", config->print_version, "Print version information.");

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

    auto *cmd_comand = program.add_subcommand("cmd", "Send a command to a running ueberzugpp instance.");
    cmd_comand->add_option("-s,--socket", config->cmd_socket, "UNIX socket of running instance");
    cmd_comand->add_option("-i,--identifier", config->cmd_id, "Preview identifier");
    cmd_comand->add_option("-a,--action", config->cmd_action, "Action to send");
    cmd_comand->add_option("-f,--file", config->cmd_file_path, "Path of image file");
    cmd_comand->add_option("-x,--xpos", config->cmd_x, "X position of preview");
    cmd_comand->add_option("-y,--ypos", config->cmd_y, "Y position of preview");
    cmd_comand->add_option("--max-width", config->cmd_max_width, "Max width of preview");
    cmd_comand->add_option("--max-height", config->cmd_max_height, "Max height of preview");

    auto *tmux_command = program.add_subcommand("tmux", "Handle tmux hooks. Used internaly.");
    tmux_command->allow_extras();

    auto *query_win_command =
        program.add_subcommand("query_windows", "**UNUSED**, only present for backwards compatibility.");
    query_win_command->allow_extras();

    try {
        program.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return program.exit(e);
    }

    if (query_win_command->parsed()) {
        return 0;
    }

    if (config->print_version) {
        std::cout << Application::get_version() << '\n';
        return 0;
    }

    if (!layer_command->parsed() && !tmux_command->parsed() && !cmd_comand->parsed()) {
        program.exit(CLI::CallForHelp());
        return 1;
    }

    if (layer_command->parsed()) {
        Application application;
        const auto result = application.initialize();
        if (!result) {
            std::cerr << result.error() << '\n';
            return 1;
        }
        Application::run();
    }

    return 0;
}
