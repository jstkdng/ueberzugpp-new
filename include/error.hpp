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
#include <iostream>
#include <source_location>
#include <string>
#include <system_error>

struct Error {
    explicit Error(int code = errno, std::source_location location = std::source_location::current()) :
        cond(code, std::generic_category()),
        loc(location)
    {
    }

    explicit Error(std::errc errc, std::source_location location = std::source_location::current()) :
        cond(errc),
        loc(location)
    {
    }

    [[nodiscard]] auto message() const -> std::string { return cond.message(); }

    static inline void log(const std::string &message = "")
    {
        Error err;
        if (message.empty()) {
            std::cout << err.message() << std::endl;
        } else {
            std::cout << message << ": " << err.message() << std::endl;
        }
    }

    std::error_condition cond;
    std::source_location loc;
};

template <class T>
using Result = std::expected<T, Error>;

using UError = std::unexpected<Error>;

using errc = std::errc;
