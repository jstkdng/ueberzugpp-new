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

#ifndef CLI_HPP
#define CLI_HPP

#include "config.hpp"

#include <memory>
#include <span>

#include "CLI11/CLI11.hpp"

namespace cli
{

struct cmd_subcommand {
    std::string id;
    std::string action;
    std::string socket;
    std::string file_path;

    int x = -1;
    int y = -1;
    int max_width = -1;
    int max_height = -1;
};

class Manager
{
  public:
    Manager();
    auto initialize(std::span<char *> args) -> int;

  private:
    CLI::App program;
    std::shared_ptr<Config> config = Config::instance();

    cmd_subcommand cmd{};

    void setup_layer_subcommand();
    void setup_cmd_subcommand();
};

} // namespace cli

#endif // CLI_HPP
