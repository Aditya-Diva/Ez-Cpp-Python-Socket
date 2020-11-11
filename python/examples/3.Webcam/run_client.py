import cv2
import time
from ezpysocket import ezpysocket as ps


def client_operation(c: ps.EzPySocket, data: dict):
    """[summary] The actual function that will be looped while streaming

    Args:
        c (ps.EzPySocket): [Socket object]
        data (dict): [Collection of all data needs to be passed through, this
        can be a empty dict too]
    """
    # Collect data
    ret, frame = data["camera"].read()
    if ret:
        frame = cv2.resize(frame, (1920, 1080))

        # Send image to server
        c.send_image(frame)

        # Receive image from server
        frame = c.receive_image()

        cv2.imshow("frame", frame)
        if cv2.waitKey(1) == ord('q'):
            c.stop_loop()


# Modes of operation
# 0 - Server is up until Client has gotten its request
MODE = 0

if MODE == 0:
    # Toggle to compare FPS when processing is done on server vs within script
    WITH_SERVER = True
    # Init
    cam = cv2.VideoCapture(0)
    print("Press key 'q' to exit !")
    c = None
    if WITH_SERVER:
        c = ps.EzPySocket(tokens=["start", "end"],
                          reconnect_on_address_busy=5.0,
                          server_mode=False)
        # Set configuration as needed (Increase this value incase end token is
        # not being read, i.e. give time for packets to be written properly from
        # server end
        # c.set_sleep_between_packets(0.00005)

    data = {"camera": cam}
    if WITH_SERVER:
        c.client_loop(client_operation, data)

    cv2.destroyAllWindows()
    cam.release()
