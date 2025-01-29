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
#include "os/os.hpp"

#include <filesystem>
#include <string>
#include <format>

namespace upp::util
{

namespace fs = std::filesystem;

auto get_log_filename() -> std::string
{
    const auto user = os::getenv("USER");
    if (!user) {
        return "ueberzugpp.log";
    }
    return std::format("ueberzugpp-{}.log", *user);
}

auto temp_directory_path() -> std::filesystem::path
{
    return os::getenv("UEBERZUGPP_TMPDIR").value_or(fs::temp_directory_path());
}

auto get_socket_path(int pid) -> std::string
{
    const auto sockname = std::format("ueberzugpp-{}.socket", pid);
    const auto tmp = fs::temp_directory_path();
    return tmp / sockname;
}

} // namespace upp::util
