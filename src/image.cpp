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

#include "base/image.hpp"
#include "buildconfig.hpp" // IWYU pragma: keep
#include "util/result.hpp"
#ifdef ENABLE_LIBVIPS
#include "image/vips.hpp"
#endif

#include <memory>
#include <utility>

namespace upp
{

auto Image::create([[maybe_unused]] std::string output, const std::string &file_path) -> Result<ImagePtr>
{
#ifdef ENABLE_LIBVIPS
    if (LibvipsImage::can_load(file_path)) {
        return std::make_unique<LibvipsImage>(std::move(output));
    }
#endif

    return Err("can't load image " + file_path, 0);
}

auto Image::fit_contain_sizes(const current_image_sizes sizes) -> target_image_sizes
{
    const auto factor = std::min(static_cast<float>(sizes.width) / static_cast<float>(sizes.image_width),
                                 static_cast<float>(sizes.height) / static_cast<float>(sizes.image_height));
    return {.width = static_cast<int>(static_cast<float>(sizes.image_width) * factor),
            .height = static_cast<int>(static_cast<float>(sizes.image_height) * factor)};
}

auto Image::contain_sizes(const current_image_sizes sizes) -> target_image_sizes
{
    return fit_contain_sizes({
        .width = std::min(sizes.image_width, sizes.width),
        .height = std::min(sizes.image_height, sizes.height),
        .image_width = sizes.image_width,
        .image_height = sizes.image_height,
    });
}

} // namespace upp
