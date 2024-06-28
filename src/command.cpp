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

#include "application.hpp"
#include "os.hpp"

#include <array>
#include <iostream>
#include <poll.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

CommandManager::CommandManager(const std::string_view socket_endpoint)
    : socket_server(socket_endpoint)
{
}

auto CommandManager::initialize() -> std::expected<void, std::string>
{
    return socket_server.bind_to_endpoint();
}

void CommandManager::wait_for_input_on_stdin()
{
    while (!Application::stop_flag) {
        auto in_event = os::wait_for_data_on_stdin(waitms);
        if (!in_event.has_value()) {
            return;
        }
        if (!in_event.value()) {
            continue;
        }
        auto data = os::read_data_from_stdin();
        if (!data.has_value()) {
            return;
        }
        stdin_buffer.append(data.value());

        while (true) {
            auto find_result = stdin_buffer.find('\n');
            if (find_result == std::string::npos) {
                break;
            }

            auto substr = stdin_buffer.substr(0, find_result);
            command_queue.emplace(substr);
            stdin_buffer.erase(0, find_result + 1);
        }
    }
}

void CommandManager::wait_for_input()
{
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
