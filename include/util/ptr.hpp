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

#include <cstdlib>
#include <memory>
#include <type_traits>

/*
template <auto &Fn>
struct deleter_from_fn {
    template <typename T>
    constexpr void operator()(T *ptr) const
    {
        Fn(const_cast<std::remove_const_t<T> *>(ptr));
    }
};

template <auto &Fn>
struct deleter_from_fn_null {
    template <typename T>
    constexpr void operator()(T *ptr) const
    {
        if (ptr != nullptr) {
            Fn(const_cast<std::remove_const_t<T> *>(ptr));
        }
    }
};*/

template <auto t>
using deleter_fn = std::integral_constant<std::decay_t<decltype(t)>, t>;

// custom unique pointer
template <typename T, auto fn>
using c_unique_ptr = std::unique_ptr<T, deleter_fn<fn>>;

// custom unique pointer that checks if null before deleting
//template <typename T, auto &Fn>
//using cn_unique_ptr = std::unique_ptr<T, deleter_from_fn_null<Fn>>;

template <typename T>
using unique_C_ptr = c_unique_ptr<T, std::free>;
