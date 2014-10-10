#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

#include <comm.h>
#include <joystick.h>

int main(int argc, char **argv)
{
	printf("OSUAR groundstation starting\n");

	/* Usage */
	if (argc < 3) {
		printf("Usage:\n  %s <joystick> <serial>\n", argv[0]);
		exit(1);
	}

	char *jspath = argv[1];
	int joydev = open(jspath, O_RDONLY);
	if (joydev == -1) {
		perror("Error opening joystick");
		exit(EXIT_FAILURE);
	}

	char *serpath = argv[2];
	int serdev = open(serpath, O_RDWR);
	if (serdev == -1) {
		perror("Error opening serial");
		exit(EXIT_FAILURE);
	}

	pthread_t joy_th, comm_th;

	pthread_create(&joy_th, NULL, joy, &joydev);
	pthread_create(&comm_th, NULL, comm, &serdev);

	pthread_join(joy_th, NULL);
	pthread_join(comm_th, NULL);

	return 0;
}

