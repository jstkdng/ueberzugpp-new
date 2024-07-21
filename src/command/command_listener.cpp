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

#include "command/command_listener.hpp"

#include "application.hpp"
#include "os/os.hpp"

#include <spdlog/spdlog.h>

auto CommandListener::initialize(moodycamel::BlockingConcurrentQueue<Command> *queue)
    -> std::expected<void, std::string>
{
    command_queue = queue;
    return socket_server.start().and_then([this] -> std::expected<void, std::string> {
        socket_thread = std::jthread([this](auto token) { wait_for_input_on_socket(token); });
        if (!config->no_stdin) {
            stdin_thread = std::jthread([this](auto token) { wait_for_input_on_stdin(token); });
        }
        return {};
    });
}

void CommandListener::wait_for_input_on_socket(const std::stop_token &token)
{
    SPDLOG_INFO("listening for commands on socket");
    while (!token.stop_requested()) {
        auto data = socket_server.read_data_from_connection();
        if (!data.has_value()) {
            SPDLOG_DEBUG(data.error());
            return;
        }

        for (const auto &cmd : data.value()) {
            std::ignore = extract_commands(cmd);
        }
    }
}

void CommandListener::wait_for_input_on_stdin(const std::stop_token &token)
{
    SPDLOG_INFO("listening for commands on stdin");
    while (!token.stop_requested()) {
        auto in_event = os::wait_for_data_on_stdin(config->waitms);
        if (!in_event.has_value()) {
            SPDLOG_TRACE("stdin thread terminated: {}", in_event.error());
            Application::terminate(); // stop this program if this thread dies
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

auto CommandListener::extract_commands(std::string_view view) const -> std::string
{
    while (true) {
        const auto find_result = view.find('\n');
        if (find_result == std::string_view::npos) {
            break;
        }

        const auto substr = view.substr(0, find_result);
        auto cmd = Command::create(config->parser, substr);
        if (cmd) {
            if (cmd->action == "exit") {
                Application::terminate();
            } else if (cmd->action == "flush") {
                flush_command_queue();
            } else {
                command_queue->enqueue(*cmd);
            }
        }

        view.remove_prefix(find_result + 1);
    }
    return {view.data(), view.length()};
}

void CommandListener::flush_command_queue() const
{
    Command ignore;
    while (command_queue->try_dequeue(ignore)) {
        // no need to process the commands
    }
}
