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
#include "os/unix.hpp"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <format>

namespace upp::unix::socket
{

Client::Client(std::string_view endpoint) :
    sockfd(::socket(AF_UNIX, SOCK_STREAM, 0))
{
    if (!sockfd) {
        throw ex::posix_error("could not create socket");
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    endpoint.copy(addr.sun_path, endpoint.length());

    int result = ::connect(sockfd.get(), reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr_un));
    if (result == -1) {
        throw ex::posix_error(std::format("could not connect to endpoint {}", endpoint));
    }
}

void Client::connect_and_write(std::string_view endpoint, std::span<const std::byte> buffer)
{
    Client client(endpoint);
    client.write(buffer);
}

auto Client::read_until_empty() const -> std::string
{
    std::string result;
    const int read_buffer_size = 4096;
    std::array<char, read_buffer_size> read_buffer;
    result.reserve(read_buffer_size);
    while (true) {
        const auto status = recv(sockfd.get(), read_buffer.data(), read_buffer_size, 0);
        if (status == -1) {
            throw ex::posix_error("could not read from socket");
        }
        if (status == 0) {
            return result;
        }
        result.append(read_buffer.data(), status);
    }
}

void Client::read(std::span<std::byte> buffer) const
{
    auto *runner = buffer.data();
    auto buflen = buffer.size();
    while (buflen != 0) {
        const auto status = recv(sockfd.get(), runner, buflen, 0);
        if (status == 0) {
            return; // no data
        }
        if (status == -1) {
            throw ex::posix_error("could not read from socket");
        }
        buflen -= status;
        runner += status;
    }
}

void Client::write(std::span<const std::byte> buffer) const
{
    const auto *runner = buffer.data();
    auto buflen = buffer.size();
    while (buflen != 0) {
        const auto bytes_sent = send(sockfd.get(), runner, buflen, MSG_NOSIGNAL);
        if (bytes_sent == -1) {
            throw ex::posix_error("could not write to socket");
        }
        buflen -= bytes_sent;
        runner += bytes_sent;
    }
}

} // namespace upp::unix::socket
