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
#include <unistd.h>

using unix_socket::Client;

Client::~Client()
{
    if (sockfd != -1) {
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
    }
}

auto Client::write(const std::byte *buffer, std::size_t buflen) const -> std::expected<void, std::string>
{
    const auto *runner = buffer;
    while (buflen != 0) {
        const auto bytes_sent = send(sockfd, runner, buflen, MSG_NOSIGNAL);
        if (bytes_sent == -1) {
            return std::unexpected("could not write to socket");
        }
        buflen -= bytes_sent;
        runner += bytes_sent;
    }
    return {};
}

auto Client::initialize(const std::string_view new_endpoint) -> std::expected<void, std::string>
{
    endpoint = new_endpoint;
    return create_socket().and_then([this] { return connect_socket(); });
}

auto Client::connect_socket() const -> std::expected<void, std::string>
{
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    endpoint.copy(addr.sun_path, endpoint.length());

    const int result = connect(sockfd, std::bit_cast<const sockaddr *>(&addr), sizeof(sockaddr_un));
    if (result == -1) {
        return os::system_error("coud not connect to endpoint");
    }
    return {};
}

auto Client::create_socket() -> std::expected<void, std::string>
{
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        return os::system_error("could not create socket");
    }
    return {};
}
