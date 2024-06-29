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

#include <bit>
#include <sys/socket.h>
#include <sys/un.h>

using unix_socket::Client;

Client::Client(const std::string_view endpoint)
    : endpoint(endpoint)
{
}

auto Client::connect_to_endpoint() -> std::expected<void, std::string>
{
    return create_socket().and_then([this] { return connect_to_socket(); });
}

auto Client::connect_to_socket() -> std::expected<void, std::string>
{
    const int result = connect(socket.fd, std::bit_cast<const sockaddr *>(&socket.addr), sizeof(sockaddr_un));
    if (result == -1) {
        return os::system_error("coud not connect to endpoint");
    }
    return {};
}

auto Client::create_socket() -> std::expected<void, std::string>
{
    auto socket_res = sockfd::create(endpoint);
    if (!socket_res) {
        return std::unexpected(socket_res.error());
    }
    socket = *socket_res;
    return {};
}
