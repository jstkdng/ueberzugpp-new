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

#ifndef X11_WINDOW_HPP
#define X11_WINDOW_HPP

#include <expected>
#include <string>
#include <xcb/xcb.h>

class X11Window
{
  public:
    auto initialize(xcb_connection_t *connection, xcb_screen_t *screen) -> std::expected<void, std::string>;

    std::string image_id;

  private:
    xcb_connection_t *connection = nullptr;
    xcb_screen_t *screen = nullptr;
    xcb_window_t window = 0;
};

template <>
struct std::hash<X11Window> {
    auto operator()(const X11Window &window) const noexcept -> std::size_t
    {
        return std::hash<std::string>()(window.image_id);
    }
};

#endif // X11_WINDOW_HPP
