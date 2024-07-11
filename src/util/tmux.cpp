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

#include "util/tmux.hpp"
#include "os/os.hpp"

#include <array>
#include <format>
#include <span>
#include <spdlog/spdlog.h>

auto TmuxClient::initialize() -> std::expected<void, std::string>
{
    const auto tmux_env = os::getenv("TMUX");
    if (!tmux_env) {
        return std::unexpected("not running tmux");
    }
    const std::string_view view(*tmux_env);
    const auto comma = view.find(',');
    const auto sockpath = view.substr(0, comma);
    auto client_ok = client.initialize(sockpath);
    if (client_ok) {
        SPDLOG_INFO("connected to tmux socket {}", sockpath);
    }
    return client_ok;
}

auto TmuxClient::get_pane() -> std::expected<std::string, std::string>
{
    auto env = os::getenv("TMUX_PANE");
    if (!env) {
        return std::unexpected("TMUX_PANE not set");
    }
    return *env;
}

auto TmuxClient::get_session_id() const -> std::expected<std::string, std::string>
{
    auto pane_val = get_pane();
    if (!pane_val) {
        return std::unexpected(pane_val.error());
    }
    const auto cmd = std::to_array<std::string_view>({"display", "-p", "-F", "'#{session_id}'", "-t", *pane_val});
    std::vector<char> payload;
    payload.push_back(cmd.size());
    payload.push_back(0);
    for (auto str : cmd) {
        payload.insert(payload.end(), str.begin(), str.end());
        payload.push_back(0);
    }
    auto write_ok = client.write(std::as_bytes(std::span{payload}));
    if (!write_ok) {
        SPDLOG_INFO(write_ok.error());
    }
    std::vector<char> buffer(100);
    auto read_ok = client.read(std::as_writable_bytes(std::span{buffer}));
    if (!read_ok) {
        SPDLOG_INFO(read_ok.error());
    }
    return "nice";
}
