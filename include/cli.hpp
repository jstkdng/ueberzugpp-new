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

#pragma once

#include <CLI/CLI.hpp>

#include <string>

namespace upp::subcommands
{

struct layer {
    bool silent = false;
    bool use_escape_codes = false;
    bool no_stdin = false;
    bool no_cache = false;
    bool origin_center = false;

    std::string pid_file;
    std::string parser = "json";
    std::string output;
};

struct cmd {
    std::string socket;
    std::string identifier;
    std::string action;
    std::string file_path;
    std::string scaler = "contain";
    int x;
    int y;
    int width;
    int height;
};

} // namespace upp::subcommands

namespace upp
{

class Cli
{
  public:
    Cli();

    CLI::App program{"Display images in the terminal", "ueberzugpp"};
    CLI::App *layer_command{program.add_subcommand("layer", "Display images on the terminal")};
    CLI::App *cmd_command{program.add_subcommand("cmd", "Send a command to a running ueberzugpp instance")};
    CLI::App *tmux_command{program.add_subcommand("tmux", "Handle tmux hooks. Used internaly")};

    subcommands::layer layer;
    subcommands::cmd cmd;

  private:
    void setup_layer_subcommand();
    void setup_cmd_subcommand();
};

} // namespace upp
