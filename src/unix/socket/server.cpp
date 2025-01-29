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

#include "unix/socket.hpp"
#include "util/util.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <filesystem>

namespace fs = std::filesystem;

namespace upp::unix::socket
{

Server::~Server()
{
    fs::remove(endpoint);
}

auto Server::start() -> Result<void>
{
    endpoint = util::get_socket_path();
    return create_socket().and_then([this] { return bind_to_endpoint(); }).and_then([this] {
        return listen_for_connections();
    });
}

auto Server::get_fd() const -> int
{
    return sockfd.get();
}

auto Server::get_endpoint() const -> std::string
{
    return endpoint;
}

auto Server::read_data_from_connection() const -> Result<std::string>
{
    fd connfd{accept(sockfd.get(), nullptr, nullptr)};
    return os::read_data_from_fd(connfd.get());
}

auto Server::create_socket() -> Result<void>
{
    sockfd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (!sockfd) {
        return Err("could not create socket");
    }
    return {};
}

auto Server::bind_to_endpoint() const -> Result<void>
{
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    endpoint.copy(addr.sun_path, endpoint.length());

    int result = bind(sockfd.get(), reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr_un));
    if (result == -1) {
        return Err("could not bind to endpoint " + endpoint);
    }
    return {};
}

auto Server::listen_for_connections() const -> Result<void>
{
    int result = listen(sockfd.get(), SOMAXCONN);
    if (result == -1) {
        return Err("could not listen to endpoint " + endpoint);
    }
    return {};
}

} // namespace upp::unix::socket
