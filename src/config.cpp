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
#include <fstream>

#include <json/json.h>

#include "config/config.hpp"
#include "os/os.hpp"

namespace fs = std::filesystem;

Config::Config()
{
    const auto home = os::getenv("HOME").value_or(fs::temp_directory_path());
    const auto config_home = os::getenv("XDG_CONFIG_HOME").value_or(std::format("{}/.config", home));
    config_file = std::format("{}/ueberzugpp/config.json", config_home);
}

auto Config::read_config_file() -> Result<void>
{
    if (!fs::exists(config_file)) {
        return {};
    }

    std::ifstream ifs(config_file);
    Json::Value json;
    Json::Reader reader;
    if (!reader.parse(ifs, json)) {
        return Err(reader.getFormattedErrorMessages());
    }

    const auto *layer = json.find("layer");
    if (layer == nullptr) {
        return {};
    }

    try {
        silent = layer->get("silent", false).asBool();
        output = layer->get("output", "").asCString();
        no_cache = layer->get("no-cache", false).asBool();
        no_opencv = layer->get("no-opencv", false).asBool();
        use_opengl = layer->get("opengl", false).asBool();
    } catch (const Json::LogicError& ex) {
        return Err(ex.what());
    }

    return {};
}
