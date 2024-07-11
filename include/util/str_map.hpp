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

#ifndef STR_MAP_HPP
#define STR_MAP_HPP

#include <string>
#include <string_view>
#include <unordered_map>

struct StringHash {
    using is_transparent = void; // Enables heterogeneous operations.

    auto operator()(const std::string_view view) const noexcept -> std::size_t
    {
        constexpr std::hash<std::string_view> hasher;
        return hasher(view);
    }
};

template <class T>
using string_map = std::unordered_map<std::string, T, StringHash, std::equal_to<>>;

#endif // STR_MAP_HPP
