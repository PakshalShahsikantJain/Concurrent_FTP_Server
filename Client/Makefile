CC = gcc
CFLAGS = -Wall -Wextra -O2

OBJ = Client.o helper.o 
TARGET = app

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

Client.o: Client.c helper.h
	$(CC) $(CFLAGS) -c $< -o $@

helper.o: helper.c helper.h
	$(CC) $(CFLAGS)	-c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: clean

