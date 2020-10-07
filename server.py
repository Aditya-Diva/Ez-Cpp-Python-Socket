import socket
import sys
import cv2

import numpy as np


def receive_image(self):
    data = self.receive(False)
    if data is not None:
        data = np.fromstring(data, dtype='uint8')
        decimg = cv2.imdecode(data, 1)
        return decimg
    return None


# Create a ipv4 TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the port
hostname = '127.0.0.1'  # socket.gethostname()  # 'localhost'
port_number = 10000
server_address = (hostname, port_number)

print('starting up on {} port {}'.format(*server_address))

sock.bind(server_address)

# Listen for incoming connections
sock.listen(1)

while True:
    # Wait for a connection
    print('waiting for a connection')
    connection, client_address = sock.accept()  # blocking
    try:
        print(f'connection from {client_address} has been established.')

        # Receive the data in small chunks and retransmit it
        # while True:
        data = connection.recv(16)  # blocking
        print('received {!r} as message length'.format(data))
        print(type(data))
        print("data:", int(data.decode("utf-8")))

        data_img = connection.recv(int(data.decode("utf-8")))
        data_img = np.frombuffer(data_img, dtype='uint8')
        decimg = cv2.imdecode(data_img, 1)
        print(decimg.shape)
        # data_img = data_img.reshape(3, 512, 512)
        # cv_img = cv2.fromarray(data_img)

        print(data_img)
        print(data_img.size)
        print(data_img.shape)
        print(type(data_img))
        # decimg = cv2.imdecode(data_img, 1)

        # print("data_img received") if data_img else print(
        #     "no data_img received")
        # print(sys.getsizeof(data_img))
        cv2.imshow("test", decimg)

        # print("receiving data of length:", data)
        # if data:
        #     # process data here
        #     print('sending data back to the client')
        #     connection.sendall(data)
        # else:
        #     print('no data from', client_address)
        #     break

        if cv2.waitKey(1) == ord('q'):
            break

    finally:
        # Clean up the connection
        connection.close()
