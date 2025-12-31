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

#include "os/unix.hpp"

#include <memory>
#include <mutex>

namespace upp
{

struct TerminalPosition {
    int x = 0;
    int y = 0;
};

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
    Terminal();
    auto operator=(Terminal &&) -> Terminal & = delete;

    static auto get() -> std::shared_ptr<Terminal>
    {
        struct enabler : Terminal {
        };
        static auto ptr = std::make_shared<enabler>();
        return ptr;
    }

    TerminalSize size;
    TerminalFont font;
    TerminalPosition position;

    void load_state();

  private:
    std::string pty_path;
    unix::fd pty_fd;
    int pty_pid = -1;
    std::mutex state_mutex;

    void open_first_pty();
    void set_terminal_size();
    void set_font_size();
};

using TermPtr = std::shared_ptr<Terminal>;

} // namespace upp
