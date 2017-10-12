CC = gcc

CFLAGS = -g -Wall

TARGET = writer

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) -rf $(TARGET) $(TARGET).dSYM submit.tar
