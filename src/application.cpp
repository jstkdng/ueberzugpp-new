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
#include "buildconfig.hpp"
#include "command/dispatch.hpp"
#include "command/layer.hpp"

#include <spdlog/cfg/env.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <CLI/CLI.hpp>

#include <memory>

namespace upp
{

auto Application::run(int argc, char **argv) -> int
{
    setup_cli();
    setup_logging();
    try {
        cli.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return cli.exit(e);
    }
    return 0;
}

void Application::setup_cli()
{
    cli.set_version_flag("-V", full_version_str);
    cli.allow_extras(false);
    cli.require_subcommand(1);

    LayerCommand::setup(cli);
    DispatchCommand::setup(cli);

    auto *query_win_command =
        cli.add_subcommand("query_windows", "**UNUSED**, only present for backwards compatibility");
    query_win_command->allow_extras();
}

void Application::setup_logging()
{
#ifdef DEBUG
    auto level = spdlog::level::trace;
#else
    auto level = spdlog::level::info;
#endif

    // auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_path);
    auto stderr_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    auto dist_sink = std::make_shared<spdlog::sinks::dist_sink_mt>();
    dist_sink->add_sink(stderr_sink);
    // dist_sink->add_sink(file_sink);

    auto logger = std::make_shared<spdlog::logger>("ueberzugpp", dist_sink);
    spdlog::initialize_logger(logger);

    logger->set_pattern("[%Y-%m-%d %T.%F] %^[%L]%$ [%@] %v");
    logger->set_level(level);
    logger->flush_on(level);

    spdlog::set_default_logger(logger);
    spdlog::cfg::load_env_levels();
}

} // namespace upp
