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

#include "context/wayland_ctx.hpp"
#include "os/os.hpp"

#include <spdlog/spdlog.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-xdg-shell-client-protocol.h>

#include <cstdint>
#include <string_view>

constexpr wl_registry_listener registry_listener = {.global = WlContext::wl_registry_global,
                                                    .global_remove = [](auto...) { /*unused*/ }};

constexpr xdg_wm_base_listener xdg_wm_base_listener = {.ping = WlContext::xdg_wm_base_ping};

WlContext::WlContext()
{
    w_display.reset(wl_display_connect(nullptr));
    if (w_display != nullptr) {
        w_registry.reset(wl_display_get_registry(w_display.get()));
        wl_registry_add_listener(w_registry.get(), &registry_listener, this);
        wl_display_roundtrip(w_display.get());

        display_fd = wl_display_get_fd(w_display.get());
        auto fd_pid = os::get_pid_from_socket(display_fd);
        if (fd_pid) {
            compositor_name = os::get_pid_process_name(*fd_pid);
            is_valid = true;
        }
    }
}

void WlContext::wl_registry_global(void *data, wl_registry *registry, uint32_t name, const char *interface,
                                   uint32_t /*version*/)
{
    const std::string_view interface_str(interface);

    const uint32_t compositor_ver = 5;
    const uint32_t shm_ver = 1;
    const uint32_t xdg_base_ver = 2;

    auto *ctx = static_cast<WlContext *>(data);
    if (interface_str == wl_compositor_interface.name) {
        ctx->w_compositor.reset(
            static_cast<wl_compositor *>(wl_registry_bind(registry, name, &wl_compositor_interface, compositor_ver)));
    } else if (interface_str == wl_shm_interface.name) {
        ctx->w_shm.reset(static_cast<wl_shm *>(wl_registry_bind(registry, name, &wl_shm_interface, shm_ver)));
    } else if (interface_str == xdg_wm_base_interface.name) {
        ctx->w_wm_base.reset(
            static_cast<xdg_wm_base *>(wl_registry_bind(registry, name, &xdg_wm_base_interface, xdg_base_ver)));
        xdg_wm_base_add_listener(ctx->w_wm_base.get(), &xdg_wm_base_listener, nullptr);
    }
}

void WlContext::xdg_wm_base_ping(void * /*data*/, xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}
