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
#include "util/unix_socket.hpp"
#include "util/util.hpp"

#include <spdlog/spdlog.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

using unix_socket::Server;

Server::~Server()
{
    if (sockfd != -1) {
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
    }
}

auto Server::start() -> std::expected<void, std::string>
{
    endpoint = util::get_socket_path();
    return bind_to_endpoint().and_then([this] -> std::expected<void, std::string> {
        SPDLOG_INFO("started unix socket server on {}", endpoint);
        accept_thread = std::jthread([this](auto token) { accept_connections(token); });
        return {};
    });
}

void Server::accept_connections(const std::stop_token &token)
{
    while (!token.stop_requested()) {
        if (!os::wait_for_data_on_fd(sockfd, config->waitms).value_or(false)) {
            continue;
        }

        while (true) {
            int filde = accept(sockfd, nullptr, nullptr);
            if (filde == -1) {
                break;
            }
            connection_queue.enqueue(filde);
            SPDLOG_TRACE("accepting connection with fd {}", filde);
        }
    }
}

auto Server::read_data_from_connection() -> std::expected<std::vector<std::string>, std::string>
{
    constexpr int max_conns = 5;
    std::vector<int> descriptors;
    connection_queue.wait_dequeue_bulk_timed(std::back_inserter(descriptors), max_conns,
                                             std::chrono::milliseconds(config->waitms));
    if (descriptors.empty()) {
        return util::unexpected_err("no available connections to read");
    }

    std::vector<pollfd> pollfds;
    for (const int filde : descriptors) {
        pollfd fds{};
        fds.fd = filde;
        fds.events = POLLIN;
        pollfds.emplace_back(fds);
    }

    const int poll_res = poll(pollfds.data(), pollfds.size(), config->waitms);
    if (poll_res == -1) {
        return os::system_error("could not poll connections");
    }

    std::vector<std::string> result;
    for (auto [fd, events, revents] : pollfds) {
        if ((revents & (POLLIN | POLLHUP)) != 0) {
            const auto data = os::read_data_from_socket(fd);
            if (data.has_value()) {
                result.push_back(data.value());
            } else {
                SPDLOG_DEBUG(data.error());
            }
        }
        if ((revents & (POLLHUP | POLLERR | POLLNVAL)) != 0) {
            shutdown(fd, SHUT_RDWR);
            close(fd);
        } else {
            connection_queue.enqueue(fd);
        }
    }

    return result;
}

auto Server::bind_to_endpoint() -> std::expected<void, std::string>
{
    return create_socket().and_then([this] { return bind_to_socket(); }).and_then([this] {
        return listen_to_socket();
    });
}

auto Server::listen_to_socket() const -> std::expected<void, std::string>
{
    const int result = listen(sockfd, SOMAXCONN);
    if (result == -1) {
        return os::system_error("could not listen to endpoint " + endpoint);
    }
    return {};
}

auto Server::bind_to_socket() const -> std::expected<void, std::string>
{
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    endpoint.copy(addr.sun_path, endpoint.length());

    const int result = bind(sockfd, reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr_un));
    if (result == -1) {
        return os::system_error("could not bind to endpoint " + endpoint);
    }
    return {};
}

auto Server::create_socket() -> std::expected<void, std::string>
{
    sockfd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (sockfd == -1) {
        return os::system_error("could not create socket");
    }
    return {};
}
