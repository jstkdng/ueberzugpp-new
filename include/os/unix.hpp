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

#include <span>
#include <string>
#include <string_view>

namespace upp::unix
{

class fd
{
  public:
    ~fd();

    // rule of 5
    fd() = default;
    fd(const fd &other) = delete;
    auto operator=(const fd &other) -> fd & = delete;
    fd(fd &&other) noexcept;
    auto operator=(fd &&other) noexcept -> fd &;

    explicit fd(int descriptor);

    auto operator=(int new_fd) -> fd &;
    explicit operator bool() const;

    [[nodiscard]] auto dup() const -> fd;
    [[nodiscard]] auto get() const -> int;

  private:
    int descriptor = -1;
};

namespace socket
{

class Client
{
  public:
    explicit Client(std::string_view endpoint);
    static void connect_and_write(std::string_view endpoint, std::span<const std::byte> buffer);
    void write(std::span<const std::byte> buffer) const;
    void read(std::span<std::byte> buffer) const;
    [[nodiscard]] auto read_until_empty() const -> std::string;

  private:
    fd sockfd;
};

class Server
{
  public:
    ~Server();
    void start();
    [[nodiscard]] auto get_fd() const -> int;
    [[nodiscard]] auto get_endpoint() const -> std::string;
    [[nodiscard]] auto read_data_from_connection() const -> std::string;

  private:
    fd sockfd;
    std::string endpoint;

    void create_socket();
    void bind_to_endpoint() const;
    void listen_for_connections() const;
};

} // namespace socket

} // namespace upp::unix
