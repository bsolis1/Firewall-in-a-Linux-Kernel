#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/kernel.h>
#include <sys/syscall.h>

#define __NR_FC421_QUERY 342

long fc421_query(const char *filename){
	return syscall(__NR_FC421_QUERY, filename);
}

int main(int argc, char *argv[]) {
	long check;

	if(argc < 2){
		printf("fw421_query.c need more args\n");
		exit(EXIT_FAILURE);
	}
	else if(argc > 2){
		printf("fw421_query remove extra arguments\n");
		exit(EXIT_FAILURE);
	}

	check = fc421_query(argv[1]);
	if(check < 0){
		perror("fc421_query syscall failed");
		exit(EXIT_FAILURE);
	}

	printf("File is blocked, tried to access %d times.\n", check);
    return 0;


}
