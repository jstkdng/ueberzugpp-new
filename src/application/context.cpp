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
#include "cli.hpp"
#include "util/result.hpp"

#ifdef ENABLE_WAYLAND
#include "wayland/socket/socket.hpp"
#endif

#include <spdlog/spdlog.h>

#include <utility>

namespace upp
{

ApplicationContext::ApplicationContext(Cli *cli) :
    cli(cli),
    terminal(this)
{
}

auto ApplicationContext::init() -> Result<void>
{
    logger = spdlog::get("application");
    logger->info("TERM = {}", term);
    if (!term_program.empty()) {
        logger->info("TERM_PROGRAM = {}", term_program);
    }
    return x11_init()
        .and_then([this] { return wayland_init(); })
        .and_then([this] { return terminal.init(); })
        .and_then([this]() -> Result<void> {
            set_detected_output();
            return {};
        });
}

auto ApplicationContext::x11_init() -> Result<void>
{
#ifdef ENABLE_X11
    auto result = x11.init();
    if (!result) {
        logger->debug(result.error().message());
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
        logger->debug(result.error().message());
    }
#endif
    return {};
}

void ApplicationContext::set_detected_output()
{
    if (!cli->layer.output.empty()) {
        output = cli->layer.output;
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
