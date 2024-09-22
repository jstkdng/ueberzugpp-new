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

class Error
{
  public:
    explicit Error(std::string message = "", std::source_location location = std::source_location::current()) :
        message_(std::move(message)),
        location_(location)
    {
    }

    [[nodiscard]] auto location() const -> std::string
    {
        const std::filesystem::path path(location_.file_name());
        return std::format("{}:{}", path.filename().string(), location_.line());
    }

    [[nodiscard]] auto message() const -> std::string
    {
        if (message_.empty()) {
            return error_code_.message();
        }
        return std::format("{}: {}", message_, error_code_.message());
    }

    [[nodiscard]] auto error_message() const -> std::string { return error_code_.message(); }

    [[nodiscard]] auto combined() const -> std::string { return std::format("[{}]: {}", location(), message()); }

  private:
    std::string message_;
    std::source_location location_;
    std::error_code error_code_{errno, std::generic_category()};
};

template <class T>
using Result = std::expected<T, Error>;
