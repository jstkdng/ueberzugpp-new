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
#include "base/image.hpp"
#include "command.hpp"
#include "os/os.hpp"
#include "terminal.hpp"
#include "util/ptr.hpp"
#include "util/result.hpp"

#include <spdlog/spdlog.h>

namespace upp
{

X11Canvas::X11Canvas(ApplicationContext *ctx, Terminal *terminal) :
    ctx(ctx),
    terminal(terminal)
{
}

auto X11Canvas::init() -> Result<void>
{
    SPDLOG_INFO("canvas created");
    event_handler = std::jthread([this](const std::stop_token &token) { handle_events(token); });
    return {};
}

void X11Canvas::execute(const Command &cmd)
{
    if (cmd.action != "add") {
        return;
    }
    auto image_result = Image::create(ctx, {.file_path = cmd.image_path.string(),
                                            .scaler = cmd.image_scaler,
                                            .width = terminal->font.width * cmd.width,
                                            .height = terminal->font.height * cmd.height});
    if (!image_result) {
        SPDLOG_WARN(image_result.error().message());
        return;
    }
    auto &image = *image_result;
    auto load_result = image->load();
    if (!load_result) {
        SPDLOG_WARN(load_result.error().message());
    }
}

void X11Canvas::handle_events(const std::stop_token &token)
{
    SPDLOG_DEBUG("started event handler");
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
        switch (const int real_event = event->response_type & ~event_mask; real_event) {
            case 0: {
                auto *err = reinterpret_cast<xcb::error_ptr>(event.get());
                ctx->x11.handle_xcb_error(err);
                break;
            }
            case XCB_EXPOSE: {
            }
            default: {
                SPDLOG_DEBUG("received unknown event {}", real_event);
                break;
            }
        }
        event.reset(xcb_poll_for_event(conn));
    }
}

} // namespace upp
