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

#include "os/os.hpp"
#include "util/unix_socket.hpp"
#include "util/util.hpp"

#include <algorithm>

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
    for (const auto filde : accepted_connections) {
        shutdown(filde, SHUT_RDWR);
        close(filde);
    }
}

auto Server::start() -> std::expected<void, std::string>
{
    endpoint = util::get_socket_path();
    auto bind_ok = bind_to_endpoint();
    if (bind_ok) {
        SPDLOG_INFO("listening for connections on {}", endpoint);
        accept_thread = std::jthread([this](auto token) { accept_connections(token); });
    } else {
        SPDLOG_DEBUG(bind_ok.error());
    }
    return bind_ok;
}

void Server::accept_connections(const std::stop_token &token)
{
    while (!token.stop_requested()) {
        const auto in_event = os::wait_for_data_on_fd(sockfd, config->waitms);
        if (!in_event.has_value()) {
            SPDLOG_DEBUG(in_event.error());
            return;
        }
        if (!in_event.value()) {
            continue;
        }
        const auto accepted_fd = accept_connection();
        if (!accepted_fd.has_value()) {
            SPDLOG_DEBUG(accepted_fd.error());
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

    const int res = poll(fds.data(), fds.size(), config->waitms);
    if (res == -1) {
        return os::system_error("can't poll for connections");
    }

    std::vector<std::string> result;
    for (const auto &[fd, events, revents] : fds) {
        if ((revents & (POLLERR | POLLNVAL)) != 0) {
            SPDLOG_TRACE("received {} on fd {}, removing connection", os::get_poll_err(revents), fd);
            remove_accepted_connection(fd);
            continue;
        }
        if ((revents & (POLLIN | POLLHUP)) != 0) {
            const auto data = os::read_data_from_socket(fd);
            if (data.has_value()) {
                result.push_back(data.value());
            } else {
                SPDLOG_DEBUG(data.error());
            }
        }
        if ((revents & POLLHUP) != 0) {
            SPDLOG_TRACE("received {} on fd {}, removing connection", os::get_poll_err(revents), fd);
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
    return create_socket().and_then([this] { return bind_to_socket(); }).and_then([this] {
        return listen_to_socket();
    });
}

auto Server::accept_connection() const -> std::expected<int, std::string>
{
    const int result = accept(sockfd, nullptr, nullptr);
    if (result == -1) {
        return os::system_error("could not accept connection");
    }
    return result;
}

auto Server::listen_to_socket() const -> std::expected<void, std::string>
{
    const int result = listen(sockfd, SOMAXCONN);
    if (result == -1) {
        return os::system_error("could not listen to endpoint");
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
        return os::system_error("could not bind to endpoint");
    }
    return {};
}

auto Server::create_socket() -> std::expected<void, std::string>
{
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        return os::system_error("could not create socket");
    }
    return {};
}
