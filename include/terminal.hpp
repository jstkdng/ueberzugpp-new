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

#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include "config.hpp"

#include <expected>
#include <string>

#include <termios.h>

struct TerminalSize {
    int fallback_xpixel = 0;
    int fallback_ypixel = 0;
    int xpixel = 0;
    int ypixel = 0;
    int rows = 0;
    int cols = 0;
};

class Terminal
{
  public:
    ~Terminal();
    auto operator=(Terminal &&) -> Terminal & = delete;

    auto initialize() -> std::expected<void, std::string>;

    bool supports_sixel = false;
    bool supports_kitty = false;

    std::string term;
    std::string term_program;

    TerminalSize size;

    int font_width = 0;
    int font_height = 0;
    int padding_horizontal = 0;
    int padding_vertical = 0;

  private:
    int pty_pid = -1;
    int pty_fd = -1;

    termios old_term{};
    termios new_term{};
    std::shared_ptr<Config> config = Config::instance();

    void open_first_pty();

    void init_termios();
    void set_padding_values();
    void reset_termios() const;
    auto read_raw_terminal_command(std::string_view command) -> std::expected<std::string, std::string>;

    auto set_size_ioctl() -> std::expected<void, std::string>;
    auto set_size_xtsm() -> std::expected<void, std::string>;
    auto set_size_escape_code() -> std::expected<void, std::string>;

    auto check_output_support() -> std::expected<void, std::string>;
    auto check_sixel_support() -> std::expected<void, std::string>;
    auto check_kitty_support() -> std::expected<void, std::string>;
};

#endif // TERMINAL_HPP
