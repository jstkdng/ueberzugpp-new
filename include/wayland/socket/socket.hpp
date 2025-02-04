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

#include <memory>
#include <string_view>

namespace upp
{

struct WaylandGeometry
{
    int width = -1;
    int height = -1;
    int x = -1;
    int y = -1;
};

class WaylandSocket;

using WaylandSocketPtr = std::unique_ptr<WaylandSocket>;

class WaylandSocket
{
  public:
    virtual ~WaylandSocket() = default;
    static auto create() -> Result<WaylandSocketPtr>;
    virtual auto setup(std::string_view app_id, int xcoord, int ycoord) -> Result<void> = 0;
    virtual auto active_window() -> WaylandGeometry = 0;
};

} // namespace upp
