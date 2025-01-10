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

#include "command.hpp"
#include "util/result.hpp"

#include <glaze/json/json_t.hpp>
#include <glaze/json/read.hpp>

#include <string>
#include <string_view>
#include <utility>
#include <format>

namespace upp::command
{

auto Command::init(std::string_view parser, std::string line) -> Result<void>
{
    if (parser == "json") {
        return parse_json(std::move(line));
    }

    return {};
}

auto Command::parse_json(std::string line) -> Result<void>
{
    glz::json_t json;
    auto err = glz::read_json(json, line);
    if (err) {
        return Err(std::format("could not read json: {}", err.custom_error_message));
    }

    action = json["action"].get_string();
    if (action == "exit" || action == "flush") {
        return {};
    }

    preview_id = json["identifier"].get_string();
    if (action == "remove") {
        return {};
    }

    return {};
}

} // namespace upp::command
