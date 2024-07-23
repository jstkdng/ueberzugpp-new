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

#include "canvas/geometry.hpp"

Geometry::Geometry(Terminal *terminal) :
    terminal(terminal)
{
}

void Geometry::update_command(Command *new_command)
{
    command = new_command;
}

auto Geometry::xpixels() const -> int
{
    return command->x * terminal->font_width;
}

auto Geometry::ypixels() const -> int
{
    return command->y * terminal->font_height;
}

auto Geometry::width_pixels() const -> int
{
    return command->width * terminal->font_width;
}

auto Geometry::height_pixels() const -> int
{
    return command->height * terminal->font_height;
}
