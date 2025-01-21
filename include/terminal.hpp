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
#include "unix/fd.hpp"
#include "util/result.hpp"

#include <string>

namespace upp
{

struct TerminalSize {
    int width = -1;
    int height = -1;
    int cols = -1;
    int rows = -1;
    int fallback_width = -1;
    int fallback_height = -1;
};

struct TerminalFont {
    int width = -1;
    int height = -1;
    int horizontal_padding = -1;
    int vertical_padding = -1;
};

class Terminal
{
  public:
    explicit Terminal(ApplicationContext *ctx);
    auto init() -> Result<void>;
    TerminalSize size;
    TerminalFont font;

  private:
    ApplicationContext *ctx;
    std::string pty_path;
    unix::fd pty_fd;
    int pid = -1;

    auto open_first_pty() -> Result<void>;
    auto set_terminal_size() -> Result<void>;
    auto set_font_size() -> Result<void>;
    void set_fallback_size_from_x11();
};

} // namespace upp
