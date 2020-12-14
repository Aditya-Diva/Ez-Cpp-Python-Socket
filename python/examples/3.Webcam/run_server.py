import cv2
import time
from ezpysocket import ezpysocket as ps


def server_operation(s: ps.EzPySocket, data: dict):
    """[summary] The actual function that will be looped while streaming

    Args:
        s (ps.EzPySocket): [Socket object]
        data (dict): [Collection of all data needs to be passed through, this
        can be a empty dict too]
    """
    # Receive image from client
    frame = s.receive_image()

    # Processing
    if frame is not None:
        edges = cv2.Canny(cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY), 100, 200)

    # Send image to client
    s.send_image(edges)


# Modes of operation
# 0 - Server is up until Client has gotten its request
MODE = 0

if __name__ == "__main__":
    # Init
    s = ps.EzPySocket(tokens=["start", "end"],
                      reconnect_on_address_busy=5.0,
                      client_connection_count=1)

    if MODE == 0:
        s.server_loop(server_operation)
