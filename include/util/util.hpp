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

#ifndef UTIL_HPP
#define UTIL_HPP

#include "os/os.hpp"
#include "os/process.hpp"

#include <charconv>
#include <chrono>
#include <expected>
#include <print>
#include <span>
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
auto bytes_to_hexstring(std::span<const std::byte> bytes) noexcept -> std::string;

template <typename Func>
void benchmark(Func func)
{
    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;

    const auto time1 = high_resolution_clock::now();
    func();
    const auto time2 = high_resolution_clock::now();
    const duration<double, std::milli> ms_double = time2 - time1;
    std::print("{}ms\n", ms_double.count());
}

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
