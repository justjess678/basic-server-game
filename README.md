# basic-server-game
Basic launcher-client-server game example.

Players are randomly placed on a map and must move around the map until one of them finds the treasure and wins.

Controls:
- "8" = up
- "4" = left
- "6" = right
- "2" = down
- "Q" = quit

To run:
- Launch server
- In another terminal, launch the launcher
- Sign in using an account listed in `users.json`
- Play the game
  - The map will display your username in the square you are positioned in.
  - Squares containing no players are marked as "None"
  - The treasure square is randomly placed at each server launch
  - The map will refresh after each move by re-printing itself in the terminal
  - Each move costs a point, finding the treasure earns you 10 points
