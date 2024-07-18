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

#include "image/image.hpp"
#include "image/libvips_image.hpp"
#include "util/util.hpp"

#ifdef ENABLE_OPENCV
#  include "image/opencv_image.hpp"
#  include <opencv2/imgcodecs.hpp>
#endif

#include <vips/vips.h>

auto Image::create(const Config *config, const std::string &file_path)
    -> std::expected<std::unique_ptr<Image>, std::string>
{
#ifdef ENABLE_OPENCV
    if (!config->no_opencv && cv::haveImageReader(file_path)) {
        return std::make_unique<OpencvImage>();
    }
#endif

    const auto *vips_loader = vips_foreign_find_load(file_path.c_str());
    if (vips_loader != nullptr) {
        return std::make_unique<LibvipsImage>();
    }
    return util::unexpected_err("no suitable preview backend found");
}
