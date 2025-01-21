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

#include "buildconfig.hpp"
#include "util/result.hpp"

#ifdef ENABLE_X11
#include "x11/context.hpp"
#endif

namespace upp
{

class ApplicationContext
{
  public:
    auto init() -> Result<void>;

#ifdef ENABLE_X11
    X11Context x11;
#endif

  private:
    auto x11_init() -> Result<void>;
};

} // namespace upp
