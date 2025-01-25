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

#include "util/result.hpp"
#include "application/context.hpp"

#include <memory>
#include <string>

namespace upp
{

class Image;

using ImagePtr = std::unique_ptr<Image>;

struct ImageProps {
    std::string file_path;
    int width = -1;
    int height = -1;
};

class Image
{
  public:
    virtual ~Image() = default;

    static auto create(ApplicationContext *ctx, ImageProps props) -> Result<ImagePtr>;
    virtual auto load() -> Result<void> = 0;
    auto num_channels() -> int;
};

} // namespace upp
