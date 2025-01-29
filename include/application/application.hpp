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
#include "command.hpp"
#include "log.hpp"
#include "util/result.hpp"
#include "util/thread.hpp"

#include <CLI/CLI.hpp>
#include <atomic>

namespace upp
{

class Application
{
  public:
    explicit Application(Cli *cli);

    auto run() -> Result<void>;

    static void terminate();
    static void signal_handler(int signal);

    inline static std::atomic_flag stop_flag = ATOMIC_FLAG_INIT;

  private:
    Cli *cli;
    CommandQueue queue;
    CommandListener listener{&queue};
    ApplicationContext ctx;
    CanvasPtr canvas;
    Logger logger;

    std::jthread command_thread;

    void print_header();
    void setup_signal_handler();
    auto setup_vips() -> Result<void>;
    auto setup_logging() -> Result<void>;
    auto handle_cli_commands() -> Result<void>;
    auto wait_for_layer_commands() -> Result<void>;
    void execute_layer_commands(const std::stop_token &token);
};

} // namespace upp
