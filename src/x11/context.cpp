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

#include "x11/context.hpp"
#include "os/os.hpp"
#include "util/result.hpp"
#include "util/util.hpp"
#include "x11/types.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <format>
#include <optional>
#include <ranges>
#include <span>
#include <stack>
#include <string_view>
#include <utility>
#include <vector>

namespace upp
{

X11Context::~X11Context()
{
    xcb_free_gc(connection.get(), gcontext);
}

auto X11Context::init() -> Result<void>
{
    logger = spdlog::get("x11");
    connection.reset(xcb_connect(nullptr, nullptr));
    if (xcb_connection_has_error(connection.get()) > 0) {
        return Err("can't connect to X11");
    }
    logger->info("connected to X11 server");
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection.get())).data;
    connection_fd = xcb_get_file_descriptor(connection.get());

    xcb_errors_context_t *tmp = nullptr;
    xcb_errors_context_new(connection.get(), &tmp);
    err_ctx.reset(tmp);

    pid_window_map.reserve(num_clients);
    create_gcontext();

    return os::get_pid_from_socket(connection_fd).and_then([this](int pid) -> Result<void> {
        auto proc_name = os::get_pid_process_name(pid);
        logger->debug("X11 vendor: {}", proc_name);
        if (proc_name != "Xorg") {
            is_xwayland = true;
        }
        is_valid = true;
        logger->debug("X11 context initialized");
        return {};
    });
}

auto X11Context::load_state(int pid) -> Result<void>
{
    if (!is_valid) {
        return Err("invalid x11 state");
    }

    set_pid_window_map();
    return set_parent_window(pid).and_then([this] {
        logger->debug("parent window: {}", parent);
        return set_parent_window_geometry();
    });
}

auto X11Context::get_window_ids() const -> std::vector<xcb::window_id>
{
    auto windows = util::make_vector<xcb::window_id>(num_clients);

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

        const auto &reply = *reply_result;
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
    logger->debug("WINDOWID not set or invalid");
    for (auto spid : os::Process::get_pid_tree(pid)) {
        auto search = pid_window_map.find(spid);
        if (search != pid_window_map.end()) {
            parent = search->second;
            return {};
        }
    }
    return Err(std::format("parent not found for pid {}", pid));
}

void X11Context::set_pid_window_map()
{
    auto windows = get_complete_window_ids();
    auto cookies = util::make_vector<xcb_res_query_client_ids_cookie_t>(windows.size());

    xcb_res_client_id_spec_t spec;
    spec.mask = XCB_RES_CLIENT_ID_MASK_LOCAL_CLIENT_PID;
    for (auto window : windows) {
        spec.client = window;
        cookies.emplace_back(xcb_res_query_client_ids(connection.get(), 1, &spec));
    }

    pid_window_map.clear();
    for (auto [window, cookie] : std::views::zip(windows, cookies)) {
        auto reply_result = xcb::get_result(xcb_res_query_client_ids_reply, connection.get(), cookie);
        if (!reply_result) {
            handle_xcb_error(reply_result.error());
            continue;
        }

        const auto &reply = *reply_result;
        auto iter = xcb_res_query_client_ids_ids_iterator(reply.get());
        auto pid = *xcb_res_client_id_value_value(iter.data);
        pid_window_map.emplace(static_cast<int>(pid), window);
    }
}

auto X11Context::get_complete_window_ids() const -> std::vector<xcb::window_id>
{
    struct cookie_props {
        xcb::window_id window_id;
        std::array<xcb_get_property_cookie_t, 2> cookies;
        cookie_props(xcb::window_id window_id, xcb::connection_ptr conn) :
            window_id(window_id)
        {
            cookies[0] = xcb_get_property(conn, 0, window_id, XCB_ATOM_WM_CLASS, XCB_ATOM_ANY, 0, 4);
            cookies[1] = xcb_get_property(conn, 0, window_id, XCB_ATOM_WM_NAME, XCB_ATOM_ANY, 0, 4);
        }
    };
    auto windows = get_window_ids();
    auto cookies = util::make_vector<cookie_props>(windows.size());
    auto result = util::make_vector<xcb::window_id>(windows.size());

    for (auto window : windows) {
        cookies.emplace_back(window, connection.get());
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
            result.emplace_back(cookie_prop.window_id);
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
    const auto &reply = *reply_result;
    logger->debug("parent window size: {}x{}", reply->width, reply->height);
    parent_geometry.width = reply->width;
    parent_geometry.height = reply->height;
    return {};
}

void X11Context::create_gcontext()
{
    gcontext = xcb_generate_id(connection.get());
    xcb_create_gc(connection.get(), gcontext, screen->root, 0, nullptr);
    logger->debug("created gc with id {}", gcontext);
}

void X11Context::handle_xcb_error(xcb::error &err) const
{
    handle_xcb_error(err.get());
}

void X11Context::handle_xcb_error(xcb::error_ptr err) const
{
    const char *extension = nullptr;
    const char *major = xcb_errors_get_name_for_major_code(err_ctx.get(), err->major_code);
    const char *minor = xcb_errors_get_name_for_minor_code(err_ctx.get(), err->major_code, err->minor_code);
    const char *error = xcb_errors_get_name_for_error(err_ctx.get(), err->error_code, &extension);

    const std::string_view ext_str = extension != nullptr ? extension : "no_extension";
    const std::string_view minor_str = minor != nullptr ? minor : "no_minor";
    logger->error("XCB: {}:{}, {}:{}, resource {} sequence {}", error, ext_str, major, minor_str, err->resource_id,
                  err->sequence);
}

} // namespace upp
