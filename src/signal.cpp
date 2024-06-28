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

#include "signal.hpp"
#include "application.hpp"

#include <csignal>

void signal_manager::setup_signals()
{
    struct sigaction sga {
    };
    sga.sa_handler = signal_handler;
    sigemptyset(&sga.sa_mask);
    sga.sa_flags = 0;
    sigaction(SIGINT, &sga, nullptr);
    sigaction(SIGTERM, &sga, nullptr);
    sigaction(SIGHUP, nullptr, nullptr);
    sigaction(SIGCHLD, nullptr, nullptr);
}

void signal_manager::signal_handler(const int signal)
{
    Application::stop_flag = true;
    const auto logger = spdlog::get("main");
    if (!logger) {
        return;
    }
    switch (signal) {
        case SIGINT:
            logger->error("SIGINT received, exiting...");
            break;
        case SIGTERM:
            logger->error("SIGTERM received, exiting...");
            break;
        default:
            logger->error("UNKNOWN({}) signal received, exiting...", signal);
            break;
    }
}
