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

#include "wayland/shm.hpp"

#include <sys/mman.h>

#include <cstring>

namespace upp
{

constexpr wl_buffer_listener buffer_listener = {
    .release = WaylandShm::wl_buffer_release,
};

void WaylandShm::wl_buffer_release(void * /*data*/, wl_buffer *buffer)
{
    wl_buffer_destroy(buffer);
}

WaylandShm::WaylandShm(wl::shm_ptr shm) :
    shm(shm)
{
}

WaylandShm::~WaylandShm()
{
    munmap(pool_data, pool_size);
}

auto WaylandShm::init(int new_width, int new_height, unsigned char *data) -> Result<void>
{
    width = new_width;
    height = new_height;
    stride = width * 4;
    const int image_size = height * stride;
    pool_size = image_size * 2;
    memfd = memfd_create("ueberzugpp-shm", 0);
    if (!memfd) {
        return Err("memfd_create");
    }
    if (ftruncate(memfd.get(), pool_size) == -1) {
        return Err("ftruncate");
    }
    auto *pool_ptr = mmap(nullptr, pool_size, PROT_READ | PROT_WRITE, MAP_SHARED, memfd.get(), 0);
    if (pool_ptr == MAP_FAILED) {
        return Err("mmap");
    }
    pool_data = static_cast<uint8_t *>(pool_ptr);
    std::memcpy(pool_data, data, image_size);
    return {};
}

auto WaylandShm::get_buffer() -> wl::buffer_ptr
{
    wl::shm_pool pool{wl_shm_create_pool(shm, memfd.get(), pool_size)};
    wl::buffer_ptr buffer = wl_shm_pool_create_buffer(pool.get(), 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);
    wl_buffer_add_listener(buffer, &buffer_listener, nullptr);
    return buffer;
}

} // namespace upp
