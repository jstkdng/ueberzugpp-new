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

#include "x11_worker.hpp"

#include <spdlog/spdlog.h>

X11Worker::X11Worker(xcb_connection_t *connection, xcb_screen_t *screen)
    : connection(connection),
      screen(screen)
{
}

X11Worker::~X11Worker()
{
    xcb_destroy_window(connection, window_id);
    xcb_free_gc(connection, gc);
    xcb_flush(connection);
}

void X11Worker::initialize()
{
    constexpr uint32_t value_mask = XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_EVENT_MASK | XCB_CW_COLORMAP;
    window_id = xcb_generate_id(connection);
    xcb_create_window_value_list_t value_list{};
    value_list.background_pixel = screen->black_pixel;
    value_list.border_pixel = screen->black_pixel;
    value_list.event_mask = XCB_EVENT_MASK_EXPOSURE;
    value_list.colormap = screen->default_colormap;

    xcb_create_window_aux(connection, screen->root_depth, window_id, screen->root, 0, 0, 0, 0, 0,
                          XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, value_mask, &value_list);
    xcb_create_gc(connection, gc, window_id, 0, nullptr);
    SPDLOG_DEBUG("created window with id {}", window_id);
}

auto X11Worker::get_internal_id() const -> std::size_t
{
    return window_id;
}

auto X11Worker::get_string_id() const -> std::string
{
    return image_id;
}
