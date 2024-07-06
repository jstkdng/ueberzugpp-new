// Display images inside a terminal
// Copyright (C) 2023  JustKidding
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "canvas.hpp"
#ifdef ENABLE_X11
#  include "canvas/x11_canvas.hpp"
#endif

auto Canvas::create([[maybe_unused]] const Config *config) -> std::expected<std::unique_ptr<Canvas>, std::string>
{
#ifdef ENABLE_X11
    if (config->output == "x11") {
        return std::make_unique<X11Canvas>();
    }
#endif

    return std::unexpected("could not create canvas");
}
