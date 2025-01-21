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

#include "application/context.hpp"
#include "util/result.hpp"

#ifdef ENABLE_WAYLAND
#include "wayland/socket/socket.hpp"
#endif

#include <spdlog/spdlog.h>

#include <utility>

namespace upp
{

auto ApplicationContext::init() -> Result<void>
{
    return x11_init().and_then([this] { return wayland_init(); });
}

auto ApplicationContext::x11_init() -> Result<void>
{
#ifdef ENABLE_X11
    auto result = x11.init();
    if (!result) {
        SPDLOG_DEBUG(result.error().message());
    }
#endif
    return {};
}

auto ApplicationContext::wayland_init() -> Result<void>
{
#ifdef ENABLE_WAYLAND
    auto result = WaylandSocket::create().and_then([this](WaylandSocketPtr socket) -> Result<void> {
        wl_socket = std::move(socket);
        return {};
    });
    if (!result) {
        SPDLOG_DEBUG(result.error().message());
    }
#endif
    return {};
}

} // namespace upp
