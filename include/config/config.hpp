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

#include <string>

#include "error.hpp"

class Config
{
  public:
    auto read_config_file() -> Result<void>;

    // some globals
    static const int waitms_ = 20;

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
    std::string parser;

  private:
    std::string config_file;
};

