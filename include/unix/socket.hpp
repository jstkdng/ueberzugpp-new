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
#include "util/thread.hpp"
#include "log.hpp"
#include "util/concurrent_deque.hpp"

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <optional>

namespace upp::unix::socket
{

class Client
{
  public:
    auto connect(std::string_view endpoint) -> Result<void>;
    [[nodiscard]] auto write(std::span<const std::byte> buffer) const -> Result<void>;
    [[nodiscard]] auto read(std::span<std::byte> buffer) const -> Result<void>;
    [[nodiscard]] auto read_until_empty() const -> Result<std::string>;

  private:
    fd sockfd;
};

class Server
{
  public:
    auto start() -> Result<void>;
    auto dequeue_message() -> std::optional<std::string>;

  private:
    Logger logger{spdlog::get("socket")};
    fd sockfd;
    std::string endpoint;

    std::jthread accept_thread;
    ConcurrentDeque<std::string> messages;

    void accept_connections(const std::stop_token &token);
    void read_data();
};

} // namespace upp::unix::socket
