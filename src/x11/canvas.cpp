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

#include "x11/canvas.hpp"
#include "application/application.hpp"
#include "application/context.hpp"
#include "command/command.hpp"
#include "os/os.hpp"
#include "terminal.hpp"
#include "util/ptr.hpp"
#include "util/result.hpp"
#include "x11/window.hpp"

namespace upp
{
X11Canvas::X11Canvas(ApplicationContext *ctx) :
    ctx(ctx)
{
}

auto X11Canvas::init() -> Result<void>
{
    LOG_INFO("canvas created");
    event_handler = jthread([this](auto token) { handle_events(token); });
    return {};
}

void X11Canvas::execute(const Command &cmd)
{
    std::scoped_lock window_lock{window_mutex};
    if (cmd.action == "add") {
        handle_add_command(cmd);
    } else if (cmd.action == "remove") {
        handle_remove_command(cmd);
    }
}

void X11Canvas::handle_add_command(const Command &cmd)
{
    std::shared_ptr<X11Window> window_ptr;
    if (auto window = window_id_map.find(cmd.preview_id); window == window_id_map.end()) {
        LOG_TRACE("creating new window");
        window_ptr = std::make_shared<X11Window>(ctx, &window_map);
        window_ptr->create_xcb_windows();
    } else {
        LOG_TRACE("reusing existing window");
        window_ptr = window->second;
    }
    if (auto result = window_ptr->init(cmd)) {
        window_id_map.try_emplace(cmd.preview_id, window_ptr);
    } else {
        LOG_WARN(result.error().message());
    }
}

void X11Canvas::handle_remove_command(const Command &cmd)
{
    if (window_id_map.size() > 1) {
        window_id_map.erase(cmd.preview_id);
        return;
    }
    auto window = window_id_map.find(cmd.preview_id);
    if (window != window_id_map.end()) {
        window->second->hide_xcb_windows();
    }
}

void X11Canvas::handle_events(SToken token)
{
    LOG_DEBUG("started event handler");
    const int filde = ctx->x11.connection_fd;
    while (!token.stop_requested()) {
        if (auto in_event = os::wait_for_data_on_fd(filde)) {
            if (*in_event) {
                dispatch_events();
            }
        } else {
            Application::terminate();
        }
    }
}

void X11Canvas::dispatch_events()
{
    auto *conn = ctx->x11.connection.get();
    auto event = unique_C_ptr<xcb_generic_event_t>{xcb_poll_for_event(conn)};
    while (event) {
        constexpr int event_mask = 0x80;
        switch (int real_event = event->response_type & ~event_mask) {
            case 0: {
                auto *err = reinterpret_cast<xcb::error_ptr>(event.get());
                ctx->x11.handle_xcb_error(err);
                break;
            }
            case XCB_EXPOSE: {
                handle_expose_event(event.get());
                break;
            }
            default: {
                LOG_DEBUG("received unknown event {}", real_event);
                break;
            }
        }
        event.reset(xcb_poll_for_event(conn));
    }
}

void X11Canvas::handle_expose_event(xcb_generic_event_t *event)
{
    std::scoped_lock window_lock{window_mutex};
    const auto *expose = reinterpret_cast<xcb_expose_event_t *>(event);
    auto window_id = expose->window;
    auto window_ptr = window_map.find(window_id);
    if (window_ptr == window_map.end()) {
        return;
    }
    if (auto window = window_ptr->second.lock()) {
        window->draw(window_id);
    }
}

} // namespace upp
