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

#ifndef LIBVIPS_IMAGE_HPP
#define LIBVIPS_IMAGE_HPP

#include "image.hpp"

class LibvipsImage final : public Image
{
  public:
    auto initialize(Terminal *term, Command *cmd) -> std::expected<void, std::string> override;
    [[nodiscard]] auto data() const -> unsigned char * override;
    [[nodiscard]] auto height() const -> int override;
    [[nodiscard]] auto width() const -> int override;

  private:
    Terminal *terminal = nullptr;
    Command *command = nullptr;
};

#endif // LIBVIPS_IMAGE_HPP
