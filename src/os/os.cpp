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

#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <spdlog/spdlog.h>

#include <cerrno>
#include <cstdlib>
#include <format>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>

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
    return std::make_optional(env_p);
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

    if (poll(&fds, 1, waitms) == -1) {
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
    auto result = make_result<std::string>(bufsize, 0);
    const auto bytes_read = read(filde, result->data(), bufsize);
    if (bytes_read == -1) {
        return Err("could not read from file descriptor");
    }
    result->resize(bytes_read);
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
#ifdef __linux__
    struct ucred ucred;
    if (socklen_t len = sizeof(struct ucred); getsockopt(sockfd, SOL_SOCKET, SO_PEERCRED, &ucred, &len) == -1) {
        return Err("getsockopt");
    }
    return ucred.pid;
#else
    return sockfd;
#endif
}

auto daemonize() -> Result<void>
{
    int pid = fork();
    if (pid == -1) {
        return Err("fork");
    }

    // kill parent process
    if (pid > 0) {
        spdlog::debug("child process {} created, terminating parent", pid);
        std::exit(EXIT_SUCCESS); // NOLINT
    }
    return {};
}

} // namespace upp::os
