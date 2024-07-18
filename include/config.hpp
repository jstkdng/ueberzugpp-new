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

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <expected>
#include <memory>
#include <string>

enum class opencl_status : std::uint8_t { UNCHECKED, AVAILABLE, UNAVAILABLE };

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

    auto operator=(Config &&) -> Config & = delete;

    auto read_config_file() -> std::expected<void, std::string>;

    // some globals
    const int waitms = 50;
    opencl_status ocl = opencl_status::UNCHECKED;
    bool vips_initialized = false;

    // configurable with config file
    bool silent = false;
    bool no_cache = false;
    bool no_opencv = false;
    bool use_opengl = false;
    std::string output;

    // configurable with cmd line switches
    bool use_escape_codes = false;
    bool no_stdin = false;
    bool origin_center = false;
    std::string pid_file;

  private:
    Config();

    std::string config_file;
};

#endif // CONFIG_HPP
