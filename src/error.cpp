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

#include <filesystem>
#include <format>
#include <string_view>

#include "buildconfig.hpp"
#include "error.hpp"

Error::Error(std::source_location location, std::string prefix, std::errc errc) :
    prefix_(std::move(prefix)),
    condition_(errc),
    location_(location)
{
}

Error::Error(std::source_location location, std::string prefix, int code) :
    prefix_(std::move(prefix)),
    condition_(code, std::generic_category()),
    location_(location)
{
}

Error::Error(std::source_location location, std::string_view prefix, const std::exception &exc) :
    prefix_(std::format("{}: {}", prefix, exc.what())),
    location_(location)
{
}

auto Error::message() const -> std::string
{
    if (condition_.value() == 0) {
        return prefix_;
    }
    return std::format("{}: {}", prefix_, condition_.message());
}

auto Error::lmessage() const -> std::string
{
    constexpr std::string_view build_dir = build_base_dir;
    std::string_view filename = location_.file_name();
    if (filename.find(build_dir) != std::string_view::npos) {
        filename.remove_prefix(build_dir.size());
    }
    return std::format("[{}:{}] {}", filename, location_.line(), message());
}
