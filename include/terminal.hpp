// Display images inside a terminal
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

#include "error.hpp"
#include "os/fd.hpp"

struct TerminalSize {
    int fallback_xpixel = 0;
    int fallback_ypixel = 0;
    int xpixel = 0;
    int ypixel = 0;
    int rows = 0;
    int cols = 0;
};

struct TerminalFontSize {
    int width = 0;
    int height = 0;
    int horizontal_padding = 0;
    int vertical_padding = 0;
};

class Terminal
{
  public:
    auto init() -> Result<void>;

    TerminalSize size;
    TerminalFontSize font;

  private:
    std::string pty_path_;
    int pty_pid_;
    os::fd pty_fd_;

    auto set_font_sizes() -> Result<void>;
    auto open_first_pty() -> Result<void>;
    auto get_ioctl_sizes() -> Result<void>;
};
