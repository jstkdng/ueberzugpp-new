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

#include "application.hpp"
#include "unix_socket.hpp"

#include <bit>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "os.hpp"

#include <sys/poll.h>

using unix_socket::Server;

Server::Server(const std::string_view endpoint)
    : endpoint(endpoint)
{
}

Server::~Server()
{
    if (socket.fd != -1) {
        shutdown(socket.fd, SHUT_RDWR);
        close(socket.fd);
    }
    for (const auto fd : accepted_connections) {
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
}

auto Server::get_descriptor() const -> int
{
    return socket.fd;
}

void Server::start()
{
    auto bind_res = bind_to_endpoint();
    if (!bind_res.has_value()) {
        return;
    }
    accept_thread = std::jthread([this] { accept_connections(); });
}

void Server::accept_connections()
{
    while (!Application::stop_flag) {
        constexpr int waitms = 100;
        const auto in_event = os::wait_for_data_on_fd(socket.fd, waitms);
        if (!in_event.has_value()) {
            return;
        }
        if (!in_event.value()) {
            continue;
        }
        const auto accepted_fd = accept_connection();
        if (!accepted_fd.has_value()) {
            return;
        }
        accepted_connections.emplace(*accepted_fd);
    }
}

auto Server::read_data_from_connection() const -> std::expected<std::string, std::string>
{
    return accept_connection().and_then(os::read_data_from_socket);
}

auto Server::bind_to_endpoint() -> std::expected<void, std::string>
{
    return util::get_socket_and_address(endpoint)
        .and_then([this](const socket_and_address &saa) {
            socket = saa;
            return bind_to_endpoint_internal();
        })
        .and_then([this] { return listen_to_endpoint(); });
}

auto Server::accept_connection() const -> std::expected<int, std::string>
{
    const int result = accept(socket.fd, nullptr, nullptr);
    if (result == -1) {
        return os::system_error("could not accept connection");
    }
    return result;
}

auto Server::listen_to_endpoint() const -> std::expected<void, std::string>
{
    const int result = listen(socket.fd, SOMAXCONN);
    if (result == -1) {
        return os::system_error("could not listen to endpoint");
    }
    return {};
}

auto Server::bind_to_endpoint_internal() -> std::expected<void, std::string>
{
    const int result = bind(socket.fd, std::bit_cast<const sockaddr *>(&socket.address), sizeof(sockaddr_un));
    if (result == -1) {
        return os::system_error("could not bind to endpoint");
    }
    return {};
}
