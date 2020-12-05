from ezpysocket import ezpysocket as ps
import cv2
import numpy as np

if __name__ == "__main__":

    # Init
    c = ps.EzPySocket(server_mode=False, tokens=[
                      "start", "end"], reconnect_on_address_busy=5)

    # Load image
    img = cv2.imread("../resources/lena.jpg", 1)

# Send examples
    print("Sending data...")

    c.send_bool(True)
    c.send_bool(False)
    c.send_bool(0)
    c.send_bool(1)

    c.send_string("It was the only way.")

    c.send_int(1024)
    c.send_float(3.14)

    # Send an int list
    a = []
    for i in range(100):
        a.append(i)
    c.send_int_list(a)

    # Send a float list
    a = np.arange(0, 10, 0.25)
    b = [i for i in a]
    c.send_float_list(b)

    c.send_image(img)

# Receive examples
    print("Receiving data...")

    print("Bool 1 : ", c.receive_bool())
    print("Bool 2 : ", c.receive_bool())
    print("Bool 3 : ", c.receive_bool())
    print("Bool 4 : ", c.receive_bool())

    data = c.receive_string()
    print("String : ", data)

    data = c.receive_int()
    print("Int : ", data)

    data = c.receive_float()
    print("Float : ", data)

    data = c.receive_int_list()
    print("Int List : ", data)

    data = c.receive_float_list()
    print("Float List : ", data)

    img = c.receive_image()
    print("Showing image...")
    cv2.imshow("Python Client Received image", img)
    print("Press a key on img to exit demo")
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    c.disconnect()