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

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "unix_socket.hpp"

#include <expected>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

/**
 * Read for commands in stdin and unix_socket socket, divide them by newlines and parse them
 */
class CommandManager
{
  public:
    explicit CommandManager(std::string_view socket_endpoint);

    auto initialize() -> std::expected<void, std::string>;

  private:
    static constexpr int waitms = 100;

    std::shared_ptr<spdlog::logger> logger;
    std::queue<nlohmann::json> command_queue;
    std::mutex queue_mutex;
    std::string stdin_buffer;
    unix_socket::Server socket_server;
    std::jthread stdin_thread;
    std::jthread socket_thread;

    void wait_for_input_on_stdin();
    void wait_for_input_on_socket();
    auto extract_commands(std::string_view view) -> std::string;
};

#endif // COMMAND_HPP
