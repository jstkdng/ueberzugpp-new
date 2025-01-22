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

#pragma once

#include "base/image.hpp"
#include "util/result.hpp"

#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>

#include <string>

namespace upp
{

class OpenclImage : public Image
{
  public:
    explicit OpenclImage(ImageProps props);
    auto load() -> Result<void> override;
    static auto can_load(const std::string &file_path) -> bool;

  private:
    ImageProps props;
    cv::UMat image;

    auto read_image() -> Result<void>;
    auto process_image() -> Result<void>;
};

} // namespace upp
