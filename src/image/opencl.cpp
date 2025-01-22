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

#include "image/opencl.hpp"
#include "base/image.hpp"
#include "util/result.hpp"

#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <spdlog/spdlog.h>

#include <string>
#include <utility>

namespace upp
{

auto OpenclImage::can_load(const std::string &file_path) -> bool
{
    return cv::haveImageReader(file_path);
}

OpenclImage::OpenclImage(ImageProps props) :
    props(std::move(props))
{
}

auto OpenclImage::load() -> Result<void>
{
    return read_image().and_then([this] { return process_image(); });
}

auto OpenclImage::read_image() -> Result<void>
{
    try {
        image = cv::imread(props.file_path, cv::IMREAD_UNCHANGED).getUMat(cv::ACCESS_RW | cv::ACCESS_FAST);
        SPDLOG_DEBUG("loaded image {}", props.file_path);
    } catch (const cv::Exception &exc) {
        return Err("opencv", exc);
    }

    if (image.empty()) {
        return Err("unable to read image");
    }

    return {};
}

auto OpenclImage::process_image() -> Result<void>
{
    if (image.depth() == CV_16U) {
        const float alpha = 0.00390625; // 1 / 256
        image.convertTo(image, CV_8U, alpha);
    }

    /*
    if (image.channels() == 4) {
        // premultiply alpha
        image.forEach<cv::Vec4b>([](cv::Vec4b &pix, const int *) {
            const uint8_t alpha = pix[3];
            const uint8_t div = 255;
            pix[0] = (pix[0] * alpha) / div;
            pix[1] = (pix[1] * alpha) / div;
            pix[2] = (pix[2] * alpha) / div;
        });
    }*/

    if (image.channels() == 1) {
        cv::cvtColor(image, image, cv::COLOR_GRAY2BGRA);
    }

    return {};
}

} // namespace upp
