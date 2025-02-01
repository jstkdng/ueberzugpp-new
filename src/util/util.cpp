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
#include "util/crypto.hpp"

#include <filesystem>
#include <format>
#include <string>

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
    const auto tmp = temp_directory_path();
    return tmp / sockname;
}

auto get_cache_path() -> std::filesystem::path
{
    fs::path home = os::getenv("HOME").value_or(temp_directory_path());
    fs::path cache_home = os::getenv("XDG_CACHE_HOME").value_or(home / ".cache");
    return cache_home / "ueberzugpp";
}

auto get_cache_file_save_location(const std::filesystem::path &path) -> std::string
{
    auto hashed_path = crypto::blake2b_encode(make_buffer(path.string())) + path.extension().string();
    return get_cache_path() / hashed_path;
}

auto get_filename(std::string_view path) -> std::string
{
    return fs::path(path).filename().string();
}

} // namespace upp::util
