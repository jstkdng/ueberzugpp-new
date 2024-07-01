// Display images inside a terminal
// Copyright (C) 2023  JustKidding
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

#include "application.hpp"
#include "util.hpp"
#include "version.hpp"

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

Application::~Application()
{
    fs::remove(util::get_socket_path());
    SPDLOG_INFO("ueberzugpp terminated");
}

void Application::run()
{
    constexpr int waitms = 10;
    while (!stop_flag) {
        std::this_thread::sleep_for(std::chrono::milliseconds(waitms));
    }
}

auto Application::initialize() noexcept -> std::expected<void, std::string>
{
    print_header();
    auto result = setup_loggers();
    if (result) {
#ifdef DEBUG
        SPDLOG_INFO("starting ueberzugpp v{}.{}.{} (debug build)", UEBERZUGPP_VERSION_MAJOR, UEBERZUGPP_VERSION_MINOR,
                    UEBERZUGPP_VERSION_PATCH);
#else
        SPDLOG_INFO("starting ueberzugpp v{}.{}.{}", UEBERZUGPP_VERSION_MAJOR, UEBERZUGPP_VERSION_MINOR,
                    UEBERZUGPP_VERSION_PATCH);
#endif
    }
    return result.and_then([this] { return terminal.initialize(); })
        .and_then([this] { return daemonize(); })
        .and_then([this] { return command_manager.initialize(); })
        .and_then([this] {
            auto canvas_ok = Canvas::create();
            if (canvas_ok) {
                canvas = std::move(*canvas_ok);
            }
            return canvas_ok;
        })
        .and_then([this](auto...) { return canvas->initialize(&command_manager); });
}

auto Application::setup_loggers() -> std::expected<void, std::string>
{
    using spdlog::set_default_logger;
    using spdlog::set_pattern;

    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::trace);

    const auto log_path = util::get_log_path();
    try {
        const auto main_logger = spdlog::basic_logger_mt("main", log_path);
        set_default_logger(main_logger);
        set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%L] [%s:%#] %v");
    } catch (const spdlog::spdlog_ex &ex) {
        return std::unexpected(std::format("log init failed: {}", ex.what()));
    }

    return {};
}

auto Application::daemonize() const -> std::expected<void, std::string>
{
    if (!config->no_stdin) {
        return {};
    }
    auto daemon_ok = os::daemonize();
    if (daemon_ok) {
        std::ofstream ofs(config->pid_file);
        ofs << os::get_pid() << std::flush;
    }
    return daemon_ok;
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

auto Application::get_version() -> std::string
{
    return std::format("ueberzugpp {}.{}.{}", UEBERZUGPP_VERSION_MAJOR, UEBERZUGPP_VERSION_MINOR,
                       UEBERZUGPP_VERSION_PATCH);
}
