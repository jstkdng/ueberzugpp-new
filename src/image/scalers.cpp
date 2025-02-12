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

#include "image/scalers.hpp"

#include <algorithm>

namespace upp::image
{

auto fit_contain_sizes(const current_sizes sizes) -> target_sizes
{
    const auto factor = std::min(static_cast<float>(sizes.width) / static_cast<float>(sizes.image_width),
                                 static_cast<float>(sizes.height) / static_cast<float>(sizes.image_height));
    return {.width = static_cast<int>(static_cast<float>(sizes.image_width) * factor),
            .height = static_cast<int>(static_cast<float>(sizes.image_height) * factor)};
}

auto contain_sizes(const current_sizes sizes) -> target_sizes
{
    return fit_contain_sizes({
        .width = std::min(sizes.image_width, sizes.width),
        .height = std::min(sizes.image_height, sizes.height),
        .image_width = sizes.image_width,
        .image_height = sizes.image_height,
    });
}

} // namespace upp::image
