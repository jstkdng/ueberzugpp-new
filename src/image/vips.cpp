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
#include <string_view>
#include <unordered_set>

namespace upp
{

template <typename T>
using glib_ptr = c_unique_ptr<T, g_free>;

VipsImage::VipsImage(ApplicationContext *ctx) :
    m_ctx(ctx)
{
}

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

    LOG_DEBUG("resizing image {} to {}x{}", util::get_filename(m_image_path), new_width, new_height);
    if (auto img = thumbnail(m_image_path, new_width, new_height)) {
        g_object_unref(m_image);
        m_image = *img;
        LOG_DEBUG("resulting sizes after resize: {}x{}", width(), height());
    } else {
        LOG_ERROR(img.error().message());
    }
}

auto VipsImage::thumbnail(const std::string &image_path, int width, int height) -> Result<VImage *>
{
    VImage *temp = nullptr;
    if (vips_thumbnail(image_path.c_str(), &temp, width, "height", height, nullptr) == -1) {
        return last_error();
    }

    auto result = make_result<VImage *>(vips_image_copy_memory(temp));
    if (*result == nullptr) {
        g_object_unref(temp);
        return last_error();
    }
    return result;
}

auto VipsImage::last_error() -> std::unexpected<Error>
{
    glib_ptr<char> err{vips_error_buffer_copy()};
    std::string_view err_view{err.get()};
    err_view.remove_suffix(1);
    return Err(err_view, 0);
}

auto VipsImage::rgb_to_bgr(VImage *image) -> Result<VImage *>
{
    int channels = vips_image_get_bands(image);
    auto bands = util::make_vector<VImage *>(channels);

    for (int i = 0; i < channels; ++i) {
        VImage *band = nullptr;
        if (vips_extract_band(image, &band, i, nullptr) == -1) {
            std::ranges::for_each(bands, g_object_unref);
            return last_error();
        }
        bands[i] = band;
    }
    std::swap(bands[0], bands[2]);

    auto result = make_result<VImage *>();
    if (vips_bandjoin(bands.data(), &(*result), channels, nullptr) == -1) {
        std::ranges::for_each(bands, g_object_unref);
        return last_error();
    }
    std::ranges::for_each(bands, g_object_unref);

    return result;
}

auto VipsImage::add_alpha(VImage *image) -> Result<VImage *>
{
    if (vips_image_hasalpha(image) == FALSE) {
        LOG_DEBUG("adding alpha channel to image");
        vips_addalpha(image, &image_out, nullptr);
        g_object_unref(image);
        image = image_out;
    }
}

/*
auto LibvipsImage::load(ImageProps props) -> Result<void>
{
    this->props = std::move(props);
    return read_image().and_then([this]() -> Result<void> {
        resize_image();
        process_image();
        return {};
    });
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

        LOG_DEBUG("converting image to BGRX");
        // convert from RGB to BGR
        int chan = num_channels();

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
