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

#include <csignal>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "application.hpp"
#include "error.hpp"

void Application::signal_handler([[maybe_unused]] int signal)
{
    SPDLOG_WARN("signal received, terminating");
    terminate();
}

Application::Application(CliManager *cli) :
    cli_(cli)
{
}

auto Application::init() -> Result<void>
{
    return setup_logger()
        .and_then(setup_signal_handler)
        .and_then([this] { return terminal_.init(); })
        .and_then([] -> Result<void> {
            stop_flag_.wait(false);
            return {};
        });
}

auto Application::setup_logger() -> Result<void>
{
    return {};
}

auto Application::setup_signal_handler() -> Result<void>
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
    return {};
}

void Application::terminate()
{
    stop_flag_.test_and_set();
    stop_flag_.notify_one();
}

auto Application::run() -> Result<void>
{
    if (cli_->layer_command->parsed()) {
        return init();
    }
    [[unlikely]] return {};
}
