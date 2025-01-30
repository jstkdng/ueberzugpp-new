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

#include "wayland/socket/hyprland.hpp"
#include "os/os.hpp"
#include "unix/socket.hpp"

#include <glaze/glaze.hpp>
#include <glaze/json/json_t.hpp>
#include <glaze/json/read.hpp>
#include <spdlog/spdlog.h>

#include <format>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace upp
{

HyprlandSocket::HyprlandSocket(std::string instance_signature) :
    signature(std::move(instance_signature))
{
    auto socket_base_dir = os::getenv("XDG_RUNTIME_DIR").value_or("/tmp");
    auto socket_rel_path = std::format("hypr/{}/.socket.sock", signature);
    socket_path = std::format("{}/{}", socket_base_dir, socket_rel_path);
    logger->info("using socket {}", socket_path);
    get_version();
}

void HyprlandSocket::get_version()
{
    auto ver_str = request_result("j/version");
    glz::json_t json;
    auto res = glz::read_json(json, ver_str);
    if (!res) {
        logger->info("version: {}", json["version"].get_string());
    } else {
        logger->info("could not find hyprland version");
    }
}

void HyprlandSocket::request(std::string_view payload)
{
    unix::socket::Client client;
    auto result = client.connect(socket_path).and_then([&client, payload] {
        return client.write(std::as_bytes(std::span{payload.data(), payload.size()}));
    });
    if (!result) {
        logger->error(result.error().message());
    }
}

auto HyprlandSocket::request_result(std::string_view payload) -> std::string
{
    unix::socket::Client client;
    auto result = client.connect(socket_path)
                      .and_then([&client, payload] {
                          return client.write(std::as_bytes(std::span{payload.data(), payload.size()}));
                      })
                      .and_then([&client] { return client.read_until_empty(); });
    if (!result) {
        logger->error(result.error().message());
        return {};
    }

    return *result;
}

} // namespace upp
