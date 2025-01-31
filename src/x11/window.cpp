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

#include "x11/window.hpp"

namespace upp
{

X11Window::X11Window(ApplicationContext *ctx, WindowMap *window_map) :
    ctx(ctx),
    window_map(window_map),
    xcb_window(ctx->x11.connection.get(), ctx->x11.screen, ctx->x11.parent)
{
}

auto X11Window::init(const Command &command) -> Result<void>
{
    auto &font = ctx->terminal.font;
    auto &x11 = ctx->x11;
    return Image::create(ctx, command.image_path.string())
        .and_then([this, &font, &command](ImagePtr new_image) {
            image = std::move(new_image);
            return image->load({.file_path = command.image_path.string(),
                                .scaler = command.image_scaler,
                                .width = font.width * command.width,
                                .height = font.height * command.height});
        })
        .and_then([this, &font, &x11, &command]() -> Result<void> {
            window_map->emplace(xcb_window.id(), weak_from_this());
            xcb_image.reset(xcb_image_create_native(x11.connection.get(), image->width(), image->height(),
                                                    XCB_IMAGE_FORMAT_Z_PIXMAP, x11.screen->root_depth, nullptr, 0,
                                                    nullptr));
            xcb_window.configure((font.width * command.x) + font.horizontal_padding,
                                 (font.height * command.y) + font.vertical_padding, image->width(), image->height());
            return {};
        });
}

void X11Window::draw(xcb::window_id window)
{
    xcb_image->data = image->data();
    xcb_image_put(ctx->x11.connection.get(), window, ctx->x11.gcontext, xcb_image.get(), 0, 0, 0);
}

} // namespace upp
