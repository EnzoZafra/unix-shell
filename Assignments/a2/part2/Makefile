CC = gcc

CFLAGS = -g -Wall

TARGET = a2chat

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) -rf $(TARGET) $(TARGET).dSYM submit.tar
