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

#include <iostream>
#include <spdlog/sinks/basic_file_sink.h>

Application::Application()
    : command_manager(socket_path)
{
    setup_loggers();
}

void Application::setup_loggers()
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
        std::cerr << "Log init failed: " << ex.what() << '\n';
    }
}
