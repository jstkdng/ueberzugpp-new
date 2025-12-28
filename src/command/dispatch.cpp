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

#include "command/dispatch.hpp"

#include <memory>

#include <CLI/CLI.hpp>

namespace upp
{

DispatchCommand::DispatchCommand(DispatchOptions *opts) :
    opts(opts)
{
}

void DispatchCommand::execute()
{
}

void DispatchCommand::setup(CLI::App &cli)
{
    auto opts = std::make_shared<DispatchOptions>();
    auto *sub = cli.add_subcommand("dispatch", "Send a command to a running ueberzugpp instance");
    sub->alias("cmd");

    sub->add_option("-s,--socket", opts->socket, "unix socket of running instance")->required();
    sub->add_option("-a,--action", opts->action, "action to send")->required();
    sub->add_option("-i,--identifier", opts->identifier, "preview identifier");
    sub->add_option("-f,--file", opts->file_path, "path of image file");
    sub->add_option("-x,--xpos", opts->x, "x position of preview");
    sub->add_option("-y,--ypos", opts->y, "y position of preview");
    sub->add_option("--max-width", opts->width, "max width of preview");
    sub->add_option("--max-height", opts->height, "max height of preview");
    sub->add_option("--scaler", opts->scaler, "scaler to use")->default_str("contain");
    sub->callback([opts] -> void {
        DispatchCommand cmd(opts.get());
        cmd.execute();
    });
}

} // namespace upp
