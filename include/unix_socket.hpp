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

#ifndef UNIX_SOCKET_HPP
#define UNIX_SOCKET_HPP

#include <expected>
#include <string>
#include <thread>
#include <vector>

#include <sys/un.h>

namespace unix_socket
{

struct socket_and_address {
    int fd;
    sockaddr_un address;
};

class Server
{
  public:
    explicit Server(std::string_view endpoint);
    ~Server();

    auto start() -> std::expected<void, std::string>;

    [[nodiscard]] auto get_descriptor() const -> int;
    auto read_data_from_connection() -> std::expected<std::vector<std::string>, std::string>;

  private:
    std::string endpoint;
    socket_and_address socket{};
    std::vector<int> accepted_connections;
    std::jthread accept_thread;

    auto bind_to_endpoint() -> std::expected<void, std::string>;
    auto bind_to_endpoint_internal() -> std::expected<void, std::string>;
    void accept_connections();

    [[nodiscard]] auto accept_connection() const -> std::expected<int, std::string>;
    [[nodiscard]] auto listen_to_endpoint() const -> std::expected<void, std::string>;
};

class Client
{
  public:
    explicit Client(std::string_view endpoint);
    auto connect_to_endpoint() -> std::expected<void, std::string>;

  private:
    std::string endpoint;
    socket_and_address socket{};

    auto connect_to_socket() -> std::expected<void, std::string>;
};

namespace util
{

auto get_socket_and_address(std::string_view endpoint) -> std::expected<socket_and_address, std::string>;

}

} // namespace unix_socket

#endif // UNIX_SOCKET_HPP
