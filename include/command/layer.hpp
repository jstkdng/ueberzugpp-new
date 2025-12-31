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

#include "util/fwd.hpp"

#include <atomic>
#include <string>

namespace upp
{

struct LayerOptions {
    bool silent = false;
    bool use_escape_codes = false;
    bool no_stdin = false;
    bool no_cache = false;
    bool origin_center = false;

    std::string pid_file;
    std::string parser = "json";
    std::string output;
};

class LayerCommand
{
  public:
    explicit LayerCommand(LayerOptions *opts);
    void execute();

    static void setup(CLI::App &cli);
    static void setup_vips();
    static void print_header();
    static void terminate();
    static void setup_signal_handler();
    static void signal_handler(int signal);
    static void sigwinch_handler(int signal);

    inline static std::atomic_flag stop_flag = ATOMIC_FLAG_INIT;

  private:
    LayerOptions *opts;

    void close_stderr();
    void daemonize();
};

} // namespace upp
