#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>

#define __NR_FC421_UNBLOCK_FILE 341


long fc421_unblock_file(const char *filename){
	return syscall(__NR_FC421_UNBLOCK_FILE, filename);
}


int main(int argc, char *argv[]) {
	long check;
	if(argc < 2){
		printf("fc421_unblock_file.c need more arguments\n");
		exit(EXIT_FAILURE);
	}
	else if(argc > 2){
		printf("fc421_unblock_file.c remove extra arguments\n");
		exit(EXIT_FAILURE);
	}

	check = fc421_unblock_file(argv[1]);
	if(check != 0){
		perror("fc421_unblock_file.c failed");
		exit(EXIT_FAILURE);
	}
    return check;
}
