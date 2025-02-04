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

#include "log.hpp"
#include "wayland/types.hpp"

#include <string>

namespace upp
{

class WaylandWindow
{
  public:
    WaylandWindow(xdg_wm_base *wm_base, wl_compositor *compositor);

    static void preferred_buffer_scale(void *data, wl_surface *surface, int factor);
    static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial);

  private:
    Logger logger{spdlog::get("wayland")};
    wl::surface surface;
    wl::xdg::surface xdg_surface;
    wl::xdg::top_level xdg_toplevel;

    int scale_factor = 1;
    std::string app_id;
};

} // namespace upp
