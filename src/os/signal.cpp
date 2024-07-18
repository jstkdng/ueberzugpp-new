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

#include "os/signal.hpp"
#include "application.hpp"

#include <algorithm>
#include <array>
#include <utility>

#include <csignal>
#include <spdlog/spdlog.h>

void signal_manager::setup_signals()
{
    struct sigaction sga {
    };
    sga.sa_handler = signal_handler;
    sigemptyset(&sga.sa_mask);
    sga.sa_flags = 0;
    sigaction(SIGINT, &sga, nullptr);
    sigaction(SIGTERM, &sga, nullptr);
    sigaction(SIGHUP, &sga, nullptr);
    sigaction(SIGCHLD, &sga, nullptr);
}

constexpr auto signal_manager::get_signal_name(int signal) -> std::string_view
{
    using pair_type = std::pair<int, std::string_view>;
    constexpr auto map = std::to_array<pair_type>({
        {SIGINT, "SIGINT"},
        {SIGTERM, "SIGTERM"},
        {SIGHUP, "SIGHUP"},
        {SIGCHLD, "SIGCHLD"},
    });
    const auto *find = std::ranges::find_if(map, [signal](const pair_type &pair) { return pair.first == signal; });
    if (find == map.end()) {
        return "UNKNOWN signal";
    }
    return find->second;
}

void signal_manager::signal_handler(const int signal)
{
    SPDLOG_WARN("{} received", get_signal_name(signal));
    Application::terminate();
}
