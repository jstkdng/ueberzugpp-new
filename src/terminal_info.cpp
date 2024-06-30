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

#include <sys/ioctl.h>

using std::unexpected;

auto TerminalInfo::initialize(const int cur_pty_fd) -> std::expected<void, std::string>
{
    pty_fd = cur_pty_fd;
    return set_size_ioctl()
        .or_else([this](const std::string &err) {
            SPDLOG_DEBUG(err);
            return set_size_escape_code();
        })
        .or_else([this](const std::string &err) {
            SPDLOG_DEBUG(err);
            return set_size_xtsm();
        });
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
    if (xpixel == 0 || ypixel == 0) {
        return unexpected("xpixel and ypixel not set by ioctl");
    }
    SPDLOG_DEBUG("ioctl sizes: COLS={} ROWS={} XPIXEL={} YPIXEL={}", cols, rows, xpixel, ypixel);
    return {};
}

auto TerminalInfo::set_size_xtsm() -> std::expected<void, std::string>
{
    auto resp = read_raw_terminal_command("\033[?2;1;0S");
    if (!resp) {
        return unexpected(resp.error());
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
    const auto semi_idx = view.find(';');
    const auto str1 = view.substr(0, semi_idx);
    const auto str2 = view.substr(semi_idx + 1, view.length());
    auto n1_conv = util::view_to_numeral<int>(str1);
    if (!n1_conv) {
        return unexpected(n1_conv.error());
    }
    xpixel = *n1_conv;
    auto n2_conv = util::view_to_numeral<int>(str2);
    if (!n2_conv) {
        return unexpected(n2_conv.error());
    }
    ypixel = *n2_conv;
    SPDLOG_DEBUG("ESC sizes XPIXEL = {} YPIXEL = {}", xpixel, ypixel);
    return {};
}

auto TerminalInfo::read_raw_terminal_command(const std::string_view command) -> std::expected<std::string, std::string>
{
    constexpr int waitms = 100;
    std::string result;
    init_termios();
    std::cout << command << std::flush;
    const auto in_event = os::wait_for_data_on_stdin(waitms);
    if (in_event.has_value() && *in_event) {
        const auto output = os::read_data_from_stdin();
        if (output) {
            result = *output;
        }
    }
    reset_termios();
    if (result.empty()) {
        return unexpected("could not read output of command");
    }
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
