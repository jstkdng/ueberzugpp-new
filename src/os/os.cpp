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

#include "os/os.hpp"

#include <unistd.h>

#include <cerrno>
#include <string>
#include <system_error>

namespace upp::os
{

auto getpid() -> int
{
    return ::getpid();
}

auto strerror() -> std::string
{
    const std::error_code code(errno, std::generic_category());
    return code.message();
}

} // namespace upp::os
