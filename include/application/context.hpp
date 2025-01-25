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

#include "buildconfig.hpp"
#include "cli.hpp"
#include "util/result.hpp"

#ifdef ENABLE_X11
#include "x11/context.hpp"
#endif

#ifdef ENABLE_WAYLAND
#include "wayland/socket/socket.hpp"
#endif

#include <string>

namespace upp
{

class ApplicationContext
{
  public:
    explicit ApplicationContext(Cli *cli);
    auto init() -> Result<void>;

    Cli *cli;
    std::string term;
    std::string term_program;
    std::string output;
#ifdef ENABLE_X11
    X11Context x11;
#endif
#ifdef ENABLE_WAYLAND
    WaylandSocketPtr wl_socket;
#endif

  private:
    auto x11_init() -> Result<void>;
    auto wayland_init() -> Result<void>;
    void set_detected_output();
};

} // namespace upp
