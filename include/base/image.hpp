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

#include "util/result.hpp"

#include <memory>
#include <string>

namespace upp
{

class Image;

using ImagePtr = std::unique_ptr<Image>;

struct ImageProps {
    std::string file_path;
    std::string scaler;
    int width = -1;
    int height = -1;
};

struct current_image_sizes {
    int width;
    int height;
    int image_width;
    int image_height;
};

struct target_image_sizes {
    int width;
    int height;
};

class Image
{
  public:
    virtual ~Image() = default;

    static auto create(std::string output, const std::string& file_path) -> Result<ImagePtr>;
    static auto fit_contain_sizes(current_image_sizes sizes) -> target_image_sizes;
    static auto contain_sizes(current_image_sizes sizes) -> target_image_sizes;
    virtual auto load(ImageProps props) -> Result<void> = 0;
    virtual auto num_channels() -> int = 0;
    virtual auto data_size() -> int = 0;
    virtual auto data() -> unsigned char* = 0;
    virtual auto width() -> int = 0;
    virtual auto height() -> int = 0;
};

} // namespace upp
