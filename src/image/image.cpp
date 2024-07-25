// Display images inside a terminal
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

#include "image/image.hpp"
#include "image/libvips_image.hpp"
#include "os/os.hpp"
#include "util/crypto.hpp"
#include "util/util.hpp"

#include <cmath>
#include <filesystem>

#ifdef ENABLE_OPENCV
#include "image/opencv_image.hpp"
#include <opencv2/imgcodecs.hpp>
#endif

#include <vips/vips.h>

namespace fs = std::filesystem;

auto Image::create([[maybe_unused]] const Config *config, const std::string &file_path)
    -> std::expected<std::unique_ptr<Image>, std::string>
{
#ifdef ENABLE_OPENCV
    if (!config->no_opencv && cv::haveImageReader(file_path)) {
        return std::make_unique<OpencvImage>();
    }
#endif

    const auto *vips_loader = vips_foreign_find_load(file_path.c_str());
    if (vips_loader != nullptr) {
        return std::make_unique<LibvipsImage>();
    }
    return util::unexpected_err("no suitable preview backend found");
}

auto Image::get_cache_path() -> std::filesystem::path
{
    fs::path rel_path = "ueberzugpp/";
    const auto xdg_cache = os::getenv("XDG_CACHE_HOME");
    if (xdg_cache) {
        return *xdg_cache / rel_path;
    }

    rel_path = ".cache/ueberzugpp/";
    const auto home = os::getenv("HOME");
    if (home) {
        return *home / rel_path;
    }

    return fs::temp_directory_path() / rel_path;
}

auto Image::get_cached_image_path(const std::filesystem::path &image_path) -> std::filesystem::path
{
    const auto hashed_path = crypto::get_b2_hash(image_path.c_str()) + image_path.extension().c_str();
    return get_cache_path() / hashed_path;
}

auto Image::image_is_cached(const std::filesystem::path &image_path) -> bool
{
    return fs::exists(get_cached_image_path(image_path));
}

auto Image::fit_contain_sizes(const current_image_sizes sizes) -> target_image_sizes
{
    const auto factor = std::min(static_cast<float>(sizes.width) / static_cast<float>(sizes.image_width),
                                 static_cast<float>(sizes.height) / static_cast<float>(sizes.image_height));
    return {.width = static_cast<int>(static_cast<float>(sizes.image_width) * factor),
            .height = static_cast<int>(static_cast<float>(sizes.image_height) * factor)};
}

auto Image::contain_sizes(const current_image_sizes sizes) -> target_image_sizes
{
    return fit_contain_sizes({
        .width = std::min(sizes.image_width, sizes.width),
        .height = std::min(sizes.image_height, sizes.height),
        .image_width = sizes.image_width,
        .image_height = sizes.image_height,
    });
}
