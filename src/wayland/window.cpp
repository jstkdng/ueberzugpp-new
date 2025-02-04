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

#include <glib.h>

#include <format>

namespace upp
{

constexpr wl_surface_listener surface_listener = {
    .enter = wl::ignore,
    .leave = wl::ignore,
    .preferred_buffer_scale = WaylandWindow::preferred_buffer_scale,
    .preferred_buffer_transform = wl::ignore,
};

constexpr xdg_surface_listener xdg_surface_listener = {
    .configure = WaylandWindow::xdg_surface_configure,
};

constexpr int id_len = 10;

void WaylandWindow::xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial)
{
    xdg_surface_ack_configure(xdg_surface, serial);
    auto *window = reinterpret_cast<WaylandWindow *>(data);
    auto &image = window->image;
    auto *buffer = window->shm.get_buffer(image->data(), image->data_size());
    auto *surface = window->surface.get();
    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_set_buffer_scale(surface, window->scale_factor);
    wl_surface_commit(surface);
}

void WaylandWindow::preferred_buffer_scale(void *data, wl_surface * /*surface*/, int factor)
{
    auto *window = reinterpret_cast<WaylandWindow *>(data);
    window->scale_factor = factor;
}

WaylandWindow::WaylandWindow(ApplicationContext *ctx, wl_compositor *compositor, wl_shm *shm, xdg_wm_base *wm_base) :
    ctx(ctx),
    shm(shm),
    surface(wl_compositor_create_surface(compositor)),
    xdg_surface(xdg_wm_base_get_xdg_surface(wm_base, surface.get())),
    xdg_toplevel(xdg_surface_get_toplevel(xdg_surface.get())),
    app_id(std::format("ueberzugpp_{}", crypto::generate_random_string(id_len)))
{
    xdg_toplevel_set_app_id(xdg_toplevel.get(), app_id.c_str());
    xdg_toplevel_set_title(xdg_toplevel.get(), app_id.c_str());
    wl_surface_add_listener(surface.get(), &surface_listener, this);
    xdg_surface_add_listener(xdg_surface.get(), &xdg_surface_listener, this);
}

auto WaylandWindow::socket_setup(const Command &command) -> Result<void>
{
    auto &pos = ctx->terminal.position;
    auto &font = ctx->terminal.font;
    int xcoord = pos.x + (font.width * command.x) + font.horizontal_padding;
    int ycoord = pos.y + (font.height * command.y) + font.vertical_padding;
    return ctx->wl_socket->setup(app_id, xcoord, ycoord);
}

auto WaylandWindow::init(const Command &command) -> Result<void>
{
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
            return shm.init(image->width(), image->height());
        })
        .and_then([this, &command] { return socket_setup(command); })
        .and_then([this]() -> Result<void> {
            wl_surface_commit(surface.get());
            return {};
        });
}

} // namespace upp
