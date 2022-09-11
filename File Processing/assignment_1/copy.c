#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define BYTE_SIZE 10
int main(int argc, char *argv[])
{
	char buf[BYTE_SIZE];
	int nbyte, fd1, fd2;

	if(argc!=3){
		fprintf(stderr, "Usage: a.out <file1> <file2>\n");
		exit(1);
	}

	if((fd1=open(argv[1], O_RDONLY))<0){
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	if((fd2=open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0644))<0){
		fprintf(stderr, "open error for %s\n", argv[2]);
		exit(1);
	}

	while((nbyte=read(fd1, buf, BYTE_SIZE))>0){
		if(write(fd2, buf, nbyte)<0){
			fprintf(stderr, "write error\n");
			exit(1);
		}
	}

	close(fd1);
	close(fd2);

	exit(0);
}
