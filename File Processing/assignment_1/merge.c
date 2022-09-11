#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define BUFFER_SIZE 1024
int main(int argc, char *argv[])
{
	char buf[BUFFER_SIZE];
	int c, fd1, fd2, fd3;

	if(argc!=4){
		fprintf(stderr, "Usage: a.out <file1> <file2> <file3>\n");
		exit(1);
	}

	if((fd1=open(argv[1], O_RDONLY))<0){
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	if((fd2=open(argv[2], O_RDONLY))<0){
		fprintf(stderr, "open error for %s\n", argv[2]);
		exit(1);
	}

	if((fd3=open(argv[3], O_WRONLY|O_CREAT|O_TRUNC, 0644))<0){
		fprintf(stderr, "open error for %s\n", argv[3]);
		exit(1);
	}

	while((c=read(fd1, buf, BUFFER_SIZE))>0)
		write(fd3, buf, c);

	while((c=read(fd2, buf, BUFFER_SIZE))>0)
		write(fd3, buf, c);

	close(fd1);
	close(fd2);

	exit(0);
}	
