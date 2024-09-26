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

#include <format>
#include <string>

#include <nlohmann/json.hpp>

#include "command/command.hpp"
#include "util/util.hpp"

#include <spdlog/spdlog.h>

using njson = nlohmann::json;

namespace
{
auto get_int_json(const nlohmann::json &json, std::string_view key) -> int
{
    const auto &value = json.at(key);
    if (value.is_string()) {
        auto result = util::view_to_numeral<int>(value.get<std::string_view>());
        if (!result) {
            SPDLOG_DEBUG(result.error().message());
        }
        return result.value_or(0);
    }
    return value.get<int>();
}

auto get_either_key_value(const nlohmann::json &json, const std::initializer_list<std::string_view> key_list) -> int
{
    for (auto key : key_list) {
        if (json.contains(key)) {
            return get_int_json(json, key);
        }
    }
    return 0;
}
} // namespace

auto Command::create(std::string_view parser, std::string_view line) -> Result<Command>
{
    if (parser == "json") {
        return from_json(line);
    }
    if (parser == "bash") {
        return Err("bash parser not implemented");
    }
    if (parser == "simple") {
        return Err("simple parser not implemented");
    }
    [[unlikely]] return Err("unknown parser received");
}

auto Command::from_json(std::string_view line) -> Result<Command>
{
    Command cmd;
    try {
        auto json = njson::parse(line);

        cmd.action = json.at("action");
        if (cmd.action == "exit" || cmd.action == "flush") {
            return cmd;
        }

        cmd.preview_id = json.at("identifier");
        if (cmd.action == "remove") {
            return cmd;
        }

        cmd.image_scaler = json.value("scaler", "contain");
        const std::string path = json.at("path");
        cmd.image_path = path;

        cmd.x = get_int_json(json, "x");
        cmd.y = get_int_json(json, "y");

        cmd.width = get_either_key_value(json, {"width", "max_width"});
        cmd.height = get_either_key_value(json, {"height", "max_height"});

    } catch (const njson::exception &ex) {
        return Err(std::format("could not parse json command: {}", ex.what()));
    }
    return cmd;
}
