CC = gcc

CFLAGS = -g -Wall

TARGET = a1shell

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) -rf $(TARGET) $(TARGET).dSYM
