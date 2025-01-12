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
#include "command.hpp"

#include <memory>
#include <string_view>

namespace upp
{

class Canvas;

using CanvasPtr = std::unique_ptr<Canvas>;

class Canvas
{
  public:
    virtual ~Canvas() = default;

    static auto create(std::string_view output) -> Result<CanvasPtr>;
    virtual auto init() -> Result<void> = 0;
    virtual void execute(const Command& cmd) = 0;
};

} // namespace upp
