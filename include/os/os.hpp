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

#include "util/result.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace upp::os
{

constexpr int bufsize = 1024; // 1K at a time
constexpr int waitms = 50;

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

auto close_stderr() -> Result<void>;
auto daemonize() -> Result<void>;
auto getenv(const std::string &var) -> std::optional<std::string>;

auto wait_for_data_on_fd(int filde) -> Result<bool>;
auto wait_for_data_on_stdin() -> Result<bool>;

auto read_data_from_fd(int filde) -> Result<std::string>;
auto read_data_from_stdin() -> Result<std::string>;

auto get_poll_err(int event) -> std::string_view;
auto get_pid_process_name(int pid) -> std::string;
auto get_pid_from_socket(int sockfd) -> Result<int>;

} // namespace upp::os
