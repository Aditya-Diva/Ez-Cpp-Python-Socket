import socket
import cv2
import numpy as np
import time


class EzPySocket:
    """[summary] Python - Cpp Communication Server Object
    """

    __sleep_between_packets = 0.00005
    __packet_size = 59625

    __loop_flag = False
    __loop_iteration_count = 0
    __loop_start_time = 0

    def __init__(self, server_address: str = "127.0.0.1",
                 server_port: int = 10000,
                 socket_family=socket.AF_INET,
                 socket_type=socket.SOCK_STREAM,
                 debug: bool = False,
                 auto_connect: bool = True,
                 client_connection_count: int = 1,
                 server_mode: bool = True,
                 reconnect_on_address_busy: float = 0.0,
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
            after time(in seconds) specified here]. Defaults to 0.0 (Don't reconnect, simply exit).
            tokens ([type], str): [Define a start and end token when communicating, helps debug
            and ensure that the right message is passed through.]. Defaults to ["", ""].
        """
        self.__debug = debug
        self.__socket_family = socket_family
        self.__socket_type = socket_type
        self.__reconnect_on_address_busy = reconnect_on_address_busy
        self.__tokens = tokens
        self.__auto_connect = auto_connect

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
                    # To disable lingering consequences immediately after closing connection
                    self.__sock.setsockopt(
                        socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                    self.__sock.bind(self.__server_address)
                    address_free_flag = True
                except OSError as oserr:
                    print(oserr)
                    if (self.__reconnect_on_address_busy != 0.0):
                        print("\nServer Trying to Bind to Address Failed...")
                        self.__polling_timeout()
                    else:
                        print("server setting:",
                              self.__reconnect_on_address_busy)
                        print("Please make sure address is free, else use reconnect_on_address_busy argument ",
                              "to keep polling in periodic intervals. If you have just run a server previously ",
                              "there's a good chance the previous server will be down in a couple of seconds.",
                              "Use polling functionality to avoid waiting for address to be free again.")
                        exit()
                except Exception as e:
                    print(e)

            # number of client connection(s) to accept
            self.__connection_count = client_connection_count

            # listen and connect auto
            self.server_listen()
        else:
            while not address_free_flag:
                try:
                    self.__sock.connect(self.__server_address)
                    address_free_flag = True
                    self.__connection = self.__sock
                    print("Socket connection successful")
                except Exception as e:
                    print("Socket connection failed : ", e)
                    if (self.__reconnect_on_address_busy != 0.0):
                        print("\nClient Trying to Connect to Server Failed...")
                        self.__polling_timeout()
                    else:
                        print("client setting: '",
                              self.__reconnect_on_address_busy, "'")
                        print("Please make sure address is free, else use reconnect_on_address_busy argument ",
                              "to keep polling in periodic intervals")
                        exit()

    def server_listen(self):
        # Listen for incoming connection(s) from clients
        self.__sock.listen(self.__connection_count)

        if self.__auto_connect:
            self.connect()

    def create_socket(self):
        """[summary]
        Create a socket
        """
        try:
            self.__sock = socket.socket(self.__socket_family,
                                        self.__socket_type)
            print("Socket created successfully")
        except socket.error as err:
            print("Socket creation failed : ", err)

        # TODO: Keep alive flag for client?
        # # Check and turn on TCP Keepalive
        # ret = self.__sock.getsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE)
        # if(ret == 0):
        #     print('Socket Keepalive off, turning on')
        #     x = self.__sock.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
        #     print('setsockopt=', x)
        # else:
        #     print('Socket Keepalive already on')

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
            self.__sock.shutdown(socket.SHUT_RDWR)
            self.__sock.close()
        except:
            print("Connection already closed successfully")

    def set_sleep_between_packets(self, seconds: float):
        """[summary] A setter function to add a delay between packet read/write
            which ensures that it does so properly. It's been observed that 
            increasing this when images don't come through properly helps.

        Args:
            seconds (float): [Time to sleep between packet read/write]
        """
        self.__sleep_between_packets = seconds

    def get_sleep_between_packets(self):
        """[summary] A getter function to get delay between packet read/write.
        """
        return self.__sleep_between_packets

    def set_packet_size(self, number_of_bytes: int):
        """[summary] A setter function to set size of packets during read/write
            Note: The value passed should not be more than 65535 (64K)

        Args:
            number_of_bytes (int): [Size of packet in bytes]
        """
        if number_of_bytes > 0:
            self.__packet_size = number_of_bytes
        else:
            print("\nInvalid packet size was provided. Not updating packet size.\n")

    def loop_func_decorator(self, func):
        def new_func(self, data, show_ips):
            self.__loop_iteration_count += 1
            func(self, data)
            if self.__debug or show_ips:
                print("IPS : ", self.__loop_iteration_count /
                      (time.time() - self.__loop_start_time))
        return new_func

    def server_loop(self, func, data: dict = {}, loop_count: int = 0, show_ips: bool = False):
        """[summary]

        Args:
            func ([function]): [The function that has to be executed in the loop]
            data (dict, optional): [The dictionary that needs to be passed to the function]. Defaults to {}.
            loop_count (int, optional): [Looping behaviour
            if -1, loops until stop_loop is called on server side,
            if 0, loops until stop_loop is called from client side (Status string is sent)
            else, loops for as many iterations as specified]. Defaults to 0.
            show_ips (bool, optional): [Show iterations per second]. Defaults to False.
        """
        self.__loop_flag = True
        self.__loop_start_time = time.time()
        decorated_func = self.loop_func_decorator(func)
        if loop_count == -1:
            while self.__loop_flag:
                decorated_func(self, data, show_ips)

        if loop_count == 0:
            # Server is up until Client has gotten its request
            status = "Active"
            while status != "Stop":
                decorated_func(self, data, show_ips)
                status = self.receive_string()
        else:
            # Server serves for certain iterations
            for _ in range(loop_count):
                decorated_func(self, data, show_ips)
        self.reset_loop()

    def client_loop(self, func, data: dict = {}, loop_count: int = 0, show_ips: bool = True):
        """[summary]

        Args:
            func ([function]): [The function that has to be executed in the loop]
            data (dict, optional): [The dictionary that needs to be passed to the function]. Defaults to {}.
            loop_count (int, optional): [Looping behaviour
            if 0, loops until stop_loop is called from client side
            else, loops for as many iterations as specified]. Defaults to 0.
            show_ips (bool, optional): [Show iterations per second]. Defaults to True.
        """
        self.__loop_flag = True
        self.__loop_start_time = time.time()
        decorated_func = self.loop_func_decorator(func)
        if loop_count == 0:
            # Client is up until Server has gotten its request
            status = "Active"
            while status != "Stop":
                decorated_func(self, data, show_ips)
                # Set & Send status
                status = "Active" if self.__loop_flag else "Stop"
                self.send_string(status)
        else:
            for _ in range(loop_count):
                decorated_func(self, data, show_ips)
        self.reset_loop()

    def stop_loop(self):
        """[summary] Set flag to stop looping
        """
        self.__loop_flag = False

    def reset_loop(self):
        """[summary] Resets all the loop related flags
        """
        self.__loop_flag = False
        self.__loop_iteration_count = 0
        self.__loop_start_time = 0

    def __del__(self):
        self.disconnect()

    def __polling_timeout(self):
        """[summary] Where polling is required in periodic intervals, this is 
            the function that implements the timeout for the same
        """
        if self.__reconnect_on_address_busy < 0.0:
            print("Reconnection time sent invalid, using default time of 10 seconds...")
            time.sleep(10)
        else:
            print("Will attempt to reconnect in",
                  self.__reconnect_on_address_busy, "seconds")
            time.sleep(self.__reconnect_on_address_busy)

    def __insert_tokens(self, message):
        """[summary] Insert tokens to the message that is being passed. This includes
            both the start token at the beginning of the message and the end token
            at the end of the message.

        Args:
            message ([str/bytes]): [Message to be sent]

        Returns:
            [str/bytes]: [Token included message]
        """
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
        """[summary] Extracting tokens from the received messages to get the actual message.
            This also serves as a check on the validity of the message. Currently throws
            an error if invalid message is received.

        Args:
            message ([str/bytes]): [Token included received message]

        Raises:
            Exception: [Starting token check in received message failed]
            Exception: [Ending token check in received message failed]

        Returns:
            [str/bytes]: [Message with tokens extracted]
        """
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
                raise Exception(
                    "Starting token check in received message failed")
            else:
                message = message[index+len(start_token):]

            # Remove end token
            end_token_in_message = False
            while not end_token_in_message:
                index = message.rfind(end_token)
                if index != (len(message) - len(end_token)):
                    print(
                        "Ending token was not found at the end of message received!",
                        " Please check if the right kind of data is being sent/received or that",
                        " the same tokens are set on server and client ends...",
                        "\nAdditionally, try increasing set_sleep_between_packets value.",
                        "Current sleep_between_packets value: " + str(self.get_sleep_between_packets()))
                    raise Exception(
                        "Ending token check in received message failed")
                else:
                    message = message[:-len(end_token)]
                    end_token_in_message = True

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

        data_img_buffer = bytes()
        packet_start_index = 0
        packet_size_curr = self.__packet_size
        while packet_start_index < message_length:
            if ((packet_start_index + self.__packet_size) > message_length):
                packet_size_curr = message_length - packet_start_index

            # blocking
            data_img_buffer += self.__connection.recv(packet_size_curr)
            if self.__debug:
                print("Receiving packet no. ",
                      packet_start_index / self.__packet_size)
                print("Current packet size : ", packet_size_curr)
                print("Current size of data accumulated : ", len(data_img_buffer))
            packet_start_index += packet_size_curr
            time.sleep(self.__sleep_between_packets)

        data_img_buffer = self.__extract_tokens(data_img_buffer)
        data_img = np.frombuffer(data_img_buffer, dtype=dtype)
        decimg = cv2.imdecode(data_img, color_format)
        return decimg

    # Outgoing

    def __send_byte_data(self, datatype: str, data):
        """[summary] Common send message as bytes functionality

        Args:
            datatype (str): [Type of data that's being sent]
            data ([type]): [Data to be sent]
        """
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

        packet_start_index = 0
        packet_size_curr = self.__packet_size
        while packet_start_index < len(data):
            if ((packet_start_index + self.__packet_size) > len(data)):
                packet_size_curr = len(data) - packet_start_index
            if self.__debug:
                print("Sending packet no. ",
                      packet_start_index / self.__packet_size)
                print("Sending packet of size : ", len(
                    data[packet_start_index:packet_start_index+packet_size_curr]))
            self.__connection.sendall(
                data[packet_start_index:packet_start_index+packet_size_curr])
            packet_start_index += packet_size_curr
            time.sleep(self.__sleep_between_packets)
