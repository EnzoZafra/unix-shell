CC = gcc

CFLAGS = -g -Wall

TARGET = a4vmsim

all: $(TARGET)

$(TARGET): $(TARGET).c stack.c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c stack.c -lm

clean:
	$(RM) -rf $(TARGET) stack.o $(TARGET).dSYM submit.tar

tar:
	tar -zcvf submit.tar $(TARGET).c a4vmsim.h stack.c stack.h Makefile report.pdf

