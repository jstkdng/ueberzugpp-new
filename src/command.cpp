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

#include "command.hpp"
#include <array>
#include <poll.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

CommandManager::CommandManager(std::string_view socket_endpoint)
    : socket_server(socket_endpoint)

{
    stdin_buffer.reserve(buffer_size);
    socket_buffer.reserve(buffer_size);
    stdin_read_buffer.reserve(buffer_size);
}

void CommandManager::wait_for_input()
{
    constexpr int waitms = 100;
    std::array<pollfd, 2> pollfds{};
    pollfds.at(0).fd = STDIN_FILENO;
    pollfds.at(0).events = POLLIN;
    pollfds.at(1).fd = socket_server.get_descriptor();
    pollfds.at(1).events = POLLIN;

    const int result = poll(pollfds.data(), 2, waitms);
    if (result == -1) {
        SPDLOG_DEBUG("received unexpected event");
        return;
    }
}
