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
#include "util/util.hpp"

#include <spdlog/spdlog.h>
#include <vips/vips.h>

#include <algorithm>
#include <filesystem>
#include <unordered_set>
#include <utility>

namespace fs = std::filesystem;

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
    fs::path path{file_path};
    std::unordered_set<std::string_view> non_working_extensions = {".psd"};
    return vips_foreign_find_load(file_path.c_str()) != nullptr &&
           !non_working_extensions.contains(path.extension().string());
}

auto LibvipsImage::load(ImageProps props) -> Result<void>
{
    this->props = std::move(props);
    return read_image().and_then([this]() -> Result<void> {
        resize_image();
        process_image();
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
    vips_colourspace(image, &image_out, VIPS_INTERPRETATION_sRGB, nullptr);
    g_object_unref(image);
    image = image_out;

    const std::unordered_set<std::string_view> bgra_outputs = {"x11", "chafa", "wayland"};
    if (bgra_outputs.contains(output)) {
        // alpha channel required
        if (vips_image_hasalpha(image) == FALSE) {
            vips_addalpha(image, &image_out, nullptr);
            g_object_unref(image);
            image = image_out;
        }

        // convert from RGB to BGR
        auto bands = util::make_vector<VipsImage *>(num_channels());
        for (int i = 0; i < num_channels(); ++i) {
            VipsImage *band = nullptr;
            vips_extract_band(image, &band, i, nullptr);
            bands.emplace_back(band);
        }
        std::swap(bands[0], bands[2]);

        vips_bandjoin(bands.data(), &image_out, num_channels(), nullptr);
        g_object_unref(image);
        image = image_out;

        std::ranges::for_each(bands, g_object_unref);

    } else if (output == "sixel") {
        // sixel expects RGB888
        if (vips_image_hasalpha(image) == TRUE) {
            vips_flatten(image, &image_out, nullptr);
            g_object_unref(image);
            image = image_out;
        }
    }

    image_buffer.reset(reinterpret_cast<unsigned char *>(vips_image_write_to_memory(image, nullptr)));
}

auto LibvipsImage::image_is_cached(int new_width, int new_height) -> bool
{
    auto cached_image_path = util::get_cache_file_save_location(props.file_path);
    VipsImage *cached_image =
        vips_image_new_from_file(cached_image_path.c_str(), "access", VIPS_ACCESS_SEQUENTIAL, nullptr);
    if (cached_image == nullptr) {
        return false;
    }

    int cached_width = vips_image_get_width(cached_image);
    int cached_height = vips_image_get_height(cached_image);
    constexpr int delta = 10;
    if ((new_width >= cached_width && new_height >= cached_height) &&
        ((new_width - cached_width) <= delta || (new_height - cached_height) <= delta)) {
        g_object_unref(image);
        image = cached_image;
        logger->info("loading image {} from cache", util::get_filename(props.file_path));
        return true;
    }

    return false;
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
    if (new_width == width() && new_height == height()) {
        return;
    }
    if (image_is_cached(new_width, new_height)) {
        return;
    }

    logger->info("resizing image {} to {}x{} and caching", util::get_filename(props.file_path), new_width, new_height);

    vips_thumbnail(props.file_path.c_str(), &image_out, new_width, "height", new_height, nullptr);
    g_object_unref(image);
    image = image_out;

    auto cached_image_path = util::get_cache_file_save_location(props.file_path);
    vips_image_write_to_file(image, cached_image_path.c_str(), nullptr);

    // reread image
    g_object_unref(image);
    image = vips_image_new_from_file(cached_image_path.c_str(), "access", VIPS_ACCESS_SEQUENTIAL, nullptr);
}

auto LibvipsImage::data() -> unsigned char *
{
    return image_buffer.get();
}

auto LibvipsImage::data_size() -> int
{
    return VIPS_IMAGE_SIZEOF_IMAGE(image);
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
