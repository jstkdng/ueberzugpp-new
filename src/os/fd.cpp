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

#include <utility>

#include <unistd.h>

#include "os/fd.hpp"

namespace os
{

fd::fd(int descriptor) :
    descriptor(descriptor)
{
}

fd::~fd()
{
    if (descriptor > 0) {
        close(descriptor);
    }
}

fd::fd(const fd &other) :
    descriptor(dup(other.descriptor))
{
}

fd::fd(fd &&other) noexcept
{
    descriptor = other.descriptor;
    other.descriptor = -1;
}

auto fd::operator=(const fd &other) -> fd &
{
    if (this == &other) {
        return *this;
    }
    fd temp(other);
    std::swap(descriptor, temp.descriptor);
    return *this;
}

auto fd::operator=(fd &&other) noexcept -> fd &
{
    fd temp(std::move(other));
    std::swap(descriptor, temp.descriptor);
    return *this;
}

auto fd::operator=(int new_fd) -> fd&
{
    fd temp(new_fd);
    std::swap(descriptor, temp.descriptor);
    return *this;
}

auto fd::operator*() const -> int
{
    return descriptor;
}

} // namespace os
