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

#include "preview/vips_preview.hpp"

VipsPreview::VipsPreview()
{
    if (!config->vips_initialized) {
        if (VIPS_INIT("ueberzugpp")) {
            vips_error_exit(nullptr);
        }
        vips_cache_set_max(1);
        config->vips_initialized = true;
    }
}

void VipsPreview::draw()
{
}
