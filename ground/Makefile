CC=clang
CFLAGS=-Wall -Wextra -lpthread

all: joystick comm

joystick:
	$(CC) main.c -o joystick $(CFLAGS)

comm:
	$(CC) -o comm comm.c serial.c ../firmware/osuar_protocol.c ../firmware/osuar_ringbuffer.c -I. -I../firmware

clean:
	rm joystick comm

