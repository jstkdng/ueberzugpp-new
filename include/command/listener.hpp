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

#pragma once

#include "command/command.hpp"
#include "log.hpp"
#include "unix/socket.hpp"

#include <string>
#include <string_view>

namespace upp
{

class CommandListener
{
  public:
    explicit CommandListener(CommandQueue *queue);
    ~CommandListener();
    auto start(std::string_view new_parser, bool no_stdin) -> Result<void>;

  private:
    void wait_for_input_on_stdin();
    void wait_for_input_on_socket();
    void extract_commands(std::string_view line);
    void flush_command_queue() const;

    CommandQueue *queue;
    std::string parser;
    std::thread stdin_thread;
    std::thread socket_thread;
    unix::socket::Server socket_server;
    Logger logger;
};

} // namespace upp
