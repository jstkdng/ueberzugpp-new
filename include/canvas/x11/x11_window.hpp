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

#ifndef X11_WINDOW_HPP
#define X11_WINDOW_HPP

#include <memory>
#include <string>
#include <xcb/xcb.h>

class X11Window : public std::enable_shared_from_this<X11Window>
{
  public:
    X11Window(xcb_connection_t *connection, xcb_screen_t *screen);

    std::string image_id;

  private:
    xcb_connection_t *connection = nullptr;
    xcb_screen_t *screen = nullptr;
    xcb_window_t window = 0;
};

#endif // X11_WINDOW_HPP
