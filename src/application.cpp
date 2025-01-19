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
#include "base/canvas.hpp"
#include "buildconfig.hpp"
#include "cli.hpp"
#include "util/result.hpp"
#include "util/util.hpp"
#include "util/thread.hpp"
#include "os/os.hpp"

#include <CLI/CLI.hpp>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <array>
#include <csignal>
#include <signal.h> // NOLINT
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

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
        print_header();
        setup_signal_handler();
        return ctx.init()
            .and_then([this] { return Canvas::create(cli->layer.output); })
            .and_then([this](CanvasPtr new_canvas) {
                canvas = std::move(new_canvas);
                return canvas->init();
            })
            .and_then([this] { return listener.start(cli->layer.parser); })
            .and_then([this] { return wait_for_layer_commands(); });
    }

    return {};
}

auto Application::wait_for_layer_commands() -> Result<void>
{
    command_thread = std::jthread([this] (const auto&token) { execute_layer_commands(token);});
    stop_flag.wait(false);
    return {};
}

void Application::execute_layer_commands(const std::stop_token &token)
{
    while (!token.stop_requested()) {
        auto cmd = queue.try_dequeue(os::waitms);
        if (!cmd) {
            continue;
        }
        canvas->execute(*cmd);
    }
}

void Application::print_header()
{
    const auto *art = R"(starting
 _   _      _
| | | |    | |                                _     _
| | | | ___| |__   ___ _ __ _____   _  __ _ _| |_ _| |_
| | | |/ _ \ '_ \ / _ \ '__|_  / | | |/ _` |_   _|_   _|   version: {}
| |_| |  __/ |_) |  __/ |   / /| |_| | (_| | |_|   |_|     build date: {}
 \___/ \___|_.__/ \___|_|  /___|\__,_|\__, |
                                       __/ |    new
                                      |___/)";
    SPDLOG_INFO(art, version_str, build_date);
}

auto Application::setup_logging() -> Result<void>
{
#ifdef DEBUG
    auto level = spdlog::level::trace;
#else
    auto level = spdlog::level::info;
#endif

    auto log_file = util::get_log_filename();
    auto log_path = util::temp_directory_path() / log_file;

    try {
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_path);
        std::vector<spdlog::sink_ptr> sinks {file_sink};
        if (!cli->layer.silent) {
            sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        }

        logger = std::make_shared<spdlog::logger>("main", sinks.begin(), sinks.end());
        logger->set_pattern("[%Y-%m-%d %T.%F] %^[%8l]%$ [%@] %v");
        logger->set_level(level);
        logger->flush_on(level);
        spdlog::set_default_logger(logger);
    } catch (const spdlog::spdlog_ex &ex) {
        return Err("spdlog", ex);
    }
    return {};
}

void Application::terminate()
{
    stop_flag.test_and_set();
    stop_flag.notify_one();
}

void Application::setup_signal_handler()
{
    SPDLOG_DEBUG("setting up signal handler");
    struct sigaction sga {
    };
    sga.sa_handler = signal_handler;
    sigemptyset(&sga.sa_mask);
    sga.sa_flags = 0;
    sigaction(SIGINT, &sga, nullptr);
    sigaction(SIGTERM, &sga, nullptr);
    sigaction(SIGHUP, &sga, nullptr);
    sigaction(SIGCHLD, nullptr, nullptr);
}

void Application::signal_handler(int signal)
{
    using pair_t = std::pair<int, std::string_view>;
    constexpr auto signal_map = std::to_array<pair_t>({
        // clang-format off
        {SIGINT, "SIGINT"},
        {SIGTERM, "SIGTERM"},
        {SIGHUP, "SIGHUP"}
        // clang-format on
    });

    const auto *found = std::ranges::find_if(signal_map, [signal](const pair_t &pair) { return pair.first == signal; });
    if (found == signal_map.end()) {
        SPDLOG_WARN("received unknown signal, terminating");
    } else {
        SPDLOG_WARN("received {}, terminating", found->second);
    }

    terminate();
}

} // namespace upp
