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

#include "result.hpp"
#include "terminal.hpp"

#include <CLI/CLI.hpp>
#include <spdlog/logger.h>

#include <memory>

namespace upp
{

class Application
{
  public:
    explicit Application(CLI::App *app);

    auto setup_logging() -> Result<void>;
    auto run() -> Result<void>;

  private:
    CLI::App *app = nullptr;
    terminal::Context terminal;

    std::shared_ptr<spdlog::logger> logger;
};

} // namespace upp
