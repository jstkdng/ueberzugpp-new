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

#include "preview/preview.hpp"
#include "preview/vips_preview.hpp"

#ifdef ENABLE_OPENCV
#  include "preview/opencv_preview.hpp"
#  include <opencv2/imgcodecs.hpp>
#endif

#include <vips/vips.h>

auto Preview::create(const Config *config, const nlohmann::json &command)
    -> std::expected<std::unique_ptr<Preview>, std::string>
{
    const std::string &path = command.value("path", "");
    if (path.empty()) {
        return std::unexpected("file path not found in command");
    }

#ifdef ENABLE_OPENCV
    if (cv::haveImageReader(path) && !config->no_opencv) {
        return std::make_unique<OpencvPreview>();
    }
#endif

    const auto *vips_loader = vips_foreign_find_load(path.c_str());
    if (vips_loader != nullptr) {
        return std::make_unique<VipsPreview>();
    }
    return std::unexpected("no suitable preview backend found");
}
