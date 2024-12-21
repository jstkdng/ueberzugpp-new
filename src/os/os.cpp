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

#include "os/os.hpp"
#include "error.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <format>
#include <fstream>
#include <optional>
#include <string>
#include <system_error>

namespace os
{

auto strerror() -> std::string
{
    const std::error_code code(errno, std::generic_category());
    return code.message();
}

auto getpid() -> int
{
    return ::getpid();
}

auto getenv(const std::string &var) -> std::optional<std::string>
{
    const char *env_p = std::getenv(var.c_str()); // NOLINT
    if (env_p == nullptr) {
        return {};
    }
    return env_p;
}

auto get_pid_process_name(int pid) -> std::string
{
    auto proc_file_name = std::format("/proc/{}/comm", pid);
    std::ifstream proc_file(proc_file_name);
    std::string proc_name;
    proc_file >> proc_name;
    return proc_name;
}

auto get_pid_from_socket(int sockfd) -> Result<int>
{
    struct ucred ucred;
    socklen_t len = sizeof(struct ucred);
    int result = getsockopt(sockfd, SOL_SOCKET, SO_PEERCRED, &ucred, &len);
    if (result == -1) {
        return Err("getsockopt");
    }
    return ucred.pid;
}

auto close_stderr() -> Result<void>
{
    const int nullfd = open("/dev/null", O_WRONLY);
    if (nullfd == -1) {
        return Err("could not open /dev/null");
    }
    int res = dup2(nullfd, STDERR_FILENO);
    if (res == -1) {
        return Err("could not reassign stderr");
    }
    res = close(nullfd);
    if (res == -1) {
        return Err("could not close /dev/null fd");
    }
    return {};
}

} // namespace os
