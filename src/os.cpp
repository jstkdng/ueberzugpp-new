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

#include "os.hpp"
#include "exceptions.hpp"

#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>

namespace upp::os
{

auto getpid() -> int
{
    return ::getpid();
}

auto getenv(std::string_view var) -> std::optional<std::string>
{
    const char *env_p = std::getenv(std::string(var).c_str()); // NOLINT
    if (env_p == nullptr) {
        return {};
    }
    return std::make_optional(env_p);
}

void close_stderr()
{
    int nullfd = open("/dev/null", O_WRONLY);
    if (nullfd == -1) {
        throw ex::posix_error("could not open /dev/null");
    }
    int res = dup2(nullfd, STDERR_FILENO);
    if (res == -1) {
        throw ex::posix_error("could not reassign stderr");
    }
    res = close(nullfd);
    if (res == -1) {
        throw ex::posix_error("could not close /dev/null fd");
    }
}

void daemonize()
{
    int pid = fork();
    if (pid == -1) {
        throw ex::posix_error("could not fork process");
    }

    // kill parent process
    if (pid > 0) {
        std::exit(EXIT_SUCCESS); // NOLINT
    }
}

} // namespace upp::os
