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

#pragma once

#include "util/ptr.hpp"

// IWYU pragma: begin_exports
#include <xcb/res.h>
#include <xcb/xcb.h>
#include <xcb/xcb_errors.h>
#include <xcb/xcb_image.h>
#include <xcb/xproto.h>
// IWYU pragma: end_exports

#include <expected>
#include <type_traits>

namespace upp::xcb
{

using connection_ptr = xcb_connection_t *;
using connection = c_unique_ptr<xcb_connection_t, &xcb_disconnect>;
using image = c_unique_ptr<xcb_image_t, &xcb_image_destroy>;
using screen_ptr = xcb_screen_t *;
using window_id = xcb_window_t;
using gcontext = xcb_gcontext_t;

using error_ptr = xcb_generic_error_t *;
using error = unique_C_ptr<std::remove_pointer_t<error_ptr>>;
using errors_context = c_unique_ptr<xcb_errors_context_t, &xcb_errors_context_free>;

template <class Fn, class... Args>
auto get_result(Fn func, Args &&...args)
{
    error_ptr err = nullptr;
    auto *result = func(std::forward<Args>(args)..., &err);
    using XCBFunc = unique_C_ptr<std::remove_pointer_t<decltype(result)>>;
    using XCBResult = std::expected<XCBFunc, error>;
    if (!result) {
        return XCBResult{std::unexpected(err)};
    }
    return XCBResult{result};
}

class window
{
  public:
    window(connection_ptr connection, screen_ptr screen, window_id parent_id);
    ~window();
    auto operator=(window &&) -> window & = delete;
    void create();
    void configure(int xcoord, int ycoord, int width, int height);
    void hide() const;
    [[nodiscard]] auto id() const -> window_id;

  private:
    connection_ptr connection;
    screen_ptr screen;
    window_id parent_id;
    window_id _id;
};

} // namespace upp::xcb
