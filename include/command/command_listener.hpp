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

#ifndef COMMAND_LISTENER_HPP
#define COMMAND_LISTENER_HPP

#include <expected>
#include <string>

#include "command/command.hpp"
#include "config.hpp"
#include "util/unix_socket.hpp"

#ifdef HAVE_STD_JTHREAD
#include <stop_token>
#include <thread>
#else
#include "jthread/jthread.hpp"
#endif

#include "moodycamel/blockingconcurrentqueue.h"

/**
 * Read for commands in stdin and unix_socket socket, divide them by newlines and parse them
 */
class CommandListener
{
  public:
    auto initialize(moodycamel::BlockingConcurrentQueue<Command> *queue) -> std::expected<void, std::string>;

  private:
    std::shared_ptr<Config> config = Config::instance();
    moodycamel::BlockingConcurrentQueue<Command> *command_queue = nullptr;
    unix_socket::Server socket_server;

    std::string stdin_buffer;
    std::jthread stdin_thread;
    std::jthread socket_thread;

    void wait_for_input_on_stdin(const std::stop_token &token);
    void wait_for_input_on_socket(const std::stop_token &token);
    [[nodiscard]] auto extract_commands(std::string_view view) const -> std::string;
    void flush_command_queue() const;
};

#endif // COMMAND_LISTENER_HPP
