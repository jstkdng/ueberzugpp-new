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

#include "os/os.hpp"
#include "util/result.hpp"

#include <charconv>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <print>
#include <ratio>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace upp::util
{

template <typename T>
constexpr auto view_to_numeral(const std::string_view view) -> Result<T>
{
    T result{};
    auto [ptr, err] = std::from_chars(view.data(), view.data() + view.size(), result);
    if (err == std::errc()) {
        return result;
    }
    return Err("parse_num", err);
}

auto get_filename(std::string_view path) -> std::string;
auto get_log_filename() -> std::string;
auto get_cache_path() -> std::filesystem::path;
auto get_cache_file_save_location(const std::filesystem::path &path) -> std::string;
auto get_socket_path(int pid = os::getpid()) -> std::string;
auto temp_directory_path() -> std::filesystem::path;

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
    std::println("{}ms", ms_double.count());
}

template <class T, class Allocator = std::allocator<T>>
auto make_vector(size_t capacity) -> std::vector<T>
{
    std::vector<T, Allocator> result;
    result.reserve(capacity);
    return result;
}

auto make_buffer(auto container) -> std::span<const std::byte>
{
    return std::as_bytes(std::span{std::data(container), std::size(container)});
}

} // namespace upp::util
