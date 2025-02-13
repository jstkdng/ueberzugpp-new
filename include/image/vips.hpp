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

#include "application/context.hpp"
#include "log.hpp"
#include "util/ptr.hpp"
#include "util/result.hpp"

#include <vips/vips.h>

#include <optional>
#include <span>
#include <string>

namespace upp
{

template <typename T>
using glib_ptr = c_unique_ptr<T, g_free>;

struct ImageProps {
    std::string file_path;
    std::string scaler;
    int width = -1;
    int height = -1;
};

class LibvipsImage
{
  public:
    explicit LibvipsImage(ApplicationContext *ctx);
    ~LibvipsImage();
    auto load(ImageProps props) -> Result<void>;
    auto num_channels() -> int;
    auto data() -> unsigned char *;
    auto data_size() -> int;
    auto width() -> int;
    auto height() -> int;

  private:
    Logger logger;
    ApplicationContext *ctx;
    ImageProps props;
    VipsImage *image;
    VipsImage *image_out;
    glib_ptr<unsigned char> image_buffer;

    auto read_image() -> Result<void>;
    void resize_image();
    void process_image();
    void contain_scaler();
    auto image_is_cached(int new_width, int new_height) -> bool;
    [[nodiscard]] auto origin_is_animated() const -> bool;
    auto get_frame_delays() -> std::optional<std::span<int>>;
};

} // namespace upp
