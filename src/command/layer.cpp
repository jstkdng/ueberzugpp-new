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

#include "command/layer.hpp"

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

namespace upp
{

LayerCommand::LayerCommand(LayerOptions *opts) :
    opts(opts)
{
}

void LayerCommand::execute()
{
    SPDLOG_DEBUG("oh noes {}", opts->parser);
    SPDLOG_INFO("hello");
}

void LayerCommand::setup(CLI::App &cli)
{
    auto opts = std::make_shared<LayerOptions>();
    auto *sub = cli.add_subcommand("layer", "Display images on the terminal");

    sub->add_flag("-s,--silent", opts->silent, "Print stderr to /dev/null")->default_val(false);
    sub->add_flag("--use-escape-codes", opts->use_escape_codes, "Use escape codes to get terminal capabilities")
        ->default_val(false);
    sub->add_option("--pid-file", opts->pid_file, "Output file where to write the daemon PID");
    sub->add_flag("--no-stdin", opts->no_stdin, "Do not listen on stdin for commands")
        ->default_val(false)
        ->needs("--pid-file");
    sub->add_flag("--no-cache", opts->no_cache, "Disable caching of resized images")->default_val(false);
    sub->add_option("-o,--output", opts->output, "Image output method")
        ->check(CLI::IsMember({"x11", "wayland", "sixel", "kitty", "iterm2", "chafa"}));
    sub->add_flag("--origin-center", opts->origin_center, "Location of the origin wrt the image")->default_val(false);
    sub->add_option("-p,--parser", opts->parser, "Command parser to use")
        ->check(CLI::IsMember({"json", "bash", "simple"}))
        ->default_str("json");
    sub->add_option("-l,--loader", nullptr, "**UNUSED**, only present for backwards compatibility");

    sub->callback([opts] -> void {
        LayerCommand cmd(opts.get());
        cmd.execute();
    });
}

} // namespace upp
