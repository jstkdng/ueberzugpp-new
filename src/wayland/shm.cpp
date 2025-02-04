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

namespace upp
{

constexpr int initial_pool_size = 256 * 1024 * 1024;

WaylandShmPool::WaylandShmPool() :
    pool_size(initial_pool_size)
{
}

WaylandShmPool::~WaylandShmPool()
{
    munmap(pool_data, pool_size);
}

auto WaylandShmPool::init(wl_shm *shm) -> Result<void>
{
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
    pool_data = static_cast<std::byte *>(pool_ptr);
    pool.reset(wl_shm_create_pool(shm, memfd.get(), pool_size));

    return {};
}

} // namespace upp
