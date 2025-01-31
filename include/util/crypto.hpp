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

#include <span>
#include <string>
#include <string_view>

namespace upp::crypto
{

auto buffer_to_hexstring(std::span<const std::byte> buffer) -> std::string;
auto blake2b_encode(std::string_view buffer) -> std::string;
auto base64_encode(std::span<const std::byte> buffer) -> std::string;
auto generate_random_string(int length) -> std::string;

} // namespace upp::crypto
