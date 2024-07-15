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

#ifndef VIPS_PREVIEW_HPP
#define VIPS_PREVIEW_HPP

#include "config.hpp"
#include "preview.hpp"

#include <vips/vips8>

#include <memory>

class VipsPreview final : public Preview
{
  public:
    VipsPreview();
    void draw() override;

  private:
    std::shared_ptr<Config> config = Config::instance();
};

#endif // VIPS_PREVIEW_HPP
