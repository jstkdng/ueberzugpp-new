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

#include <spdlog/spdlog.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include "context/x11_ctx.hpp"
#include "os/os.hpp"

X11Context::X11Context()
{
    connection = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(connection) == 0) {
        setup = xcb_get_setup(connection);
        screen = xcb_setup_roots_iterator(setup).data;
        connection_fd = xcb_get_file_descriptor(connection);

        auto fd_pid = os::get_pid_from_socket(connection_fd);
        if (fd_pid) {
            if (os::get_pid_process_name(*fd_pid) != "Xorg") {
                SPDLOG_INFO("running xwayland");
                is_xwayland = true;
            }
        } else {
            SPDLOG_INFO("running xorg");
            is_valid = true;
        }
    }
}

X11Context::~X11Context()
{
    xcb_disconnect(connection);
}
