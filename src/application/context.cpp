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

auto ApplicationContext::init(std::string_view cli_output) -> Result<void>
{
    logger = spdlog::get("application");
    LOG_INFO("TERM = {}", term);
    if (!term_program.empty()) {
        LOG_INFO("TERM_PROGRAM = {}", term_program);
    }
    return x11_init()
        .and_then([this] { return wayland_init(); })
        .and_then([this] { return terminal.init(); })
        .and_then([this, cli_output]() -> Result<void> {
            set_detected_output(cli_output);
            is_initialized = true;
            return {};
        });
}

auto ApplicationContext::x11_init() -> Result<void>
{
#ifdef ENABLE_X11
    auto result = x11.init();
    if (!result) {
        LOG_DEBUG(result.error().message());
    }
#endif
    return {};
}

auto ApplicationContext::wayland_init() -> Result<void>
{
#ifdef ENABLE_WAYLAND
    if (auto result = WaylandSocket::create()) {
        wl_socket = std::move(*result);
    } else {
        LOG_DEBUG(result.error().message());
    }
#endif
    return {};
}

void ApplicationContext::set_detected_output(std::string_view cli_output)
{
    if (!cli_output.empty()) {
        output = cli_output;
        return;
    }
#ifdef ENABLE_X11
    if (x11.is_valid) {
        output = "x11";
    }
#endif
#ifdef ENABLE_WAYLAND
    if (wl_socket) {
        output = "wayland";
    }
#endif
    // TODO: detect other stdout outputs
}

} // namespace upp
