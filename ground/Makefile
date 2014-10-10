CC=clang
CFLAGS=-Wall -Wextra -lpthread

all: comm
	

comm:
	$(CC) -o comm comm.c serial.c joystick.c ../firmware/osuar_protocol.c ../firmware/osuar_ringbuffer.c -I. -I../firmware $(CFLAGS)

clean:
	rm comm

