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

#include "preview/opencv_preview.hpp"

#include <opencv2/core/ocl.hpp>
#include <spdlog/spdlog.h>

OpencvPreview::OpencvPreview()
{
    if (config->ocl == opencl_status::UNCHECKED) {
        const auto *opencl_ctx = cv::ocl::Context::getDefault().ptr();
        if (opencl_ctx == nullptr) {
            SPDLOG_DEBUG("opencl is unavailable");
            config->ocl = opencl_status::UNAVAILABLE;
        } else {
            SPDLOG_DEBUG("opencl is available");
            config->ocl = opencl_status::AVAILABLE;
        }
    }
}

void OpencvPreview::draw()
{
}
