// Display images inside a terminal
// Copyright (C) 2024  JustKidding
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef UTIL_HPP
#define UTIL_HPP

#include "os.hpp"
#include "process.hpp"

#include <charconv>
#include <expected>
#include <format>
#include <string>
#include <string_view>
#include <vector>

namespace util
{

auto get_socket_path(int pid = os::get_pid()) -> std::string;
auto get_log_path() -> std::string;
auto get_process_tree(int pid) -> std::vector<Process>;
auto get_process_pid_tree(int pid) -> std::vector<int>;
auto str_split(std::string_view str, std::string_view delim = " ") -> std::vector<std::string>;

template <typename T>
constexpr auto view_to_numeral(std::string_view view) -> std::expected<T, std::string>
{
    T result{};
    const auto last_char = view.data() + view.size();
    auto [ptr, err] = std::from_chars(view.data(), last_char, result);
    if (err == std::errc()) {
        return result;
    }
    if (err == std::errc::invalid_argument) {
        return std::unexpected(std::format("{} is not a number", view));
    }
    if (err == std::errc::result_out_of_range) {
        return std::unexpected(std::format("{} is out of range", view));
    }
    return std::unexpected("unknown error");
}

} // namespace util

#endif // UTIL_HPP
