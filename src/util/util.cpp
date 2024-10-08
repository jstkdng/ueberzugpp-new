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

#include "util/util.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <ranges>

namespace fs = std::filesystem;

auto util::unexpected_err(const std::string_view message, const std::source_location location)
    -> std::unexpected<std::string>
{
    const fs::path file_path = location.file_name();
    return std::unexpected(std::format("[{}:{}] {}", file_path.filename().string(), location.line(), message));
}

auto util::bytes_to_hexstring(const std::span<const std::byte> bytes) noexcept -> std::string
{
    std::string result(bytes.size() * 2, 0);
    constexpr std::string_view lower_hex_chars = "0123456789abcdef";
    constexpr int hexint = 16;

    std::size_t idx = 0;
    for (const auto byte : bytes) {
        int chars_idx = std::to_integer<int>(byte);
        result.at(idx + 1) = lower_hex_chars.at(chars_idx % hexint);
        chars_idx /= hexint;
        result.at(idx) = lower_hex_chars.at(chars_idx % hexint);
        idx += 2;
    }
    return result;
}

auto util::get_socket_path(int pid) -> std::string
{
    const auto sockname = std::format("ueberzugpp-{}.socket", pid);
    const auto tmp = fs::temp_directory_path();
    return tmp / sockname;
}

auto util::get_log_path() -> std::string
{
    const auto user = os::getenv("USER").value_or("NOUSER");
    const auto logname = std::format("ueberzugpp-{}.log", user);
    const auto tmp = fs::temp_directory_path();
    return tmp / logname;
}

auto util::get_process_tree(const int pid) -> std::vector<Process>
{
    std::vector<Process> result;
    Process runner(pid);
    while (runner.pid > 1) {
        result.emplace_back(runner.pid);
        runner = Process(runner.ppid);
    }
    return result;
}

auto util::get_process_pid_tree(const int pid) -> std::vector<int>
{
    const auto tree = get_process_tree(pid);
    std::vector<int> result;
    std::ranges::transform(tree, std::back_inserter(result), [](const auto &proc) { return proc.pid; });
    return result;
}

auto util::str_split(std::string_view str, std::string_view delim) -> std::vector<std::string_view>
{
    return std::views::split(str, delim) | std::views::transform([](auto rng) { return std::string_view(rng); }) |
           std::ranges::to<std::vector>();
}

void util::str_split_cb(std::string_view str, const std::function<void(std::string_view)> &callback,
                        std::string_view delim)
{
    auto range = std::views::split(str, delim) | std::views::transform([](auto rng) { return std::string_view(rng); });
    std::ranges::for_each(range, callback);
}
