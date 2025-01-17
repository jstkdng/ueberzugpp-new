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

template<auto Fn>
struct deleter_type {
    template <typename T>
    constexpr void operator()(T *ptr) const
    {
        Fn(const_cast<std::remove_const_t<T> *>(ptr));
    }
};

struct free_deleter {
    template <typename T>
    constexpr void operator()(T *ptr) const
    {
        std::free(const_cast<std::remove_const_t<T> *>(ptr)); // NOLINT
    }
};

template <auto Fn>
using deleter_fn = std::integral_constant<std::decay_t<decltype(Fn)>, Fn>;

template <typename T, auto Fn>
using c_unique_ptr = std::unique_ptr<T, deleter_type<Fn>>;

template <typename T>
using unique_C_ptr = std::unique_ptr<T, free_deleter>;

} // namespace upp
