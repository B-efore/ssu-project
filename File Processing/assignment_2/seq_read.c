#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#define RECORD_SIZE 200
// input parameters: 레코드 파일
int main(int argc, char **argv)
{
	int fd;
	char buf[RECORD_SIZE];
	off_t size;
	struct timeval t, start, end;

	if(argc!=2){
		printf("Usage: %s <record_file_name>\n", argv[0]);
		return 1;
	}

	if((fd=open(argv[1], O_RDONLY))<0){
		fprintf(stderr, "open error for %s\n", argv[1]);
		return 1;
	}

	gettimeofday(&start, NULL);
	while(read(fd, buf, RECORD_SIZE)>0);
	gettimeofday(&end, NULL);


	t.tv_sec = end.tv_sec - start.tv_sec;
	t.tv_usec = end.tv_usec - start.tv_usec;

	printf("%ld usec\n", t.tv_sec*1000000 + t.tv_usec);
	return 0;
}
