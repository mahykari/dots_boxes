# Dots & Boxes
The classic game, with sockets.

# Features

The game server is responsible for initiating matches
and communicating with clients before they enter _player_ mode.

After a match is announced from the server, players will communicate only with each other via UDP connections.
All game logic is also handled by players.

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

To stop the client, press Ctrl+D (before _player_ mode begins).

You can choose the number of players in your game with the following command:

```
set_match_size <a number between 2 and 4>
```

Your client will automatically change to _player_ mode after a match is announced by the server.
You will also receive an identifier (from here on referred to as `id`),
which you will need further in the game.

In each round of the game, you have 30 seconds to play;
your turn will be skipped if this time expires before your move.

**Caution:** Move checks are not supported,
so be a little bit cautious about the move you enter.

For drawing a wall in the game map, enter the following command: 
```
wall id <player id> p1 <point 1 row> <point 1 column> p2 <point 2 row> <point 2 column>
```

Be careful to not draw a line which is already drawn!

## Notes

This project was assigned as part of the Operating Systems course at University of Tehran, Fall 2020.
