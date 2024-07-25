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

#ifndef OPENCV_IMAGE_HPP
#define OPENCV_IMAGE_HPP

#include "image.hpp"

#include <opencv2/core.hpp>

class OpencvImage final : public Image
{
  public:
    auto initialize(Terminal *term, Command *cmd) -> std::expected<void, std::string> override;
    [[nodiscard]] auto data() const -> unsigned char * override;
    [[nodiscard]] auto height() const -> int override;
    [[nodiscard]] auto width() const -> int override;

  private:
    std::shared_ptr<Config> config = Config::instance();
    Terminal *terminal = nullptr;
    Command *command = nullptr;

    int cmd_width = 0;
    int cmd_height = 0;

    cv::Mat image;

    auto read_image() -> std::expected<void, std::string>;
    void rotate_image();
    auto resize_image() -> std::expected<void, std::string>;
};

#endif // OPENCV_IMAGE_HPP
