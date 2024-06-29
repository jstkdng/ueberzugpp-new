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
#include "unix_socket.hpp"

#include <sys/socket.h>

using unix_socket::sockfd;

auto sockfd::create(const std::string_view endpoint) -> std::expected<sockfd, std::string>
{
    sockfd res{};
    res.addr.sun_family = AF_UNIX;
    endpoint.copy(res.addr.sun_path, endpoint.size());

    res.fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (res.fd == -1) {
        return os::system_error("could not create socket");
    }

    return res;
}

sockfd::~sockfd()
{
    if (fd != -1) {
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
}
