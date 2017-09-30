CC = gcc

CFLAGS = -g -Wall

TARGET = a1shell

all: $(TARGET)

$(TARGET): $(TARGET).c a1commands.c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c a1commands.c

clean:
	$(RM) -rf $(TARGET) a1commands.o $(TARGET).dSYM
