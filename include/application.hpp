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

#pragma once

#include <atomic>

#include "command/command.hpp"
#include "error.hpp"
#include "terminal.hpp"

class Application
{
  public:
    auto init() -> Result<void>;

    static void signal_handler(int signal);
    static void terminate();
    static auto setup_signal_handler() -> Result<void>;
    static auto setup_logger() -> Result<void>;
    static auto run() -> Result<void>;

    inline static std::atomic_flag stop_flag_ = ATOMIC_FLAG_INIT;

  private:
    Terminal terminal_;
    CommandQueue queue_;
};
