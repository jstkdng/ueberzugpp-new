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

#pragma once

#include <wayland-client.h>
#include <wayland-xdg-shell-client-protocol.h>

#include <memory>

namespace upp::wl
{

struct deleter {
    void operator()(wl_display *ptr) const { wl_display_disconnect(ptr); }
    void operator()(wl_registry *ptr) const { wl_registry_destroy(ptr); }
    void operator()(wl_compositor *ptr) const { wl_compositor_destroy(ptr); }
    void operator()(wl_shm *ptr) const { wl_shm_destroy(ptr); }
    void operator()(wl_shm_pool *ptr) const { wl_shm_pool_destroy(ptr); }
    void operator()(wl_surface *ptr) const { wl_surface_destroy(ptr); }

    void operator()(xdg_wm_base *ptr) const { xdg_wm_base_destroy(ptr); }
    void operator()(xdg_surface *ptr) const { xdg_surface_destroy(ptr); }
    void operator()(xdg_toplevel *ptr) const { xdg_toplevel_destroy(ptr); }
};

using display = std::unique_ptr<wl_display, deleter>;
using registry = std::unique_ptr<wl_registry, deleter>;
using compositor = std::unique_ptr<wl_compositor, deleter>;
using shm = std::unique_ptr<wl_shm, deleter>;
using shm_pool = std::unique_ptr<wl_shm_pool, deleter>;
using surface = std::unique_ptr<wl_surface, deleter>;
using buffer_ptr = wl_buffer *;
using shm_ptr = wl_shm *;

template <typename... Args>
void ignore([[maybe_unused]] Args... args) { /* ignore wayland callback */ };

namespace xdg
{

using wm_base = std::unique_ptr<xdg_wm_base, deleter>;
using surface = std::unique_ptr<xdg_surface, deleter>;
using top_level = std::unique_ptr<xdg_toplevel, deleter>;

} // namespace xdg

} // namespace upp::wl
