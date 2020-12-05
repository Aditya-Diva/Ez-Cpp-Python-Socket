from ezpysocket import ezpysocket as ps
import cv2
import numpy as np

if __name__ == "__main__":

    while True:
        # Init
        s = ps.EzPySocket(reconnect_on_address_busy=True)

        print("Receiving image...")
        img = s.receive_image()

        print("Sending image...")
        s.send_image(img)

        s.disconnect()