// Display images inside a terminal
// Copyright (C) 2023  JustKidding
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "terminal_info.hpp"
#include "os.hpp"
#include "util.hpp"

#include <iostream>
#include <spdlog/spdlog.h>
#include <unordered_set>

#include <sys/ioctl.h>

using std::unexpected;

auto TerminalInfo::initialize(const int cur_pty_fd) -> std::expected<void, std::string>
{
    pty_fd = cur_pty_fd;
    auto result = set_size_ioctl();

    if (config->use_escape_codes) {
        result = result
                     .or_else([this]([[maybe_unused]] std::string_view err) {
                         SPDLOG_DEBUG(err);
                         return set_size_escape_code();
                     })
                     .or_else([this]([[maybe_unused]] std::string_view err) {
                         SPDLOG_DEBUG(err);
                         return set_size_xtsm();
                     });
        check_output_support();
    }
    return result;
}

void TerminalInfo::check_output_support()
{
    SPDLOG_DEBUG("checking output support");
    auto supports_sixel = check_sixel_support();
    if (!supports_sixel) {
        SPDLOG_DEBUG("sixel not supported: {}", supports_sixel.error());
    }
    auto supports_kitty = check_kitty_support();
    if (!supports_kitty) {
        SPDLOG_DEBUG("kitty not supported: {}", supports_kitty.error());
    }
}

auto TerminalInfo::check_sixel_support() -> std::expected<void, std::string>
{
    // some terminals support sixel but don't respond to escape sequences
    const auto supported_terms = std::unordered_set<std::string_view>{"yaft-256color", "iTerm.app"};
    const auto resp = read_raw_terminal_command("\033[?1;1;0S");
    if (!resp) {
        return unexpected(resp.error());
    }
    std::string_view view = resp.value();
    view.remove_prefix(3);
    view.remove_suffix(1);
    const auto vals = util::str_split(view, ";");
    if (vals.size() <= 2) {
        return unexpected("invalid escape code results");
    }
    SPDLOG_DEBUG("sixel is supported");
    return {};
}

auto TerminalInfo::check_kitty_support() -> std::expected<void, std::string>
{
    const auto resp = read_raw_terminal_command("\033_Gi=31,s=1,v=1,a=q,t=d,f=24;AAAA\033\\\033[c");
    if (!resp) {
        return unexpected(resp.error());
    }
    if (resp.value().find("OK") == std::string_view::npos) {
        return unexpected("invalid escape code results");
    }
    SPDLOG_INFO("kitty is supported");
    return {};
}

auto TerminalInfo::set_size_ioctl() -> std::expected<void, std::string>
{
    winsize size{};
    const int result = ioctl(pty_fd, TIOCGWINSZ, &size);
    if (result == -1) {
        return os::system_error("could not get ioctl sizes");
    }
    cols = size.ws_col;
    rows = size.ws_row;
    xpixel = size.ws_xpixel;
    ypixel = size.ws_ypixel;
    SPDLOG_DEBUG("ioctl sizes: COLS={} ROWS={} XPIXEL={} YPIXEL={}", cols, rows, xpixel, ypixel);
    if (xpixel == 0 || ypixel == 0) {
        return unexpected("xpixel and ypixel not set by ioctl");
    }
    return {};
}

auto TerminalInfo::set_size_xtsm() -> std::expected<void, std::string>
{
    auto resp = read_raw_terminal_command("\033[?2;1;0S");
    if (!resp) {
        return unexpected(resp.error());
    }
    std::string_view view = resp.value();
    view.remove_prefix(3);
    view.remove_suffix(1);
    const auto values = util::str_split(view, ";");
    if (values.size() != 4) {
        return unexpected("got bad values from xtsm");
    }
    xpixel = util::view_to_numeral<int>(values.at(2)).value_or(0);
    ypixel = util::view_to_numeral<int>(values.at(3)).value_or(0);
    SPDLOG_DEBUG("XTSM sizes XPIXEL={} YPIXEL={}", xpixel, ypixel);
    if (xpixel == 0 || ypixel == 0) {
        return unexpected("xpixel and/or ypixel not set by xtsm");
    }
    return {};
}

auto TerminalInfo::set_size_escape_code() -> std::expected<void, std::string>
{
    const auto resp = read_raw_terminal_command("\033[14t");
    if (!resp) {
        return unexpected(resp.error());
    }
    std::string_view view = resp.value();
    view.remove_prefix(4);
    view.remove_suffix(1);
    const auto values = util::str_split(view, ";");
    if (values.size() != 2) {
        return unexpected("got bad values from escape code");
    }
    xpixel = util::view_to_numeral<int>(values.at(0)).value_or(0);
    ypixel = util::view_to_numeral<int>(values.at(1)).value_or(0);
    SPDLOG_DEBUG("ESC sizes XPIXEL = {} YPIXEL = {}", xpixel, ypixel);
    if (xpixel == 0 || ypixel == 0) {
        return unexpected("xpixel and/or ypixel not set by escape code");
    }
    return {};
}

auto TerminalInfo::read_raw_terminal_command(const std::string_view command) -> std::expected<std::string, std::string>
{
    constexpr int waitms = 100;
    std::expected<std::string, std::string> result;
    init_termios();
    std::cout << command << std::flush;
    const auto in_event = os::wait_for_data_on_stdin(waitms);
    if (in_event.has_value()) {
        if (*in_event) {
            result = os::read_data_from_stdin();
        } else {
            result = unexpected("could not read any data");
        }
    } else {
        result = unexpected(in_event.error());
    }
    reset_termios();
    return result;
}

void TerminalInfo::init_termios()
{
    tcgetattr(pty_fd, &old_term);          /* grab old terminal i/o settings */
    new_term = old_term;                   /* make new settings same as old settings */
    new_term.c_lflag &= ~ICANON;           /* disable buffered i/o */
    new_term.c_lflag &= ~ECHO;             /* set echo mode */
    tcsetattr(pty_fd, TCSANOW, &new_term); /* use these new terminal i/o settings now */
}

void TerminalInfo::reset_termios() const
{
    tcsetattr(pty_fd, TCSANOW, &old_term);
}
