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

#include "application/context.hpp"
#include "base/canvas.hpp"
#include "cli.hpp"
#include "command/command.hpp"
#include "command/listener.hpp"
#include "log.hpp"
#include "util/result.hpp"
#include "util/thread.hpp"

#include <CLI/CLI.hpp>

#include <atomic>
#include <memory>

namespace upp
{

class Application
{
  public:
    explicit Application(Cli *cli);

    auto run() -> Result<void>;

    static void terminate();
    static void signal_handler(int signal);
    static void sigwinch_handler(int signal);

    inline static std::atomic_flag stop_flag = ATOMIC_FLAG_INIT;

  private:
    Cli *cli;
    std::shared_ptr<ApplicationContext> ctx{ApplicationContext::get()};
    CommandQueue queue;
    CommandListener command_listener{&queue};
    CanvasPtr canvas;
    Logger logger;

    jthread command_thread;

    void print_header();
    void setup_signal_handler();
    auto daemonize() -> Result<void>;
    auto setup_vips() -> Result<void>;
    auto setup_logging() -> Result<void>;
    auto handle_cli_commands() -> Result<void>;
    auto handle_cmd_subcommand() -> Result<void>;
    auto wait_for_layer_commands() -> Result<void>;
    [[nodiscard]] auto set_silent() const -> Result<void>;
    void execute_layer_commands(SToken token);
};

} // namespace upp
