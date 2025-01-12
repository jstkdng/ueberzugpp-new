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

#include <cstdlib>
#include <memory>
#include <type_traits>

namespace upp
{

template <auto fn>
using deleter_fn = std::integral_constant<std::decay_t<decltype(fn)>, fn>;

template <typename T, auto fn>
using c_unique_ptr = std::unique_ptr<T, deleter_fn<fn>>;

template <typename T>
using unique_C_ptr = c_unique_ptr<T, std::free>;

} // namespace upp
