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

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <expected>
#include <memory>
#include <string>

class Config
{
  public:
    static auto instance() -> std::shared_ptr<Config>
    {
        struct enabler : Config {
        };
        static auto ptr = std::make_shared<enabler>();
        return ptr;
    }

    Config(const Config &) = delete;
    Config(Config &&) = delete;
    Config &operator=(const Config &) = delete;
    Config &operator=(Config &&) = delete;

    auto read_config_file() -> std::expected<void, std::string>;

    // configurable with config file
    bool silent = false;
    bool no_cache = false;
    bool no_opencv = false;
    bool use_opengl = false;
    std::string output;

    // configurable with cmd line switches
    bool print_version = false;
    bool use_escape_codes = false;
    bool no_stdin = false;
    bool origin_center = false;
    std::string pid_file;

    // for the cmd subcommand
    std::string cmd_id;
    std::string cmd_action;
    std::string cmd_socket;
    std::string cmd_x;
    std::string cmd_y;
    std::string cmd_max_width;
    std::string cmd_max_height;
    std::string cmd_file_path;

  private:
    Config();

    std::string config_file;
};

#endif // CONFIG_HPP
