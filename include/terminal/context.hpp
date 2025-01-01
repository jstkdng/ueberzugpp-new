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

#include <string>
#include <utility>

namespace upp::terminal
{

namespace geometry
{

struct x11 {
    std::pair<int, int> coords;
    int width;
    int height;
};

struct wayland {
    std::pair<int, int> coords;
    int width;
    int height;
};

struct iotctl {
    int cols;
    int rows;
    int xpixel;
    int ypixel;
};

}; // namespace geometry

class Geometry
{
  public:
    Geometry(int pty_fd, int pid);
};

class Context
{
  public:
    void open_first_pty();

  private:
    std::string pty_path;
    os::unix::fd pty_fd;
    int pid;
};

}; // namespace upp::terminal
