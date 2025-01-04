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

#include "application.hpp"
#include "buildconfig.hpp"

#include <CLI/CLI.hpp>

#include <print>

auto main(int argc, char *argv[]) -> int
{
    CLI::App app{"Display images in the terminal", "ueberzugpp"};
    app.set_version_flag("-V,--version", version_str);

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    upp::Application application(&app);
    auto result = application.run();
    if (!result) {
        std::println(stderr, "{}", result.error().lmessage());
        return 1;
    }

    return 0;
}
