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
    logger->info("canvas created");
    event_handler = std::jthread([this](const std::stop_token &token) { handle_events(token); });
    return {};
}

void X11Canvas::execute(const Command &cmd)
{
    if (cmd.action == "add") {
        auto window = std::make_shared<X11Window>(ctx, &window_map);
        auto result = window->init(cmd);
        if (!result) {
            logger->warn(result.error().message());
            return;
        }
        window_id_map.emplace(cmd.preview_id, window);
    } else if (cmd.action == "remove") {
        window_id_map.erase(cmd.preview_id);
    }
}

void X11Canvas::handle_events(const std::stop_token &token)
{
    logger->debug("started event handler");
    const int filde = ctx->x11.connection_fd;
    while (!token.stop_requested()) {
        auto in_event = os::wait_for_data_on_fd(filde);
        if (!in_event) {
            Application::terminate();
            return;
        }

        if (!*in_event) {
            continue;
        }

        dispatch_events();
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
                logger->debug("received unknown event {}", real_event);
                break;
            }
        }
        event.reset(xcb_poll_for_event(conn));
    }
}

void X11Canvas::handle_expose_event(xcb_generic_event_t *event)
{
    auto *expose = reinterpret_cast<xcb_expose_event_t *>(event);
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
