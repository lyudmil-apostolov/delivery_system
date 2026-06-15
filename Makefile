# Makefile for delivery_system
# Usage:
#   make          → compile the project
#   make run      → compile and run
#   make clean    → remove build artefacts

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g
TARGET  = delivery_system
SRCS    = main.c product.c order.c customer.c admin.c utils.c
OBJS    = $(SRCS:.c=.o)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: all
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
