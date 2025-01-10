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
#include "terminal.hpp"
#include "util/result.hpp"

#include <fcntl.h>
#include <sys/stat.h>

#include <spdlog/spdlog.h>

#include <algorithm>

namespace upp::terminal
{

auto Context::open_first_pty() -> Result<void>
{
    struct stat stat_info {
    };
    auto tree = os::Process::get_tree(os::getpid());
    std::ranges::reverse(tree);

    for (const auto &proc : tree) {
        const auto &path = proc.pty_path;
        if (stat(path.c_str(), &stat_info) == -1) {
            SPDLOG_DEBUG("stat {}: {}", path, os::strerror());
            continue;
        }
        if (proc.tty_nr != static_cast<int>(stat_info.st_rdev)) {
            SPDLOG_DEBUG("device {} != {}", proc.tty_nr, stat_info.st_rdev);
            continue;
        }
        pty_fd = open(path.c_str(), O_RDONLY | O_NOCTTY);
        if (!pty_fd) {
            SPDLOG_DEBUG("open: {}", os::strerror());
            continue;
        }
        pid = proc.pid;
        SPDLOG_INFO("PTY={}", path);
        return {};
    }
    return Err("could not open terminal");
}

} // namespace upp::terminal
