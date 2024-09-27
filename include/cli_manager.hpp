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

#include <span>

#include <CLI11.hpp>

#include "error.hpp"

class CliManager
{
  public:
    auto init(std::span<char *> args) -> Result<void>;

  private:
    void setup_layer_command();

    CLI::App program{"Display images in the terminal", "ueberzugpp"};
    CLI::App *layer_command{program.add_subcommand("layer", "Display images on the terminal")};
};
