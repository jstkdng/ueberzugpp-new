# Display images inside a terminal
# Copyright (C) 2024  JustKidding
#
# This file is part of ueberzugpp.
#
# ueberzugpp is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ueberzugpp is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with ueberzugpp.  If not, see <https://www.gnu.org/licenses/>.

import socket
import tempfile
import json
from pathlib import Path


def main():
    image_files = Path(Path.home() / "Pictures").glob("*.png")
    upp_sockets = Path(tempfile.gettempdir()).glob("ueberzugpp-*.socket")
    upp_socket = next(upp_sockets)

    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.connect(str(upp_socket))

    for image in image_files:
        payload = {
            "action": "add",
            "identifier": "preview",
            "path": str(image),
            "x": 0,
            "y": 0,
            "max_width": 50,
            "max_height": 50
        }
        s.sendall(str.encode(json.dumps(payload) + "\n"))

    s.close()


if __name__ == "__main__":
    main()
