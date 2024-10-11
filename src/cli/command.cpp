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

#include <string>

#include <json/json.h>
#include <spdlog/spdlog.h>

#include "cli/command.hpp"
#include "util/util.hpp"

namespace
{
auto get_int_json(const Json::Value &json, std::string_view key) -> int
{
    const auto value = json.get(key.begin(), key.end());
    if (value.isInt()) {
        return value.asInt();
    }
    auto result = util::view_to_numeral<int>(value.asCString());
    if (!result) {
        SPDLOG_DEBUG(result.error().message());
    }
    return result.value_or(0);
}

auto get_either_key_value(const Json::Value &json, const std::initializer_list<std::string_view> key_list) -> int
{
    for (auto key : key_list) {
        if (json.isMember(key.begin(), key.end())) {
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

auto Command::from_json([[maybe_unused]] std::string_view line) -> Result<Command>
{
    Command cmd;
    Json::Value json;
    Json::Reader reader;

    if (!reader.parse(line.begin(), line.end(), json)) {
        return Err(reader.getFormattedErrorMessages());
    }

    try {
        cmd.action = json.get("action", "").asCString();

        if (cmd.action == "exit" || cmd.action == "flush") {
            return cmd;
        }

        cmd.preview_id = json.get("identifier", "").asCString();
        if (cmd.action == "remove") {
            return cmd;
        }

        cmd.image_scaler = json.get("scaler", "contain").asCString();
        cmd.image_path = json.get("path", "").asCString();

        cmd.x = get_int_json(json, "x");
        cmd.y = get_int_json(json, "y");

        cmd.width = get_either_key_value(json, {"width", "max_width"});
        cmd.height = get_either_key_value(json, {"height", "max_height"});
    } catch (const Json::LogicError &ex) {
        return Err("could not parse json command", ex);
    }
    return cmd;
}
