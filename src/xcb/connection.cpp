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
#include <stack>

#include <xcb/xcb.h>

#include "util/ptr.hpp"
#include "xcb.hpp"

namespace xcb
{

using PassKey = util::PassKey<connection>;

auto connection::connect() -> Result<void>
{
    connection_ = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(connection_) > 0) {
        return Err("could not connect to server");
    }
    setup_ = xcb_get_setup(connection_);
    screen_ = xcb_setup_roots_iterator(setup_).data;
    return {};
}

connection::~connection()
{
    if (connection_ != nullptr) {
        xcb_disconnect(connection_);
    }
}

auto connection::create_window() -> window
{
    auto wid = xcb_generate_id(connection_);
    return {PassKey{}, connection_, screen_, wid, screen_->root};
}

auto connection::get_server_window_ids() const -> std::vector<xcb_window_t>
{
    constexpr int default_max_clients = 256;
    std::vector<xcb_window_t> windows;
    std::stack<xcb_query_tree_cookie_t> cookies_st;
    windows.reserve(default_max_clients);

    cookies_st.push(xcb_query_tree_unchecked(connection_, screen_->root));
    windows.push_back(screen_->root);

    while (!cookies_st.empty()) {
        const auto cookie = cookies_st.top();
        cookies_st.pop();

        const auto reply = unique_C_ptr<xcb_query_tree_reply_t>{xcb_query_tree_reply(connection_, cookie, nullptr)};
        if (!reply) {
            continue;
        }

        const auto num_children = xcb_query_tree_children_length(reply.get());
        if (num_children == 0) {
            continue;
        }
        const auto *children = xcb_query_tree_children(reply.get());
        for (int i = 0; i < num_children; ++i) {
            const auto child = children[i];
            if (window_has_properties(child, {XCB_ATOM_WM_CLASS, XCB_ATOM_WM_NAME})) {
                windows.push_back(child);
            }
            cookies_st.push(xcb_query_tree_unchecked(connection_, child));
        }
    }

    return windows;
}

auto connection::window_has_properties(xcb_window_t window, std::initializer_list<xcb_atom_t> properties) const -> bool
{
    std::vector<xcb_get_property_cookie_t> cookies;
    cookies.reserve(properties.size());
    for (const auto prop : properties) {
        cookies.push_back(xcb_get_property_unchecked(connection_, 0, window, prop, XCB_ATOM_ANY, 0, 4));
    }
    return std::ranges::any_of(cookies, [this](xcb_get_property_cookie_t cookie) {
        const auto reply = unique_C_ptr<xcb_get_property_reply_t>{xcb_get_property_reply(connection_, cookie, nullptr)};
        return reply && xcb_get_property_value_length(reply.get()) != 0;
    });
}

} // namespace xcb
