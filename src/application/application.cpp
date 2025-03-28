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

#include "application/application.hpp"
#include "base/canvas.hpp"
#include "buildconfig.hpp"
#include "cli.hpp"
#include "os/os.hpp"
#include "unix/socket.hpp"
#include "util/result.hpp"
#include "util/util.hpp"

#include <CLI/CLI.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#ifdef ENABLE_LIBVIPS
#include <vips/vips.h>
#endif

#include <algorithm>
#include <array>
#include <csignal>
#include <fstream>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace upp
{

Application::Application(Cli *cli) :
    cli(cli)
{
}

auto Application::run() -> Result<void>
{
    return set_silent().and_then([this] { return setup_logging(); }).and_then([this] { return handle_cli_commands(); });
}

auto Application::handle_cli_commands() -> Result<void>
{
    if (cli->layer_command->parsed()) {
        print_header();
        setup_signal_handler();
        return setup_vips()
            .and_then([this] { return ctx->init(cli->layer.output); })
            .and_then([this] { return daemonize(); })
            .and_then([this] { return Canvas::create(ctx.get()); })
            .and_then([this](CanvasPtr new_canvas) {
                canvas = std::move(new_canvas);
                return canvas->init();
            })
            .and_then([this] { return command_listener.start(cli->layer.parser, cli->layer.no_stdin); })
            .and_then([this] { return wait_for_layer_commands(); });
    }
    if (cli->cmd_command->parsed()) {
        return handle_cmd_subcommand();
    }

    return {};
}

auto Application::handle_cmd_subcommand() -> Result<void>
{
    auto payload = cli->cmd.get_json_string();
    if (payload.empty()) {
        return {};
    }
    unix::socket::Client client;
    auto result = client.connect_and_write(cli->cmd.socket, util::make_buffer(payload));
    if (!result) {
        LOG_DEBUG("could not send command: {}", result.error().message());
    }
    return {};
}

auto Application::wait_for_layer_commands() -> Result<void>
{
    command_thread = jthread([this](auto token) { execute_layer_commands(token); });
    stop_flag.wait(false);
#ifdef ENABLE_LIBVIPS
    vips_shutdown();
#endif
    LOG_INFO("ueberzugpp terminated");
    return {};
}

void Application::execute_layer_commands(SToken token)
{
    while (!token.stop_requested()) {
        if (auto cmd = queue.try_dequeue(os::waitms)) {
            std::scoped_lock state_lock{ctx->state_mutex};
            canvas->execute(*cmd);
        } else {
            continue;
        }
    }
}

void Application::print_header()
{
    constexpr auto *art = R"(starting
 _   _      _
| | | |    | |                                _     _
| | | | ___| |__   ___ _ __ _____   _  __ _ _| |_ _| |_
| | | |/ _ \ '_ \ / _ \ '__|_  / | | |/ _` |_   _|_   _|   version: {}
| |_| |  __/ |_) |  __/ |   / /| |_| | (_| | |_|   |_|     build date: {}
 \___/ \___|_.__/ \___|_|  /___|\__,_|\__, |
                                       __/ |    new
                                      |___/)";
    LOG_INFO(art, version_str, build_date);
}

auto Application::setup_logging() -> Result<void>
{
#ifdef DEBUG
    auto level = spdlog::level::debug;
#else
    auto level = spdlog::level::info;
#endif

    auto log_file = util::get_log_filename();
    auto log_path = util::temp_directory_path() / log_file;

    try {
        spdlog::set_pattern("[%Y-%m-%d %T.%F] %^[%L]%$ [%n] %v");
        spdlog::set_level(level);
        spdlog::flush_on(level);

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_path);
        auto stderr_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
        auto dist_sink = std::make_shared<spdlog::sinks::dist_sink_mt>();
        dist_sink->add_sink(stderr_sink);
        dist_sink->add_sink(file_sink);

        logger = std::make_shared<spdlog::logger>("application", dist_sink);
        auto term = std::make_shared<spdlog::logger>("terminal", dist_sink);
        auto x11 = std::make_shared<spdlog::logger>("X11", dist_sink);
        auto vips = std::make_shared<spdlog::logger>("vips", dist_sink);
        auto listener = std::make_shared<spdlog::logger>("listener", dist_sink);
        auto wayland = std::make_shared<spdlog::logger>("wayland", dist_sink);
        auto hyprland = std::make_shared<spdlog::logger>("hyprland", dist_sink);
        auto socket = std::make_shared<spdlog::logger>("socket", dist_sink);

        spdlog::initialize_logger(logger);
        spdlog::initialize_logger(term);
        spdlog::initialize_logger(x11);
        spdlog::initialize_logger(vips);
        spdlog::initialize_logger(listener);
        spdlog::initialize_logger(wayland);
        spdlog::initialize_logger(hyprland);

        spdlog::set_default_logger(logger);
    } catch (const spdlog::spdlog_ex &ex) {
        return Err("spdlog", ex);
    }

    spdlog::cfg::load_env_levels();
    return {};
}

auto Application::setup_vips() -> Result<void>
{
#ifdef ENABLE_LIBVIPS
    if (VIPS_INIT("ueberzugpp")) {
        return Err("can't startup vips");
    }
    vips_cache_set_max(0);
    LOG_DEBUG("libvips initialized");
#endif
    return {};
}

void Application::terminate()
{
    stop_flag.test_and_set();
    stop_flag.notify_one();
}

void Application::setup_signal_handler()
{
    LOG_DEBUG("setting up signal handler");
    struct sigaction sga{};
    sga.sa_handler = signal_handler;
    sigemptyset(&sga.sa_mask);
    sga.sa_flags = 0;
    sigaction(SIGINT, &sga, nullptr);
    sigaction(SIGTERM, &sga, nullptr);
    sigaction(SIGHUP, &sga, nullptr);
    sigaction(SIGCHLD, nullptr, nullptr);

    struct sigaction sgaw{};
    sgaw.sa_handler = sigwinch_handler;
    sigemptyset(&sgaw.sa_mask);
    sgaw.sa_flags = 0;
    sigaction(SIGWINCH, &sgaw, nullptr);
}

void Application::signal_handler(int signal)
{
    using pair_t = std::pair<int, std::string_view>;
    constexpr auto signal_map = std::to_array<pair_t>({
        // clang-format off
        {SIGINT, "SIGINT"},
        {SIGTERM, "SIGTERM"},
        {SIGHUP, "SIGHUP"} // clang-format on
    });

    const auto *found = std::ranges::find_if(signal_map, [signal](const pair_t &pair) { return pair.first == signal; });
    auto logger = spdlog::get("application");
    if (found == signal_map.end()) {
        LOG_WARN("received unknown signal, terminating");
    } else {
        LOG_WARN("received {}, terminating", found->second);
    }

    terminate();
}

void Application::sigwinch_handler([[maybe_unused]] int signal)
{
    auto ctx = ApplicationContext::get();
    if (!ctx->is_initialized) {
        return;
    }
    auto logger = ctx->logger;
    LOG_DEBUG("received SIGWINCH, recalculating terminal state");
    std::scoped_lock state_lock{ctx->state_mutex};
    auto state = ctx->terminal.load_state();
    if (!state) {
        LOG_WARN(state.error().message());
    }
}

auto Application::daemonize() -> Result<void>
{
    if (!cli->layer.no_stdin) {
        return {};
    }
    return os::daemonize().and_then([this]() -> Result<void> {
        std::ofstream ofs(cli->layer.pid_file);
        ofs << os::getpid() << std::flush;
        return {};
    });
}

auto Application::set_silent() const -> Result<void>
{
    if (!cli->layer.silent) {
        return {};
    }
    return os::close_stderr();
}

} // namespace upp
