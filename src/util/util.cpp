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

#include "util/util.hpp"

#include <spdlog/spdlog.h>

#include <string>
#include <variant>

namespace upp::util
{

auto variant_to_int(const std::variant<int, std::string> &var) -> int
{
    try {
        return std::get<int>(var);
    } catch (const std::bad_variant_access &) {
        auto result = util::view_to_numeral<int>(std::get<std::string>(var));
        if (!result) {
            SPDLOG_DEBUG(result.error().message());
        }
        return result.value_or(0);
    }
}

} // namespace upp::util
