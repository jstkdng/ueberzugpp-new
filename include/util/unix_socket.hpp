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

#ifndef UNIX_SOCKET_HPP
#define UNIX_SOCKET_HPP

#include "config.hpp"
#include "moodycamel/blockingconcurrentqueue.h"

#include <cstddef>
#include <expected>
#include <span>
#include <string>
#include <vector>

#ifdef HAVE_STD_JTHREAD
#include <stop_token>
#include <thread>
#else
#include "jthread/jthread.hpp"
#endif

namespace unix_socket
{

class Server
{
  public:
    ~Server();
    auto operator=(Server &&) -> Server & = delete;

    auto start() -> std::expected<void, std::string>;
    auto read_data_from_connection() -> std::expected<std::vector<std::string>, std::string>;

  private:
    int sockfd = -1;
    std::string endpoint;

    std::shared_ptr<Config> config = Config::instance();
    moodycamel::BlockingConcurrentQueue<int> connection_queue;
    std::jthread accept_thread;

    auto bind_to_endpoint() -> std::expected<void, std::string>;
    auto create_socket() -> std::expected<void, std::string>;
    void accept_connections(const std::stop_token &token);

    [[nodiscard]] auto bind_to_socket() const -> std::expected<void, std::string>;
    [[nodiscard]] auto listen_to_socket() const -> std::expected<void, std::string>;
};

class Client
{
  public:
    ~Client();
    auto operator=(Client &&) -> Client & = delete;

    auto initialize(std::string_view new_endpoint) -> std::expected<void, std::string>;
    [[nodiscard]] auto write(std::span<const std::byte> buffer) const -> std::expected<void, std::string>;
    [[nodiscard]] auto read(std::span<std::byte> buffer) const -> std::expected<void, std::string>;

  private:
    int sockfd = -1;
    std::string endpoint;

    auto create_socket() -> std::expected<void, std::string>;
    [[nodiscard]] auto connect_socket() const -> std::expected<void, std::string>;
};

} // namespace unix_socket

#endif // UNIX_SOCKET_HPP
