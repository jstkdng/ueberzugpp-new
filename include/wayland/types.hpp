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

struct display_deleter {
    void operator()(wl_display *ptr) const { wl_display_disconnect(ptr); }
};

struct registry_deleter {
    void operator()(wl_registry *ptr) const { wl_registry_destroy(ptr); }
};

struct compositor_deleter {
    void operator()(wl_compositor *ptr) const { wl_compositor_destroy(ptr); }
};

struct shm_deleter {
    void operator()(wl_shm *ptr) const { wl_shm_destroy(ptr); }
};

using display = std::unique_ptr<wl_display, display_deleter>;
using registry = std::unique_ptr<wl_registry, registry_deleter>;
using compositor = std::unique_ptr<wl_compositor, compositor_deleter>;
using shm = std::unique_ptr<wl_shm, shm_deleter>;

template <typename... Args>
void ignore([[maybe_unused]] Args... args) { /* ignore wayland callback */ };

namespace xdg
{

struct xdg_deleter {
    void operator()(xdg_wm_base *ptr) const { xdg_wm_base_destroy(ptr); }
};

using wm_base = std::unique_ptr<xdg_wm_base, xdg_deleter>;
} // namespace xdg

} // namespace upp::wl
