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
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <spdlog/spdlog.h>

#include <string>

namespace upp
{

auto OpenclImage::init(ImageProps props) -> Result<void>
{
    try {
        image = cv::imread(std::string{props.file_path}, cv::IMREAD_UNCHANGED).getUMat(cv::ACCESS_RW | cv::ACCESS_FAST);
        SPDLOG_DEBUG("loaded image {}", props.file_path);
    } catch (const cv::Exception &exc) {
        return Err("opencv", exc);
    }
    return {};
}

} // namespace upp
