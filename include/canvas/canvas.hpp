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

#ifndef CANVAS_HPP
#define CANVAS_HPP

#include "command/command.hpp"
#include "config.hpp"
#include "terminal.hpp"

#include "moodycamel/blockingconcurrentqueue.h"

#include <expected>
#include <memory>

class Canvas
{
  public:
    static auto create(Config *config, const Terminal *terminal) -> std::expected<std::unique_ptr<Canvas>, std::string>;
    static void check_supported_canvas(Config *config, const Terminal *terminal);

    virtual auto initialize(moodycamel::BlockingConcurrentQueue<Command> *queue, Terminal *terminal)
        -> std::expected<void, std::string> = 0;
    virtual ~Canvas() = default;
};

#endif // CANVAS_HPP
