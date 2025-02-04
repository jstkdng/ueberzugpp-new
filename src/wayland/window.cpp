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

namespace upp
{

constexpr wl_surface_listener surface_listener = {.enter = wl::ignore,
                                                  .leave = wl::ignore,
                                                  .preferred_buffer_scale = WaylandWindow::preferred_buffer_scale,
                                                  .preferred_buffer_transform = wl::ignore};

void WaylandWindow::preferred_buffer_scale(void *data, [[maybe_unused]] wl_surface *surface, int factor)
{
    auto *window = reinterpret_cast<WaylandWindow *>(data);
    window->scale_factor = factor;
        window->logger->debug(factor);
}

WaylandWindow::WaylandWindow(xdg_wm_base *wm_base, wl_compositor *compositor) :
    surface(wl_compositor_create_surface(compositor)),
    xdg_surface(xdg_wm_base_get_xdg_surface(wm_base, surface.get())),
    xdg_toplevel(xdg_surface_get_toplevel(xdg_surface.get()))
{
}

void WaylandWindow::finish_init()
{
    wl_surface_add_listener(surface.get(), &surface_listener, this);
}

} // namespace upp
