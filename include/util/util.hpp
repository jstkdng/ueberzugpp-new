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

#pragma once

#include <charconv>
#include <functional>
#include <string_view>
#include <system_error>
#include <vector>

#include "error.hpp"

namespace util
{

auto str_split(std::string_view str, std::string_view delim = " ") -> std::vector<std::string_view>;
void str_split_cb(std::string_view str, const std::function<void(std::string_view)> &callback,
                  std::string_view delim = " ");

template <typename T>
constexpr auto view_to_numeral(const std::string_view view) noexcept -> Result<T>
{
    T result{};
    auto [ptr, err] = std::from_chars(view.data(), view.data() + view.size(), result);
    if (err == std::errc()) {
        return result;
    }
    return Err("view_to_numeral", err);
}

} // namespace util
