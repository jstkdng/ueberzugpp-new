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

#include "config.hpp"
#include "unix_socket.hpp"

#ifndef HAVE_STD_JTHREAD
#  include "jthread/jthread.hpp"
#endif

#include <condition_variable>
#include <expected>
#include <queue>
#include <string>

#include <nlohmann/json.hpp>

/**
 * Read for commands in stdin and unix_socket socket, divide them by newlines and parse them
 */
class CommandManager
{
  public:
    auto initialize() -> std::expected<void, std::string>;

    auto unqueue() -> std::expected<nlohmann::json, std::string>;

  private:
    static constexpr int waitms = 100;

    unix_socket::Server socket_server;
    std::shared_ptr<Config> config = Config::instance();

    std::queue<nlohmann::json> command_queue;
    std::mutex queue_mutex;
    std::condition_variable cond;
    std::string stdin_buffer;
    std::jthread stdin_thread;
    std::jthread socket_thread;

    void wait_for_input_on_stdin();
    void wait_for_input_on_socket();
    auto extract_commands(std::string_view view) -> std::string;
};

#endif // COMMAND_HPP
