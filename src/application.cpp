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

#include "application.hpp"
#include "cli.hpp"
#include "util/result.hpp"

#include <CLI/CLI.hpp>
#include <spdlog/common.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <signal.h> // NOLINT
#include <string>

namespace upp
{

Application::Application(Cli *cli) :
    cli(cli)
{
}

auto Application::run() -> Result<void>
{
    return setup_logging().and_then([this] { return handle_cli_commands(); });
}

auto Application::handle_cli_commands() -> Result<void>
{
    if (cli->layer_command->parsed()) {
        setup_signal_handler();
        return terminal.open_first_pty().and_then([this] { return listener.init(); }).and_then([] -> Result<void> {
            stop_flag_.wait(false);
            return {};
        });
    }

    return {};
}

auto Application::setup_logging() -> Result<void>
{
#ifdef DEBUG
    auto level = spdlog::level::trace;
#else
    auto level = spdlog::level::info;
#endif

    spdlog::set_level(level);
    spdlog::flush_on(level);

    try {
        logger = spdlog::stdout_color_mt("main");
        logger->set_pattern("[%Y-%m-%d %T.%F] [%^%L%$] [%@] %v");
        spdlog::set_default_logger(logger);
    } catch (const spdlog::spdlog_ex &ex) {
        return Err("spdlog", ex);
    }
    return {};
}

void Application::terminate()
{
    stop_flag_.test_and_set();
    stop_flag_.notify_one();
}

void Application::setup_signal_handler()
{
    SPDLOG_DEBUG("setting up signal handlers");
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

void Application::signal_handler([[maybe_unused]] int signal)
{
    SPDLOG_WARN("received signal, terminating");
    terminate();
}

} // namespace upp
