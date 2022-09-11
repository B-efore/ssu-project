#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
int main(int argc, char *argv[])
{
	int i, fd, offset, bytes;
	char c;

	if(argc!=4){
		fprintf(stderr, "Usage: a.out <offset> <bytes> <filename>\n");
		exit(1);
	}
	else{
		offset = atoi(argv[1]);
		bytes = atoi(argv[2]);
	}

	if((fd=open(argv[3], O_RDONLY))<0){
		fprintf(stderr, "open error for %s\n", argv[3]);
		exit(1);
	}

	if(bytes<0){
		if(offset+bytes<0){
			bytes = (-offset);
		}

		if(lseek(fd, offset+bytes, SEEK_SET)<0){
			fprintf(stderr, "lseek error\n");
			exit(1);
		}

		bytes = (-bytes);
	}
	else{
		if(lseek(fd, offset+1, SEEK_SET)<0){
			fprintf(stderr, "lseek error\n");
			exit(1);
		}
	}

	for(i=0; i<bytes; i++){
		if(read(fd, &c, 1)>0)
			putchar(c);
		else
			break;
	}

	close(fd);
	exit(0);
}
