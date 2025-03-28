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

namespace upp::image
{

struct current_sizes {
    int width;
    int height;
    int image_width;
    int image_height;
};

struct target_sizes {
    int width;
    int height;
};

auto fit_contain_sizes(current_sizes sizes) -> target_sizes;
auto contain_sizes(current_sizes sizes) -> target_sizes;

} // namespace upp::image
