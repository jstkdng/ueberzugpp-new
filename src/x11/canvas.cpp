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

#include "x11/x11.hpp"
#include "command.hpp"
#include "util/result.hpp"
#include "x11/types.hpp"

#include <spdlog/spdlog.h>

namespace upp
{

auto X11Canvas::init() -> Result<void>
{
    connection.reset(xcb_connect(nullptr, nullptr));
    if (xcb_connection_has_error(connection.get()) > 0) {
        return Err("can't connect to x11");
    }
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection.get())).data;

    SPDLOG_INFO("canvas created");
    return {};
}

void X11Canvas::execute([[maybe_unused]] const Command &cmd)
{
}

} // namespace upp
