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

#include <cerrno>
#include <cstdlib>
#include <optional>
#include <string>
#include <system_error>

#include <unistd.h>

#include "os/os.hpp"

namespace os
{

auto strerror() -> std::string
{
    const std::error_code code(errno, std::generic_category());
    return code.message();
}

auto getpid() -> int
{
    return ::getpid();
}

auto getenv(const std::string &var) -> std::optional<std::string>
{
    const char *env_p = std::getenv(var.c_str()); // NOLINT
    if (env_p == nullptr) {
        return {};
    }
    return env_p;
}

} // namespace os
