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

#include "base/canvas.hpp"
#include "command.hpp"
#include "util/result.hpp"
#include "x11/types.hpp"

#include <expected>
#include <unordered_map>
#include <vector>

namespace upp
{

class ApplicationContext;

struct X11Geometry {
    int width = -1;
    int height = -1;
};

class X11Context
{
  public:
    auto init() -> Result<void>;
    auto load_state(int pid) -> Result<void>;
    static constexpr int num_clients = 256;

    xcb::connection connection;
    xcb::screen screen = nullptr;
    xcb::window_id parent = -1;
    X11Geometry parent_geometry;
    int connection_fd = -1;
    bool is_xwayland = false;
    bool is_valid = false;

  private:
    xcb::errors_context err_ctx;
    std::unordered_multimap<int, xcb::window_id> pid_window_map;

    void set_pid_window_map();
    auto set_parent_window(int pid) -> Result<void>;
    auto set_parent_window_geometry() -> Result<void>;

    void handle_xcb_error(xcb::error &err) const;

    [[nodiscard]] auto get_window_ids() const -> std::vector<xcb::window_id>;
    [[nodiscard]] auto get_complete_window_ids() const -> std::vector<xcb::window_id>;
};

class X11Canvas final : public Canvas
{
  public:
    auto init(ApplicationContext *new_ctx) -> Result<void> override;
    void execute(const Command &cmd) override;

  private:
    ApplicationContext *ctx;
};

} // namespace upp
