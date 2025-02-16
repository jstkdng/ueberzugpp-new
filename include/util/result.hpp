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
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

namespace upp
{

class Error
{
  public:
    Error(std::string_view prefix, std::errc errc) :
        m_prefix(prefix),
        m_condition(errc)
    {
    }

    explicit Error(std::string_view prefix, int code = errno) :
        m_prefix(prefix),
        m_condition(code, std::generic_category())
    {
    }

    Error(std::string_view prefix, const std::exception &exc) :
        m_prefix(std::format("{}: {}", prefix, exc.what()))
    {
    }

    [[nodiscard]] auto message() const -> std::string
    {
        if (m_condition.value() == 0) {
            return m_prefix;
        }
        return std::format("{}: {}", m_prefix, m_condition.message());
    }

  private:
    std::string m_prefix;
    std::error_condition m_condition;
};

template <class T>
using Result = std::expected<T, Error>;

template <class... Args>
auto Err(Args &&...args) -> std::unexpected<Error> // NOLINT
{
    return std::unexpected<Error>(std::in_place, std::forward<Args>(args)...);
}

template <class T, class... Args>
auto make_result(Args &&...args) -> Result<T>
{
    return Result<T>(std::in_place, std::forward<Args>(args)...);
}

} // namespace upp
