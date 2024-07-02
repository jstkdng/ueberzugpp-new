// Display images inside a terminal
// Copyright (C) 2023  JustKidding
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef WORKER_POOL_HPP
#define WORKER_POOL_HPP

#include "canvas_worker.hpp"

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#ifndef HAVE_STD_JTHREAD
#  include "jthread/jthread.hpp"
#endif

template <WorkerType T>
class WorkerPool
{
  public:
    void start()
    {
        const uint32_t num_threads = std::thread::hardware_concurrency();
        for (uint32_t i = 0; i < num_threads; ++i) {
            auto ptr = std::make_shared<T>();
            auto thread = std::jthread([this, ptr](auto token) { do_work(token, ptr); });
            threads.emplace_back(std::move(ptr), std::move(thread));
        }
    }

    void do_work(const std::stop_token &token, [[maybe_unused]] std::shared_ptr<T> worker)
    {
        while (!token.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void stop()
    {
        for (auto &[worker, thread] : threads) {
            thread.request_stop();
        }
    }

  private:
    std::vector<std::pair<std::shared_ptr<T>, std::jthread>> threads;
    std::mutex queue_mutex;
    std::condition_variable condition;
};

#endif // WORKER_POOL_HPP
