import socket
import cv2
import numpy as np
import time


class EzPySocket:
    """[summary] Python - Cpp Communication Server Object
    """

    def __init__(self, server_address: str = "127.0.0.1",
                 server_port: int = 10000,
                 socket_family=socket.AF_INET,
                 socket_type=socket.SOCK_STREAM,
                 debug: bool = False,
                 auto_connect: bool = True,
                 client_connection_count: int = 1,
                 server_mode: bool = True,
                 reconnect_on_address_busy: float = -1.0,
                 tokens: [str, str] = ["", ""]):
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
            client_connection_count (int, optional): [Number of connections to support]. Defaults to 1.
            server_mode (bool, optional): [Run as server (True) or client (False)]. Defaults to True.
            reconnect_on_address_busy (float, optional): [Attempt to reconnect
            after time(in seconds) specified here]. Defaults to -1 (Don't reconnect, simply exit).
            tokens ([type], str): [Define a start and end token when communicating, helps debug
            and ensure that the right message is passed through.]. Defaults to ["", ""].
        """
        self.__debug = debug

        self.__socket_family = socket_family

        self.__socket_type = socket_type

        self.__reconnect_on_address_busy = reconnect_on_address_busy

        self.__tokens = tokens

        self.create_socket()

        # Bind sever socket to specific address and port
        self.__server_address = (server_address, server_port)

        if self.__debug:
            print("Starting " + ("server" if server_mode else "client"))
            print('Starting up on {} port {}'.format(*self.__server_address))

        address_free_flag = False
        if server_mode:
            while not address_free_flag:
                try:
                    self.__sock.bind(self.__server_address)
                    address_free_flag = True
                except OSError as oserr:
                    print(oserr)
                    if (self.__reconnect_on_address_busy != -1):
                        print("Trying to bind to address...")
                        self.polling_timeout()
                    else:
                        print("server setting:",
                              self.__reconnect_on_address_busy)
                        print("Please make sure address is free, else use reconnect_on_address_busy argument ",
                              "to keep polling as per required passed ")
                        exit()
                except Exception as e:
                    print(e)

            # number of client connection(s) to accept
            self.__connection_count = client_connection_count

            # Listen for incoming connection(s) from clients
            self.__sock.listen(self.__connection_count)

            if auto_connect:
                self.connect()
        else:
            while not address_free_flag:
                try:
                    self.__sock.connect(self.__server_address)
                    address_free_flag = True
                    self.__connection = self.__sock
                    print("Socket connection successful")
                except Exception as e:
                    print("Socket connection failed : ", e)
                    if (self.__reconnect_on_address_busy != -1):
                        print("Trying to bind to address...")
                        self.polling_timeout()
                    else:
                        print("client setting: '",
                              self.__reconnect_on_address_busy, "'")
                        print("Please make sure address is free, else use reconnect_on_address_busy argument ",
                              "to keep polling as per required passed ")
                        exit()

    def polling_timeout(self):
        """[summary] Implement timeout between networking calls in case an exception is
            raised at the moment
        """
        if self.__reconnect_on_address_busy < 0:
            print("Reconnection time sent invalid, using default time of 10 seconds...")
            time.sleep(10)
        else:
            print("Will attempt to reconnect in",
                  self.__reconnect_on_address_busy, "seconds")
            time.sleep(self.__reconnect_on_address_busy)

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

    def __insert_tokens(self, message):
        if self.__tokens != ["", ""]:
            if type(message) is bytes:
                try:  # works for the usual bytes that is being sent
                    message = self.__tokens[0] + message + self.__tokens[1]
                except:  # for sending images where its utf16
                    message = bytes(self.__tokens[0], encoding='utf8') + \
                        message + bytes(self.__tokens[1], encoding='utf8')
            else:
                message = self.__tokens[0] + str(message) + self.__tokens[1]
        return message

    def __extract_tokens(self, message):
        if self.__tokens != ["", ""]:
            if type(message) is bytes:
                start_token = bytes(self.__tokens[0], encoding='utf8')
                end_token = bytes(self.__tokens[1], encoding='utf8')
            else:
                start_token = self.__tokens[0]
                end_token = self.__tokens[1]

            # Remove start token
            index = message.find(start_token)
            if index != 0:
                print(
                    "Starting token was not found at the beginning of message received!",
                    " Please check if the right kind of data is being sent/received or that",
                    " the same tokens are set on server and client ends...")
                exit(0)
            else:
                message = message[index+len(start_token):]

            # Remove end token
            index = message.rfind(end_token)
            if index != (len(message) - len(end_token)):
                print(
                    "Ending token was not found at the end of message received!",
                    " Please check if the right kind of data is being sent/received or that",
                    " the same tokens are set on server and client ends...")
                exit(0)
            else:
                message = message[:-len(end_token)]
        return message

    # Incoming

    def receive_bool(self) -> bool:
        """[summary] Receive a bytes array

        Returns:
            [bool]: [Boolean that was received.]
        """
        received = self.receive_string()
        if self.__debug:
            print("Bool String Received:", received)
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
        received = self.__connection.recv(string_length)  # blocking
        if self.__debug:
            print("receive_string: string_length received : ",
                  string_length)
            print('Received {!r} as message'.format(received))

        # remove the "b'" and the "'" at the end
        received = str(received)[2:-1]
        received = self.__extract_tokens(received)
        return received

    def receive_int(self, message_length: int = 16) -> int:
        """[summary] Receive an int value

        Args:
            message_length (int, optional): [Size of message to
            be read in bytes]. Defaults to 16.

        Returns:
            [int]: [The integer value that was received]
        """
        received = self.__connection.recv(
            message_length + len(self.__tokens[0]) + len(self.__tokens[1]))  # blocking
        if self.__debug:
            print("Receiving Buffer data of size (in bytes): ",
                  message_length + len(self.__tokens[0]) + len(self.__tokens[1]))
            print('Received {!r} as message'.format(received))

        received = int(self.__extract_tokens(received.decode("utf-8")))
        return received

    def receive_float(self, message_length: int = 16) -> float:
        """[summary] Receive an float value

        Args:
            message_length (int, optional): [Size of message to
            be read in bytes]. Defaults to 16.

        Returns:
            [float]: [The float value that was received]
        """
        received = self.__connection.recv(
            message_length + len(self.__tokens[0]) + len(self.__tokens[1]))  # blocking
        if self.__debug:
            print("Receiving Buffer data of size (in bytes): ",
                  message_length + len(self.__tokens[0]) + len(self.__tokens[1]))
            print('Received {!r} as message'.format(received))

        received = float(self.__extract_tokens(received.decode("utf-8")))
        return received

    def receive_int_list(self):
        """[summary] Receive a list of int

        Returns:
            [list]: [List of ints]
        """
        received = self.receive_string()
        return eval(received)

    def receive_float_list(self):
        """[summary] Receive a list of floats

        Returns:
            [list]: [List of floats]
        """
        received = self.receive_string()
        return eval(received)

    def receive_image(self,
                      color_format: int = cv2.IMREAD_COLOR,
                      dtype: str = 'uint8'):
        """[summary] Receive an image

        Args:
            color_format (int, optional): [The color format to be used].
            Defaults to cv2.IMREAD_COLOR.

            dtype (str, optional): [Datatype to be used when reading buffer].
            Defaults to 'uint8'.

        Returns:
            [cv2.Mat]: [cv2 image that was received]
        """
        message_length = self.receive_int()
        if self.__debug:
            print("receive_image: message_length received : ",
                  message_length)

        data_img_buffer = self.__connection.recv(message_length)  # blocking
        data_img_buffer = self.__extract_tokens(data_img_buffer)
        data_img = np.frombuffer(data_img_buffer, dtype=dtype)
        decimg = cv2.imdecode(data_img, color_format)
        return decimg

    # Outgoing

    def __send_byte_data(self, datatype: str, data):
        data = bytes(data, 'utf-8')
        if self.__debug:
            print("Sending " + datatype + " ...", data)
        self.__connection.sendall(data)

    def send_bool(self, data: bool):
        """[summary] Send a boolean value
        Note: Sending String values is discouraged.
        Args:
            data (bool): [Any one of (True, False, 0, 1)]
        """
        if data in [1, 0]:
            data = bool(data)
        data = str(data).lower()
        self.send_string(data)

    def send_string(self, data: str):
        """[summary] Send a string value

        Args:
            data (str): [String to be sent]
        """
        data = self.__insert_tokens(data)
        self.send_int(len(data))
        self.__send_byte_data("String", data)

    def send_int(self, data: int):
        """[summary] Send an int value

        Args:
            data (int): [Integer to be sent]
        """
        self.__send_byte_data(
            "Int", self.__insert_tokens(format(data, '016d')))

    def send_float(self, data: float):
        """[summary] Send an float value

        Args:
            data (float): [Float to be sent]
        """
        self.__send_byte_data(
            "Float", self.__insert_tokens(format(data, '016f')))

    def send_int_list(self, data: list):
        """[summary] Send a list of values

        Args:
            data (list): [List of values(integers) to be sent]
        """
        data = self.__insert_tokens(str(data))
        self.send_int(len(data))  # send size of list
        self.__send_byte_data("Int List", data)

    def send_float_list(self, data: list):
        """[summary] Send a list of values

        Args:
            data (list): [List of values(floats) to be sent]
        """
        data = self.__insert_tokens(str(data))
        self.send_int(len(data))  # send size of list
        self.__send_byte_data("Float List", data)

    def send_image(self, img):
        """[summary] Send an image

        Args:
            img ([cv2.Mat]): [OpenCV Image]
        """
        data = self.__insert_tokens(cv2.imencode('.jpg', img)[1].tobytes())
        self.send_int(len(data))
        self.__connection.sendall(data)


# TODO: Add a security check with goes through a list to understand
# which connections should be allowed
