CC = gcc
CFLAGS = -Wall -Wextra -O2

OBJ = Server.o helper.o 
TARGET = app

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

Server.o: Server.c header.h
	$(CC) $(CFLAGS) -c $< -o $@

helper.o: helper.c header.h
	$(CC) $(CFLAGS)	-c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: clean

