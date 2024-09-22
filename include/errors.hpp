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

#pragma once

#include <cerrno>
#include <expected>
#include <filesystem>
#include <format>
#include <source_location>
#include <string>
#include <system_error>

struct Error {
    explicit Error(std::string message = "", std::source_location location = std::source_location::current()) :
        message(std::move(message)),
        location(location)
    {
    }

    std::string message;
    std::source_location location;
    std::error_code error_code{errno, std::generic_category()};
};

template <class T>
using Result = std::expected<T, Error>;

template <>
struct std::formatter<Error> : std::formatter<std::string> {
    auto format(Error err, format_context &ctx) const
    {
        const std::filesystem::path path = err.location.file_name();
        if (err.message.empty()) {
            return formatter<std::string>::format(
                std::format("[{}:{}] {}", path.filename().string(), err.location.line(), err.error_code.message()),
                ctx);
        }
        return formatter<std::string>::format(std::format("[{}:{}] {}: {}", path.filename().string(),
                                                          err.location.line(), err.message, err.error_code.message()),
                                              ctx);
    }
};
