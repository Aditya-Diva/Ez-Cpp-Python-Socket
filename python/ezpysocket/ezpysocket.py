import socket
import cv2
import numpy as np


class EzPySocket:
    """[summary] Python - Cpp Communication Server Object
    """

    def __init__(self, server_address: str = "127.0.0.1",
                 server_port: int = 10000, socket_family=socket.AF_INET,
                 socket_type=socket.SOCK_STREAM, debug: bool = False,
                 auto_connect: bool = True,
                 client_connection_count: int = 1,
                 server_mode: bool = True):
        """[summary]

        Args:
            server_address (str, optional): [localhost or 127.0.0.1 searches
            for connections within the same system]. Defaults to "127.0.0.1".

            server_port (int, optional): [Port number]. Defaults to 10000.

            socket_family ([type], optional): [IPV4/IPV6]. Defaults to
            socket.AF_INET.
            # AF_INET (IPv4 protocol)
            # AF_INET6 (IPv6 protocol)

            socket_type ([type], optional): [TCP/UDP]. Defaults to
            socket.SOCK_STREAM.
            # SOCK_STREAM: TCP(reliable, connection oriented)
            # SOCK_DGRAM: UDP(unreliable, connectionless)

            debug (bool, optional): [For printing debug]. Defaults to False.

            auto_connect (bool, optional): [In case the connection must be
            initialized on construction of object directly]. Defaults to True.
        """
        self.__debug = debug

        self.__socket_family = socket_family

        self.__socket_type = socket_type

        self.create_socket() 

        # Bind sever socket to specific address and port
        self.__server_address = (server_address, server_port)

        if self.__debug:
                print("Starting " + ("server" if server_mode else "client"))
                print('Starting up on {} port {}'.format(*self.__server_address))

        if server_mode:
            self.__sock.bind(self.__server_address)

            # number of client connection(s) to accept
            self.__connection_count = client_connection_count

            # Listen for incoming connection(s) from clients
            self.__sock.listen(self.__connection_count)

            if auto_connect:
                self.connect()
        else:
            self.__sock.connect(self.__server_address)
            self.__connection = self.__sock

    def create_socket(self):
        """[summary]
        Create a socker
        """
        try:
            self.__sock = socket.socket(self.__socket_family,
                                        self.__socket_type)
            print("Socket created successfully")
        except socket.error as err:
            print("Socket creation failed : ", err)

    def connect(self):
        """[summary]
        Call this to verify client side connection with server.
        This is a blocking function and hence won't return until a connection
        is established.
        """
        # Wait for a connection
        print('Waiting for a connection ...')

        # (Blocking) Extract first connection request and connect
        self.__connection, self.__client_address = self.__sock.accept()

        print(f'Connection from {self.__client_address} has been '
              'established.')

    def disconnect(self):
        """[summary] Disconnect the connection if any
        """
        try:
            self.__connection.close()
        except:
            print("Connection already closed successfully")

    def __del__(self):
        self.disconnect()

    # Incoming

    def receive_bool(self) -> bool:
        """[summary] Receive a bytes array

        Returns:
            [bool]: [Boolean that was received.]
        """
        received = self.receive_string()
        print("debug:", received)
        ret = True
        value = False
        if received == "true":
            value = True
        elif received == "false":
            pass
        else:
            ret = False
            print(" Message received: ", received)
            print(" Unable to collect boolean information from message.")
        return ret, value

    def receive_string(self) -> str:
        """[summary] Receive a bytes array

        Returns:
            [bytes]: [String that was received.]
        """
        string_length = self.receive_int()
        if self.__debug:
            print("receive_string: string_length received : ",
                  string_length)

        data = self.__connection.recv(string_length)  # blocking
        if self.__debug:
            print('Received {!r} as message'.format(data))
        return str(data)[2:-1]  # remove the "b'" and the "'" at the end

    def receive_int(self, message_length: int = 16) -> int:
        """[summary] Receive an int value

        Args:
            message_length (int, optional): [Size of message to
            be read in bytes]. Defaults to 16.

        Returns:
            [int]: [The integer value that was received]
        """
        data = self.__connection.recv(message_length)  # blocking
        print("data received : ", repr(data))
        data = int(data.decode("utf-8"))
        if self.__debug:
            print("Receiving Buffer data of size (in bytes): ", message_length)
            print('Received {!r} as message'.format(data))
        return data

    def receive_float(self, message_length: int = 16) -> float:
        """[summary] Receive an float value

        Args:
            message_length (int, optional): [Size of message to
            be read in bytes]. Defaults to 16.

        Returns:
            [float]: [The float value that was received]
        """
        data = self.__connection.recv(message_length)  # blocking
        data = float(data.decode("utf-8"))
        if self.__debug:
            print("Receiving Buffer data of size (in bytes): ", message_length)
            print('Received {!r} as message'.format(data))
        return data

    def receive_int_list(self):
        """[summary] Receive a list of int

        Returns:
            [list]: [List of ints]
        """
        received = self.receive_string()
        print("debug:", received)
        return eval(received)

    def receive_float_list(self):
        """[summary] Receive a list of floats

        Returns:
            [list]: [List of floats]
        """
        received = self.receive_string()
        print("debug:", received)
        return eval(received)

    def receive_image(self, message_length: int = 1024,
                      receive_size_first: bool = True,
                      color_format: int = cv2.IMREAD_COLOR,
                      dtype: str = 'uint8'):
        """[summary] Receive an image

        Args:
            message_length (int, optional): [Size of message to
            be read in bytes]. Defaults to 1024.

            receive_size_first (bool, optional): [Receive flag which waits
            for the size of message for the image to be received first, and
            then reads buffer for the image accordingly. If false the
            message_length value is considered.]. Defaults to True.

            color_format (int, optional): [The color format to be used].
            Defaults to cv2.IMREAD_COLOR.

            dtype (str, optional): [Datatype to be used when reading buffer].
            Defaults to 'uint8'.

        Returns:
            [cv2.Mat]: [cv2 image that was received]
        """

        if receive_size_first:
            message_length = self.receive_int()
            if self.__debug:
                print("receive_image: message_length received : ",
                      message_length)

        data_img_buffer = self.__connection.recv(message_length)  # blocking
        data_img = np.frombuffer(data_img_buffer, dtype=dtype)
        decimg = cv2.imdecode(data_img, color_format)
        return decimg

    # Outgoing

    def send_bool(self, data: bool):
        """[summary] Send a boolean value
        Note: Sending String values is discouraged.
        Args:
            data (bool): [Any one of (True, False, 0, 1)]
        """
        if data in [1,0]:
            data = bool(data)
        print("sending bool ...", data)
        self.send_string(str(data).lower())

    def send_string(self, data: str):
        """[summary] Send a string value

        Args:
            data (str): [String to be sent]
        """
        self.send_int(len(data))
        self.__connection.sendall(bytes(data, 'utf-8'))

    def send_int(self, data: int):
        """[summary] Send an int value

        Args:
            data (int): [Integer to be sent]
        """
        if self.__debug:
            print("Sending int ... : ", bytes(format(data, '016d'), 'utf-8'))
        self.__connection.sendall(bytes(format(data, '016d'), 'utf-8'))

    def send_float(self, data: float):
        """[summary] Send an float value

        Args:
            data (float): [Float to be sent]
        """
        if self.__debug:
            print("Sending float ... : ", bytes(format(data, '016f'), 'utf-8'))
        self.__connection.sendall(bytes(format(data, '016f'), 'utf-8'))

    def send_int_list(self, data: list):
        """[summary] Send a list of values

        Args:
            data (list): [List of values(integers) to be sent]
        """
        if self.__debug:
            print("Sending list...", data)
        self.send_int(len(str(data)))  # send size of list
        # usually (bytes size -3 ) for character "b" and 2 "'"
        self.__connection.sendall(bytes(str(data), 'utf-8'))

    def send_float_list(self, data: list):
        """[summary] Send a list of values

        Args:
            data (list): [List of values(floats) to be sent]
        """
        if self.__debug:
            print("Sending list...", data)
        self.send_int(len(str(data)))  # send size of list
        # usually (bytes size -3 ) for character "b" and 2 "'"
        self.__connection.sendall(bytes(str(data), 'utf-8'))

    def send_image(self, img):
        """[summary] Send an image

        Args:
            img ([cv2.Mat]): [OpenCV Image]
        """
        en = cv2.imencode('.jpg', img)[1].tobytes()
        self.send_int(len(en))
        self.__connection.sendall(en)


# TODO: Add a security check with goes through a list to understand
# which connections should be allowed
