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

#include "wayland/window.hpp"
#include "util/crypto.hpp"
#include "util/ptr.hpp"
#include "wayland/canvas.hpp"

#include <format>
#include <glib.h>

namespace upp
{

constexpr wl_surface_listener surface_listener = {.enter = wl::ignore,
                                                  .leave = wl::ignore,
                                                  .preferred_buffer_scale = WaylandWindow::preferred_buffer_scale,
                                                  .preferred_buffer_transform = wl::ignore};

constexpr struct xdg_surface_listener xdg_surface_listener = {.configure = WaylandWindow::xdg_surface_configure};

constexpr int id_len = 10;

void WaylandWindow::xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial)
{
    xdg_surface_ack_configure(xdg_surface, serial);
    auto *window = reinterpret_cast<WaylandWindow *>(data);
    auto &image = window->image;
    c_unique_ptr<unsigned char, g_free> image_ptr{image->data()};
    if (auto buffer = window->canvas->create_buffer(image->width(), image->height(), image_ptr.get())) {
        wl_surface_attach(window->surface.get(), *buffer, 0, 0);
        wl_surface_commit(window->surface.get());
    } else {
        window->logger->debug(buffer.error().message());
    }
}

void WaylandWindow::preferred_buffer_scale(void *data, wl_surface * /*surface*/, int factor)
{
    auto *window = reinterpret_cast<WaylandWindow *>(data);
    wl_surface_set_buffer_scale(window->surface.get(), factor);
    window->logger->debug("factor changed! {}", factor);
}

WaylandWindow::WaylandWindow(WaylandCanvas *canvas) :
    canvas(canvas),
    surface(wl_compositor_create_surface(canvas->compositor.get())),
    xdg_surface(xdg_wm_base_get_xdg_surface(canvas->wm_base.get(), surface.get())),
    xdg_toplevel(xdg_surface_get_toplevel(xdg_surface.get())),
    app_id(std::format("ueberzugpp_{}", crypto::generate_random_string(id_len)))
{
    xdg_toplevel_set_app_id(xdg_toplevel.get(), app_id.c_str());
    xdg_toplevel_set_title(xdg_toplevel.get(), app_id.c_str());
    wl_surface_add_listener(surface.get(), &surface_listener, this);
    xdg_surface_add_listener(xdg_surface.get(), &xdg_surface_listener, this);
}

auto WaylandWindow::init(const Command &command) -> Result<void>
{
    auto *ctx = canvas->ctx;
    auto &font = ctx->terminal.font;
    return Image::create(ctx->output, command.image_path.string())
        .and_then([this, &font, &command](ImagePtr new_image) {
            image = std::move(new_image);
            return image->load({.file_path = command.image_path.string(),
                                .scaler = command.image_scaler,
                                .width = font.width * command.width,
                                .height = font.height * command.height});
        })
        .and_then([this]() -> Result<void> {
            wl_surface_commit(surface.get());
            return {};
        });
}

} // namespace upp
