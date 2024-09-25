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

#include "error.hpp"
#include "xcb_fwd.hpp"

namespace xcb
{

class connection
{
  public:
    ~connection();
    auto connect() -> Result<void>;
    auto create_window() -> window;

  private:
    xcb_connection_t *connection_ = nullptr;
    xcb_screen_t *screen_ = nullptr;
};

class window
{
    friend connection;

  private:
    xcb_window_t id_;
};

} // namespace xcb
