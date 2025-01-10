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

#include <filesystem>
#include <string>
#include <string_view>

namespace upp::command
{

class Command
{
  public:
    auto init(std::string_view parser, std::string line) -> Result<void>;

    std::string action;
    std::string preview_id;
    std::filesystem::path image_path;
    std::string image_scaler;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;

  private:
    auto parse_json(std::string line) -> Result<void>;
};

class Listener
{
  private:
    void wait_for_input_on_stdin(const std::stop_token &token);

    std::jthread stdin_thread;
};

} // namespace upp::command
