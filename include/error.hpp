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

class Error
{
  public:
    Error(std::source_location location, std::string prefix, std::errc errc);
    Error(std::source_location location, std::string prefix, int code = errno);

    [[nodiscard]] auto message() const -> std::string;
    [[nodiscard]] auto lmessage() const -> std::string;

  private:
    std::string prefix_;
    std::error_condition condition_;
    std::source_location location_;
};

template <class T>
using Result = std::expected<T, Error>;

// NOLINTNEXTLINE
#define Err(...) std::unexpected<Error>(std::in_place, std::source_location::current() __VA_OPT__(, ) __VA_ARGS__)
