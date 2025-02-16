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
#include "image/scalers.hpp"
#include "util/result.hpp"
#include "util/util.hpp"

#include <vips/vips.h>

#include <algorithm>
#include <unordered_set>

namespace upp
{

VipsImage::~VipsImage()
{
    if (m_image != nullptr) {
        g_object_unref(m_image);
    }
}

auto VipsImage::read(const std::string &image_path) -> Result<void>
{
    m_image_path = image_path;
    m_image = vips_image_new_from_file(m_image_path.c_str(), "access", VIPS_ACCESS_SEQUENTIAL, nullptr);
    if (m_image == nullptr) {
        return Err("failed to load image");
    }
    LOG_DEBUG("loading image {}", m_image_path);
    return {};
}

auto VipsImage::width() -> int
{
    return vips_image_get_width(m_image);
}

auto VipsImage::height() -> int
{
    return vips_image_get_height(m_image);
}

void VipsImage::scale_image(int target_width, int target_height, std::string_view scaler)
{
    if (scaler == "contain") {
        contain_scaler(target_width, target_height);
    }
}

void VipsImage::contain_scaler(int target_width, int target_height)
{
    auto [new_width, new_height] = image::contain_sizes({
        .width = target_width,
        .height = target_height,
        .image_width = width(),
        .image_height = height(),
    });
    if (new_width == width() && new_height == height()) {
        return;
    }

    LOG_INFO("resizing image {} to {}x{}", util::get_filename(m_image_path), new_width, new_height);
}

/*
LibvipsImage::LibvipsImage(ApplicationContext *ctx) :
    ctx(ctx)
{
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
    if (origin_is_animated()) {
        LOG_INFO("image is animated");
        VipsImage *temp =
            vips_image_new_from_file(props.file_path.c_str(), "n", -1, "access", VIPS_ACCESS_SEQUENTIAL, nullptr);
        int n_pages = vips_image_get_n_pages(temp);
        int temp_height = vips_image_get_height(temp);
        LOG_DEBUG("animated sizes: {}x{}", vips_image_get_width(temp), vips_image_get_height(temp));
        LOG_DEBUG("number of frames: {}", n_pages);
        LOG_DEBUG("frame height: {}  original height: {}", temp_height / n_pages, height());
        g_object_unref(temp);
    }
    return {};
}

void LibvipsImage::process_image()
{
    vips_colourspace(image, &image_out, VIPS_INTERPRETATION_sRGB, nullptr);
    g_object_unref(image);
    image = image_out;

    const std::unordered_set<std::string_view> bgra_outputs = {"x11", "chafa", "wayland"};
    std::string_view output = ctx->output;
    if (bgra_outputs.contains(output)) {
        // alpha channel required
        if (vips_image_hasalpha(image) == FALSE) {
            LOG_DEBUG("adding alpha channel to image");
            vips_addalpha(image, &image_out, nullptr);
            g_object_unref(image);
            image = image_out;
        }

        LOG_DEBUG("converting image to BGRX");
        // convert from RGB to BGR
        int chan = num_channels();
        std::vector<VipsImage *> bands(chan, nullptr);
        for (int i = 0; i < chan; ++i) {
            VipsImage *band = nullptr;
            vips_extract_band(image, &band, i, nullptr);
            bands[i] = band;
        }
        std::swap(bands[0], bands[2]);

        vips_bandjoin(bands.data(), &image_out, chan, nullptr);
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

    image_buffer.reset(static_cast<unsigned char *>(vips_image_write_to_memory(image, nullptr)));
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
        LOG_INFO("loading image {} from cache", util::get_filename(props.file_path));
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
    if (image_is_cached(new_width, new_height)) {
        return;
    }


    g_object_unref(image);
    vips_thumbnail(props.file_path.c_str(), &image, new_width, "height", new_height, nullptr);

    // images from vips_thumbnail can only be read once
    image_out = vips_image_copy_memory(image);
    g_object_unref(image);
    image = image_out;

    auto cached_image_path = util::get_cache_file_save_location(props.file_path);
    vips_image_write_to_file(image, cached_image_path.c_str(), nullptr);
}

auto LibvipsImage::data() -> unsigned char *
{
    return image_buffer.get();
}

auto LibvipsImage::data_size() -> int
{
    return VIPS_IMAGE_SIZEOF_IMAGE(image);
}

auto LibvipsImage::get_frame_delays() -> std::optional<std::span<int>>
{
    int size = -1;
    int *array = nullptr;
    if (vips_image_get_array_int(image, "delay", &array, &size) == -1) {
        return {};
    }
    return std::make_optional<std::span<int>>(array, static_cast<size_t>(size));
}

auto LibvipsImage::origin_is_animated() const -> bool
{
    return vips_image_get_typeof(image, "delay") == VIPS_TYPE_ARRAY_INT;
}
*/

} // namespace upp
