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
#include "buildconfig.hpp"
#include "exceptions.hpp"
#include "os.hpp"
#include "util/log.hpp"

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>
#include <vips/operation.h>
#include <vips/vips.h>

#include <fstream>
#include <memory>

namespace upp
{

LayerCommand::LayerCommand(LayerOptions *opts) :
    opts(opts)
{
}

void LayerCommand::execute()
{
    close_stderr();
    print_header();
    daemonize();
    setup_vips();
    // TODO: initialize context
}

void LayerCommand::setup_vips()
{
    if (VIPS_INIT("ueberzugpp")) {
        throw ex::vips_error("could not initialize libvips");
    }
    vips_cache_set_max(0);
    LOG_DEBUG("libvips initialized");
}

void LayerCommand::daemonize()
{
    if (!opts->no_stdin) {
        return;
    }
    os::daemonize();
    int new_pid = os::getpid();
    LOG_DEBUG("child process {} created, parent terminated", new_pid);
    std::ofstream ofs(opts->pid_file);
    ofs << new_pid;
}

void LayerCommand::close_stderr()
{
    if (opts->silent) {
        os::close_stderr();
        LOG_DEBUG("stderr closed");
    }
}

void LayerCommand::print_header()
{
    constexpr auto *art = R"(starting
 _   _      _
| | | |    | |                                _     _
| | | | ___| |__   ___ _ __ _____   _  __ _ _| |_ _| |_
| | | |/ _ \ '_ \ / _ \ '__|_  / | | |/ _` |_   _|_   _|   version: {}
| |_| |  __/ |_) |  __/ |   / /| |_| | (_| | |_|   |_|     build date: {}
 \___/ \___|_.__/ \___|_|  /___|\__,_|\__, |
                                       __/ |    new
                                      |___/)";
    LOG_INFO(art, version_str, build_date);
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
