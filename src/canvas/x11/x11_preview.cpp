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

#include "canvas/x11/x11_preview.hpp"

X11Preview::X11Preview(xcb_connection_t *connection, xcb_screen_t *screen, Terminal *terminal) :
    connection(connection),
    screen(screen),
    terminal(terminal)
{
}

auto X11Preview::initialize(Command cmd) -> std::expected<void, std::string>
{
    command = std::move(cmd);
    return Image::create(config.get(), command.image_path).and_then([this](auto ptr) {
        image = std::move(ptr);
        return image->initialize(terminal, &command);
    });
}
