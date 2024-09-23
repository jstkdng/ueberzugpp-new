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

#pragma once

namespace os
{

class fd
{
  public:
    fd() = default;
    explicit fd(int descriptor);
    fd(const fd &other);
    fd(fd &&other) noexcept;
    auto operator=(int new_fd) -> fd&;
    auto operator=(const fd &other) -> fd &;
    auto operator=(fd &&other) noexcept -> fd &;

    auto operator*() const -> int;

    ~fd();

  private:
    int descriptor = -1;
};

} // namespace os
