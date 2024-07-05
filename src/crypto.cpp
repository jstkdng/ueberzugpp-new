// Display images inside a terminal
// Copyright (C) 2024  JustKidding
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

#include "crypto.hpp"
#include "util.hpp"
#include "util/ptr.hpp"

#include <span>
#include <vector>

#include <openssl/evp.h>

auto crypto::get_b2_hash(const std::string_view str) -> std::string
{
    const auto mdctx = c_unique_ptr<EVP_MD_CTX, EVP_MD_CTX_free>{EVP_MD_CTX_new()};
#ifdef LIBRESSL_VERSION_NUMBER
    const auto *evp = EVP_sha256();
#else
    const auto *evp = EVP_blake2b512();
#endif

    std::vector<std::byte> digest(EVP_MAX_MD_SIZE);
    EVP_DigestInit_ex(mdctx.get(), evp, nullptr);
    EVP_DigestUpdate(mdctx.get(), str.data(), str.size());
    unsigned int digest_len = 0;
    EVP_DigestFinal_ex(mdctx.get(), std::bit_cast<unsigned char *>(digest.data()), &digest_len);

    return util::bytes_to_hexstring(std::span{digest.data(), digest_len});
}

auto crypto::base64_encode(const std::string_view str) -> std::string
{
    const auto length = str.length();
    const size_t bufsize = 4 * ((length + 2) / 3);
    std::string buffer(bufsize, 0);
#ifdef ENABLE_TURBOBASE64
    tb64enc(input, length, out);
#else
    EVP_EncodeBlock(std::bit_cast<unsigned char *>(buffer.data()), std::bit_cast<unsigned char *>(str.data()),
                    static_cast<int>(length));
#endif

    return buffer;
}
