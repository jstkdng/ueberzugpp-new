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
#include <glaze/core/reflect.hpp>
#include <glaze/core/common.hpp>
#include <glaze/core/meta.hpp>
#include <glaze/core/wrappers.hpp>

#include <glaze/json/read.hpp>
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
        "x", glz::quoted_num<&T::x>,
        "y", glz::quoted_num<&T::y>,
        "width", glz::quoted_num<&T::width>,
        "height", glz::quoted_num<&T::height>,
        "max_width", glz::quoted_num<&T::width>,
        "max_height", glz::quoted_num<&T::height>
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
