# Dots & Boxes
The classic game, with sockets.

## How to Run

Make sure you are using Linux!

In addition, make sure you have `make` installed on your system.

You can build this project as follows:

```
git clone https://github.com/moyarka/dots_boxes # Or the SSH alternative
cd dots_boxes/
make
```
## Instructions

### Server

The game server will listen for TCP connections on a port you specify.
Run the following command to start the server:

```
./server <port number>
```

To stop the server, press Ctrl+D.

### Client

The client connects to your server on the same port the server is listening on.
Run the following command to start the client:

```
./client <port number>
```

CLIENT: The CTRL+D format also works here, too (before running the player.)
To run the client, do the following:
    1. Type  in your terminal; 
     This must be the same port number you used for running your server.
    2. Type "set_match_size <a number between 2 and 4>" to be added to a match.
     Your client will automatically change to player after a match is announced 
     by the server.

PLAYER: Each player has 30 seconds to make a move; We have excluded move checks as well,
so be a little bit cautious about the move you enter.
After a game is announced, your player will have an id which is also written in
the server announcement. For making a move, write: 
    wall id <player id> p1 <point1 row> <point1 column> <point2 row> <point2 column>
Be careful to not draw a line which is already drawn;
as this condition is not handled in our code. Also after each timer reset,
The standard input buffer is not flushed; so your current move can be a consequence of
your previous moves :).
