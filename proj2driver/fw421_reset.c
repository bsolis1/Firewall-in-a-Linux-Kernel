#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/kernel.h>
#include <sys/syscall.h>

#define __NR_FW421_RESET 335

long fw421_reset(void){
	return syscall(__NR_FW421_RESET);
}


int main(int argc, char *argv[]) {

	long check = fw421_reset();

	if(check != 0){
		perror("fw421_reset.c failed");
		exit(EXIT_FAILURE);
	}

  return check;
}
