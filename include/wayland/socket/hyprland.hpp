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

#include "log.hpp"
#include "wayland/socket/socket.hpp"

#include <array>
#include <string>
#include <string_view>

namespace upp
{

class HyprlandSocket : public WaylandSocket
{
  public:
    explicit HyprlandSocket(std::string instance_signature);
    auto setup(std::string_view app_id, int xcoord, int ycoord) -> Result<void> override;
    auto active_window(int pid) -> WaylandGeometry override;

  private:
    Logger logger{spdlog::get("hyprland")};
    std::string signature;
    std::string socket_path;

    void get_version();
    void request(std::string_view payload);
    auto request_result(std::string_view payload) -> std::string;
};

struct HyprlandVersion {
    std::string branch;
    std::string commit;
    std::string version;
};

struct HyprlandClient {
    std::array<int, 2> at;
    std::array<int, 2> size;
    int monitor;
    int pid;
};

} // namespace upp
