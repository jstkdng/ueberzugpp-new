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

#pragma once

#include <spdlog/spdlog.h>

#include <memory>

#define LOG_TRACE(...) SPDLOG_LOGGER_TRACE(logger __VA_OPT__(, ) __VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(logger __VA_OPT__(, ) __VA_ARGS__)
#define LOG_INFO(...) SPDLOG_LOGGER_INFO(logger __VA_OPT__(, ) __VA_ARGS__)
#define LOG_WARN(...) SPDLOG_LOGGER_WARN(logger __VA_OPT__(, ) __VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_LOGGER_ERROR(logger __VA_OPT__(, ) __VA_ARGS__)

namespace upp
{
using Logger = std::shared_ptr<spdlog::logger>;
} // namespace upp
