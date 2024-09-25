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

#include <xcb/xcb.h>

#include "xcb.hpp"

namespace xcb
{

auto connection::connect() -> Result<void>
{
    connection_ = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(connection_) > 0) {
    }
    screen_ = xcb_setup_roots_iterator(xcb_get_setup(connection_)).data;
    return {};
}

connection::~connection()
{
    if (connection_ != nullptr) {
        xcb_disconnect(connection_);
    }
}

} // namespace xcb
