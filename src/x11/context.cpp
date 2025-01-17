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

#include "util/result.hpp"
#include "x11/types.hpp"
#include "x11/x11.hpp"

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <stack>
#include <vector>

namespace upp
{

auto X11Context::init() -> Result<void>
{
    connection.reset(xcb_connect(nullptr, nullptr));
    if (xcb_connection_has_error(connection.get()) > 0) {
        return Err("can't connect to x11");
    }
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection.get())).data;
    get_server_window_ids();
    return {};
}

auto X11Context::get_server_window_ids() -> std::vector<xcb::window_id>
{
    constexpr int num_clients = 256;
    std::vector<xcb::window_id> windows;
    windows.reserve(num_clients);
    windows.push_back(screen->root);

    std::stack<xcb_query_tree_cookie_t> cookies_st;
    cookies_st.push(xcb_query_tree(connection.get(), screen->root));
    while (!cookies_st.empty()) {
        auto cookie = cookies_st.top();
        cookies_st.pop();

        auto reply_result = xcb::get_result(xcb_query_tree_reply, connection.get(), cookie);
        if (!reply_result) {
            const auto& err = reply_result.error();
            SPDLOG_WARN("X11 error: code {}", err->error_code);
            continue;
        }

        auto& reply = *reply_result;
        const auto num_children = xcb_query_tree_children_length(reply.get());
        if (num_children == 0) {
            continue;
        }

        const auto *children = xcb_query_tree_children(reply.get());
        for (int i = 0; i < num_children; ++i) {
            auto child = children[i];
            windows.push_back(child);
            /*
            const bool is_complete_window = window_has_properties(child, {XCB_ATOM_WM_CLASS, XCB_ATOM_WM_NAME});
            if (is_complete_window) {
                windows.push_back(child);
            }*/
            cookies_st.push(xcb_query_tree(connection.get(), child));
        }
    }
    return windows;
}

} // namespace upp
