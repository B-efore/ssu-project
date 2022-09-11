#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define BUFFER_SIZE 1024
int main(int argc, char *argv[])
{
	char buf[BUFFER_SIZE];
	int data_bytes, offset, fd;

	if(argc!=4){
		fprintf(stderr, "Usage: a.out <offset> <data> <filename>\n");
		exit(1);
	}
	else{
		offset = atoi(argv[1]);
		data_bytes = strlen(argv[2]);
	}

	if((fd=open(argv[3], O_RDWR))<0){
		fprintf(stderr, "open error for %s\n", argv[3]);
		exit(1);
	}

	if(lseek(fd, offset, SEEK_SET)<0){
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	if(write(fd, argv[2], data_bytes)<0){
		fprintf(stderr, "write error\n");
		exit(1);
	}

	close(fd);
	exit(0);
}
