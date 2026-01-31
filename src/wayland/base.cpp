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
#include "exceptions.hpp"

namespace upp
{

constexpr wl_registry_listener registry_listener = {
    .global = WaylandBase::wl_registry_global,
    .global_remove = wl::ignore,
};

void WaylandBase::wl_registry_global(void *data, wl_registry *registry, uint32_t name, const char *interface,
                                     [[maybe_unused]] uint32_t version)
{
    auto *base = static_cast<WaylandBase*>(data);
}

WaylandBase::WaylandBase() :
    display(wl_display_connect(nullptr))
{
    if (!display) {
        throw ex::wayland_error("could not connect to display");
    }
    registry.reset(wl_display_get_registry(display.get()));
}

void WaylandBase::init()
{
    wl_registry_add_listener(registry.get(), &registry_listener, this);
    wl_display_roundtrip(display.get());
}

} // namespace upp
