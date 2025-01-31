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

#include "util/crypto.hpp"
#include "util/ptr.hpp"

#include <openssl/evp.h>
#include <openssl/rand.h>

#include <vector>

namespace upp::crypto
{

auto buffer_to_hexstring(std::span<const std::byte> buffer) -> std::string
{
    std::string result(buffer.size() * 2, 0);
    constexpr std::string_view lower_hex_chars = "0123456789abcdef";
    constexpr int hexint = 16;

    std::size_t idx = 0;
    for (auto byte : buffer) {
        int chars_idx = std::to_integer<int>(byte);
        result[idx + 1] = lower_hex_chars[chars_idx % hexint];
        chars_idx /= hexint;
        result[idx] = lower_hex_chars[chars_idx % hexint];
        idx += 2;
    }

    return result;
}

auto blake2b_encode(std::span<const std::byte> buffer) -> std::string
{
    const auto mdctx = c_unique_ptr<EVP_MD_CTX, EVP_MD_CTX_free>{EVP_MD_CTX_new()};
#ifdef LIBRESSL_VERSION_NUMBER
    const auto *evp = EVP_sha256();
#else
    const auto *evp = EVP_blake2b512();
#endif

    std::vector<unsigned char> digest(EVP_MAX_MD_SIZE);
    EVP_DigestInit_ex(mdctx.get(), evp, nullptr);
    EVP_DigestUpdate(mdctx.get(), buffer.data(), buffer.size());
    unsigned int digest_len = 0;
    EVP_DigestFinal_ex(mdctx.get(), digest.data(), &digest_len);

    return buffer_to_hexstring(std::as_bytes(std::span{digest.data(), digest_len}));
}

auto base64_encode(std::span<const std::byte> buffer) -> std::string
{
    const auto length = buffer.size();
    const size_t bufsize = 4 * ((length + 2) / 3);
    std::vector<unsigned char> result(bufsize);
    EVP_EncodeBlock(result.data(), reinterpret_cast<const unsigned char *>(buffer.data()), static_cast<int>(length));
    return buffer_to_hexstring(std::as_bytes(std::span{result}));
}

auto generate_random_string(int length) -> std::string
{
    std::vector<unsigned char> buffer(length);
    RAND_bytes(buffer.data(), length);
    return buffer_to_hexstring(std::as_bytes(std::span{buffer}));
}

} // namespace upp::crypto
