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
// a bunch of includes to shutup clang-tidy
#include <glaze/core/common.hpp>
#include <glaze/core/meta.hpp>
#include <glaze/core/opts.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>

#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

template <>
struct glz::meta<upp::Command> {
    using T = upp::Command;

    template <auto MemberPointer>
    static constexpr auto maybe_quoted_int_read = [](T &self, const glz::raw_json &json) {
        // first attempt to parse without quotes
        auto err = glz::read_json(self.*MemberPointer, json.str);
        if (err) {
            // if we error then attempt parsing as a quoted number
            err = glz::read<glz::opts{.quoted_num = true}>(self.*MemberPointer, json.str);
        }
        if (err) {
            throw std::runtime_error("maybe_quoted_int_read failure");
        }
    };

    template <auto MemberPointer>
    static constexpr auto custom_int = custom<maybe_quoted_int_read<MemberPointer>, MemberPointer>;

    // NOLINTNEXTLINE
    static constexpr auto value = object(
        // clang-format off
        &T::action,
        "identifier", &T::preview_id,
        "scaler", &T::image_scaler,
        "path", &T::image_path,
        "x", custom_int<&T::x>,
        "y", custom_int<&T::y>,
        "width", custom_int<&T::width>,
        "height", custom_int<&T::height>,
        "max_width", custom_int<&T::width>,
        "max_height", custom_int<&T::height>
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
    if (auto err = glz::read_json(cmd, line); err) {
        return Err(glz::format_error(err, line));
    }
    if (cmd.image_scaler.empty()) {
        cmd.image_scaler = "contain";
    }
    return cmd;
}

} // namespace upp
