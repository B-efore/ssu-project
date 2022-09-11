#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#define SUFFLE_NUM	10000
#define RECORD_SIZE 200

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
int main(int argc, char **argv)
{
	char buf[RECORD_SIZE];
	off_t size;
	struct timeval t, start, end;
	int *read_order_list;
	int fd, num_of_records;

	if(argc!=2){
		printf("Usage: %s <record_file_name>\n", argv[0]);
		return 1;
	}

	if((fd=open(argv[1], O_RDONLY))<0){
		fprintf(stderr, "open error for %s\n", argv[1]);
		return 1;
	}

	if((size = lseek(fd, 0, SEEK_END))<0){
		fprintf(stderr, "lseek error\n");
		return 1;
	}

	num_of_records = size/RECORD_SIZE;

	gettimeofday(&start, NULL);
	GenRecordSequence(read_order_list, num_of_records);

	int i=0;

	while(i<num_of_records){

		if(lseek(fd, read_order_list[i]*RECORD_SIZE, SEEK_SET)<0){
			fprintf(stderr, "lseek error\n");
			break;
		}

		if(read(fd, buf, RECORD_SIZE)<0){
			fprintf(stderr, "read error\n");
			break;
		}

		i++;
	}
	gettimeofday(&end, NULL);

	t.tv_sec = end.tv_sec - start.tv_sec;
	t.tv_usec = end.tv_usec - start.tv_usec;

	printf("%ld usec\n", t.tv_sec*1000000 + t.tv_usec);
	return 0;
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for(i=0; i<n; i++)
	{
		list[i] = i;
	}
	
	for(i=0; i<SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}

	return;
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

	return;
}
