#include "DataStructure.h"
#include "func.h"
#include "ssu_find-md5.h"

int main(int argc, char *argv[])
{
	fmd5(argv);
	return 0;
}

void fmd5(char* argv[])
{
	struct timeval startT, endT, tT;
	char dirname[PATH_SIZE+1];	// 탐색할 디렉터리 임시 저장
	char curPath[PATH_SIZE+1];	// 현재 탐색중인 파일의 경로 임시 저장
	struct stat curFile;		// 탐색중인 파일의 파일 정보 받아오기

	// argv[1] : [FILE_EXTENSION]
	if(strcmp(argv[1], "*") && strncmp(argv[1], "*.", 2)){
		printf("Error: Usage [FILE_EXTENSION]\n");
		printf("	 : * or *.[extension]\n");
		return;
	}

	// argv[2] : [MINSIZE], argv[3] : [MAXSIZE]
	int minU, maxU;
	double minSize, maxSize;

	minSize = getSize(argv[2]);
	maxSize = getSize(argv[3]);

	minU = getUnit(argv[2]);
	maxU = getUnit(argv[3]);

	if(minU == -1 || maxU == -1){
		printf("Error: Unit error\n");
		return;
	}
	
	minSize *= minU;
	maxSize *= maxU;

	//홈 디렉터리 처리
	pw = getpwuid(getuid());
    homedir = pw->pw_dir;
    sprintf(trash, "%s%s", homedir, "/.local/share/Trash/files");

	char *ptr = strstr(argv[4], "~");

	if(ptr!=NULL){
		char tmp[PATH_SIZE];
		int i=0, j=0, len;

		strcpy(tmp, ptr+1);

		len = strlen(homedir);
		strncpy(ptr, homedir, len);
		ptr += len;
		*ptr = '\0';

		strcat(ptr, tmp);
	}

	// argv[4] : [TARGET_DIRECTORY]
	if(realpath(argv[4], dirname)==NULL){
		printf("Error: Path exist error\n");
		return;
	}

	if(lstat(dirname, &curFile)<0){
		fprintf(stderr, "lstat error for %s\n", dirname);
		return;
	}

	if(!S_ISDIR(curFile.st_mode)){
		printf("ERROR: Path must be directory\n");
		return;
	}

	gettimeofday(&startT, NULL);

	long long int size, count;	//디렉터리 안 파일의 개수
	struct dirent **namelist;	//디렉터리 안 파일 목록

	Queue queue;
	QueueInit(&queue);
	TableInit(&table);

	Enqueue(&queue, dirname);

	while(!isEmpty(&queue)){

		Dequeue(&queue, dirname);

		if (access(dirname, F_OK) != 0) {
			if (errno == 13)
				return; 	
			fprintf(stderr, "access error for %s\n", dirname);
			continue;
		}

		//directory 안에 있는 파일 받아오기
		if((count = scandir(dirname, &namelist, filter, alphasort)) == -1){
			fprintf(stderr, "Error: scandir error for %s\n", dirname);
			continue;
		}

		//파일 탐색
		for(int i=0; i<count; i++){
			if(!strcmp(dirname, "/")){
				if(!strcmp(namelist[i]->d_name, "proc")
					|| !strcmp(namelist[i]->d_name, "run")
					|| !strcmp(namelist[i]->d_name, "sys"))
					continue;
				else
					sprintf(curPath, "%s%s", dirname, namelist[i]->d_name);
			}
			else{
				sprintf(curPath, "%s/%s", dirname, namelist[i]->d_name);
			}

			if((lstat(curPath, &curFile)<0) && (!access(curPath, F_OK))){
				fprintf(stderr, "Error: lstat error for %s\n", curPath);
				exit(1);
			}

			//디렉토리인 경우
			if(S_ISDIR(curFile.st_mode)){
				Enqueue(&queue, curPath);
			}
			else if(S_ISREG(curFile.st_mode)){
	
				size = curFile.st_size;

				if(((minU==0)||(minSize <= size)) && ((size <= maxSize)||(maxU==0))){
	
					char *ex1 = getExtension(namelist[i]->d_name);
					char *ex2 = getExtension(argv[1]);

					if((!strcmp(argv[1], "*"))||(!strcmp(ex1, ex2))){
						FILE *IN = fopen(curPath, "r");
						unsigned char key[KEY_SIZE];

						if(IN==NULL){
							fprintf(stderr, "foepn error for %s\n", curPath);
							continue;
						}

						do_fp_md5(IN, key);
						TableInsert(&table, curPath, key, size);
					}
				}
			}
		}
	}

	//중복 파일이 없는 경우 테이블에서 삭제
	DeleteNotEqual(&table);

	//발견한 중복 파일이 없는 경우
	if(table.dataNum == 0){
		
		realpath(argv[4], dirname);

		printf("No duplicates in %s\n", dirname);

		gettimeofday(&endT, NULL);

		tT.tv_sec = endT.tv_sec - startT.tv_sec;
		tT.tv_usec = endT.tv_usec - startT.tv_usec;

		if(tT.tv_usec<0){
			tT.tv_sec--;
			tT.tv_usec += 1000000;
		}

		printf("Searching time: %ld:%06ld(sec:usec)\n\n", tT.tv_sec, tT.tv_usec);
		return;
	}

	//찾은 파일 목록 출력
	printFileList(&table, 'm');

	gettimeofday(&endT, NULL);

	tT.tv_sec = endT.tv_sec - startT.tv_sec;
	tT.tv_usec = endT.tv_usec - startT.tv_usec;

	if(tT.tv_usec<0){
		tT.tv_sec--;
		tT.tv_usec += 1000000;
	}

	printf("Searching time: %ld:%06ld(sec:usec)\n\n", tT.tv_sec, tT.tv_usec);
	prompt('m');
}
