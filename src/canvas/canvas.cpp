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

#include "canvas/canvas.hpp"
#include "util/util.hpp"
#ifdef ENABLE_X11
#  include "canvas/x11/x11_canvas.hpp"
#endif

auto Canvas::create([[maybe_unused]] const Config *config) -> std::expected<std::unique_ptr<Canvas>, std::string>
{
#ifdef ENABLE_X11
    if (config->output == "x11") {
        return std::make_unique<X11Canvas>();
    }
#endif

    return util::unexpected_err("could not create canvas");
}
