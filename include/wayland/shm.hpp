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

#include "unix/fd.hpp"
#include "util/result.hpp"
#include "wayland/types.hpp"

namespace upp
{

class WaylandShm
{
  public:
    explicit WaylandShm(wl::shm_ptr shm);
    ~WaylandShm();
    auto init(int new_width, int new_height) -> Result<void>;
    auto get_buffer(unsigned char* data, int data_size) -> wl::buffer_ptr;

    static void wl_buffer_release(void *data, wl_buffer *buffer);

  private:
    wl::shm_ptr shm;
    unix::fd memfd;

    uint8_t *pool_data = nullptr;
    int pool_size = 0;
    int width = 0;
    int height = 0;
    int stride = 0;
};

} // namespace upp
