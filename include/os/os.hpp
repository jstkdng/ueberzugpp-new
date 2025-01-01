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

#include <string>
#include <vector>

namespace upp::os
{

struct Process {
    explicit Process(int pid);

    static auto get_tree(int pid) -> std::vector<Process>;
    static auto get_pid_tree(int pid) -> std::vector<int>;

    int pid;
    int ppid;
    int tty_nr;
    int minor_dev;
    std::string pty_path;

    char state;
    int pgrp;
    int session;
};

auto getpid() -> int;
auto strerror() -> std::string;

} // namespace upp::os
