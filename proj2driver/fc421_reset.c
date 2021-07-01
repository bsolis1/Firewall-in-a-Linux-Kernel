#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>

#define __NR_FC421_RESET 339

long fc421_reset(void){
	return syscall(__NR_FC421_RESET);
}

int main(int argc, char *argv[]) {

	long check = fc421_reset();
	if(check != 0){
		perror("fc421_reset.c failed");
		exit(EXIT_FAILURE);
	}
    return check;
}
