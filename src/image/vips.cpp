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
#include <vips/vips.h>

#include <ranges>
#include <unordered_set>
#include <utility>

namespace upp
{

LibvipsImage::LibvipsImage(std::string output) :
    output(std::move(output))
{
}

LibvipsImage::~LibvipsImage()
{
    g_object_unref(image);
}

auto LibvipsImage::can_load(const std::string &file_path) -> bool
{
    return vips_foreign_find_load(file_path.c_str()) != nullptr;
}

auto LibvipsImage::load(ImageProps props) -> Result<void>
{
    this->props = std::move(props);
    return read_image().and_then([this]() -> Result<void> {
        resize_image();
        process_image();
        logger->info("loaded image {}", this->props.file_path);
        return {};
    });
}

auto LibvipsImage::read_image() -> Result<void>
{
    image = vips_image_new_from_file(props.file_path.c_str(), "access", VIPS_ACCESS_SEQUENTIAL, nullptr);
    if (image == nullptr) {
        return Err("failed to load image");
    }
    return {};
}

void LibvipsImage::process_image()
{
    const std::unordered_set<std::string_view> bgra_outputs = {"x11", "chafa", "wayland"};
    if (bgra_outputs.contains(output)) {
        // alpha channel required
        if (vips_image_hasalpha(image) == FALSE) {
            vips_addalpha(image, &image_out, nullptr);
            g_object_unref(image);
            image = image_out;
        }

        // convert from RGB to BGR
        std::vector<VipsImage *> bands(num_channels());
        for (auto [idx, band] : std::views::enumerate(bands)) {
            vips_extract_band(image, &band, static_cast<int>(idx), nullptr);
        }
        std::swap(bands[0], bands[2]);

        vips_bandjoin(bands.data(), &image_out, static_cast<int>(bands.size()), nullptr);
        g_object_unref(image);
        image = image_out;

        for (auto *band : bands) {
            g_object_unref(band);
        }

    } else if (output == "sixel") {
        // sixel expects RGB888
        if (vips_image_hasalpha(image) == TRUE) {
            vips_flatten(image, &image_out, nullptr);
        }
        g_object_unref(image);
        image = image_out;
    }

    image_buffer.reset(static_cast<unsigned char *>(vips_image_write_to_memory(image, nullptr)));
}

void LibvipsImage::resize_image()
{
    if (props.scaler == "contain") {
        contain_scaler();
    }
}

auto LibvipsImage::num_channels() -> int
{
    return vips_image_get_bands(image);
}

void LibvipsImage::contain_scaler()
{
    auto [new_width, new_height] =
        contain_sizes({.width = props.width, .height = props.height, .image_width = width(), .image_height = height()});

    logger->debug("resizing image to {}x{}", new_width, new_height);

    vips_thumbnail_image(image, &image_out, new_width, "height", new_height, nullptr);
    g_object_unref(image);
    image = image_out;

    vips_colourspace(image, &image_out, VIPS_INTERPRETATION_sRGB, nullptr);
    g_object_unref(image);
    image = image_out;
}

auto LibvipsImage::data() -> unsigned char *
{
    return image_buffer.get();
}

auto LibvipsImage::width() -> int
{
    return vips_image_get_width(image);
}

auto LibvipsImage::height() -> int
{
    return vips_image_get_height(image);
}

} // namespace upp
