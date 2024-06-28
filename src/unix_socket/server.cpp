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

#include "unix_socket.hpp"

#include <bit>
#include <functional>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "os.hpp"

using unix_socket::server;

server::server(const std::string_view endpoint)
    : endpoint(endpoint)
{
}

auto server::read_data_from_connection() const -> std::expected<std::string, std::string>
{
    return accept_connection().and_then(os::read_data_from_fd);
}

auto server::bind_to_endpoint() -> std::expected<void, std::string>
{
    return util::get_socket_and_address(endpoint)
        .and_then([this](const socket_and_address &saa) {
            socket = saa;
            return bind_to_endpoint_internal();
        })
        .and_then([this] { return listen_to_endpoint(); });
}

auto server::accept_connection() const -> std::expected<int, std::string>
{
    int result = accept(socket.fd, nullptr, nullptr);
    if (result == -1) {
        return os::system_error("could not accept connection");
    }
    return result;
}

auto server::listen_to_endpoint() const -> std::expected<void, std::string>
{
    int result = listen(socket.fd, SOMAXCONN);
    if (result == -1) {
        return os::system_error("could not listen to endpoint");
    }
    return {};
}

auto server::bind_to_endpoint_internal() -> std::expected<void, std::string>
{
    int result = bind(socket.fd, std::bit_cast<const sockaddr *>(&socket.address), sizeof(sockaddr_un));
    if (result == -1) {
        return os::system_error("could not bind to endpoint");
    }
    return {};
}
