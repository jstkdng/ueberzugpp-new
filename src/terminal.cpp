// Display images inside a terminal
// Copyright (C) 2023  JustKidding
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

#include "terminal.hpp"
#include "os.hpp"
#include "util.hpp"

#include <algorithm>
#include <system_error>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

auto Terminal::initialize() -> std::expected<void, std::string>
{
    logger = spdlog::get("terminal");
    open_first_pty();
    return {};
}

auto Terminal::get_terminal_sizes_ioctl() -> std::expected<void, std::string>
{
    winsize size{};
    const int result = ioctl(pty_fd, TIOCGWINSZ, &size);
    if (result == -1) {
        return os::system_error("could not get ioctl sizes");
    }
    return {};
}

void Terminal::open_first_pty()
{
    struct stat stat_info {
    };
    auto tree = util::get_process_tree(os::get_pid());
    std::ranges::reverse(tree);
    for (const auto &proc : tree) {
        const auto pty_path = proc.pty_path.c_str();
        const int stat_result = stat(pty_path, &stat_info);
        if (stat_result == -1) {
            const auto err = std::error_code(errno, std::generic_category());
            SPDLOG_LOGGER_DEBUG(logger, "stat failed for pty {}: {}", pty_path, err.message());
            continue;
        }
        if (proc.tty_nr != static_cast<int>(stat_info.st_rdev)) {
            SPDLOG_LOGGER_DEBUG(logger, "device number different {} != {}", proc.tty_nr, stat_info.st_rdev);
            continue;
        }
        pty_fd = open(pty_path, O_RDONLY | O_NOCTTY);
        if (pty_fd == -1) {
            const auto err = std::error_code(errno, std::generic_category());
            SPDLOG_LOGGER_DEBUG(logger, "could not open pty {}: {}", pty_path, err.message());
            continue;
        }
        opened_terminal_pid = proc.pid;
        SPDLOG_LOGGER_INFO(logger, "PTY = {}", pty_path);
        return;
    }
    SPDLOG_LOGGER_WARN(logger, "could not open any pty, using stdout as fallback");
    pty_fd = STDOUT_FILENO;
}
