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
#include "util/result.hpp"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <array>
#include <cstddef>
#include <format>
#include <span>
#include <string>
#include <string_view>

namespace upp::unix::socket
{

auto Client::connect(std::string_view endpoint) -> Result<void>
{
    sockfd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (!sockfd) {
        return Err("could not create socket");
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    endpoint.copy(addr.sun_path, endpoint.length());

    const int result = ::connect(sockfd.get(), reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr_un));
    if (result == -1) {
        return Err(std::format("could not connect to endpoint {}", endpoint));
    }

    return {};
}

auto Client::read(std::span<std::byte> buffer) const -> Result<void>
{
    auto *runner = buffer.data();
    auto buflen = buffer.size();
    while (buflen != 0) {
        const auto status = recv(sockfd.get(), runner, buflen, 0);
        if (status == 0) {
            return {}; // no data
        }
        if (status == -1) {
            return Err("could not read from socket");
        }
        buflen -= status;
        runner += status;
    }
    return {};
}

auto Client::read_until_empty() const -> std::string
{
    std::string result;
    const int read_buffer_size = 4096;
    std::array<char, read_buffer_size> read_buffer;
    result.reserve(read_buffer_size);
    while (true) {
        const auto status = recv(sockfd.get(), read_buffer.data(), read_buffer_size, 0);
        if (status <= 0) {
            return result;
        }
        result.append(read_buffer.data(), status);
    }
}

auto Client::write(const std::span<const std::byte> buffer) const -> Result<void>
{
    const auto *runner = buffer.data();
    auto buflen = buffer.size();
    while (buflen != 0) {
        const auto bytes_sent = send(sockfd.get(), runner, buflen, MSG_NOSIGNAL);
        if (bytes_sent == -1) {
            return Err("could not write to socket");
        }
        buflen -= bytes_sent;
        runner += bytes_sent;
    }
    return {};
}

} // namespace upp::unix::socket
