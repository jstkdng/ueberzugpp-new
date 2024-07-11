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

#include "command.hpp"
#include "application.hpp"
#include "os/os.hpp"

#include <chrono>
#include <format>
#include <string>
#include <string_view>
#include <thread>

#include <spdlog/spdlog.h>
#include <sys/poll.h>

using njson = nlohmann::json;

auto CommandManager::initialize() -> std::expected<void, std::string>
{
    const auto result = socket_server.start();
    if (result.has_value()) {
        SPDLOG_INFO("listening for commands on socket");
        socket_thread = std::jthread([this](auto token) { wait_for_input_on_socket(token); });
        if (!config->no_stdin) {
            SPDLOG_INFO("listening for commands on stdin");
            stdin_thread = std::jthread([this](auto token) { wait_for_input_on_stdin(token); });
        }
    } else {
        SPDLOG_DEBUG(result.error());
    }
    return result;
}

void CommandManager::wait_for_input_on_stdin(const std::stop_token &token)
{
    while (!token.stop_requested()) {
        auto in_event = os::wait_for_data_on_stdin(config->waitms);
        if (!in_event.has_value()) {
            SPDLOG_DEBUG(std::format("stdin thread terminated: {}", in_event.error()));
            Application::stop_flag = true; // stop this program if this thread dies
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

void CommandManager::wait_for_input_on_socket(const std::stop_token &token)
{
    while (!token.stop_requested()) {
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

auto CommandManager::unqueue() -> std::expected<njson, std::string>
{
    std::unique_lock lock{queue_mutex};
    const bool command_available =
        cond.wait_for(lock, std::chrono::milliseconds(config->waitms), [this] { return !command_queue.empty(); });
    if (!command_available) {
        return std::unexpected("no command available");
    }
    auto value = command_queue.front();
    command_queue.pop();
    return value;
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

            std::unique_lock lock{queue_mutex};
            if (action == "flush") {
                SPDLOG_DEBUG("flushing command queue");
                command_queue = {};
            } else if (action == "exit") {
                Application::stop_flag = true;
            } else {
                command_queue.emplace(json);
            }
            lock.unlock();
            cond.notify_one(); // there's only one consumer
        } catch (const njson::parse_error &) {
            SPDLOG_WARN("Received invalid json.");
        }
        view.remove_prefix(find_result + 1);
    }
    return {view.data(), view.length()};
}
