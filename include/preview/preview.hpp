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

#ifndef PREVIEW_HPP
#define PREVIEW_HPP

#include "config.hpp"

#include <expected>
#include <memory>
#include <string>

class Preview
{
  public:
    static auto create(const Config *config, const std::string &file_path)
        -> std::expected<std::unique_ptr<Preview>, std::string>;

    virtual void draw() = 0;
    virtual ~Preview() = default;
};

#endif // PREVIEW_HPP
