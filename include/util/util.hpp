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

#include <string_view>
#include <system_error>

namespace upp::util
{

template <typename T>
constexpr auto view_to_numeral(const std::string_view view) -> Result<T>
{
    T result{};
    auto [ptr, err] = std::from_chars(view.data(), view.data() + view.size(), result);
    if (err == std::errc()) {
        return result;
    }
    return Err("parse_num", err);
}

} // namespace upp::util
