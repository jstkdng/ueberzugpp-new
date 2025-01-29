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

#include "application/application.hpp"
#include "os/os.hpp"
#include "unix/socket.hpp"
#include "util/util.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace upp::unix::socket
{

Server::Server(CommandQueue *queue) :
    queue(queue)
{
}

auto Server::start() -> Result<void>
{
    endpoint = util::get_socket_path();

    sockfd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (!sockfd) {
        return Err("could not create socket");
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    endpoint.copy(addr.sun_path, endpoint.length());

    int result = bind(sockfd.get(), reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr_un));
    if (result == -1) {
        return Err("could not bind to endpoint " + endpoint);
    }

    result = listen(sockfd.get(), SOMAXCONN);
    if (result == -1) {
        return Err("could not listen to endpoint " + endpoint);
    }

    accept_thread = std::jthread([this](auto token) { accept_connections(token); });
    return {};
}

void Server::accept_connections(const std::stop_token &token)
{
    while (!token.stop_requested()) {
        if (auto in_event = os::wait_for_data_on_fd(sockfd.get())) {
            if (!*in_event) {
                continue;
            }
        } else {
            Application::terminate();
            return;
        }

        read_data();
    }
}

void Server::read_data()
{
    fd connfd{accept(sockfd.get(), nullptr, nullptr)};
    if (auto data = os::read_data_from_fd(connfd.get())) {
    } else {
        logger->warn("received no data on fd {}: {}", connfd.get(), data.error().message());
    }
}

} // namespace upp::unix::socket
