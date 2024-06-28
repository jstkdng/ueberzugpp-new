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

#include <filesystem>
#include <format>
#include <fstream>
#include <spdlog/sinks/basic_file_sink.h>

namespace fs = std::filesystem;

Application::Application()
    : command_manager(socket_path)
{
}

Application::~Application()
{
    logger->info("Exiting ueberzugpp");
    fs::remove(util::get_socket_path());
}

auto Application::initialize() -> std::expected<void, std::string>
{
    print_header();
    return setup_loggers().and_then([this] { return command_manager.initialize(); });
}

auto Application::setup_loggers() -> std::expected<void, std::string>
{
    using spdlog::initialize_logger;
    using spdlog::logger;

    const auto log_path = util::get_log_path();
    try {
        spdlog::set_level(spdlog::level::trace);
        spdlog::flush_on(spdlog::level::trace);

        const auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_path);
        const auto main_logger = std::make_shared<logger>("main", sink);
        const auto command = std::make_shared<logger>("command", sink);

        initialize_logger(main_logger);
        initialize_logger(command);

        this->logger = spdlog::get("main");
    } catch (const spdlog::spdlog_ex &ex) {
        return std::unexpected(std::format("log init failed: {}", ex.what()));
    }

    return {};
}

void Application::print_header()
{
    const auto log_path = util::get_log_path();
    const auto art = std::format(R"(
 _   _      _
| | | |    | |                                _     _
| | | | ___| |__   ___ _ __ _____   _  __ _ _| |_ _| |_
| | | |/ _ \ '_ \ / _ \ '__|_  / | | |/ _` |_   _|_   _|
| |_| |  __/ |_) |  __/ |   / /| |_| | (_| | |_|   |_|
 \___/ \___|_.__/ \___|_|  /___|\__,_|\__, |
                                       __/ |
                                      |___/     v{}.{}.{})",
                                 UEBERZUGPP_VERSION_MAJOR, UEBERZUGPP_VERSION_MINOR, UEBERZUGPP_VERSION_PATCH);
    std::ofstream ofs(log_path, std::ios::out | std::ios::app);
    ofs << art << '\n' << std::flush;
}
