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

#include "config.hpp"

#include <expected>
#include <stop_token>
#include <string>
#include <thread>
#include <vector>

#include <sys/un.h>

#ifndef HAVE_STD_JTHREAD
#  include "jthread/jthread.hpp"
#endif

namespace unix_socket
{

struct sockfd {
    int fd = -1;
    sockaddr_un addr{};
};

class Server
{
  public:
    ~Server();

    auto start() -> std::expected<void, std::string>;
    auto read_data_from_connection() -> std::expected<std::vector<std::string>, std::string>;

  private:
    std::string endpoint;
    sockfd socket{};

    std::shared_ptr<Config> config = Config::instance();
    std::vector<int> accepted_connections;
    std::jthread accept_thread;

    auto bind_to_endpoint() -> std::expected<void, std::string>;
    auto bind_to_socket() -> std::expected<void, std::string>;
    auto create_socket() -> std::expected<void, std::string>;
    void accept_connections(const std::stop_token &token);
    void remove_accepted_connection(int filde);

    [[nodiscard]] auto accept_connection() const -> std::expected<int, std::string>;
    [[nodiscard]] auto listen_to_socket() const -> std::expected<void, std::string>;
};

class Client
{
  public:
    explicit Client(std::string_view endpoint);
    auto connect_to_endpoint() -> std::expected<void, std::string>;

  private:
    std::string endpoint;
    sockfd socket;

    auto create_socket() -> std::expected<void, std::string>;
    auto connect_to_socket() -> std::expected<void, std::string>;
};

namespace util
{
auto create_socket(std::string_view endpoint) -> std::expected<sockfd, std::string>;
}

} // namespace unix_socket

#endif // UNIX_SOCKET_HPP
