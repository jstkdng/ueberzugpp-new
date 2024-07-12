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

#include "terminal.hpp"
#include "os/os.hpp"
#include "util/util.hpp"

#include <algorithm>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <spdlog/spdlog.h>

Terminal::~Terminal()
{
    if (pty_fd > 2) {
        close(pty_fd);
    }
}

auto Terminal::initialize() -> std::expected<void, std::string>
{
    term = os::getenv("TERM").value_or("xterm-256color");
    term_program = os::getenv("TERM_PROGRAM").value_or("");
    SPDLOG_INFO("TERM = {}", term);
    if (!term_program.empty()) {
        SPDLOG_INFO("TERM_PROGRAM = {}", term_program);
    }
    open_first_pty();
    return info.initialize(pty_fd);
}

void Terminal::open_first_pty()
{
    struct stat stat_info {
    };
    auto tree = util::get_process_tree(os::get_pid());
    std::ranges::reverse(tree);
    for (const auto &proc : tree) {
        const auto *pty_path = proc.pty_path.c_str();
        const int stat_result = stat(pty_path, &stat_info);
        if (stat_result == -1) {
            [[maybe_unused]] const auto err = std::error_code(errno, std::generic_category());
            SPDLOG_DEBUG("stat failed for pty {}: {}", pty_path, err.message());
            continue;
        }
        if (proc.tty_nr != static_cast<int>(stat_info.st_rdev)) {
            SPDLOG_DEBUG("device number different: {} != {}", proc.tty_nr, stat_info.st_rdev);
            continue;
        }
        pty_fd = open(pty_path, O_RDONLY | O_NOCTTY);
        if (pty_fd == -1) {
            [[maybe_unused]] const auto err = std::error_code(errno, std::generic_category());
            SPDLOG_DEBUG("could not open pty {}: {}", pty_path, err.message());
            continue;
        }
        opened_terminal_pid = proc.pid;
        SPDLOG_INFO("PTY = {}", pty_path);
        return;
    }
    SPDLOG_WARN("could not open any pty, using stdout as fallback");
    pty_fd = STDOUT_FILENO;
}
