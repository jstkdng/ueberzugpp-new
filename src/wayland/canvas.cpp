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

#include "wayland/canvas.hpp"
#include "application/application.hpp"
#include "application/context.hpp"
#include "command/command.hpp"
#include "unix/fd.hpp"
#include "util/result.hpp"
#include "wayland/types.hpp"

#include <sys/mman.h>

#include <spdlog/spdlog.h>

#include <cstdint>
#include <cstring>
#include <string_view>

namespace upp
{

constexpr wl_registry_listener registry_listener = {.global = WaylandCanvas::wl_registry_global,
                                                    .global_remove = wl::ignore};

constexpr xdg_wm_base_listener xdg_wm_base_listener = {.ping = WaylandCanvas::xdg_wm_base_ping};
constexpr wl_buffer_listener buffer_listener = {
    .release = WaylandCanvas::wl_buffer_release,
};

void WaylandCanvas::wl_registry_global(void *data, wl_registry *registry, uint32_t name, const char *interface,
                                       uint32_t version)
{
    const std::string_view interface_str(interface);

    const uint32_t compositor_ver = 6;
    const uint32_t shm_ver = 1;
    const uint32_t xdg_base_ver = 2;

    auto *canvas = static_cast<WaylandCanvas *>(data);
    if (interface_str == wl_compositor_interface.name) {
        canvas->logger->debug("compositor version wanted: {}, have: {}", compositor_ver, version);
        canvas->compositor.reset(
            static_cast<wl_compositor *>(wl_registry_bind(registry, name, &wl_compositor_interface, compositor_ver)));
    } else if (interface_str == wl_shm_interface.name) {
        canvas->shm.reset(static_cast<wl_shm *>(wl_registry_bind(registry, name, &wl_shm_interface, shm_ver)));
    } else if (interface_str == xdg_wm_base_interface.name) {
        canvas->wm_base.reset(
            static_cast<xdg_wm_base *>(wl_registry_bind(registry, name, &xdg_wm_base_interface, xdg_base_ver)));
        xdg_wm_base_add_listener(canvas->wm_base.get(), &xdg_wm_base_listener, nullptr);
    }
}

void WaylandCanvas::xdg_wm_base_ping(void * /*data*/, xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

void WaylandCanvas::wl_buffer_release(void * /*data*/, wl_buffer *buffer)
{
    wl_buffer_destroy(buffer);
}

WaylandCanvas::WaylandCanvas(ApplicationContext *ctx) :
    ctx(ctx)
{
}

WaylandCanvas::~WaylandCanvas()
{
    if (event_handler.joinable()) {
        event_handler.join();
    }
}

auto WaylandCanvas::init() -> Result<void>
{
    display.reset(wl_display_connect(nullptr));
    if (!display) {
        return Err("could not connect to wayland display");
    }
    registry.reset(wl_display_get_registry(display.get()));
    wl_registry_add_listener(registry.get(), &registry_listener, this);
    wl_display_roundtrip(display.get());

    display_fd = wl_display_get_fd(display.get());
    event_handler = std::thread(&WaylandCanvas::handle_events, this);

    logger->info("canvas created");
    return {};
}

void WaylandCanvas::handle_events()
{
    logger->debug("started event handler");
    auto *display_ptr = display.get();

    while (!Application::stop_flag.test()) {
        while (wl_display_prepare_read(display_ptr) != 0) {
            wl_display_dispatch_pending(display_ptr);
        }
        wl_display_flush(display_ptr);

        if (auto in_event = os::wait_for_data_on_fd(display_fd)) {
            if (*in_event) {
                wl_display_read_events(display_ptr);
                wl_display_dispatch_pending(display_ptr);
            } else {
                wl_display_cancel_read(display_ptr);
            }
        } else {
            Application::terminate();
        }
    }
}

auto WaylandCanvas::create_buffer(int width, int height, unsigned char *image_data) -> Result<wl::buffer_ptr>
{
    const int stride = width * 4;
    const int image_size = height * stride;
    const int pool_size = image_size * 2;
    unix::fd memfd{memfd_create("ueberzugpp-shm", 0)};
    if (!memfd) {
        return Err("memfd_create");
    }
    if (ftruncate(memfd.get(), pool_size) == -1) {
        return Err("ftruncate");
    }
    auto *pool_ptr = mmap(nullptr, pool_size, PROT_READ | PROT_WRITE, MAP_SHARED, memfd.get(), 0);
    if (pool_ptr == MAP_FAILED) {
        return Err("mmap");
    }
    wl::shm_pool pool{wl_shm_create_pool(shm.get(), memfd.get(), pool_size)};
    wl::buffer_ptr buffer = wl_shm_pool_create_buffer(pool.get(), 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);
    std::memcpy(pool_ptr, image_data, image_size);
    wl_buffer_add_listener(buffer, &buffer_listener, nullptr);
    return buffer;
}

void WaylandCanvas::execute(const Command &cmd)
{
    if (cmd.action == "add") {
        auto [entry, inserted] = window_map.try_emplace(cmd.preview_id, this);
        entry->second.init(cmd);
    } else {
        window_map.erase(cmd.preview_id);
    }
}

} // namespace upp
