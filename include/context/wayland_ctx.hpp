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

#pragma once

#include <cstdint>
#include <string>

#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-xdg-shell-client-protocol.h>

#include "util/ptr.hpp"

struct WlContext {
    WlContext();

    std::string compositor_name;
    bool is_valid = false;
    int display_fd = -1;

    c_unique_ptr<wl_display, wl_display_disconnect> w_display;
    c_unique_ptr<wl_registry, wl_registry_destroy> w_registry;
    c_unique_ptr<wl_compositor, wl_compositor_destroy> w_compositor;
    c_unique_ptr<wl_shm, wl_shm_destroy> w_shm;
    c_unique_ptr<xdg_wm_base, xdg_wm_base_destroy> w_wm_base;

    static void wl_registry_global(void *data, wl_registry *registry, uint32_t name, const char *interface,
                                   uint32_t version);
    static void xdg_wm_base_ping(void *data, xdg_wm_base *xdg_wm_base, uint32_t serial);
};
