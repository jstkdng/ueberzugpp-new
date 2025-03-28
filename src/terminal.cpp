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

#include "terminal.hpp"
#include "application/context.hpp"
#include "buildconfig.hpp"
#include "os/os.hpp"
#include "util/result.hpp"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <cmath>

namespace upp
{

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

Terminal::Terminal(ApplicationContext *ctx) :
    ctx(ctx)
{
}

auto Terminal::init() -> Result<void>
{
    logger = spdlog::get("terminal");
    return open_first_pty().and_then([this] { return load_state(); });
}

auto Terminal::load_state() -> Result<void>
{
    return set_terminal_size().and_then([this] { return set_font_size(); });
}

auto Terminal::set_terminal_size() -> Result<void>
{
    winsize termsize;
    if (ioctl(pty_fd.get(), TIOCGWINSZ, &termsize) == -1) {
        return Err("ioctl");
    }
    size.cols = termsize.ws_col;
    size.rows = termsize.ws_row;
    size.width = termsize.ws_xpixel;
    size.height = termsize.ws_ypixel;
    LOG_DEBUG("ioctl sizes: COLS={} ROWS={} XPIXEL={} YPIXEL={}", size.cols, size.rows, size.width, size.height);

    set_fallback_size_from_x11();
    set_fallback_size_from_wayland();

    if (size.width == 0 || size.height == 0) {
        size.width = size.fallback_width;
        size.height = size.fallback_height;
    }

    if (size.width == 0 || size.height == 0) {
        return Err("unable to calculate terminal sizes");
    }

    return {};
}

auto Terminal::set_font_size() -> Result<void>
{
    const double padding_horiz = guess_padding(size.cols, size.width);
    const double padding_vert = guess_padding(size.rows, size.height);
    font.horizontal_padding = static_cast<int>(std::max(padding_horiz, padding_vert));
    font.vertical_padding = font.horizontal_padding;
    font.width = static_cast<int>(std::floor(guess_font_size(size.cols, size.width, font.horizontal_padding)));
    font.height = static_cast<int>(std::floor(guess_font_size(size.rows, size.height, font.vertical_padding)));

    if (size.width < size.fallback_width && size.height < size.fallback_height) {
        font.horizontal_padding = static_cast<int>(std::ceil(static_cast<float>(size.fallback_width - size.width) / 2));
        font.vertical_padding = static_cast<int>(std::ceil(static_cast<float>(size.fallback_height - size.height) / 2));
        font.width = size.width / size.cols;
        font.height = size.height / size.rows;
    }

    LOG_DEBUG("padding_horiz={} padding_vert={}", font.horizontal_padding, font.vertical_padding);
    LOG_DEBUG("font_width={} font_height={}", font.width, font.height);
    return {};
}

void Terminal::set_fallback_size_from_x11()
{
#ifdef ENABLE_X11
    if (auto result = ctx->x11.load_state(pty_pid); !result) {
        LOG_DEBUG(result.error().message());
        return;
    }
    auto [width, height] = ctx->x11.parent_geometry;
    size.fallback_width = width;
    size.fallback_height = height;
    LOG_DEBUG("x11 sizes: XPIXEL={} YPIXEL={}", width, height);
#endif
}

void Terminal::set_fallback_size_from_wayland()
{
#ifdef ENABLE_WAYLAND
    if (!ctx->wl_socket) {
        return;
    }
    auto geometry = ctx->wl_socket->active_window(pty_pid);
    position.x = geometry.x;
    position.y = geometry.y;
    size.fallback_width = geometry.width;
    size.fallback_height = geometry.height;
    LOG_DEBUG("wayland sizes: X={} Y={} XPIXEL={} YPIXEL={}", position.x, position.y, geometry.width, geometry.height);
#endif
}

auto Terminal::open_first_pty() -> Result<void>
{
    struct stat stat_info{};
    auto tree = os::Process::get_tree(os::getpid());
    std::ranges::reverse(tree);

    for (const auto &proc : tree) {
        const auto &path = proc.pty_path;
        if (stat(path.c_str(), &stat_info) == -1) {
            LOG_DEBUG("stat {}: {}", path, os::strerror());
            continue;
        }
        if (proc.tty_nr != static_cast<int>(stat_info.st_rdev)) {
            LOG_DEBUG("device {} != {}", proc.tty_nr, stat_info.st_rdev);
            continue;
        }
        pty_fd = open(path.c_str(), O_RDONLY | O_NOCTTY);
        if (!pty_fd) {
            LOG_DEBUG("open: {}", os::strerror());
            continue;
        }
        pty_pid = proc.pid;
        LOG_INFO("PTY = {}", path);
        LOG_INFO("PID = {}", pty_pid);
        return {};
    }
    return Err("could not open terminal");
}

} // namespace upp
