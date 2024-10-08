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

#ifndef OS_HPP
#define OS_HPP

#include <expected>
#include <optional>
#include <source_location>
#include <string>
#include <string_view>

namespace os
{

constexpr int bufsize = 4 * 1024; // 4K at a time

auto system_error(std::string_view message = "os error",
                  std::source_location location = std::source_location::current()) noexcept
    -> std::unexpected<std::string>;
auto get_poll_err(int event) noexcept -> std::string_view;
auto exec(const std::string &cmd) noexcept -> std::expected<std::string, std::string>;
auto getenv(const std::string &var) noexcept -> std::optional<std::string>;

auto get_pid() noexcept -> int;
auto get_ppid() noexcept -> int;

auto read_data_from_fd(int filde) noexcept -> std::expected<std::string, std::string>;
auto read_data_from_socket(int sockfd) noexcept -> std::expected<std::string, std::string>;
auto read_data_from_stdin() noexcept -> std::expected<std::string, std::string>;

auto wait_for_data_on_fd(int filde, int waitms) noexcept -> std::expected<bool, std::string>;
auto wait_for_data_on_stdin(int waitms) noexcept -> std::expected<bool, std::string>;

auto daemonize() -> std::expected<void, std::string>;
auto close_stderr() -> std::expected<void, std::string>;

} // namespace os

#endif // OS_HPP
