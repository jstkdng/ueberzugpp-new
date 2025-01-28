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

#include "x11/types.hpp"

namespace upp::xcb
{

window::window(connection_ptr connection, screen_ptr screen, window_id parent_id) :
    connection(connection),
    screen(screen),
    parent_id(parent_id),
    _id(xcb_generate_id(connection))
{
}

window::~window()
{
    xcb_destroy_window(connection, _id);
    xcb_flush(connection);
}

void window::configure(int xcoord, int ycoord, int width, int height)
{
    const uint32_t value_mask = XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_EVENT_MASK | XCB_CW_COLORMAP;
    struct xcb_create_window_value_list_t value_list;
    value_list.background_pixel = screen->black_pixel;
    value_list.border_pixel = screen->black_pixel;
    value_list.event_mask = XCB_EVENT_MASK_EXPOSURE;
    value_list.colormap = screen->default_colormap;
    xcb_create_window_aux(connection, screen->root_depth, _id, parent_id, static_cast<int16_t>(xcoord),
                          static_cast<int16_t>(ycoord), width, height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                          screen->root_visual, value_mask, &value_list);
    xcb_map_window(connection, _id);
    xcb_flush(connection);
}

auto window::id() const -> window_id
{
    return _id;
}

} // namespace upp::xcb
