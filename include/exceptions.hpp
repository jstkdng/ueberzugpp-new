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

#include "util/ptr.hpp"

#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace upp::ex
{

class posix_error : public std::system_error
{
  public:
    explicit posix_error(std::string_view what = "");
    explicit posix_error(int errc, std::string_view what = "");
};

class vips_error : public std::runtime_error
{
  public:
    explicit vips_error(std::string_view what = "");

    [[nodiscard]] auto what() const noexcept -> const char * override { return full_msg.c_str(); }

  private:
    unique_C_ptr<char> vips_msg;
    std::string full_msg;
};

class wayland_error : public std::runtime_error
{
  public:
    explicit wayland_error(std::string_view what = "");
};

} // namespace upp::ex
