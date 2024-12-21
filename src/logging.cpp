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

#include "logging.hpp"
#include "config.hpp"
#include "error.hpp"

#include <spdlog/common.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

auto LoggingManager::init(const ProgramConfig &config) -> Result<void>
{
    if (config.logging.silent) {
        spdlog::set_level(spdlog::level::off);
        return {};
    }

#ifdef DEBUG
    auto level = spdlog::level::trace;
#else
    auto level = spdlog::level::info;
#endif

    spdlog::set_level(level);
    spdlog::flush_on(level);

    try {
        const auto logger = spdlog::stdout_color_mt("main");
        spdlog::set_default_logger(logger);
    } catch (const spdlog::spdlog_ex &ex) {
        return Err("LoggingManager", ex);
    }

    return {};
}
