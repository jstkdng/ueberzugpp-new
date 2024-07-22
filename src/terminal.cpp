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

#include "terminal.hpp"
#include "os/os.hpp"
#include "util/util.hpp"

#include <algorithm>
#include <iostream>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <spdlog/spdlog.h>

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

Terminal::~Terminal()
{
    if (pty_fd > 2) {
        close(pty_fd);
    }
}

auto Terminal::initialize() -> std::expected<void, std::string>
{
    open_first_pty();
    term = os::getenv("TERM").value_or("xterm-256color");
    term_program = os::getenv("TERM_PROGRAM").value_or("");
    if (term_program.empty()) {
        SPDLOG_INFO("TERM={}", term);
    } else {
        SPDLOG_INFO("TERM={} TERM_PROGRAM={}", term, term_program);
    }

    auto result = set_size_ioctl();

    if (config->use_escape_codes) {
        result = result
                     .or_else([this]([[maybe_unused]] const std::string_view err) {
                         SPDLOG_DEBUG(err);
                         return set_size_escape_code();
                     })
                     .or_else([this]([[maybe_unused]] const std::string_view err) {
                         SPDLOG_DEBUG(err);
                         return set_size_xtsm();
                     })
                     .and_then([this] { return check_output_support(); });
    }
    set_padding_values();
    return result;
}

auto Terminal::check_output_support() -> std::expected<void, std::string>
{
    SPDLOG_DEBUG("checking output support");
    return check_kitty_support().or_else([this](auto) { return check_sixel_support(); });
}

void Terminal::set_padding_values()
{
    const double padding_horiz = guess_padding(size.cols, size.xpixel);
    const double padding_vert = guess_padding(size.rows, size.ypixel);
    padding_horizontal = static_cast<int>(std::max(padding_horiz, padding_vert));
    padding_vertical = padding_horizontal;
    font_width = static_cast<int>(std::floor(guess_font_size(size.cols, size.xpixel, padding_horizontal)));
    font_height = static_cast<int>(std::floor(guess_font_size(size.rows, size.ypixel, padding_vertical)));
    SPDLOG_DEBUG("padding_horiz={} padding_vert={}", padding_horizontal, padding_vertical);
    SPDLOG_DEBUG("font_width={} font_height={}", font_width, font_height);
}

auto Terminal::check_sixel_support() -> std::expected<void, std::string>
{
    return read_raw_terminal_command("\033[?1;1;0S")
        .and_then([this](std::string_view view) -> std::expected<void, std::string> {
            view.remove_prefix(3);
            view.remove_suffix(1);
            const auto vals = util::str_split(view, ";");
            if (vals.size() <= 2) {
                return util::unexpected_err("invalid escape code results");
            }
            supports_sixel = true;
            SPDLOG_DEBUG("sixel is supported");
            return {};
        });
}

auto Terminal::check_kitty_support() -> std::expected<void, std::string>
{
    return read_raw_terminal_command("\033_Gi=31,s=1,v=1,a=q,t=d,f=24;AAAA\033\\\033[c")
        .and_then([this](const std::string_view resp) -> std::expected<void, std::string> {
            if (resp.find("OK") == std::string_view::npos) {
                return util::unexpected_err("invalid escape code results");
            }
            supports_kitty = true;
            SPDLOG_DEBUG("kitty is supported");
            return {};
        });
}

auto Terminal::set_size_ioctl() -> std::expected<void, std::string>
{
    winsize wsize{};
    const int result = ioctl(pty_fd, TIOCGWINSZ, &wsize);
    if (result == -1) {
        return os::system_error("could not get ioctl sizes");
    }
    size.cols = wsize.ws_col;
    size.rows = wsize.ws_row;
    size.xpixel = wsize.ws_xpixel;
    size.ypixel = wsize.ws_ypixel;
    SPDLOG_DEBUG("ioctl sizes: COLS={} ROWS={} XPIXEL={} YPIXEL={}", size.cols, size.rows, size.xpixel, size.ypixel);
    if (size.xpixel == 0 || size.ypixel == 0) {
        return util::unexpected_err("xpixel and ypixel not set by ioctl");
    }
    return {};
}

auto Terminal::set_size_xtsm() -> std::expected<void, std::string>
{
    return read_raw_terminal_command("\033[?2;1;0S")
        .and_then([this](std::string_view view) -> std::expected<void, std::string> {
            view.remove_prefix(3);
            view.remove_suffix(1);
            const auto values = util::str_split(view, ";");
            if (values.size() != 4) {
                return util::unexpected_err("got bad values from xtsm");
            }
            size.xpixel = util::view_to_numeral<int>(values.at(2)).value_or(0);
            size.ypixel = util::view_to_numeral<int>(values.at(3)).value_or(0);
            SPDLOG_DEBUG("XTSM sizes XPIXEL={} YPIXEL={}", size.xpixel, size.ypixel);
            if (size.xpixel == 0 || size.ypixel == 0) {
                return util::unexpected_err("xpixel and/or ypixel not set by xtsm");
            }
            return {};
        });
}

auto Terminal::set_size_escape_code() -> std::expected<void, std::string>
{
    return read_raw_terminal_command("\033[14t")
        .and_then([this](std::string_view view) -> std::expected<void, std::string> {
            view.remove_prefix(4);
            view.remove_suffix(1);
            const auto values = util::str_split(view, ";");
            if (values.size() != 2) {
                return util::unexpected_err("got bad values from escape code");
            }
            size.xpixel = util::view_to_numeral<int>(values.at(0)).value_or(0);
            size.ypixel = util::view_to_numeral<int>(values.at(1)).value_or(0);
            SPDLOG_DEBUG("ESC sizes XPIXEL={} YPIXEL={}", size.xpixel, size.ypixel);
            if (size.xpixel == 0 || size.ypixel == 0) {
                return util::unexpected_err("xpixel and/or ypixel not set by escape code");
            }
            return {};
        });
}

auto Terminal::read_raw_terminal_command(const std::string_view command) -> std::expected<std::string, std::string>
{
    std::expected<std::string, std::string> result;
    init_termios();
    std::cout << command << std::flush;
    const auto in_event = os::wait_for_data_on_stdin(config->waitms);
    if (in_event.has_value()) {
        if (*in_event) {
            result = os::read_data_from_stdin();
        } else {
            result = util::unexpected_err("could not read data from escape code");
        }
    } else {
        result = std::unexpected(in_event.error());
    }
    reset_termios();
    return result;
}

void Terminal::init_termios()
{
    tcgetattr(pty_fd, &old_term);          /* grab old terminal i/o settings */
    new_term = old_term;                   /* make new settings same as old settings */
    new_term.c_lflag &= ~ICANON;           /* disable buffered i/o */
    new_term.c_lflag &= ~ECHO;             /* set echo mode */
    tcsetattr(pty_fd, TCSANOW, &new_term); /* use these new terminal i/o settings now */
}

void Terminal::reset_termios() const
{
    tcsetattr(pty_fd, TCSANOW, &old_term);
}

void Terminal::open_first_pty()
{
    struct stat stat_info {
    };
    auto tree = util::get_process_tree(os::get_pid());
    std::ranges::reverse(tree);
    for (const auto &proc : tree) {
        const auto *pty_path = proc.pty_path.c_str();
        const int stat_result = stat(pty_path, &stat_info);
        if (stat_result == -1) {
            [[maybe_unused]] const auto err = std::error_code(errno, std::generic_category());
            SPDLOG_DEBUG("stat failed for pty {}: {}", pty_path, err.message());
            continue;
        }
        if (proc.tty_nr != static_cast<int>(stat_info.st_rdev)) {
            SPDLOG_DEBUG("device number different: {} != {}", proc.tty_nr, stat_info.st_rdev);
            continue;
        }
        pty_fd = open(pty_path, O_RDONLY | O_NOCTTY);
        if (pty_fd == -1) {
            [[maybe_unused]] const auto err = std::error_code(errno, std::generic_category());
            SPDLOG_DEBUG("could not open pty {}: {}", pty_path, err.message());
            continue;
        }
        pty_pid = proc.pid;
        SPDLOG_INFO("PTY={}", pty_path);
        return;
    }
    SPDLOG_WARN("could not open any pty, using stdout as fallback");
    pty_fd = STDOUT_FILENO;
}
