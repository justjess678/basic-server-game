import json
import random
from thread import *
import time
import socket


class Server:

    def __init__(self):
        self.current_users = []
        # map dimensions
        self.height, self.width = 5, 5

        # map showing the position of the players
        self.map = [[None for i in range(self.width)] for j in range(self.height)]
        self.treasure_location = [random.randint(0, self.width), random.randint(0, self.height)]
        # list of players scores
        self.scores = {}
        # list of players positions
        self.positions = {}
        with open('users.json') as json_file:
            data = json.load(json_file)
            self.usernames = [u["username"] for u in data['users']]
        # set the hostname
        host = "127.0.0.1"
        # initiate port no above 1024
        port = 6000

        server_socket = socket.socket()  # get instance
        server_socket.bind((host, port))  # bind host address and port together

        # configure how many client the server can listen simultaneously
        server_socket.listen(2)
        self.conn, address = server_socket.accept()  # accept new connection
        print("Connection from: " + str(address))

    """
    Returns a visual representation of the 5x5 map
    """
    def show_map(self):
        out = ''
        for i in range (0, self.width):
            line = '\n---------------------------------------------\n'
            line += '\t'
            for j in range(0, self.height):
                line += str(self.map[i][j]) + '\t|'
            out += line
        out += '\n|||||||||||||||||||||||||||||||||||||||||||||\n'
        return out

    def update(self):

        # Move the player position on the map
        def update_map(user, prev_pos, new_pos):
            self.map[prev_pos[0]][prev_pos[1]] = None
            self.map[new_pos[0]][new_pos[1]] = user

        # receive data stream. it won't accept data packet greater than 1024 bytes
        data = self.conn.recv(1024).decode()
        if not data:
            # if data is not received break
            return False
        print("From connected user: " + str(data) + "\n" if "_" not in str(data) else "From connected user: " + str(data.split('_')[1]) + "\n")
        if data in self.usernames:
            self.scores[data] = 0
            # place player on map
            i, j = [0, 0]
            while self.map[i][j] is not None:
                i, j = random.randint(0, self.width), random.randint(0, self.height)
            self.map[i][j] = data
            self.positions[data] = [i, j]
            return_data = "MAP_" + self.show_map()

        if data[0] == "Q":
            print(data.split('_')[1] + ' quit the game!')
            # player quit the game
            self.map[self.positions[data.split('_')[1]][0]][self.positions[data.split('_')[1]][1]] = None
            return_data = str(data.split('_')[1]) + " finished with a score of : " + str(self.scores.get(data.split('_')[1], 0))

        elif data[0] == "8" or data[0] == "4" or data[0] == "6" or data[0] == "2":
            username = data[2:]
            # directional keys
            old_pos = self.positions.get(username, [0, 0])
            if data[0] == "6":
                # UP
                self.positions[username] = [old_pos[0], old_pos[1]+1]
                self.scores[username] -= 1

            elif data[0] == "4":
                # DOWN
                self.positions[username] = [old_pos[0], old_pos[1]-1]
                self.scores[username] -= 1

            elif data[0] == "8":
                # LEFT
                self.positions[username] = [old_pos[0]-1, old_pos[1]]
                self.scores[username] -= 1

            elif data[0] == "2":
                # RIGHT
                self.positions[username] = [old_pos[0]+1, old_pos[1]]
                self.scores[username] -= 1

            if old_pos != self.positions[username]:
                update_map(username, old_pos, self.positions[username])

            return_data = "MAP_" + self.show_map() + "_" + str(self.scores.get(username, "SCORE_ERROR"))

            # check if found treasure
            if self.positions[username][0] == self.treasure_location[0] and self.positions[username][1] == \
                    self.treasure_location[1]:
                # found the treasure!
                self.scores[username] += 10
                return_data = "TREASURE_" + str(self.scores.get(username, "SCORE_ERROR")) + "_" + return_data

        else:
            return_data = "OK" + self.show_map()

        self.conn.send(return_data.encode())  # send data to the client

        return True

    """
    Closes the connections
    """
    def close(self):
        self.conn.close()  # close the connection


if __name__ == "__main__":
    server = Server()
    while True:
        if server.update():
            print(server.show_map())