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

window::window(connection_ptr connection, screen_ptr screen, window_id parent_id, gcontext gctx) :
    connection(connection),
    screen(screen),
    parent_id(parent_id),
    gctx(gctx),
    _id(xcb_generate_id(connection))
{
    const uint32_t value_mask = XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_EVENT_MASK | XCB_CW_COLORMAP;
    struct xcb_create_window_value_list_t value_list;
    value_list.background_pixel = screen->black_pixel;
    value_list.border_pixel = screen->black_pixel;
    value_list.event_mask = XCB_EVENT_MASK_EXPOSURE;
    value_list.colormap = screen->default_colormap;
    xcb_create_window_aux(connection, screen->root_depth, _id, parent_id, 0, 0, 0, 0, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                          screen->root_visual, value_mask, &value_list);
}

window::~window()
{
    xcb_destroy_window(connection, _id);
    xcb_flush(connection);
}

void window::configure(int xcoord, int ycoord, int width, int height)
{
    const uint32_t mask =
        XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
    xcb_configure_window_value_list_t values;
    values.x = xcoord;
    values.y = ycoord;
    values.width = width;
    values.height = height;
    xcb_configure_window_aux(connection, _id, mask, &values);
}

} // namespace upp::xcb
