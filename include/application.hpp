// Display images inside a terminal
// Copyright (C) 2023  JustKidding
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

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "canvas/canvas.hpp"
#include "command.hpp"
#include "config.hpp"
#include "terminal.hpp"

#include <atomic>
#include <expected>
#include <memory>

class Application
{
  public:
    ~Application();

    auto initialize() noexcept -> std::expected<void, std::string>;

    void run() const;
    static auto get_version() -> std::string;
    static void print_header();
    static auto setup_loggers() -> std::expected<void, std::string>;
    inline static std::atomic_bool stop_flag = false; // NOLINT

  private:
    std::shared_ptr<Config> config = Config::instance();
    Terminal terminal;
    CommandManager command_manager;
    std::unique_ptr<Canvas> canvas;

    [[nodiscard]] auto daemonize() const -> std::expected<void, std::string>;
};

#endif // APPLICATION_HPP
