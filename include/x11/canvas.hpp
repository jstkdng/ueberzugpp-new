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
#include "terminal.hpp"
#include "util/result.hpp"
#include "util/str_map.hpp"
#include "x11/window.hpp"

#include <expected>
#include <memory>
#include <mutex>
#include <stop_token>
#include <thread>

namespace upp
{

using WindowIdMap = string_map<std::shared_ptr<X11Window>>;

class X11Canvas final : public Canvas
{
  public:
    explicit X11Canvas(ApplicationContext *ctx);
    auto init() -> Result<void> override;
    void execute(const Command &cmd) override;

  private:
    ApplicationContext *ctx;
    Logger logger{spdlog::get("X11")};

    WindowMap window_map;
    WindowIdMap window_id_map;
    std::mutex window_mutex;
    std::jthread event_handler;

    void handle_events(const std::stop_token &token);
    void handle_expose_event(xcb_generic_event_t *event);
    void handle_add_command(const Command &cmd);
    void handle_remove_command(const Command &cmd);
    void dispatch_events();
};

} // namespace upp
