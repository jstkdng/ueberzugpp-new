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

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <expected>
#include <filesystem>
#include <string>
#include <string_view>

struct Command {
    static auto create(std::string_view parser, std::string_view line) -> std::expected<Command, std::string>;
    static auto parse_json(std::string_view line) -> std::expected<Command, std::string>;

    std::string action;
    std::string preview_id;
    std::filesystem::path image_path;
    std::string image_scaler;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

#endif // COMMAND_HPP
