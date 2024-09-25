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

#include <algorithm>

#include <fcntl.h>
#include <spdlog/spdlog.h>
#include <sys/stat.h>
#include <unistd.h>

#include "os/os.hpp"
#include "os/process.hpp"
#include "terminal.hpp"

auto Terminal::init() -> Result<void>
{
    return open_first_terminal();
}

auto Terminal::open_first_terminal() -> Result<void>
{
    struct stat stat_info {
    };
    auto tree = Process::get_tree(getpid());
    std::ranges::reverse(tree);
    for (const auto &proc : tree) {
        const auto &path = proc.pty_path;
        if (stat(path.c_str(), &stat_info) == -1) {
            SPDLOG_DEBUG("stat: {}", os::last_err());
            continue;
        }
        if (proc.tty_nr != static_cast<int>(stat_info.st_rdev)) {
            SPDLOG_DEBUG("device {} != {}", proc.tty_nr, stat_info.st_rdev);
            continue;
        }
        pty_fd_ = open(path.c_str(), O_RDONLY | O_NOCTTY);
        if (*pty_fd_ == -1) {
            SPDLOG_DEBUG("open: {}", os::last_err());
            continue;
        }
        pty_pid_ = proc.pid;
        SPDLOG_INFO("using {} with descriptor {}", path, *pty_fd_);
        return {};
    }
    return Err("could not open terminal");
}
