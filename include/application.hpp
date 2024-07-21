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

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "canvas/canvas.hpp"
#include "command/command_listener.hpp"
#include "config.hpp"
#include "terminal.hpp"

#include "moodycamel/blockingconcurrentqueue.h"

#include <atomic>
#include <expected>
#include <memory>

class Application
{
  public:
    Application();
    ~Application();

    auto initialize() noexcept -> std::expected<void, std::string>;

    static void run();
    static void terminate();
    static void print_header();
    static auto setup_loggers() -> std::expected<void, std::string>;

    inline static std::atomic_flag stop_flag = ATOMIC_FLAG_INIT;

  private:
    std::shared_ptr<Config> config = Config::instance();
    moodycamel::BlockingConcurrentQueue<Command> command_queue;
    Terminal terminal;
    CommandListener command_listener;
    std::unique_ptr<Canvas> canvas;

    [[nodiscard]] auto daemonize() const -> std::expected<void, std::string>;
};

#endif // APPLICATION_HPP
