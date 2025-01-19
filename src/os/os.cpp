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
#include "util/result.hpp"

#include <sys/socket.h>
#include <sys/poll.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <format>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace upp::os
{

auto getpid() -> int
{
    return ::getpid();
}

auto strerror() -> std::string
{
    const std::error_code code(errno, std::generic_category());
    return code.message();
}

auto getenv(const std::string &var) -> std::optional<std::string>
{
    const char *env_p = std::getenv(var.c_str()); // NOLINT
    if (env_p == nullptr) {
        return {};
    }
    return env_p;
}

auto get_poll_err(int event) -> std::string_view
{
    if ((event & POLLHUP) != 0) {
        return "POLLHUP";
    }
    if ((event & POLLERR) != 0) {
        return "POLLERR";
    }
    if ((event & POLLNVAL) != 0) {
        return "POLLNVAL";
    }
    [[unlikely]] return "unknown event";
}

auto wait_for_data_on_fd(int filde) -> Result<bool>
{
    pollfd fds{};
    fds.fd = filde;
    fds.events = POLLIN;

    if (int res = poll(&fds, 1, waitms); res == -1) {
        return Err("could not poll on file descriptor");
    }

    if ((fds.revents & (POLLERR | POLLNVAL | POLLHUP)) != 0) {
        return Err(std::format("poll received {}", get_poll_err(fds.revents)));
    }

    return (fds.revents & POLLIN) != 0;
}

auto wait_for_data_on_stdin() -> Result<bool>
{
    return wait_for_data_on_fd(STDIN_FILENO);
}

// will block if there is no data available
auto read_data_from_fd(int filde) -> Result<std::string>
{
    std::vector<char> buffer(bufsize);
    const auto bytes_read = read(filde, buffer.data(), bufsize);
    if (bytes_read == -1) {
        return Err("could not read from file descriptor");
    }
    std::string result(buffer.data(), bytes_read);
    return result;
}

auto read_data_from_stdin() -> Result<std::string>
{
    return read_data_from_fd(STDIN_FILENO);
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

} // namespace upp::os
