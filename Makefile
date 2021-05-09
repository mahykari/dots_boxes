CC = c99
CC_OPTS = -Wall --pedantic

OBJS = string_buf.o util.o

PLYR_OBJS = player.o game.o

SRVR_OBJS = server.o server_handlers.o server_announces.o server_main.o\
	matchmaker.o $(OBJS)

CLNT_OBJS = client.o client_main.o $(PLYR_OBJS) $(OBJS)

SRVR_DIR = _server
CLNT_DIR = _client

UTIL_DIR = util
STR_BUF_DIR = string_buf
MTCH_DIR = matchmaker
GAME_DIR = game
PLYR_DIR = player

BUILD_DIR = build

# game_test.out udp_test.out: udp_test.o game_test.o player.o game.o $(OBJS)
# 	$(CC) $(CC_OPTS) udp_test.o player.o game.o build/util.o build/string_buf.o -o udp_test.out
# 	$(CC) $(CC_OPTS) game_test.o game.o build/util.o -o game_test.out

# udp_test.o: $(PLYR_DIR)/udp_test.c
	# $(CC) $(CC_OPTS) -c $(PLYR_DIR)/udp_test.c

# game_test.o: $(GAME_DIR)/game_test.c
	# $(CC) $(CC_OPTS) -c $(GAME_DIR)/game_test.c

server.out client.out: $(CLNT_OBJS) $(SRVR_OBJS)
	$(CC) $(CC_OPTS) $(patsubst %.o, $(BUILD_DIR)/%.o, $(SRVR_OBJS)) -o server
	$(CC) $(CC_OPTS) $(patsubst %.o, $(BUILD_DIR)/%.o, $(CLNT_OBJS)) -o client

player.o: $(PLYR_DIR)/player.c
	$(CC) $(CC_OPTS) -c $(PLYR_DIR)/player.c -o $(BUILD_DIR)/player.o

game.o: $(GAME_DIR)/game.c
	$(CC) $(CC_OPTS) -c $(GAME_DIR)/game.c -o $(BUILD_DIR)/game.o

string_buf.o: $(STR_BUF_DIR)/string_buf.c
	$(CC) $(CC_OPTS) -c $(STR_BUF_DIR)/string_buf.c -o $(BUILD_DIR)/string_buf.o

util.o: $(UTIL_DIR)/util.c
	$(CC) $(CC_OPTS) -c $(UTIL_DIR)/util.c -o $(BUILD_DIR)/util.o

matchmaker.o: $(MTCH_DIR)/matchmaker.c
	$(CC) $(CC_OPTS) -c $(MTCH_DIR)/matchmaker.c -o $(BUILD_DIR)/matchmaker.o

server_main.o: $(SRVR_DIR)/server_main.c
	$(CC) $(CC_OPTS) -c $(SRVR_DIR)/server_main.c -o $(BUILD_DIR)/server_main.o

server.o: $(SRVR_DIR)/server.c
	$(CC) $(CC_OPTS) -c $(SRVR_DIR)/server.c -o $(BUILD_DIR)/server.o

server_handlers.o: $(SRVR_DIR)/server_handlers.c
	$(CC) $(CC_OPTS) -c $(SRVR_DIR)/server_handlers.c -o $(BUILD_DIR)/server_handlers.o

server_announces.o: $(SRVR_DIR)/server_announces.c
	$(CC) $(CC_OPTS) -c $(SRVR_DIR)/server_announces.c -o $(BUILD_DIR)/server_announces.o

client.o: $(CLNT_DIR)/client.c
	$(CC) $(CC_OPTS) -c $(CLNT_DIR)/client.c -o $(BUILD_DIR)/client.o

client_main.o: $(CLNT_DIR)/client_main.c
	$(CC) $(CC_OPTS) -c $(CLNT_DIR)/client_main.c -o $(BUILD_DIR)/client_main.o

clean:
	rm -f $(BUILD_DIR)/*.o ./*.o *.out server client

line_count:
	wc -l $(SRVR_DIR)/* $(CLNT_DIR)/* $(UTIL_DIR)/* \
	$(STR_BUF_DIR)/* $(MTCH_DIR)/* $(PLYR_DIR)/* $(GAME_DIR)/*
