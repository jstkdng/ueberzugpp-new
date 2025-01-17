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

#include "x11/x11.hpp"
#include "command.hpp"
#include "util/result.hpp"

#include <spdlog/spdlog.h>

namespace upp
{

auto X11Canvas::init() -> Result<void>
{
    SPDLOG_INFO("canvas created");
    return {};
}

void X11Canvas::execute([[maybe_unused]] const Command &cmd)
{
}

} // namespace upp
