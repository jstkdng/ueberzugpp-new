
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

    int x;
    int y;
    int width;
    int height;

    float scaling_position_x;
    float scaling_position_y;
};

using CommandQueue = ConcurrentDeque<Command>;

} // namespace upp
