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

#include "util/result.hpp"
#include "util/thread.hpp"

#include <glaze/glaze.hpp>
#include <moodycamel/blockingconcurrentqueue.h>

#include <filesystem>
#include <string>
#include <string_view>

namespace upp
{

struct Command {
    static auto create(std::string_view parser, std::string line) -> Result<Command>;
    static auto from_json(std::string line) -> Result<Command>;

    std::string action;
    std::string preview_id;
    std::string image_scaler = "contain";
    std::filesystem::path image_path;

    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

using CommandQueue = moodycamel::BlockingConcurrentQueue<Command>;

class CommandListener
{
  public:
    explicit CommandListener(CommandQueue *queue);
    auto start(std::string_view parser) -> Result<void>;

  private:
    void wait_for_input_on_stdin(const std::stop_token &token);

    void extract_commands(std::string_view line);

    CommandQueue *queue;
    std::string parser;
    std::jthread stdin_thread;
    std::string stdin_buffer;
};

} // namespace upp

template <>
struct glz::meta<upp::Command> {
    using T = upp::Command;
    // NOLINTNEXTLINE
    static constexpr auto value = object(
        // clang-format off
        &T::action,
        "identifier", &T::preview_id,
        "scaler", &T::image_scaler,
        "path", &T::image_path,
        "x", glz::quoted_num<&T::x>,
        "y", glz::quoted_num<&T::y>,
        "width", glz::quoted_num<&T::width>,
        "height", glz::quoted_num<&T::height>,
        "max_width", glz::quoted_num<&T::width>,
        "max_height", glz::quoted_num<&T::height>
        // clang-format on
    );
};
