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

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <mutex>
#include <optional>

namespace upp
{

template <class T>
class ConcurrentDeque
{
  public:
    void enqueue(T &&item)
    {
        std::scoped_lock lock{queue_mutex};
        queue.push_back(std::move(item));
        cond.notify_all();
    }

    void enqueue(const T &item)
    {
        std::scoped_lock lock{queue_mutex};
        queue.push_back(item);
        cond.notify_all();
    }

    auto dequeue() -> T
    {
        std::unique_lock lock{queue_mutex};
        cond.wait(lock, [this] { return !queue.empty(); });
        auto elem = queue.front();
        queue.pop_front();
        return elem;
    }

    auto try_dequeue(int waitms) -> std::optional<T>
    {
        std::unique_lock lock{queue_mutex};
        bool ready = cond.wait_for(lock, std::chrono::milliseconds(waitms), [this] { return !queue.empty(); });
        if (!ready) {
            return {};
        }

        auto elem = queue.front();
        queue.pop_front();
        return elem;
    }

    void clear()
    {
        std::scoped_lock lock{queue_mutex};
        queue.clear();
    }

    void run_locked(std::function<void(std::deque<T> &)> func)
    {
        std::scoped_lock lock{queue_mutex};
        func(queue);
    }

    auto size() -> std::size_t
    {
        std::scoped_lock lock{queue_mutex};
        return queue.size();
    }

  private:
    std::deque<T> queue;
    std::mutex queue_mutex;
    std::condition_variable cond;
};

} // namespace upp
