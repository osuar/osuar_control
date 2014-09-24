CC=clang
CFLAGS=-Wall -Wextra -lpthread

all: joystick comm

joystick:
	$(CC) main.c -o joystick $(CFLAGS)

comm:
	$(CC) -o comm comm.c ../firmware/osuar_protocol.c ../firmware/osuar_ringbuffer.c -I../firmware

clean:
	rm joystick comm

