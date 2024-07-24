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

#include "image/opencv_image.hpp"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <vips/vips8>

enum class exif_orientation : uint8_t {
    EXIF_ORIENTATION_2 = 2,
    EXIF_ORIENTATION_3,
    EXIF_ORIENTATION_4,
    EXIF_ORIENTATION_5,
    EXIF_ORIENTATION_6,
    EXIF_ORIENTATION_7,
    EXIF_ORIENTATION_8,
};

auto OpencvImage::data() -> unsigned char *
{
    return image.data;
}

auto OpencvImage::initialize(Terminal *term, Command *cmd) -> std::expected<void, std::string>
{
    terminal = term;
    command = cmd;
    return read_image().and_then([this] { return rotate_image(); });
}

void OpencvImage::check_cache()
{
}

auto OpencvImage::read_image() -> std::expected<void, std::string>
{
    using cv::imread;

    image = imread(command->image_path, cv::IMREAD_UNCHANGED);
    if (image.empty()) {
        return std::unexpected("could not read image");
    }
    return {};
}

auto OpencvImage::rotate_image() -> std::expected<void, std::string>
{
    using cv::flip;
    using enum exif_orientation;

    const auto vipsimg = vips::VImage::new_from_file(command->image_path.c_str());
    int orientation{};
    try {
        orientation = vipsimg.get_int("orientation");
    } catch (const vips::VError &) {
        return std::unexpected("could not get image orientation");
    }

    // kudos https://jdhao.github.io/2019/07/31/image_rotation_exif_info/
    switch (static_cast<exif_orientation>(orientation)) {
        case EXIF_ORIENTATION_2:
            flip(image, image, 1);
            break;
        case EXIF_ORIENTATION_3:
            flip(image, image, -1);
            break;
        case EXIF_ORIENTATION_4:
            flip(image, image, 0);
            break;
        case EXIF_ORIENTATION_5:
            rotate(image, image, cv::ROTATE_90_CLOCKWISE);
            flip(image, image, 1);
            break;
        case EXIF_ORIENTATION_6:
            rotate(image, image, cv::ROTATE_90_CLOCKWISE);
            break;
        case EXIF_ORIENTATION_7:
            rotate(image, image, cv::ROTATE_90_COUNTERCLOCKWISE);
            flip(image, image, 1);
            break;
        case EXIF_ORIENTATION_8:
            rotate(image, image, cv::ROTATE_90_COUNTERCLOCKWISE);
            break;
        default:
            break;
    }
    return {};
}

auto OpencvImage::resize_image() -> std::expected<void, std::string>
{
    return {};
}
