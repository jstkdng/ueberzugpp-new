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
#include "canvas/x11/x11_canvas.hpp"
#endif

#include <string_view>

auto Canvas::create(Config *config, const Terminal *terminal) -> std::expected<std::unique_ptr<Canvas>, std::string>
{
    check_supported_canvas(config, terminal);
#ifdef ENABLE_X11
    if (config->output == "x11") {
        return std::make_unique<X11Canvas>();
    }
#endif

    return util::unexpected_err("could not create canvas");
}

void Canvas::check_supported_canvas(Config *config, const Terminal *terminal)
{
    if (!config->output.empty()) {
        return;
    }

    std::string_view detected_canvas = "chafa";

    if (terminal->supports_sixel) {
        detected_canvas = "sixel";
    }

    if (terminal->supports_kitty) {
        detected_canvas = "kitty";
    }

#ifdef ENABLE_X11
    if (X11Canvas::supported()) {
        detected_canvas = "x11";
    }
#endif
    config->output = detected_canvas;
}
