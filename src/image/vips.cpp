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

#include "image/vips.hpp"
#include "base/image.hpp"
#include "util/result.hpp"

#include <spdlog/spdlog.h>
#include <vips/vips8>

#include <utility>

namespace upp
{

VipsImage::VipsImage(ImageProps props) :
    props(std::move(props))
{
}

auto VipsImage::can_load(const std::string &file_path) -> bool
{
    return vips_foreign_find_load(file_path.c_str()) != nullptr;
}

auto VipsImage::load() -> Result<void>
{
    return read_image();
}

auto VipsImage::read_image() -> Result<void>
{
    try {
        image = vips::VImage::new_from_file(props.file_path.c_str()).colourspace(VIPS_INTERPRETATION_sRGB);
        SPDLOG_INFO("loaded image {}", props.file_path);
    } catch (const vips::VError &err) {
        return Err("failed to load image");
    }

    return {};
}

} // namespace upp
