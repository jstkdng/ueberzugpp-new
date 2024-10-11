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

#pragma once

#include <string_view>

#include <moodycamel/blockingconcurrentqueue.h>

#include "error.hpp"

struct Command {
    static auto create(std::string_view parser, std::string_view line) -> Result<Command>;
    static auto from_json(std::string_view line) -> Result<Command>;

    std::string action;
    std::string preview_id;
    std::string image_path;
    std::string image_scaler;
    int x = -1;
    int y = -1;
    int width = -1;
    int height = -1;
};

using CommandQueue = moodycamel::BlockingConcurrentQueue<Command>;
