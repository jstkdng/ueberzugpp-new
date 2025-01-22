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
#include "image/vips.hpp"
#include "util/result.hpp"
#ifdef ENABLE_OPENCV
#include "image/opencl.hpp"
#endif

#include <memory>
#include <utility>

namespace upp
{

auto Image::create(ImageProps props) -> Result<ImagePtr>
{
#ifdef ENABLE_OPENCV
    if (OpenclImage::can_load(props.file_path)) {
        return std::make_unique<OpenclImage>(std::move(props));
    }
#endif
    if (VipsImage::can_load(props.file_path)) {
        return std::make_unique<VipsImage>(std::move(props));
    }

    return Err("could not create image loader");
}

} // namespace upp
