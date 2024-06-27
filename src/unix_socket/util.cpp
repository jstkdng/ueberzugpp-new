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

#include "unix_socket.hpp"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "os.hpp"

using namespace unix_socket;

auto util::get_socket_and_address(const std::string_view endpoint) -> std::expected<socket_and_address, std::string>
{
    socket_and_address result{};
    result.address.sun_family = AF_UNIX;
    endpoint.copy(result.address.sun_path, endpoint.size());

    result.fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (result.fd == -1) {
        return os::system_error("could not create socket");
    }

    return result;
}
