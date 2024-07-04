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
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace util
{

auto get_socket_path(int pid = os::get_pid()) -> std::string;
auto get_log_path() -> std::string;
auto get_process_tree(int pid) -> std::vector<Process>;
auto get_process_pid_tree(int pid) -> std::vector<int>;
auto str_split(std::string_view str, std::string_view delim = " ") -> std::vector<std::string>;
void benchmark(auto Func);

template <typename T>
constexpr auto view_to_numeral(const std::string_view view) noexcept -> std::expected<T, std::string>
{
    T result{};
    auto [ptr, err] = std::from_chars(view.data(), view.data() + view.size(), result);
    if (err == std::errc()) {
        return result;
    }
    auto error = std::make_error_code(err);
    return std::unexpected(error.message());
}

} // namespace util

#endif // UTIL_HPP
