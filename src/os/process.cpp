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

#include "os/os.hpp"

#include <sys/sysmacros.h>

#include <format>
#include <fstream>
#include <ios>
#include <limits>
#include <ranges>
#include <vector>

namespace upp::os
{

Process::Process(int pid)
{
    constexpr auto max_size = std::numeric_limits<std::streamsize>::max();
    const auto stat_file = std::format("/proc/{}/stat", pid);

    std::ifstream ifs(stat_file);
    ifs.ignore(max_size, ')'); // skip pid and executable name

    ifs >> state >> ppid >> pgrp >> session >> tty_nr;
    minor_dev = minor(tty_nr);
    pty_path = std::format("/dev/pts/{}", minor_dev);
}

auto Process::get_tree(int pid) -> std::vector<Process>
{
    std::vector<Process> result;
    Process runner(pid);
    while (runner.pid > 1) {
        result.emplace_back(runner.pid);
        runner = Process(runner.ppid);
    }
    return result;
}

auto Process::get_pid_tree(int pid) -> std::vector<int>
{
    return get_tree(pid) | std::views::transform(&Process::pid) | std::ranges::to<std::vector>();
}

}; // namespace upp::os