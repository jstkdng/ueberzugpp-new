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

#include "util/concurrent_deque.hpp"
#include "util/result.hpp"
#include "util/thread.hpp"
#include "util/util.hpp"

#include <moodycamel/blockingconcurrentqueue.h>

#include <filesystem>
#include <format>
#include <string>
#include <string_view>
#include <variant>

namespace upp
{

struct Command {
    static auto create(std::string_view parser, std::string line) -> Result<Command>;
    static auto from_json(std::string line) -> Result<Command>;

    [[nodiscard]] auto x() const -> int { return util::variant_to_int(x_); }
    [[nodiscard]] auto y() const -> int { return util::variant_to_int(y_); }
    [[nodiscard]] auto width() const -> int { return util::variant_to_int(width_); }
    [[nodiscard]] auto height() const -> int { return util::variant_to_int(height_); }

    std::string action;
    std::string preview_id;
    std::string image_scaler = "contain";
    std::filesystem::path image_path;

    std::variant<int, std::string> x_;
    std::variant<int, std::string> y_;
    std::variant<int, std::string> width_;
    std::variant<int, std::string> height_;
};

using CommandQueue = ConcurrentDeque<Command>;

class CommandListener
{
  public:
    explicit CommandListener(CommandQueue *queue);
    auto start(std::string_view new_parser) -> Result<void>;

  private:
    void wait_for_input_on_stdin(const std::stop_token &token);
    void extract_commands(std::string &line);
    void flush_command_queue() const;
    void enqueue_or_discard(const Command &cmd);

    CommandQueue *queue;
    std::string parser;
    std::jthread stdin_thread;
    std::string stdin_buffer;
};

} // namespace upp

template <>
struct std::formatter<upp::Command> : std::formatter<std::string> {
    static auto format(const upp::Command &cmd, format_context &ctx)
    {
        if (cmd.action == "exit" || cmd.action == "flush") {
            return std::format_to(ctx.out(), "Command[action={}]", cmd.action);
        }

        if (cmd.action == "remove") {
            return std::format_to(ctx.out(), "Command[action={} identifier={}]", cmd.action, cmd.preview_id);
        }

        return std::format_to(ctx.out(), "Command[action={} identifier={} path={}]", cmd.action, cmd.preview_id,
                              cmd.image_path.string());
    }
};
