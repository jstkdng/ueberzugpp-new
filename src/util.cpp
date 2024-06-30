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

#include "util.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <ranges>

namespace fs = std::filesystem;

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

auto util::str_split(std::string_view str, std::string_view delim) -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (auto word : std::ranges::views::split(str, delim)) {
        result.emplace_back(std::string_view(word));
    }
    return result;
}
