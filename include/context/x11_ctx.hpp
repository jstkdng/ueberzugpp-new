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

#include <xcb/xproto.h>
#include <xcb/xcb.h>

struct X11Context {
    X11Context();
    ~X11Context();

    bool is_valid = false;
    bool is_xwayland = false;

    xcb_connection_t *connection = nullptr;
    xcb_screen_t *screen = nullptr;
    const xcb_setup_t *setup = nullptr;
};
