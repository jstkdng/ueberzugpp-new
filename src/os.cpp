// Display images inside a terminal
// Copyright (C) 2024  JustKidding
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

#include "os.hpp"
#include "util/ptr.hpp"

#include <cerrno>
#include <format>
#include <poll.h>
#include <unistd.h>

using std::error_code;
using std::expected;
using std::string;
using std::unexpected;

constexpr auto system_error(std::string_view msg) -> std::unexpected<std::string>
{
    const auto err = error_code(errno, std::system_category());
    return unexpected(std::format("{}: {}", msg, err.message()));
}

auto os::exec(const std::string &cmd) -> std::expected<std::string, std::string>
{
    constexpr int bufsize = 128;
    std::array<char, bufsize> buffer{};
    std::string result;
    const c_unique_ptr<FILE, pclose> pipe{popen(cmd.c_str(), "r")};
    if (!pipe) {
        return system_error("Could not open pipe");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result.append(buffer.data());
    }
    if (!result.empty()) {
        result.erase(result.length() - 1);
    }
    return result;
}

auto os::read_data_from_fd(const int filde, const char sep) -> std::expected<std::string, std::string>
{
    string response;
    char readch = 0;

    while (true) {
        const auto status = read(filde, &readch, 1);
        if (status == -1) {
            return system_error("could not read from file descriptor");
        }
        if (status == 0 || readch == sep) {
            if (response.empty()) {
                return std::unexpected("got no data from descriptor after read");
            }
            break;
        }
        response.push_back(readch);
    }
    return response;
}

auto os::read_data_from_stdin(const char sep) -> std::expected<std::string, std::string>
{
    return read_data_from_fd(STDIN_FILENO, sep);
}

auto os::wait_for_data_on_fd(const int filde, const int waitms) -> std::expected<bool, std::string>
{
    pollfd fds{};
    fds.fd = filde;
    fds.events = POLLIN;

    poll(&fds, 1, waitms);

    if ((fds.revents & (POLLERR | POLLNVAL | POLLHUP)) != 0) {
        return std::unexpected("poll received unexpected event");
    }

    return (fds.revents & POLLIN) != 0;
}

auto os::wait_for_data_on_stdin(int waitms) -> std::expected<bool, std::string>
{
    return wait_for_data_on_fd(STDIN_FILENO, waitms);
}

auto os::get_pid() -> int
{
    return getpid();
}

auto os::get_ppid() -> int
{
    return getppid();
}

auto os::getenv(const std::string &var) -> std::optional<std::string>
{
    const char *env_p = std::getenv(var.c_str()); // NOLINT
    if (env_p == nullptr) {
        return {};
    }
    return env_p;
}
