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
#include "log.hpp"
#include "util/result.hpp"

#include <vips/vips.h>

#include <string>

namespace upp
{

class LibvipsImage : public Image
{
  public:
    explicit LibvipsImage(std::string output);
    ~LibvipsImage() override;
    auto load(ImageProps props) -> Result<void> override;
    static auto can_load(const std::string &file_path) -> bool;
    auto num_channels() -> int override;
    auto data() -> unsigned char * override;
    auto data_size() -> int override;
    auto width() -> int override;
    auto height() -> int override;

  private:
    Logger logger{spdlog::get("vips")};
    std::string output;
    ImageProps props;
    VipsImage *image;
    VipsImage *image_out;

    auto read_image() -> Result<void>;
    void resize_image();
    void process_image();
    void contain_scaler();
};

} // namespace upp
