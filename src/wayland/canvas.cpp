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

#include "wayland/canvas.hpp"
#include "application.hpp"
#include "command.hpp"
#include "util/result.hpp"
#include "wayland/socket/socket.hpp"
#include "wayland/types.hpp"

#include <spdlog/spdlog.h>

#include <cstdint>
#include <string_view>

namespace upp::wl
{

constexpr wl_registry_listener registry_listener = {.global = WaylandCanvas::wl_registry_global,
                                                    .global_remove = ignore};

constexpr xdg_wm_base_listener xdg_wm_base_listener = {.ping = WaylandCanvas::xdg_wm_base_ping};

void WaylandCanvas::wl_registry_global(void *data, wl_registry *registry, uint32_t name, const char *interface,
                                       uint32_t /*version*/)
{
    const std::string_view interface_str(interface);

    const uint32_t compositor_ver = 5;
    const uint32_t shm_ver = 1;
    const uint32_t xdg_base_ver = 2;

    auto *canvas = static_cast<WaylandCanvas *>(data);
    if (interface_str == wl_compositor_interface.name) {
        canvas->compositor.reset(
            static_cast<wl_compositor *>(wl_registry_bind(registry, name, &wl_compositor_interface, compositor_ver)));
    } else if (interface_str == wl_shm_interface.name) {
        canvas->shm.reset(static_cast<wl_shm *>(wl_registry_bind(registry, name, &wl_shm_interface, shm_ver)));
    } else if (interface_str == xdg_wm_base_interface.name) {
        canvas->wm_base.reset(
            static_cast<xdg_wm_base *>(wl_registry_bind(registry, name, &xdg_wm_base_interface, xdg_base_ver)));
        xdg_wm_base_add_listener(canvas->wm_base.get(), &xdg_wm_base_listener, nullptr);
    }
}

void WaylandCanvas::xdg_wm_base_ping(void * /*data*/, xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

auto WaylandCanvas::init(ApplicationContext *new_ctx) -> Result<void>
{
    ctx = new_ctx;
    display.reset(wl_display_connect(nullptr));
    if (display) {
        registry.reset(wl_display_get_registry(display.get()));
        wl_registry_add_listener(registry.get(), &registry_listener, this);
        wl_display_roundtrip(display.get());

        display_fd = wl_display_get_fd(display.get());
    } else {
        return Err("could not connect to wayland display");
    }

    SPDLOG_INFO("canvas created");
    auto sock = Socket::create();
    return {};
}

void WaylandCanvas::execute([[maybe_unused]] const Command &cmd)
{
    // do nothing for now
}

} // namespace upp::wl
