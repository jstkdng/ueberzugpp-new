// Display images inside a terminal
// Copyright (C) 2024  JustKidding
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

#include "sub_commands/cmd.hpp"
#include "unix_socket.hpp"
#include "util.hpp"

#include <cstddef>
#include <format>

#include <nlohmann/json.hpp>

using njson = nlohmann::json;

auto cmd_subcommand::send() const -> std::expected<void, std::string>
{
    unix_socket::Client client;
    auto init_ok = client.initialize(socket);
    if (!init_ok) {
        return std::unexpected(init_ok.error());
    }

    njson json;
    if (action == "exit" || action == "flush") {
        json = {{"action", action}};
    }

    if (action == "remove") {
        json = {{"action", "remove"}, {"identifier", id}};
    }

    if (action == "add") {
        json = {{"action", "add"}, {"identifier", id}, {"max_width", max_width}, {"max_height", max_height},
                {"x", x},          {"y", y},           {"path", file_path}};
    }

    if (json.empty()) {
        return std::unexpected("unknown command");
    }

    auto payload = std::format("{}\n", json.dump());
    return client.write(std::bit_cast<const std::byte *>(payload.data()), payload.length());
}
