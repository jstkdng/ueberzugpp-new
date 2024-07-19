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

#include "canvas/geometry.hpp"

#include <spdlog/spdlog.h>

#include "util/util.hpp"

using njson = nlohmann::json;
using std::string;
using view = std::string_view;

auto Geometry::initialize(const nlohmann::json &json) -> std::expected<void, std::string>
{
    SPDLOG_DEBUG("parsing command {}", json.dump());

    string width_key = "max_width";
    string height_key = "max_height";
    if (json.contains("width")) {
        width_key = "width";
        height_key = "height";
    }

    scaler = json.value("scaler", "contain");
    try {
        max_width = get_int_json(json, width_key);
        max_height = get_int_json(json, height_key);
        x = get_int_json(json, "x");
        y = get_int_json(json, "y");
    } catch (const njson::exception &) {
        return util::unexpected_err("could not parse json command");
    }

    return {};
}

auto Geometry::get_int_json(const nlohmann::json &json, std::string_view key) -> int
{
    const auto &value = json.at(key);
    if (value.is_string()) {
        return util::view_to_numeral<int>(value.get<view>()).value_or(0);
    }
    return value.get<int>();
}
