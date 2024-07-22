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
#include "util/ptr.hpp"
#include "util/util.hpp"

#include <cerrno>
#include <filesystem>
#include <format>
#include <vector>

#include <fcntl.h>
#include <spdlog/spdlog.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

namespace fs = std::filesystem;
using std::expected;
using std::string;
using std::unexpected;

auto os::system_error(const std::string_view message, const std::source_location location) noexcept
    -> std::unexpected<std::string>
{
    const fs::path file_path = location.file_name();
    const std::error_condition econd(errno, std::generic_category());
    return std::unexpected(
        std::format("[{}:{}] {}: {}", file_path.filename().string(), location.line(), message, econd.message()));
}

auto os::exec(const std::string &cmd) noexcept -> std::expected<std::string, std::string>
{
    std::vector<char> buffer(bufsize);
    std::string result;
    const c_unique_ptr<FILE, pclose> pipe{popen(cmd.c_str(), "r")};
    if (!pipe) {
        return system_error("could not open pipe");
    }
    while (fgets(buffer.data(), bufsize, pipe.get()) != nullptr) {
        result.append(buffer.data());
    }
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}

// will block if there is no data available
auto os::read_data_from_fd(const int filde) noexcept -> std::expected<std::string, std::string>
{
    std::vector<char> buffer(bufsize);
    const auto bytes_read = read(filde, buffer.data(), bufsize);
    if (bytes_read == -1) {
        return system_error("could not read from file descriptor");
    }
    std::string result(buffer.data(), bytes_read);
    return result;
}

auto os::read_data_from_socket(const int sockfd) noexcept -> std::expected<std::string, std::string>
{
    std::vector<char> buffer(bufsize);
    std::string result;
    while (true) {
        const auto bytes_read = recv(sockfd, buffer.data(), bufsize, 0);
        if (bytes_read == -1) {
            return system_error("could not read from socket");
        }
        if (bytes_read == 0) {
            break;
        }
        result.append(buffer.data(), bytes_read);
    }
    return result;
}

auto os::read_data_from_stdin() noexcept -> std::expected<std::string, std::string>
{
    return read_data_from_fd(STDIN_FILENO);
}

auto os::get_poll_err(const int event) noexcept -> std::string_view
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

auto os::wait_for_data_on_fd(const int filde, const int waitms) noexcept -> std::expected<bool, std::string>
{
    pollfd fds{};
    fds.fd = filde;
    fds.events = POLLIN;

    const int res = poll(&fds, 1, waitms);
    if (res == -1) {
        return system_error("could not poll on file descriptor");
    }

    if ((fds.revents & (POLLERR | POLLNVAL | POLLHUP)) != 0) {
        return util::unexpected_err(std::format("poll received {}", get_poll_err(fds.revents)));
    }

    return (fds.revents & POLLIN) != 0;
}

auto os::wait_for_data_on_stdin(const int waitms) noexcept -> std::expected<bool, std::string>
{
    return wait_for_data_on_fd(STDIN_FILENO, waitms);
}

auto os::get_pid() noexcept -> int
{
    return getpid();
}

auto os::get_ppid() noexcept -> int
{
    return getppid();
}

auto os::getenv(const std::string &var) noexcept -> std::optional<std::string>
{
    const char *env_p = std::getenv(var.c_str()); // NOLINT
    if (env_p == nullptr) {
        return {};
    }
    return env_p;
}

auto os::daemonize() -> std::expected<void, std::string>
{
    const int pid = fork();
    if (pid == -1) {
        return system_error("could not fork process");
    }

    // kill parent process
    if (pid > 0) {
        SPDLOG_DEBUG("child process {} created, terminating parent", pid);
        std::exit(EXIT_SUCCESS); // NOLINT
    }
    return {};
}

auto os::close_stderr() -> std::expected<void, std::string>
{
    const int nullfd = open("/dev/null", O_WRONLY);
    if (nullfd == -1) {
        return system_error("could not open /dev/null");
    }
    int res = dup2(nullfd, STDERR_FILENO);
    if (res == -1) {
        return system_error("could not reassign stderr");
    }
    res = close(nullfd);
    if (res == -1) {
        return system_error("could not close /dev/null fd");
    }
    return {};
}
