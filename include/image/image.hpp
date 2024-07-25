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

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "command/command.hpp"
#include "config.hpp"
#include "terminal.hpp"

#include <expected>
#include <filesystem>
#include <memory>
#include <string>

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
    static auto create(const Config *config, const std::string &file_path)
        -> std::expected<std::unique_ptr<Image>, std::string>;
    static auto get_cache_path() -> std::filesystem::path;
    static auto get_cached_image_path(const std::filesystem::path &image_path) -> std::filesystem::path;
    static auto image_is_cached(const std::filesystem::path &image_path) -> bool;

    static auto fit_contain_sizes(current_image_sizes sizes) -> target_image_sizes;
    static auto contain_sizes(current_image_sizes sizes) -> target_image_sizes;

    virtual auto initialize(Terminal *term, Command *cmd) -> std::expected<void, std::string> = 0;
    virtual ~Image() = default;
    [[nodiscard]] virtual auto data() const -> unsigned char * = 0;
    [[nodiscard]] virtual auto width() const -> int = 0;
    [[nodiscard]] virtual auto height() const -> int = 0;
};

#endif // IMAGE_HPP
