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

#include "wayland/socket/socket.hpp"
#include "os/os.hpp"
#include "util/result.hpp"
#include "wayland/socket/hyprland.hpp"
#include "wayland/socket/dummy.hpp"

#include <memory>

namespace upp
{

auto WaylandSocket::create() -> Result<WaylandSocketPtr>
{
    if (auto hyprland_signature = os::getenv("HYPRLAND_INSTANCE_SIGNATURE")) {
        return std::make_unique<HyprlandSocket>(*hyprland_signature);
    }

    return std::make_unique<DummySocket>();
}

} // namespace upp
