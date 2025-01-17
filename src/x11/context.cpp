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

#include <algorithm>
#include <cstdlib>
#include <initializer_list>
#include <stack>
#include <string_view>
#include <utility>
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

    xcb_errors_context_t *tmp = nullptr;
    xcb_errors_context_new(connection.get(), &tmp);
    err_ctx.reset(tmp);

    return {};
}

auto X11Context::get_server_window_ids() const -> std::vector<xcb::window_id>
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
            handle_xcb_error(reply_result.error());
            continue;
        }

        auto &reply = *reply_result;
        const auto num_children = xcb_query_tree_children_length(reply.get());
        if (num_children == 0) {
            continue;
        }

        const auto *children = xcb_query_tree_children(reply.get());
        for (int i = 0; i < num_children; ++i) {
            auto child = children[i];
            const bool is_complete_window = window_has_properties(child, {XCB_ATOM_WM_CLASS, XCB_ATOM_WM_NAME});
            if (is_complete_window) {
                windows.push_back(child);
            }
            cookies_st.push(xcb_query_tree(connection.get(), child));
        }
    }
    return windows;
}

auto X11Context::window_has_properties(xcb_window_t window, std::initializer_list<xcb_atom_t> properties) const -> bool
{
    std::vector<xcb_get_property_cookie_t> cookies;
    cookies.reserve(properties.size());
    for (auto prop : properties) {
        cookies.push_back(xcb_get_property(connection.get(), 0, window, prop, XCB_ATOM_ANY, 0, 4));
    }
    return std::ranges::any_of(cookies, [this](xcb_get_property_cookie_t cookie) -> bool {
        auto reply = xcb::get_result(xcb_get_property_reply, connection.get(), cookie);
        if (!reply) {
            handle_xcb_error(reply.error());
            return false;
        }
        return xcb_get_property_value_length(reply->get()) != 0;
    });
}

auto X11Context::get_window_dimensions(xcb_window_t window) const -> std::pair<int, int>
{
    auto cookie = xcb_get_geometry(connection.get(), window);
    auto reply_result = xcb::get_result(xcb_get_geometry_reply, connection.get(), cookie);
    if (!reply_result) {
        handle_xcb_error(reply_result.error());
        return std::make_pair(0, 0);
    }
    auto &reply = *reply_result;
    return std::make_pair(reply->width, reply->height);
}

void X11Context::handle_xcb_error(xcb::error &err) const
{
    const char *extension = nullptr;
    const char *major = xcb_errors_get_name_for_major_code(err_ctx.get(), err->major_code);
    const char *minor = xcb_errors_get_name_for_minor_code(err_ctx.get(), err->major_code, err->minor_code);
    const char *error = xcb_errors_get_name_for_error(err_ctx.get(), err->error_code, &extension);

    const std::string_view ext_str = extension != nullptr ? extension : "no_extension";
    const std::string_view minor_str = minor != nullptr ? minor : "no_minor";
    SPDLOG_ERROR("XCB: {}:{}, {}:{}, resource {} sequence {}", error, ext_str, major, minor_str, err->resource_id,
                 err->sequence);
}

} // namespace upp
