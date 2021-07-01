#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>

#define __NR_FC421_BLOCK_FILE 340

long fc421_block_file(const char *filename){
	return syscall(__NR_FC421_BLOCK_FILE, filename);
}



int main(int argc, char *argv[]) {
	long check;

	if(argc < 2){
		printf("fc421_block_file.c need more arguments\n");
		exit(EXIT_FAILURE);
	}
	else if(argc > 2){
		printf("fc421_block_file.c remove extra arguments\n");
		exit(EXIT_FAILURE);
	}

	check = fc421_block_file(argv[1]);
	if(check != 0){
		perror("Problems in fc421_block_file.c");
		exit(EXIT_FAILURE);
	}
  return check;
}
