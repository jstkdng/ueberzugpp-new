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

#pragma once

#include <cerrno>
#include <exception>
#include <expected>
#include <format>
#include <source_location>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

namespace upp
{

class Error
{
  public:
    Error(std::source_location location, std::string prefix, std::errc errc) :
        prefix(std::move(prefix)),
        condition(errc),
        location(location)
    {
    }

    Error(std::source_location location, std::string_view prefix, const std::exception &exc) :
        prefix(std::format("{}: {}", prefix, exc.what())),
        location(location)
    {
    }

    Error(std::source_location location, std::string prefix, int code = errno) :
        prefix(std::move(prefix)),
        condition(code, std::generic_category()),
        location(location)
    {
    }

    [[nodiscard]] auto message() const -> std::string
    {
        if (condition.value() == 0) {
            return prefix;
        }
        return std::format("{}: {}", prefix, condition.message());
    }

    [[nodiscard]] auto lmessage() const -> std::string
    {
        return std::format("[{}:{}] {}", location.file_name(), location.line(), message());
    }

  private:
    std::string prefix;
    std::error_condition condition;
    std::source_location location;
};

template <class T>
using Result = std::expected<T, Error>;

} // namespace upp

// NOLINTNEXTLINE
#define Err(...) std::unexpected<upp::Error>(std::in_place, std::source_location::current() __VA_OPT__(, ) __VA_ARGS__)
