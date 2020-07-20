import socket
import sys
import time

COMMANDS = ["Q", "8", "4", "6", "2"]


class Client:

    def __init__(self, name):
        self.name = name

        host = "127.0.0.1"  # as both code is running on same pc
        port = 6000  # socket server port number

        self.client_socket = socket.socket()  # instantiate
        self.client_socket.connect((host, port))  # connect to the server

        # send ID information
        self.client_socket.send(name.encode())  # send message
        data = self.client_socket.recv(1024).decode()  # receive response
        if data is None:
            print("Error connecting client!")
        else:
            self.send_command('')
            print("Hello, " + name + "!")

    def send_command(self, command):
        command += "_" + self.name
        self.client_socket.send(command.encode())  # send message
        data = self.client_socket.recv(1024).decode()  # receive response
        if data is None:
            print("Error receiving command!")
        if data.split('_')[0] == "TREASURE":
            # User found the treasure
            print("You found the treasure!\nYou have a score of : " + data.split('_')[1] + "\nPress Q to quit.")
        if "MAP" in data:
            print(str(data.split('_')[1]) + '\n' + "Score:" + str(data.split('_')[2]))

    def close(self):
        self.client_socket.close()  # close the connection


def main(user="None"):
    _in = ""
    client = Client(user)

    while _in.upper() != "Q":
        _in = input("Move?")
        if _in in COMMANDS:
            client.send_command(_in.upper())
    print("Bye bye!")
    client.close()


if __name__ == "__main__":
    main()
