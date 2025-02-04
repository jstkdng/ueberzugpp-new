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
#include "util/util.hpp"

#include <glaze/glaze.hpp>
#include <spdlog/spdlog.h>

#include <format>
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

auto HyprlandSocket::setup(std::string_view app_id, int xcoord, int ycoord) -> Result<void>
{
    auto payload = std::format("[[BATCH]]"
                               "/keyword windowrulev2 nofocus,title:{0};"
                               "/keyword windowrulev2 float,title:{0};"
                               "/keyword windowrulev2 noborder,title:{0};"
                               //"/keyword windowrulev2 noanim,title:{0};"
                               "/keyword windowrulev2 rounding 0,title:{0};"
                               //"/dispatch movewindowpixel exact {1} {2},title:{0};",
                               "/keyword windowrulev2 move {1} {2},title:{0};",
                               app_id,
                               xcoord,
                               ycoord);
    unix::socket::Client client;
    return client.connect_and_write(socket_path, util::make_buffer(payload));
}

void HyprlandSocket::get_version()
{
    auto ver_str = request_result("j/version");
    glz::json_t json;
    if (auto err = glz::read_json(json, ver_str)) {
        logger->info("could not find hyprland version");
    } else {
        logger->info("version: {}", json["version"].get_string());
    }
}

auto HyprlandSocket::active_window() -> WaylandGeometry
{
    auto active = request_result("j/activewindow");
    glz::json_t json;
    if (auto err = glz::read_json(json, active)) {
        return {};
    }
    auto coords = json["at"].get_array();
    auto sizes = json["size"].get_array();
    return {
        .width = sizes[0].as<int>(),
        .height = sizes[1].as<int>(),
        .x = coords[0].as<int>(),
        .y = coords[1].as<int>(),
    };
}

void HyprlandSocket::request(std::string_view payload)
{
    unix::socket::Client client;
    auto result = client.connect_and_write(socket_path, util::make_buffer(payload));
    if (!result) {
        logger->error(result.error().message());
    }
}

auto HyprlandSocket::request_result(std::string_view payload) -> std::string
{
    unix::socket::Client client;
    auto result = client.connect_and_write(socket_path, util::make_buffer(payload)).and_then([&client] {
        return client.read_until_empty();
    });
    if (!result) {
        logger->error(result.error().message());
        return {};
    }

    return *result;
}

} // namespace upp
