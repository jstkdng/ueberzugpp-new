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

#include "wayland/base.hpp"
#include "command/layer.hpp"
#include "exceptions.hpp"
#include "os/os.hpp"
#include "util/log.hpp"

#include <format>

namespace upp
{

constexpr wl_registry_listener registry_listener = {
    .global = WaylandBase::wl_registry_global,
    .global_remove = wl::ignore,
};

constexpr xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = WaylandBase::xdg_wm_base_ping,
};

void WaylandBase::wl_registry_global(void *data, [[maybe_unused]] wl_registry *registry, uint32_t name,
                                     const char *interface, uint32_t version)
{
    auto *base = static_cast<WaylandBase *>(data);
    base->globals.try_emplace(interface, name, version);
}

void WaylandBase::xdg_wm_base_ping([[maybe_unused]] void *data, xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

WaylandBase::WaylandBase() :
    display(wl_display_connect(nullptr))
{
    if (!display) {
        throw ex::wayland_error("could not connect to display");
    }
    display_fd = wl_display_get_fd(display.get());
    registry.reset(wl_display_get_registry(display.get()));
}

void WaylandBase::init()
{
    wl_registry_add_listener(registry.get(), &registry_listener, this);
    wl_display_roundtrip(display.get());
    handle_events();
    bind_base_protocols();
    bind_protocols(); // must be overriden in child classes
}

void WaylandBase::bind_base_protocols()
{
    const uint32_t compositor_ver = 6;
    const uint32_t shm_ver = 1;
    const uint32_t xdg_base_ver = 2;

    compositor.reset(bind_interface<wl_compositor>("wl_compositor", compositor_ver, &wl_compositor_interface));
    shm.reset(bind_interface<wl_shm>("wl_shm", shm_ver, &wl_shm_interface));
    wm_base.reset(bind_interface<xdg_wm_base>("xdg_wm_base", xdg_base_ver, &xdg_wm_base_interface));
    xdg_wm_base_add_listener(wm_base.get(), &xdg_wm_base_listener, nullptr);
}

auto WaylandBase::find_global(std::string_view name, uint32_t version) -> WaylandGlobal
{
    auto global = globals.find(name);
    if (global == globals.end()) {
        throw ex::wayland_error(std::format("global {} not found", name));
    }
    if (version > global->second.version) {
        throw ex::wayland_error(std::format("requested version {} for {} is not available", version, name));
    }
    return global->second;
}

void WaylandBase::handle_events()
{
    LOG_DEBUG("started event handler");

    event_handler = jthread([display_ptr = display.get(), display_fd = display_fd](SToken token) {
        while (!token.stop_requested()) {
            while (wl_display_prepare_read(display_ptr) != 0) {
                wl_display_dispatch_pending(display_ptr);
            }
            wl_display_flush(display_ptr);

            try {
                if (os::wait_for_data_on_fd(display_fd)) {
                    wl_display_read_events(display_ptr);
                    wl_display_dispatch_pending(display_ptr);
                } else {
                    wl_display_cancel_read(display_ptr);
                }
            } catch (const ex::posix_error &) {
                LayerCommand::terminate();
            }
        }
    });
}

} // namespace upp
