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

#include "canvas/x11/x11_canvas.hpp"
#include "os/os.hpp"
#include "util/ptr.hpp"
#include "util/util.hpp"

#include <spdlog/spdlog.h>

using std::unexpected;

X11Canvas::~X11Canvas()
{
#ifdef ENABLE_XCB_ERRORS
    xcb_errors_context_free(err_ctx);
#endif
    xcb_disconnect(connection);
}

auto X11Canvas::initialize(CommandManager *manager) -> std::expected<void, std::string>
{
    SPDLOG_DEBUG("initializing canvas");
    command_manager = manager;
    connection = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(connection) > 0) {
        return util::unexpected_err("can't connect to x11 server");
    }
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
#ifdef ENABLE_XCB_ERRORS
    xcb_errors_context_new(connection, &err_ctx);
#endif

    event_handler = std::jthread([this](auto token) { handle_events(token); });
    command_reader = std::jthread([this](auto token) { read_commands(token); });
    SPDLOG_INFO("canvas initialized");
    return {};
}

void X11Canvas::read_commands(const std::stop_token &token)
{
    while (!token.stop_requested()) {
        auto command_ok = command_manager->unqueue();
        if (!command_ok) {
            continue;
        }
        const auto &json = *command_ok;
        const auto &action = json.value("action", "");
        const auto &preview_id = json.value("identifier", "");
        if (action == "add") {
            handle_add_command(preview_id, json);
        } else if (action == "remove") {
            windows.erase(preview_id);
        } else {
            SPDLOG_WARN("unknown command received: {}", json.dump());
        }
    }
}

void X11Canvas::handle_add_command(const std::string &preview_id, const nlohmann::json &json)
{
    const auto found = windows.find(preview_id);
    if (found == windows.end()) {
        SPDLOG_INFO("creating new window");
        auto new_win = std::make_shared<X11Window>(connection, screen);
        auto init_ok = new_win->initialize(json);
        if (!init_ok) {
            SPDLOG_WARN("could not initialize window with id {}: {}", preview_id, init_ok.error());
            return;
        }
        auto [entry, inserted] = windows.try_emplace(preview_id, new_win);
        if (!inserted) {
            SPDLOG_WARN("could not insert new window with id {}", preview_id);
        }
    } else {
        auto init_ok = found->second->initialize(json);
        if (!init_ok) {
            SPDLOG_WARN("could not reinitialize window with id {}: {}", preview_id, init_ok.error());
        }
    }
}

void X11Canvas::handle_events(const std::stop_token &token) const
{
    constexpr std::byte event_mask{0x80};
    const int connfd = xcb_get_file_descriptor(connection);

    SPDLOG_DEBUG("started event handler");
    while (!token.stop_requested()) {
        auto in_event = os::wait_for_data_on_fd(connfd, config->waitms);
        if (!in_event) {
            SPDLOG_TRACE("stopping event handler: {}", in_event.error());
            return;
        }
        if (!*in_event) {
            continue;
        }

        auto event = unique_C_ptr<xcb_generic_event_t>{xcb_poll_for_event(connection)};
        while (event) {
            const std::byte res_type{event->response_type};
            auto real_event = std::to_integer<int>(res_type & ~event_mask);
            switch (real_event) {
                case 0: {
                    auto *err = reinterpret_cast<xcb_generic_error_t *>(event.get());
                    print_xcb_error(err);
                    break;
                }
                case XCB_EXPOSE: {
                    [[maybe_unused]] const auto *expose = reinterpret_cast<xcb_expose_event_t *>(event.get());
                    SPDLOG_DEBUG("received expose event for window {}", expose->window);
                    break;
                }
                default: {
                    SPDLOG_DEBUG("received unknown event: {}", real_event);
                    break;
                }
            }
            event.reset(xcb_poll_for_event(connection));
        }
    }
}

void X11Canvas::print_xcb_error(xcb_generic_error_t *err) const
{
#ifdef ENABLE_XCB_ERRORS
    const char *extension = nullptr;
    const char *major = xcb_errors_get_name_for_major_code(err_ctx, err->major_code);
    const char *minor = xcb_errors_get_name_for_minor_code(err_ctx, err->major_code, err->minor_code);
    const char *error = xcb_errors_get_name_for_error(err_ctx, err->error_code, &extension);

    const std::string_view ext_str = extension != nullptr ? extension : "no_extension";
    const std::string_view minor_str = minor != nullptr ? minor : "no_minor";
    SPDLOG_ERROR("XCB: {}:{}, {}:{}, resource {} sequence {}", error, ext_str, major, minor_str, err->resource_id,
                 err->sequence);
#else
    SPDLOG_ERROR("XCB: resource {} sequence {}", err->resource_id, err->sequence);
#endif
}
