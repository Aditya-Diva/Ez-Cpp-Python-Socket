from ezpysocket import ezpysocket as ps
import cv2
import numpy as np

if __name__ == "__main__":

    # Init
    c = ps.EzPySocket(server_mode=False)

    # Load image
    img = cv2.imread("../resources/lena.jpg", 1)

    # Sending 
    print("Sending image...")
    c.send_image(img)

    # Receiving image
    print("Receiving image...")
    img = c.receive_image()
