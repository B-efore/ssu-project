#include <stdio.h>
#include <stdlib.h>
#define BUFFER_SIZE 4096
int main(int argc, char *argv[])
{
	FILE *fp;

	char c, buf[BUFFER_SIZE];
	long offset, tmp;
   	int i, bytes;

	if(argc!=4){
		fprintf(stderr, "Usage: a.out <offset> <bytes> <filename>\n");
		exit(1);
	}
	else{
		offset = atoi(argv[1]);
		bytes = atoi(argv[2]);
	}

	if((fp=fopen(argv[3], "r+"))==NULL){
		fprintf(stderr, "fopen error for %s\n", argv[3]);
		exit(1);
	}

	if(bytes<0){
		if(offset+bytes<0){
			bytes = (-offset);
		}

		if(fseek(fp, offset+bytes, SEEK_SET)!=0){
			fprintf(stderr, "fseek error\n");
			exit(1);
		}

		bytes = (-bytes);
	}
	else{
		if(!fseek(fp, 0, SEEK_END)){
			tmp = ftell(fp);

			if(tmp<offset+bytes+1)
				bytes = tmp - offset;
		}

		if(fseek(fp, offset+1, SEEK_SET)!=0){
			fprintf(stderr, "fseek error\n");
			exit(1);
		}
	}

	for(i=0; i<bytes; i++)
		fwrite("*", 1, sizeof(char), fp);

	i=0;
	fseek(fp, 0, SEEK_SET);

	while(!feof(fp)){
		if((c=fgetc(fp))=='*')
			continue;
		else
			buf[i++] = c;
	}

	if((fp=freopen(argv[3], "w", fp))==NULL){
		fprintf(stderr, "freopen error for %s\n", argv[3]);
		exit(1);
	}

	fwrite(buf, i-1, sizeof(char), fp);

	return 0;
}
