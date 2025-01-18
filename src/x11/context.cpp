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

#include "os/os.hpp"
#include "util/result.hpp"
#include "util/util.hpp"
#include "x11/types.hpp"
#include "x11/x11.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <format>
#include <iterator>
#include <optional>
#include <span>
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

    pid_window_map.reserve(num_clients);
    return {};
}

auto X11Context::load_state(int pid) -> Result<void>
{
    set_pid_window_map();
    return set_parent_window(pid).and_then([this] {
        SPDLOG_INFO("parent window: {}", parent);
        return set_parent_window_geometry();
    });
}

auto X11Context::get_window_ids() const -> std::vector<xcb::window_id>
{
    std::vector<xcb::window_id> windows;
    windows.reserve(num_clients);

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
        auto num_children = xcb_query_tree_children_length(reply.get());
        auto *children_ptr = xcb_query_tree_children(reply.get());

        const std::span children{children_ptr, static_cast<size_t>(num_children)};
        windows.insert(windows.end(), children.begin(), children.end());
        for (auto child : children) {
            cookies_st.push(xcb_query_tree(connection.get(), child));
        }
    }
    return windows;
}

auto X11Context::set_parent_window(int pid) -> Result<void>
{
    auto wid = os::getenv("WINDOWID").transform([](std::string_view var) {
        return util::view_to_numeral<int>(var).value_or(-1);
    });
    if (wid && *wid != -1) {
        parent = *wid;
        return {};
    }
    auto search = pid_window_map.find(pid);
    if (search == pid_window_map.end()) {
        return Err(std::format("parent not found for pid {}", pid));
    }
    parent = search->second;
    return {};
}

void X11Context::set_pid_window_map()
{
    auto windows = get_complete_window_ids();
    std::vector<xcb_res_query_client_ids_cookie_t> cookies;
    cookies.reserve(windows.size());

    xcb_res_client_id_spec_t spec;
    spec.mask = XCB_RES_CLIENT_ID_MASK_LOCAL_CLIENT_PID;
    for (auto window : windows) {
        spec.client = window;
        cookies.push_back(xcb_res_query_client_ids(connection.get(), 1, &spec));
    }

    pid_window_map.clear();
    auto win_iter = windows.cbegin();
    for (auto &cookie : cookies) {
        auto reply_result = xcb::get_result(xcb_res_query_client_ids_reply, connection.get(), cookie);
        if (!reply_result) {
            handle_xcb_error(reply_result.error());
            continue;
        }

        auto &reply = *reply_result;
        auto iter = xcb_res_query_client_ids_ids_iterator(reply.get());
        auto pid = *xcb_res_client_id_value_value(iter.data);
        pid_window_map.emplace(static_cast<int>(pid), *win_iter);
        std::advance(win_iter, 1);
    }
}

auto X11Context::get_complete_window_ids() const -> std::vector<xcb::window_id>
{
    struct cookie_props {
        xcb::window_id window_id;
        std::array<xcb_get_property_cookie_t, 2> cookies;
    };
    auto windows = get_window_ids();
    std::vector<cookie_props> cookies;
    std::vector<xcb::window_id> result;
    cookies.reserve(windows.size());
    result.reserve(windows.size());

    for (auto window : windows) {
        cookies.push_back(
            {.window_id = window,
             .cookies = {xcb_get_property(connection.get(), 0, window, XCB_ATOM_WM_CLASS, XCB_ATOM_ANY, 0, 4),
                         xcb_get_property(connection.get(), 0, window, XCB_ATOM_WM_NAME, XCB_ATOM_ANY, 0, 4)}});
    }

    for (auto &cookie_prop : cookies) {
        const bool is_complete = std::ranges::any_of(cookie_prop.cookies, [this](auto &cookie) {
            auto reply = xcb::get_result(xcb_get_property_reply, connection.get(), cookie);
            if (!reply) {
                handle_xcb_error(reply.error());
                return false;
            }
            return xcb_get_property_value_length(reply->get()) != 0;
        });
        if (is_complete) {
            result.push_back(cookie_prop.window_id);
        }
    }

    return result;
}

auto X11Context::set_parent_window_geometry() -> Result<void>
{
    auto cookie = xcb_get_geometry(connection.get(), parent);
    auto reply_result = xcb::get_result(xcb_get_geometry_reply, connection.get(), cookie);
    if (!reply_result) {
        handle_xcb_error(reply_result.error());
        return Err(std::format("failed to set geometry for window {}", parent));
    }
    auto &reply = *reply_result;
    parent_geometry.width = reply->width;
    parent_geometry.height = reply->height;
    parent_geometry.x = reply->x;
    parent_geometry.y = reply->y;
    parent_geometry.border_width = reply->border_width;
    return {};
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
