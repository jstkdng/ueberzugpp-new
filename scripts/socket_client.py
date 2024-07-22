import socket
import tempfile
import json
from pathlib import Path


def main():
    image_files = Path(Path.home() / "Pictures").glob("*.png")
    tmp_dir = Path(tempfile.gettempdir())
    upp_sockets = tmp_dir.glob("ueberzugpp-*.socket")
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
