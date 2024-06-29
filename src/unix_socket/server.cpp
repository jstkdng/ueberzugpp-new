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
#include "os.hpp"
#include "unix_socket.hpp"

#include <algorithm>
#include <bit>

#include <spdlog/spdlog.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

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
    for (const auto filde : accepted_connections) {
        shutdown(filde, SHUT_RDWR);
        close(filde);
    }
}

auto Server::get_descriptor() const -> int
{
    return socket.fd;
}

auto Server::start() -> std::expected<void, std::string>
{
    logger = spdlog::get("socket");
    auto bind_res = bind_to_endpoint();
    if (bind_res.has_value()) {
        logger->info("Listening for connections on {}", endpoint);
        accept_thread = std::jthread([this] { accept_connections(); });
    } else {
        SPDLOG_LOGGER_DEBUG(logger, bind_res.error());
    }
    return bind_res;
}

void Server::accept_connections()
{
    while (!Application::stop_flag) {
        constexpr int waitms = 100;
        const auto in_event = os::wait_for_data_on_fd(socket.fd, waitms);
        if (!in_event.has_value()) {
            SPDLOG_LOGGER_DEBUG(logger, in_event.error());
            return;
        }
        if (!in_event.value()) {
            continue;
        }
        const auto accepted_fd = accept_connection();
        if (!accepted_fd.has_value()) {
            SPDLOG_LOGGER_DEBUG(logger, accepted_fd.error());
            return;
        }
        accepted_connections.push_back(*accepted_fd);
    }
}

auto Server::read_data_from_connection() -> std::expected<std::vector<std::string>, std::string>
{
    std::vector<pollfd> fds(accepted_connections.size());
    for (const auto filde : accepted_connections) {
        pollfd tmp{};
        tmp.fd = filde;
        tmp.events = POLLIN;
        fds.emplace_back(tmp);
    }

    const int res = poll(fds.data(), fds.size(), 100);
    if (res == -1) {
        return os::system_error("can't poll for connections");
    }

    std::vector<std::string> result;
    for (const auto &[fd, events, revents] : fds) {
        if ((revents & (POLLERR | POLLNVAL)) != 0) {
            SPDLOG_LOGGER_DEBUG(logger, "received error on fd {}", fd);
            remove_accepted_connection(fd);
            continue;
        }
        if ((revents & (POLLIN | POLLHUP)) != 0) {
            const auto data = os::read_data_from_socket(fd);
            if (data.has_value()) {
                result.push_back(data.value());
            } else {
                SPDLOG_LOGGER_DEBUG(logger, data.error());
            }
        }
        if ((revents & POLLHUP) != 0) {
            SPDLOG_LOGGER_DEBUG(logger, "removing connection from fd {}", fd);
            remove_accepted_connection(fd);
        }
    }

    return result;
}

void Server::remove_accepted_connection(int filde)
{
    shutdown(filde, SHUT_RDWR);
    close(filde);
    auto [begin, end] = std::ranges::remove(accepted_connections, filde);
    accepted_connections.erase(begin, end);
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
