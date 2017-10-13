CC = gcc-5

CFLAGS = -g -Wall

TARGET = a2rchat

all: $(TARGET)

$(TARGET): $(TARGET).c client.c server.c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c client.c server.c

clean:
	$(RM) -rf $(TARGET) client.o server.o $(TARGET).dSYM submit.tar

tar:
	tar -zcvf submit.tar $(TARGET).c a2rchat.h client.c client.h server.c server.h Makefile project_report.pdf

