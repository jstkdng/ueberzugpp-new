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

#pragma once

#include <string>

struct cmd_subcommand {
    std::string id;
    std::string action;
    std::string socket;
    std::string file_path;
    int x = -1;
    int y = -1;
    int max_width = -1;
    int max_height = -1;

    [[nodiscard]] auto to_json() const -> std::string;
};

struct tmux_subcommand {
    std::string hook;
    int pid = -1;

    [[nodiscard]] auto to_json() const -> std::string;
};

struct layer_subcommand {
    bool silent;
    bool use_escape_codes;
    bool no_stdin;
    bool no_opencv;
    bool no_cache;
    bool origin_center;

    std::string pid_file;
    std::string output;
    std::string parser;
};
