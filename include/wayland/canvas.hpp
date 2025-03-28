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

#include "application/context.hpp"
#include "base/canvas.hpp"
#include "command/command.hpp"
#include "log.hpp"
#include "util/result.hpp"
#include "util/str_map.hpp"
#include "util/thread.hpp"
#include "wayland/types.hpp"
#include "wayland/window.hpp"

#include <cstdint>

namespace upp
{

class WaylandCanvas final : public Canvas
{
  public:
    explicit WaylandCanvas(ApplicationContext *ctx);
    auto init() -> Result<void> override;
    void execute(const Command &cmd) override;

    static void wl_registry_global(void *data, wl_registry *registry, uint32_t name, const char *interface,
                                   uint32_t version);
    static void xdg_wm_base_ping(void *data, xdg_wm_base *xdg_wm_base, uint32_t serial);

  private:
    ApplicationContext *ctx;
    Logger logger{spdlog::get("wayland")};
    wl::display display;
    wl::registry registry;
    wl::compositor compositor;
    wl::shm shm;
    wl::xdg::wm_base wm_base;

    string_map<std::shared_ptr<WaylandWindow>> window_map;
    WindowPtrs window_ptrs;

    jthread event_handler;

    void handle_events(SToken token);

    int display_fd = -1;
};

} // namespace upp
