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

#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <expected>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

class Geometry
{
  public:
    auto initialize(const nlohmann::json &json) -> std::expected<void, std::string>;
    static auto get_int_json(const nlohmann::json &json, std::string_view key) -> int;

  private:
    std::string scaler;

    int x = 0;
    int y = 0;
    int max_width = 0;
    int max_height = 0;
};

#endif // GEOMETRY_HPP
