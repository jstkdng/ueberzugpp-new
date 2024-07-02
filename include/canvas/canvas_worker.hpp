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

#ifndef CANVAS_WORKER_HPP
#define CANVAS_WORKER_HPP

#include <string>
#include <type_traits>

class CanvasWorker
{
  public:
    [[nodiscard]] virtual auto get_string_id() const -> std::string = 0;
    [[nodiscard]] virtual auto get_internal_id() const -> std::size_t = 0;
    virtual void initialize() = 0;
    virtual ~CanvasWorker() = default;
};

template <class T>
concept WorkerType = std::is_base_of_v<CanvasWorker, T>;

#endif // CANVAS_WORKER_HPP
