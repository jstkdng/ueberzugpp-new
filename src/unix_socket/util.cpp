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

#include "os.hpp"
#include "unix_socket.hpp"

#include <format>
#include <sys/socket.h>

namespace unix_socket
{

auto util::create_socket(const std::string_view endpoint) -> std::expected<sockfd, std::string>
{
    sockfd result{};
    result.addr.sun_family = AF_UNIX;
    endpoint.copy(result.addr.sun_path, endpoint.length());

    result.fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (result.fd == -1) {
        return os::system_error(std::format("could not create socket for {}", endpoint));
    }
    return result;
}

} // namespace unix_socket
