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

#include <string>

#include <CLI/CLI.hpp>

namespace upp
{

struct DispatchOptions {
    std::string socket;
    std::string identifier;
    std::string action;
    std::string file_path;
    std::string scaler = "contain";

    int x;
    int y;
    int width;
    int height;
};

class DispatchCommand
{
  public:
    explicit DispatchCommand(DispatchOptions *opts);
    void execute();
    static void setup(CLI::App &cli);

  private:
    DispatchOptions *opts;
};

} // namespace upp
