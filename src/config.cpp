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

#include "config.hpp"
#include "os/os.hpp"
#include "util/util.hpp"

#include <filesystem>
#include <format>
#include <fstream>

#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using njson = nlohmann::json;

Config::Config()
{
    const auto home = os::getenv("HOME").value_or(fs::temp_directory_path());
    const auto config_home = os::getenv("XDG_CONFIG_HOME").value_or(std::format("{}/.config", home));
    config_file = std::format("{}/ueberzugpp/config.json", config_home);
}

auto Config::read_config_file() -> std::expected<void, std::string>
{
    if (!fs::exists(config_file)) {
        return {};
    }

    std::ifstream ifs(config_file);
    njson json;
    try {
        json = njson::parse(ifs);
    } catch (const njson::parse_error &err) {
        return util::unexpected_err(std::format("could not read config file: {}", err.what()));
    }

    if (!json.contains("layer")) {
        return {};
    }
    const auto &layer = json.at("layer");
    silent = layer.value("silent", false);
    output = layer.value("output", "");
    no_cache = layer.value("no-cache", false);
    no_opencv = layer.value("no-opencv", false);
    use_opengl = layer.value("opengl", false);
    return {};
}
