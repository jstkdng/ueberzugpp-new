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
#include "os/os.hpp"
#include "result.hpp"
#include "unix/socket.hpp"

#include <CLI/CLI.hpp>
#include <span>
#include <spdlog/common.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <format>
#include <string>

namespace upp
{

auto test(unix::socket::Client &client) -> Result<void>
{
    std::string input = std::format("[[BATCH]]j/{0};j/{0}", "activewindow");
    auto result = client.write(std::as_bytes(std::span{input}));
    if (!result) {
        SPDLOG_WARN(result.error().lmessage());
    }
    SPDLOG_INFO(client.read_until_empty());
    return {};
}

Application::Application(CLI::App *app) :
    app(app)
{
}

auto Application::run() -> Result<void>
{
    unix::socket::Client client;
    return setup_logging()
        .and_then([this] -> Result<void> { return terminal.open_first_pty(); })
        .and_then([&client] {
            const auto signature = os::getenv("HYPRLAND_INSTANCE_SIGNATURE").value_or("");
            const auto socket_base_dir = os::getenv("XDG_RUNTIME_DIR").value_or("/tmp");
            const auto socket_rel_path = std::format("hypr/{}/.socket.sock", signature);
            const auto socket_path = std::format("{}/{}", socket_base_dir, socket_rel_path);
            return client.connect(socket_path);
        })
        .and_then([&client] { return test(client); });
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

} // namespace upp
