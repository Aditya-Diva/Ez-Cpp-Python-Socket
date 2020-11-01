from ezpysocket import ezpysocket as ps
import cv2
import numpy as np

if __name__ == "__main__":

    # Init
    s = ps.EzPySocket(server_mode=False, tokens=[
                      "start", "end"], debug=True, reconnect_on_address_busy=5)

    # Load image
    img = cv2.imread("lena.jpg", 1)

# Send examples
    print("Sending data...")

    s.send_bool(True)
    s.send_bool(False)
    s.send_bool(0)
    s.send_bool(1)

    s.send_string("It was the only way.")

    s.send_int(1024)
    s.send_float(3.14)

    # Send an int list
    a = []
    for i in range(100):
        a.append(i)
    s.send_int_list(a)

    # Send a float list
    a = np.arange(0, 10, 0.25)
    b = [i for i in a]
    s.send_float_list(b)

    s.send_image(img)

# Receive examples
    print("Receiving data...")

    print("Bool 1 : ", s.receive_bool())
    print("Bool 2 : ", s.receive_bool())
    print("Bool 3 : ", s.receive_bool())
    print("Bool 4 : ", s.receive_bool())

    data = s.receive_string()
    print("String : ", data)

    data = s.receive_int()
    print("Int : ", data)

    data = s.receive_float()
    print("Float : ", data)

    data = s.receive_int_list()
    print("Int List : ", data)

    data = s.receive_float_list()
    print("Float List : ", data)

    img = s.receive_image()
    print("Showing image...")
    cv2.imshow("Python Client Received image", img)
    print("Press a key on img to exit demo")
    cv2.waitKey(0)

    s.disconnect()
