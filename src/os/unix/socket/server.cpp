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

#include "exceptions.hpp"
#include "os/os.hpp"
#include "os/unix.hpp"
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

void Server::start()
{
    endpoint = util::get_socket_path(os::getpid());
    create_socket();
    bind_to_endpoint();
    listen_for_connections();
}

auto Server::get_fd() const -> int
{
    return sockfd.get();
}

auto Server::get_endpoint() const -> std::string
{
    return endpoint;
}

auto Server::read_data_from_connection() const -> std::string
{
    fd connfd{accept(sockfd.get(), nullptr, nullptr)};
    return os::read_data_from_fd(connfd.get());
}

void Server::create_socket()
{
    sockfd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (!sockfd) {
        throw ex::posix_error("could not create socket");
    }
}

void Server::bind_to_endpoint() const
{
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    endpoint.copy(addr.sun_path, endpoint.length());

    int result = bind(sockfd.get(), reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr_un));
    if (result == -1) {
        throw ex::posix_error("could not bind to endpoint " + endpoint);
    }
}

void Server::listen_for_connections() const
{
    int result = listen(sockfd.get(), SOMAXCONN);
    if (result == -1) {
        throw ex::posix_error("could not listen to endpoint " + endpoint);
    }
}

} // namespace upp::unix::socket
