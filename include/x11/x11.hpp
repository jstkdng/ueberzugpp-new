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
#include <utility>
#include <vector>

namespace upp
{

class X11Context
{
  public:
    auto init() -> Result<void>;

  private:
    xcb::connection connection;
    xcb::screen screen = nullptr;
    xcb::errors_context err_ctx;

    [[nodiscard]] auto get_window_ids() const -> std::vector<xcb::window_id>;
    [[nodiscard]] auto get_complete_window_ids() const -> std::vector<xcb::window_id>;
    [[nodiscard]] auto get_window_dimensions(xcb::window_id window) const -> std::pair<int, int>;

    void handle_xcb_error(xcb::error &err) const;
};

class X11Canvas final : public Canvas
{
  public:
    auto init() -> Result<void> override;
    void execute(const Command &cmd) override;
};

} // namespace upp
