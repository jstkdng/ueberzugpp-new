// Display images inside a terminal
// Copyright (C) 2023  JustKidding
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef X11_WORKER_HPP
#define X11_WORKER_HPP

#include "canvas/canvas_worker.hpp"

#include <xcb/xcb.h>

class X11Worker final : public CanvasWorker
{
  public:
    X11Worker(xcb_connection_t *connection, xcb_screen_t *screen);
    void initialize() override;
    [[nodiscard]] auto get_string_id() const -> std::string override;
    [[nodiscard]] auto get_internal_id() const -> std::size_t override;
    ~X11Worker() override;

  private:
    std::string image_id;
    xcb_connection_t *connection = nullptr;
    xcb_screen_t *screen = nullptr;

    xcb_window_t window_id = 0;
    xcb_gcontext_t gc = 0;
};

#endif // X11_WORKER_HPP
