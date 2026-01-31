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

#include "wayland/types.hpp"

namespace upp
{

class WaylandBase
{
  public:
    WaylandBase();

    static void wl_registry_global(void *data, wl_registry *registry, uint32_t name, const char *interface,
                                   uint32_t version);
    void init();

  protected:
    wl::display display;
    wl::registry registry;
};

} // namespace upp
