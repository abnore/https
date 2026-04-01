CC = clang

CFLAGS  = -Wall -Wextra
LDFLAGS = -lssl -lcrypto \
          -L/opt/homebrew/opt/openssl@3/lib
INC     = -I/opt/homebrew/opt/openssl@3/include -I.

OBJ = client.c https.c
BIN = client

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(INC) client.c https.c -o $(BIN) $(LDFLAGS)
