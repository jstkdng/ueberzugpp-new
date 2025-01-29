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

#include "application/application.hpp"
#include "command.hpp"
#include "os/os.hpp"
#include "util/result.hpp"
#include "util/thread.hpp"

#include <format>
#include <string>
#include <string_view>

namespace upp
{

CommandListener::CommandListener(CommandQueue *queue) :
    queue(queue)
{
}

auto CommandListener::start(std::string_view new_parser) -> Result<void>
{
    logger = spdlog::get("listener");
    parser = new_parser;
    logger->info("using {} parser", parser);
    stdin_thread = std::jthread([this](const auto &token) { wait_for_input_on_stdin(token); });
    return {};
}

void CommandListener::wait_for_input_on_stdin(const std::stop_token &token)
{
    logger->info("listening for commands on stdin");
    while (!token.stop_requested()) {
        auto in_event = os::wait_for_data_on_stdin();
        if (!in_event) {
            logger->warn(std::format("stdin thread terminated: {}", in_event.error().lmessage()));
            Application::terminate(); // stop this program if this thread dies
            return;
        }
        if (!in_event.value()) {
            continue;
        }
        auto data = os::read_data_from_stdin();
        if (!data) {
            logger->debug(data.error().lmessage());
            return;
        }
        // append new data to old data and search
        stdin_buffer.append(data.value());
        extract_commands(data.value());
    }
}

void CommandListener::extract_commands(std::string &line)
{
    while (true) {
        const auto find_result = line.find('\n');
        if (find_result == std::string::npos) {
            break;
        }

        auto cmd = Command::create(parser, line.substr(0, find_result));
        if (cmd) {
            if (cmd->action == "exit") {
                Application::terminate();
            } else if (cmd->action == "flush") {
                flush_command_queue();
            } else {
                enqueue_or_discard(*cmd);
            }
        } else {
            logger->error(cmd.error().message());
        }

        line.erase(0, find_result + 1);
    }
};

void CommandListener::flush_command_queue() const
{
    logger->debug("flushing command queue");
    queue->clear();
}

void CommandListener::enqueue_or_discard(const Command &cmd)
{
    queue->enqueue(cmd, [this, &cmd](auto &deque) {
        if (deque.size() <= 2 || cmd.action != "remove") {
            return true;
        }
        if (auto last = deque.back(); last.action == "add" && last.preview_id == cmd.preview_id) {
            logger->debug("discarding add/remove command pair");
            deque.pop_back();
            return false;
        }
        return true;
    });
}

} // namespace upp
