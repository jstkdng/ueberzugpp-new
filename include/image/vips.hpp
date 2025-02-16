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

#include <vips/basic.h>

#include <optional>
#include <span>
#include <string>

namespace upp
{

using VImage = ::VipsImage;

class VipsImage
{
  public:
    ~VipsImage();
    auto read(const std::string &image_path) -> Result<void>;
    void scale_image(int target_width, int target_height, std::string_view scaler);

    auto width() -> int;
    auto height() -> int;

  private:
    Logger logger{spdlog::get("vips")};
    std::string m_image_path;
    VImage *m_image;
    VImage *m_image_out;

    void contain_scaler(int target_width, int target_height);
};

/*
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
    auto load(ImageProps props) -> Result<void>;
    auto num_channels() -> int;
    auto data() -> unsigned char *;
    auto data_size() -> int;

  private:
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
};*/

} // namespace upp
