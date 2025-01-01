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

namespace upp::os::unix
{

class fd
{
  public:
    ~fd();

    // rule of 5
    fd() = default;
    fd(const fd &other) = delete;
    auto operator=(const fd &other) -> fd & = delete;
    fd(fd &&other) noexcept;
    auto operator=(fd &&other) noexcept -> fd &;

    explicit fd(int descriptor);

    auto operator=(int new_fd) -> fd &;

    [[nodiscard]] auto dup() const -> fd;
    [[nodiscard]] auto get() const -> int;

  private:
    int descriptor = -1;
};

namespace socket
{
};

}; // namespace upp::os::unix
