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

#include "os/process.hpp"

#include <format>
#include <fstream>
#include <ios>
#include <limits>

#include <sys/sysmacros.h>

static constexpr auto max_size = std::numeric_limits<std::streamsize>::max();

Process::Process(const int pid)
    : pid(pid)
{
    int ignore{};
    char ignc{};
    const auto stat_file = std::format("/proc/{}/stat", pid);
    std::ifstream ifs(stat_file);
    ifs.ignore(max_size, ')'); // skip pid and executable name

    ifs >> ignc >> ppid >> ignore >> ignore >> tty_nr;
    minor_dev = minor(tty_nr);
    pty_path = std::format("/dev/pts/{}", minor_dev);
}
