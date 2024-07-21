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

#include "application.hpp"
#include "util/util.hpp"
#include "version.hpp"

#include <filesystem>
#include <format>
#include <fstream>

#include <spdlog/cfg/env.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <vips/vips.h>

namespace fs = std::filesystem;

Application::Application()
{
    if (VIPS_INIT("ueberzugpp")) {
        vips_error_exit(nullptr);
    }
    vips_cache_set_max(1);
}

Application::~Application()
{
    fs::remove(util::get_socket_path());
    SPDLOG_INFO("ueberzugpp terminated");
}

void Application::run()
{
    stop_flag.wait(false);
}

void Application::terminate()
{
    stop_flag.test_and_set();
    stop_flag.notify_one();
}

auto Application::initialize() noexcept -> std::expected<void, std::string>
{
    print_header();
    SPDLOG_INFO("starting {}", version_str);
    return terminal.initialize()
        .and_then([this] { return daemonize(); })
        .and_then([this] { return command_listener.initialize(&command_queue); })
        .and_then([this] { return Canvas::create(config.get()); })
        .and_then([this](auto ptr) -> std::expected<void, std::string> {
            canvas = std::move(ptr);
            return {};
        })
        .and_then([this] { return canvas->initialize(&command_queue); });
}

auto Application::setup_loggers() -> std::expected<void, std::string>
{
    using spdlog::set_default_logger;
    using spdlog::set_pattern;

#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_on(spdlog::level::debug);
#else
    spdlog::flush_on(spdlog::level::info);
#endif

    try {
        const auto main_logger = spdlog::basic_logger_mt("main", util::get_log_path());
        set_default_logger(main_logger);
        set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%L] [%s:%#] %v");
    } catch (const spdlog::spdlog_ex &ex) {
        return util::unexpected_err(std::format("log init failed: {}", ex.what()));
    }

    return {};
}

auto Application::daemonize() const -> std::expected<void, std::string>
{
    if (!config->no_stdin) {
        return {};
    }
    return os::daemonize().and_then([this] -> std::expected<void, std::string> {
        std::ofstream ofs(config->pid_file);
        ofs << os::get_pid() << std::flush;
        return {};
    });
}

void Application::print_header()
{
    const auto log_path = util::get_log_path();
    const std::string art = R"(
 _   _      _
| | | |    | |                                _     _
| | | | ___| |__   ___ _ __ _____   _  __ _ _| |_ _| |_
| | | |/ _ \ '_ \ / _ \ '__|_  / | | |/ _` |_   _|_   _|
| |_| |  __/ |_) |  __/ |   / /| |_| | (_| | |_|   |_|
 \___/ \___|_.__/ \___|_|  /___|\__,_|\__, |
                                       __/ |    new
                                      |___/ )";
    std::ofstream ofs(log_path, std::ios::out | std::ios::app);
    ofs << art << '\n' << std::flush;
}
