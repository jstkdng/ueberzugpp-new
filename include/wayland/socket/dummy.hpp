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

namespace upp
{

class DummySocket : public WaylandSocket
{
  public:
    DummySocket() { LOG_WARN("using dummy socket, functionality will be limited"); };
    auto setup([[maybe_unused]] std::string_view app_id, [[maybe_unused]] int xcoord, [[maybe_unused]] int ycoord)
        -> Result<void> override
    {
        return {};
    }

    auto active_window([[maybe_unused]] int pid) -> WaylandGeometry override { return {}; }

  private:
    Logger logger{spdlog::get("wayland")};
};

}; // namespace upp
