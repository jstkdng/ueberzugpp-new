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

#include "util/str_map.hpp"
#include "wayland/types.hpp"

#include <any>

namespace upp
{

struct WaylandGlobal {
    uint32_t name;
    uint32_t version;
};

class WaylandBase
{
  public:
    WaylandBase();
    virtual ~WaylandBase() = default;

    static void wl_registry_global(void *data, wl_registry *registry, uint32_t name, const char *interface,
                                   uint32_t version);
    static void xdg_wm_base_ping(void *data, xdg_wm_base *xdg_wm_base, uint32_t serial);

    void init();

  private:
    wl::display display;
    wl::registry registry;
    string_map<WaylandGlobal> globals;

    void bind_base_protocols();

  protected:
    wl::compositor compositor;
    wl::shm shm;
    xdg::wm_base wm_base;

    auto bind_interface(const char *name, uint32_t version, const wl_interface *interface) const -> std::any;
    virtual void bind_protocols() {};
};

} // namespace upp
