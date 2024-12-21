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

#include <filesystem>
#include <format>
#include <fstream>

#include <json/json.h>

#include "error.hpp"
#include "os/os.hpp"

namespace fs = std::filesystem;

ProgramConfig::ProgramConfig()
{
    const auto home = os::getenv("HOME").value_or(fs::temp_directory_path());
    const auto config_home = os::getenv("XDG_CONFIG_HOME").value_or(std::format("{}/.config", home));
    config_file = std::format("{}/ueberzugpp/config.json", config_home);
}

auto ProgramConfig::read_config_file() -> Result<void>
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

    try {
        const auto *logging_obj = json.find("logging");
        if (logging_obj != nullptr) {
            logging.silent = logging_obj->get("silent", false).asBool();
        }

        const auto *layer_obj = json.find("layer");
        if (layer_obj != nullptr) {
            layer.output = layer_obj->get("output", "").asCString();
            layer.no_cache = layer_obj->get("no-cache", false).asBool();
            layer.no_opencv = layer_obj->get("no-opencv", false).asBool();
            layer.use_opengl = layer_obj->get("opengl", false).asBool();
        }
    } catch (const Json::LogicError &ex) {
        return Err("could not parse config file", ex);
    }

    return {};
}
