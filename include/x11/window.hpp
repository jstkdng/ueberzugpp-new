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
#include "terminal.hpp"
#include "x11/types.hpp"

#include <memory>
#include <unordered_map>

namespace upp
{

class X11Window;

using WindowPtr = std::shared_ptr<X11Window>;
using WindowMap = std::unordered_map<xcb::window_id, X11Window>;
using WindowIdMap = std::unordered_map<std::string, X11Window>;

class X11Window : std::enable_shared_from_this<X11Window>
{
  public:
    X11Window(Terminal *terminal, ApplicationContext *ctx, WindowMap* window_map);

  private:
    Terminal *terminal;
    ApplicationContext *ctx;
    WindowMap *window_map;
};

} // namespace upp
