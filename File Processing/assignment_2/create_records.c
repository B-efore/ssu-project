#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#define RECORD_SIZE 200
int main(int argc, char *argv[])
{
	int n, fd;
	char buf[RECORD_SIZE];

	if(argc!=3){
		printf("Usage: %s <#records> <record_file_name>\n", argv[0]);
		return 1;
	}

	if((n=atoi(argv[1]))==0){
		fprintf(stderr, "atoi error\n");
		return 1;
	}

	if((fd=open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0664))<0){
		fprintf(stderr, "open error for %s\n", argv[2]);
		return 1;
	}

	for(int i=0; i<RECORD_SIZE; i++)
		buf[i] = 'a';

	for(int i=0; i<n; i++){

		if(write(fd, buf, RECORD_SIZE)<1){
			fprintf(stderr, "write error\n");
			return 1;
		}

	}

	close(fd);
	return 0;
}
