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

#include "unix/fd.hpp"
#include "util/result.hpp"

#include <cstddef>
#include <span>
#include <string>
#include <string_view>

namespace upp::unix::socket
{

class Client
{
  public:
    auto connect(std::string_view endpoint) -> Result<void>;
    auto connect_and_write(std::string_view endpoint, std::span<const std::byte> buffer) -> Result<void>;
    [[nodiscard]] auto write(std::span<const std::byte> buffer) const -> Result<void>;
    [[nodiscard]] auto read(std::span<std::byte> buffer) const -> Result<void>;
    [[nodiscard]] auto read_until_empty() const -> Result<std::string>;

  private:
    fd sockfd;
};

class Server
{
  public:
    ~Server();
    auto start() -> Result<void>;
    [[nodiscard]] auto get_fd() const -> int;
    [[nodiscard]] auto get_endpoint() const -> std::string;
    [[nodiscard]] auto read_data_from_connection() const -> Result<std::string>;

  private:
    fd sockfd;
    std::string endpoint;

    auto create_socket() -> Result<void>;
    [[nodiscard]] auto bind_to_endpoint() const -> Result<void>;
    [[nodiscard]] auto listen_for_connections() const -> Result<void>;
};

} // namespace upp::unix::socket
