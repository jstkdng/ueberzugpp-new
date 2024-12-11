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

#include <initializer_list>
#include <vector>

#include <xcb/xproto.h>

#include "error.hpp"
#include "util/passkey.hpp"

namespace xcb
{

class connection;
class window;
class gc;

class connection
{
  public:
    ~connection();
    auto connect() -> Result<void>;
    auto create_window() -> window;
    auto create_gc() -> gc;
    [[nodiscard]] auto get_server_window_ids() const -> std::vector<xcb_window_t>;
    [[nodiscard]] auto window_has_properties(xcb_window_t window,
                                             std::initializer_list<xcb_atom_t> properties) const -> bool;

  private:
    xcb_connection_t *connection_ = nullptr;
    const xcb_setup_t *setup_ = nullptr;
    xcb_screen_t *screen_ = nullptr;
};

class window
{
  public:
    window(util::PassKey<connection> key, xcb_connection_t *connection, xcb_screen_t *screen, xcb_window_t wid,
           xcb_window_t parent);
    ~window();

  private:
    xcb_connection_t *connection_;
    [[maybe_unused]] xcb_screen_t *screen_;
    xcb_window_t id_;
    [[maybe_unused]] xcb_window_t parent_;
};

class gc
{
  public:
    gc();
    ~gc();

  private:
    xcb_gc_t id_;
};

} // namespace xcb
