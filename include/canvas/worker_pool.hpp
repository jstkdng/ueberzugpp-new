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
#include "config.hpp"

#include <chrono>
#include <condition_variable>
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
    template <class... Params>
    void start(Params &&...params)
    {
        for (int i = 0; i < config->num_workers; ++i) {
            auto ptr = std::make_shared<T>(std::forward<Params>(params)...);
            ptr->initialize();
            auto thread = std::jthread([this, ptr](auto token) { do_work(token, ptr); });
            threads.emplace_back(std::move(ptr), std::move(thread));
        }
    }

    void do_work(const std::stop_token &token, [[maybe_unused]] std::shared_ptr<T> worker)
    {
        while (!token.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(config->waitms));
        }
    }

    void stop()
    {
        for (auto &[worker, thread] : threads) {
            thread.request_stop();
        }
    }

  private:
    std::shared_ptr<Config> config = Config::instance();
    std::vector<std::pair<std::shared_ptr<T>, std::jthread>> threads;
    std::mutex queue_mutex;
    std::condition_variable condition;
};

#endif // WORKER_POOL_HPP
