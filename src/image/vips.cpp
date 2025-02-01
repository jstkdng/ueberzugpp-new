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

#include "image/vips.hpp"
#include "base/image.hpp"
#include "util/result.hpp"

#include <spdlog/spdlog.h>
#include <vips/vips8>

#include <unordered_set>
#include <utility>

namespace upp
{

VipsImage::VipsImage(std::string output) :
    output(std::move(output))
{
}

auto VipsImage::can_load(const std::string &file_path) -> bool
{
    return vips_foreign_find_load(file_path.c_str()) != nullptr;
}

auto VipsImage::load(ImageProps props) -> Result<void>
{
    this->props = std::move(props);
    return read_image().and_then([this]() -> Result<void> {
        resize_image();
        process_image();
        return {};
    });
}

auto VipsImage::is_cached() const -> bool
{
    return false;
}

auto VipsImage::read_image() -> Result<void>
{
    try {
        image = vips::VImage::new_from_file(props.file_path.c_str()).colourspace(VIPS_INTERPRETATION_sRGB);
        logger->info("loaded image {}", props.file_path);
    } catch (const vips::VError &err) {
        return Err("failed to load image");
    }

    return {};
}

void VipsImage::process_image()
{
    const std::unordered_set<std::string_view> bgra_outputs = {"x11", "chafa", "wayland"};
    if (bgra_outputs.contains(output)) {
        // alpha channel required
        if (!image.has_alpha()) {
            const int alpha_value = 255;
            image = image.bandjoin(alpha_value);
        }
        // convert from RGB to BGR
        auto bands = image.bandsplit();
        std::swap(bands[0], bands[2]);
        image = vips::VImage::bandjoin(bands);
    } else if (output == "sixel") {
        // sixel expects RGB888
        if (image.has_alpha()) {
            image = image.flatten();
        }
    }
    image_size = VIPS_IMAGE_SIZEOF_IMAGE(image.get_image());
    image_buffer.reset(static_cast<unsigned char *>(image.write_to_memory(&image_size)));
}

void VipsImage::resize_image()
{
    if (props.scaler == "contain") {
        contain_scaler();
    }
}

auto VipsImage::num_channels() -> int
{
    return image.bands();
}

void VipsImage::contain_scaler()
{
    int img_width = image.width();
    int img_height = image.height();
    auto [new_width, new_height] = contain_sizes(
        {.width = props.width, .height = props.height, .image_width = img_width, .image_height = img_height});

    logger->debug("resizing image to {}x{}", new_width, new_height);
    auto *opts = vips::VImage::option()->set("height", new_height)->set("size", VIPS_SIZE_FORCE);
    image = image.thumbnail_image(new_width, opts);
}

auto VipsImage::data() -> unsigned char *
{
    return image_buffer.get();
}

auto VipsImage::width() -> int
{
    return image.width();
}

auto VipsImage::height() -> int
{
    return image.height();
}

} // namespace upp
