#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <strings.h>

#define __NR_FW421_UNBLOCK_PORT 337
#define __IPPROTO_TCP 6
#define __IPPROTO_UDP 17

long fw421_unblock_port(int proto, int dir, unsigned short port){
	return syscall(__NR_FW421_UNBLOCK_PORT, proto, dir, port);
}

int main(int argc, char *argv[]) {

		long check;
		int proto;
		int dir;
		unsigned short port;

		if(argc < 4){
			printf("fw421_unblock_port.c need more args\n");
			exit(EXIT_FAILURE);
		}
		else if(argc > 4){
			printf("fw421_unblock_port.c remove extra args\n");
			exit(EXIT_FAILURE);
		}

		if(strcasecmp(argv[1], "tcp") == 0) {
			proto = __IPPROTO_TCP;
		}
		else if(strcasecmp(argv[1], "udp") == 0) {
			proto = __IPPROTO_UDP;
		}
		else {
			printf("fw421_unblock_port.c you can only use tcp or udp\n");
			exit(EXIT_FAILURE);
		}

		if(strcasecmp(argv[2], "in")) {
			dir = 1;
		}
		else if(strcasecmp(argv[2], "out")) {
			dir = 0;
		}

		else {
			printf("fw421_unblock_port.c you can only use in or out(in=1, out=0)\n");
			exit(EXIT_FAILURE);
		}
		port = (unsigned short)atoi(argv[3]);

	check = fw421_unblock_port(proto, dir, port);
	if(check != 0){
		perror("fw421_unblock_port.c failed");
		exit(EXIT_FAILURE);
	}
    return check;
}
