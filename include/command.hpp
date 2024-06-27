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

#include <queue>
#include <shared_mutex>
#include <string>
#include <vector>

/**
 * Read for commands in stdin and unix_socket socket, divide them by newlines and parse them
 */
class CommandManager
{
  public:
    CommandManager();

    void wait_for_input();

  private:
    static constexpr int buffer_size = 32 * 1024;
    std::queue<std::string> command_queue;
    std::shared_mutex queue_mutex;

    std::string stdin_buffer;
    std::vector<char> stdin_read_buffer;

    std::string socket_buffer;
    std::vector<char> socket_read_buffer;
};

#endif // COMMAND_HPP
