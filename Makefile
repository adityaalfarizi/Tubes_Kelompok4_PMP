CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = jadwal
SRCS = main.c dokter.c jadwal.c fileio.c sleep.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean