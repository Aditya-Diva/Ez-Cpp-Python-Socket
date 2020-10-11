import pycserver as ps
import cv2

if __name__ == "__main__":

    # Init
    s = ps.PyCServer()

    # Load image
    img = cv2.imread("lena.jpg", 1)

# Receive examples
    print("Receiving data...")

    data = s.receive_int()
    print(data)

    data = s.receive_string()
    print(data)

    img = s.receive_image()
    print("Showing image...")
    cv2.imshow("Python Received image", img)
    cv2.waitKey(0)

# Send examples
    print("Sending data...")

    s.send_string("Hello there, Dr. Strange")

    s.send_int(1024)

    # Send a list
    a = []
    for i in range(100):
        a.append(i)
    s.send_int_list(a)

    s.send_image(img)
