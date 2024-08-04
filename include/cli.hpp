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

#include <expected>
#include <memory>
#include <span>

#include <CLI11.hpp>

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

    [[nodiscard]] auto get_json() const noexcept -> std::string;
};

struct tmux_subcommand {
    std::string hook;
    int pid = -1;

    [[nodiscard]] auto get_json() const noexcept -> std::string;
};

class Manager
{
  public:
    auto initialize(std::span<char *> args) -> std::expected<void, int>;

    [[nodiscard]] auto handle_tmux_subcommand() const noexcept -> std::expected<void, std::string>;
    [[nodiscard]] auto handle_cmd_subcommand() const noexcept -> std::expected<void, std::string>;
    [[nodiscard]] auto handle_layer_subcommand() const noexcept -> std::expected<void, std::string>;

  private:
    CLI::App program{"Display images in the terminal", "ueberzugpp"};
    CLI::App *layer_command{program.add_subcommand("layer", "Display images on the terminal.")};
    CLI::App *cmd_command{program.add_subcommand("cmd", "Send a command to a running ueberzugpp instance.")};
    CLI::App *tmux_command{program.add_subcommand("tmux", "Handle tmux hooks. Used internaly.")};

    std::shared_ptr<Config> config = Config::instance();

    cmd_subcommand cmd{};
    tmux_subcommand tmux{};

    void setup_layer_subcommand() const noexcept;
    void setup_cmd_subcommand() noexcept;
    void setup_tmux_subcommand() noexcept;
};

} // namespace cli

#endif // CLI_HPP
