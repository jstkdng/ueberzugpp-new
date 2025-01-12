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

#include <glaze/glaze.hpp>

#include <string>
#include <string_view>
#include <utility>

template <>
struct glz::meta<upp::Command> {
    using T = upp::Command;
    // NOLINTNEXTLINE
    static constexpr auto value = object(
        // clang-format off
        &T::action,
        "identifier", &T::preview_id,
        "scaler", &T::image_scaler,
        "path", &T::image_path,
        "x", &T::x_,
        "y", &T::y_,
        "width", &T::width_,
        "height", &T::height_,
        "max_width", &T::width_,
        "max_height", &T::height_
        // clang-format on
    );
};

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
    return cmd;
}

} // namespace upp
