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

#include "application/context.hpp"
#include "base/image.hpp"
#include "command/command.hpp"
#include "log.hpp"
#include "wayland/shm.hpp"
#include "wayland/types.hpp"

#include <string>

namespace upp
{

class WaylandCanvas;

class WaylandWindow
{
  public:
    WaylandWindow(ApplicationContext *ctx, wl_compositor *compositor, wl_shm *shm, xdg_wm_base *wm_base);
    auto init(const Command &command) -> Result<void>;

    static void preferred_buffer_scale(void *data, wl_surface *surface, int factor);
    static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial);

  private:
    Logger logger{spdlog::get("wayland")};
    ApplicationContext *ctx;
    WaylandShm shm;
    wl::surface surface;
    wl::xdg::surface xdg_surface;
    wl::xdg::top_level xdg_toplevel;
    std::string app_id;
    ImagePtr image;

    auto socket_setup(const Command &command) -> Result<void>;
};

} // namespace upp
