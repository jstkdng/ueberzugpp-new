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
#include "base/image.hpp"
#include "command/command.hpp"
#include "x11/types.hpp"

#include <memory>
#include <unordered_map>

namespace upp
{

class X11Window;

using WindowPtr = std::weak_ptr<X11Window>;
using WindowMap = std::unordered_map<xcb::window_id, WindowPtr>;

class X11Window : public std::enable_shared_from_this<X11Window>
{
  public:
    X11Window(ApplicationContext *ctx, WindowMap *window_map);
    auto init(const Command &command) -> Result<void>;
    void draw(xcb::window_id window);

  private:
    ApplicationContext *ctx;
    WindowMap *window_map;

    ImagePtr image;
    xcb::image xcb_image;
    xcb::window xcb_window;
};

} // namespace upp
