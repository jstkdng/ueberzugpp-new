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

#include "util/ptr.hpp"

// IWYU pragma: begin_exports
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-xdg-shell-client-protocol.h>
// IWYU pragma: end_exports

namespace upp::wl
{
using display = c_unique_ptr<wl_display, wl_display_disconnect>;
using registry = c_unique_ptr<wl_registry, wl_registry_destroy>;
using compositor = c_unique_ptr<wl_compositor, wl_compositor_destroy>;
using shm = c_unique_ptr<wl_shm, wl_shm_destroy>;

template <typename... Args>
void ignore([[maybe_unused]] Args... args) {};

namespace xdg
{
using wm_base = c_unique_ptr<xdg_wm_base, xdg_wm_base_destroy>;
}

} // namespace upp::wl
