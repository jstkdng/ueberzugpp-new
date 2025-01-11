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

#include <glaze/core/reflect.hpp>
#include <glaze/json/json_t.hpp>
#include <glaze/json/read.hpp>

#include <string>
#include <string_view>
#include <utility>

namespace upp
{

auto Command::create(std::string_view parser, std::string line) -> Result<Command>
{
    if (parser == "json") {
        return from_json(std::move(line));
    }

    return {};
}

auto Command::from_json(std::string line) -> Result<Command>
{
    Command cmd;
    auto err = glz::read_json(cmd, line);
    if (err) {
        return Err(glz::format_error(err, line));
    }

    /*
    try {
        Command cmd;
        cmd.action = json["action"].get_string();
        if (cmd.action == "exit" || cmd.action == "flush") {
            return cmd;
        }

        cmd.preview_id = json["identifier"].get_string();
        if (cmd.action == "remove") {
            return cmd;
        }

        return cmd;
    } catch (const std::exception& ex) {
        return Err(
    }*/
    return cmd;
}

} // namespace upp
