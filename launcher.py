import json
import client

if __name__ == "__main__":
    username_in, password_in = [None, None]

    # access user file
    with open('users.json') as json_file:
        data = json.load(json_file)
        user = []

        while len(user) < 1:
            # get user input
            username_in = input("Please enter your username:\n")
            password_in = input("Please enter your password:\n")

            # check user input against DB
            user = [u for u in data['users'] if username_in == u["username"] and password_in == u["password"]]
            if len(user) > 0:
                print(user[0]['username'] + ' is launching a client!\n')
            else:
                print("Invalid username or password!\n")

        # launch game
        print("Launching client application...")
        try:
            client.main(user[0]['username'])
        except Exception as e:
            print(e)
