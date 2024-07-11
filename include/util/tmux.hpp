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

#ifndef TMUX_HPP
#define TMUX_HPP

#include "unix_socket.hpp"

#include <expected>
#include <string>
#include <string_view>

class TmuxClient
{
  public:
    auto initialize() -> std::expected<void, std::string>;

    auto get_session_id() const -> std::expected<std::string, std::string>;
    static auto get_pane() -> std::expected<std::string, std::string>;

  private:
    unix_socket::Client client;
};

#endif // TMUX_HPP
