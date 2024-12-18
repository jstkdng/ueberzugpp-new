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

#include <algorithm>
#include <cmath>

#include <spdlog/spdlog.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include "os/os.hpp"
#include "os/process.hpp"
#include "terminal.hpp"
#include "error.hpp"

namespace
{

auto guess_padding(const int chars, const int pixels) -> double
{
    const double font_size = std::floor(static_cast<double>(pixels) / chars);
    return (pixels - font_size * chars) / 2;
}

auto guess_font_size(const int chars, const int pixels, const int padding) -> double
{
    return (static_cast<double>(pixels) - 2 * padding) / chars;
}

} // namespace

auto Terminal::init() -> Result<void>
{
    return open_first_pty().and_then([this] { return get_ioctl_sizes(); }).and_then([this] {
        return set_font_sizes();
    });
}

auto Terminal::set_font_sizes() -> Result<void>
{
    const double padding_horiz = guess_padding(size.cols, size.xpixel);
    const double padding_vert = guess_padding(size.rows, size.ypixel);
    font.horizontal_padding = static_cast<int>(std::max(padding_horiz, padding_vert));
    font.vertical_padding = font.horizontal_padding;
    font.width = static_cast<int>(std::floor(guess_font_size(size.cols, size.xpixel, font.horizontal_padding)));
    font.height = static_cast<int>(std::floor(guess_font_size(size.rows, size.ypixel, font.vertical_padding)));
    SPDLOG_DEBUG("padding_horiz={} padding_vert={}", font.horizontal_padding, font.vertical_padding);
    SPDLOG_DEBUG("font_width={} font_height={}", font.width, font.height);
    return {};
}

auto Terminal::open_first_pty() -> Result<void>
{
    struct stat stat_info {
    };
    auto tree = Process::get_tree(os::getpid());
    std::ranges::reverse(tree);
    for (const auto &proc : tree) {
        const auto &path = proc.pty_path;
        if (stat(path.c_str(), &stat_info) == -1) {
            SPDLOG_DEBUG("stat {}: {}", path, os::strerror());
            continue;
        }
        if (proc.tty_nr != static_cast<int>(stat_info.st_rdev)) {
            SPDLOG_DEBUG("device {} != {}", proc.tty_nr, stat_info.st_rdev);
            continue;
        }
        pty_fd_ = open(path.c_str(), O_RDONLY | O_NOCTTY);
        if (*pty_fd_ == -1) {
            SPDLOG_DEBUG("open: {}", os::strerror());
            continue;
        }
        pty_pid_ = proc.pid;
        SPDLOG_INFO("PTY={}", path);
        return {};
    }
    return Err("could not open terminal");
}

auto Terminal::get_ioctl_sizes() -> Result<void>
{
    winsize wsize{};
    if (ioctl(*pty_fd_, TIOCGWINSZ, &wsize) == -1) {
        return Err("could not get ioctl sizes");
    }
    size.cols = wsize.ws_col;
    size.rows = wsize.ws_row;
    size.xpixel = wsize.ws_xpixel;
    size.ypixel = wsize.ws_ypixel;
    SPDLOG_DEBUG("ioctl sizes: COLS={} ROWS={} XPIXEL={} YPIXEL={}", size.cols, size.rows, size.xpixel, size.ypixel);
    if (size.xpixel == 0 || size.ypixel == 0) {
        return Err("xpixel and ypixel not set by ioctl");
    }
    return {};
}
