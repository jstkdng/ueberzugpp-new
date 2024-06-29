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

#include <poll.h>
#include <spdlog/spdlog.h>

using njson = nlohmann::json;

CommandManager::CommandManager(const std::string_view socket_endpoint)
    : socket_server(socket_endpoint)
{
}

auto CommandManager::initialize() -> std::expected<void, std::string>
{
    const auto result = socket_server.start();
    if (result.has_value()) {
        stdin_thread = std::jthread([this] { wait_for_input_on_stdin(); });
        socket_thread = std::jthread([this] { wait_for_input_on_socket(); });
    } else {
        SPDLOG_DEBUG(result.error());
    }
    return result;
}

void CommandManager::wait_for_input_on_stdin()
{
    while (!Application::stop_flag) {
        auto in_event = os::wait_for_data_on_stdin(waitms);
        if (!in_event.has_value()) {
            SPDLOG_DEBUG(in_event.error());
            return;
        }
        if (!in_event.value()) {
            continue;
        }
        auto data = os::read_data_from_stdin();
        if (!data.has_value()) {
            SPDLOG_DEBUG(data.error());
            return;
        }

        // append new data to old data and search
        stdin_buffer.append(data.value());
        stdin_buffer = extract_commands(stdin_buffer);
    }
}

void CommandManager::wait_for_input_on_socket()
{
    while (!Application::stop_flag) {
        auto data = socket_server.read_data_from_connection();
        if (!data.has_value()) {
            SPDLOG_DEBUG(data.error());
            return;
        }

        for (const auto &cmd : data.value()) {
            extract_commands(cmd);
        }
    }
}

// parse any commands, return remaining data for next batches
// could turn into a memory hog if invalid data keeps coming in
auto CommandManager::extract_commands(std::string_view view) -> std::string
{
    while (true) {
        const auto find_result = view.find('\n');
        if (find_result == std::string_view::npos) {
            break;
        }

        auto substr = view.substr(0, find_result);
        try {
            const auto json = njson::parse(substr);
            const auto action = json.value("action", "");

            std::lock_guard lock{queue_mutex};
            if (action == "clear_queue") {
                command_queue = {};
            } else {
                SPDLOG_INFO("Received command {}.", substr);
                command_queue.emplace(json);
            }
        } catch (const njson::parse_error &) {
            SPDLOG_WARN("Received invalid json.");
        }
        view.remove_prefix(find_result + 1);
    }
    return {view.data(), view.length()};
}
