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

#include "exceptions.hpp"

#include <cerrno>
#include <format>

#include <vips/vips.h>

namespace upp::ex
{

posix_error::posix_error(std::string_view what) :
    std::system_error(errno, std::generic_category(), std::string(what))
{
}

posix_error::posix_error(int errc, std::string_view what) :
    std::system_error(errc, std::generic_category(), std::string(what))
{
}

vips_error::vips_error(std::string_view what) :
    std::runtime_error(""),
    vips_msg(vips_error_buffer_copy())
{
    std::string_view vips_msg_v{vips_msg.get()};
    if (vips_msg_v.empty()) {
        full_msg = what;
    } else {
        full_msg = std::format("{}: {}", what, vips_msg_v);
    }
}

wayland_error::wayland_error(std::string_view what) :
    std::runtime_error(std::string(what))
{
}

} // namespace upp::ex
