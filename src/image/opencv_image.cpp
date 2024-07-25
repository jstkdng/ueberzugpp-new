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
#include <spdlog/spdlog.h>

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

auto OpencvImage::data() const -> unsigned char *
{
    return image.data;
}

auto OpencvImage::height() const -> int
{
    return image.rows;
}

auto OpencvImage::width() const -> int
{
    return image.cols;
}

auto OpencvImage::initialize(Terminal *term, Command *cmd) -> std::expected<void, std::string>
{
    terminal = term;
    command = cmd;
    cmd_width = command->width * terminal->font_width;
    cmd_height = command->height * terminal->font_height;
    return read_image()
        .and_then([this] -> std::expected<void, std::string> {
            rotate_image();
            return {};
        })
        .and_then([this] { return resize_image(); });
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

void OpencvImage::rotate_image()
{
    using cv::flip;
    using enum exif_orientation;

    const auto vipsimg = vips::VImage::new_from_file(command->image_path.c_str());
    int orientation{};
    try {
        orientation = vipsimg.get_int("orientation");
    } catch (const vips::VError &) {
        return;
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
}

auto OpencvImage::resize_image() -> std::expected<void, std::string>
{
    target_image_sizes target;
    const current_image_sizes cur{
        .width = cmd_width, .height = cmd_height, .image_width = image.cols, .image_height = image.rows};
    if (command->image_scaler == "contain") {
        target = contain_sizes(cur);
    } else if (command->image_scaler == "fit_contain") {
        target = fit_contain_sizes(cur);
    }

    SPDLOG_DEBUG("width: {}, height: {}", target.width, target.height);
    cv::resize(image, image, cv::Size(target.width, target.height), 0, 0, cv::INTER_AREA);
    return {};
}
