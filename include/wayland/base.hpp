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
#include "util/thread.hpp"
#include "wayland/types.hpp"

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
    int display_fd;
    string_map<WaylandGlobal> globals;
    jthread event_handler;

    void bind_base_protocols();
    void handle_events();
    auto find_global(std::string_view name, uint32_t version) -> WaylandGlobal;

  protected:
    wl::compositor compositor;
    wl::shm shm;
    xdg::wm_base wm_base;

    template <class T>
    auto bind_interface(std::string_view name, uint32_t version, const wl_interface *interface) -> T *
    {
        auto global = find_global(name, version);
        return static_cast<T *>(
            wl_registry_bind(registry.get(), global.name, interface, std::min(version, global.version)));
    }

    virtual void bind_protocols() {};
};

} // namespace upp
