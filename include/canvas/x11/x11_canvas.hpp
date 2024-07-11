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

#ifndef X11_CANVAS_HPP
#define X11_CANVAS_HPP

#include "canvas/canvas.hpp"
#include "config.hpp"
#include "util/str_map.hpp"
#include "x11_window.hpp"

#include <thread>
#include <unordered_map>

#ifndef HAVE_STD_JTHREAD
#  include "jthread/jthread.hpp"
#endif

#include <xcb/xcb.h>
#ifdef ENABLE_XCB_ERRORS
#  include <xcb/xcb_errors.h>
#endif

class X11Canvas final : public Canvas
{
  public:
    ~X11Canvas() override;
    auto initialize(CommandManager *manager) -> std::expected<void, std::string> override;

  private:
    std::shared_ptr<Config> config = Config::instance();
    CommandManager *command_manager = nullptr;
    xcb_connection_t *connection = nullptr;
    xcb_screen_t *screen = nullptr;

#ifdef ENABLE_XCB_ERRORS
    xcb_errors_context_t *err_ctx = nullptr;
#endif

    std::jthread event_handler;
    std::jthread command_reader;

    string_map<std::shared_ptr<X11Window>> windows;

    auto connect_to_x11() -> std::expected<void, std::string>;
    void handle_events(const std::stop_token &token) const;
    void read_commands(const std::stop_token &token);
    void print_xcb_error(xcb_generic_error_t *err) const;
};

#endif // X11_CANVAS_HPP
