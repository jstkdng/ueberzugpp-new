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
#include <source_location>
#include <string>
#include <system_error>

// logged with spdlog, program should continue
struct Error {
    explicit Error(std::string title, std::errc errc = std::errc::state_not_recoverable) :
        title(std::move(title)),
        cond(errc)
    {
    }

    [[nodiscard]] auto message() const -> std::string
    {
        if (title.empty()) {
            return cond.message();
        }
        return title + ": " + cond.message();
    }

    std::string title;
    std::error_condition cond;
};

// logged to stdout, program should terminate afterwards
struct UnrecoverableError {
    explicit UnrecoverableError(std::string message, std::source_location location = std::source_location::current()) :
        message(std::move(message)),
        location(location)
    {
    }

    std::string message;
    std::source_location location;
};

inline auto uerror(auto... args) -> std::unexpected<Error>
{
    return std::unexpected(Error(args...));
}

template <class T>
using Result = std::expected<T, Error>;

template <class T>
using BadResult = std::expected<T, UnrecoverableError>;

template <class T>
using Err = std::unexpected<T>;
