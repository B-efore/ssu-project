#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>

#define N 1024
#define FIND_ERR_MSG "error/ > find [FILENAME] [PATH]\n"
#define EXIT_ERR_MSG "error/ > exit\n"

//큐에 저장하는 데이터의 형식
typedef struct Node {
	char data[N];
	struct Node *next;
} Node;

//디렉토리 탐색을 진행하는 동안 디렉토리 경로를 저장하는 큐
typedef struct Queue {
	Node *front;
	Node *rear;
} Queue;

//파일 비교를 위한 구간체크
typedef struct check {
	int s1;
	int s2;
	int e1;
	int e2;

	short sameNum;
	short diffNum;
} Check;

typedef short Section;

//각 라인을 비교한 정보 저장
Section section[2][N+1];
Check c;

bool isDIR;
int fileNumber;
long int directorySize;
struct dirent **findFile;
struct stat searchFile, tmpFile;
char fileName[N+1], path[9999999];
char file1[N+1], file2[N+1];
char **searchFilePath;

int find(char *, char *);
void diff(int, bool, bool *);
void compareTwoFile(int, bool*, char *, char *);
int filter(const struct dirent *);
int isSearchFile(struct dirent **, int);
int getLineNumber(FILE *);
int getDirSize(char *);
void resize(int);
char *fileMode(struct stat);
void printPath(char *, char*);
void printMode(int);
char printAdd();
char printDelete();
char printChange();
void printLine(FILE *, FILE *, char);
void printFileInfo(int, char *);
void enqueue(Queue *, char[]);
void dequeue(Queue *, char[]);
bool isEmpty(Queue *);

int main(void)
{
	//시간 측정용 변수
	struct timeval startT, endT, t;
	gettimeofday(&startT, NULL);

	//command: 명령어를 입력받기 위한 배열
	int i, j;
	char command[N+1];

	//프롬프트 시작
	while(1)
	{
		printf("20202914> ");
		memset(command, '\0', (N+1)*sizeof(char));

		scanf("%[^\n]s", command);
		getchar();

		char *tmp = strtok(command, " ");

		//공백 혹은 엔터인 경우 프롬프트 재출력
		if(tmp==NULL){
			continue;
		}
		else if(!strcmp(tmp, "find"))
		{
			memset(fileName, '\0', (N+1)*sizeof(char));
			memset(path, '\0', (N+1)*sizeof(char));

			//fileName 받기
			if((tmp = strtok(NULL, " "))==NULL)
			{
				printf(FIND_ERR_MSG);
				continue;
			}
			else
				strcpy(fileName, tmp);

			//path 받기
			if((tmp = strtok(NULL, " "))==NULL)
			{
				printf(FIND_ERR_MSG);
				continue;
			}
			else
				strcpy(path, tmp);

			//filename, path 이외에 다른 인자가 들어온 경우
			if((tmp = strtok(NULL, " "))!=NULL)
			{
				printf(FIND_ERR_MSG);
				continue;
			}

			//error
			if((stat(fileName, &searchFile)==-1) || (stat(path, &tmpFile)==-1)){
				printf("error/파일 혹은 디렉토리가 존재하지 않음\n");
				continue;
			}
			else{
			//입력받은 fileName이 경로로 들어온 경우 파일명만 추출하기

				char * fName;
				
				if((fName = strrchr(fileName, '/'))!=NULL)
					strcpy(fileName, fName+1);
			}

			isDIR = S_ISDIR(searchFile.st_mode);

			if(isDIR)
				directorySize = getDirSize(fileName);

			//check: 탐색이 성공적으로 진행 되었는지를 확인하기 위한 변수
			//ok: 옵션 명령어가 정확히 들어왔는지 확인하기 위한 변수
			//option: 각각의 옵션이 존재하는지에 대한 유무 체크 배열
			int check = find(fileName, path);
			int idx;
			bool ok;
			bool option[4];

			memset(option, false, sizeof(bool)*4);

			while(!check){
			
				ok = true;
				printf(">> ");

				memset(command, '\0', sizeof(char)*(N+1));

				scanf("%[^\n]s", command);
				getchar();

				char *tmp = strtok(command, " ");

				//입력이 없는 경우
				if(tmp == NULL){
					printf("error/명령어를 입력해주세요.\n");
					continue;
				}

				//idx 받기
				if((idx = atoi(tmp))==0||idx>=fileNumber){
					printf("error/ >> [INDEX] [OPTION ... ]\n");
					continue;
				}

				//option 받기
				while((tmp = strtok(NULL, " ")) != NULL)
				{
					if(!strcmp(tmp, "q"))
						option[0] = true;
					else if(!strcmp(tmp, "s"))
						option[1] = true;
					else if(!strcmp(tmp, "i"))
						option[2] = true;
					else if(!strcmp(tmp, "r"))
						option[3] = true;
					else
					{
						printf("error/ >> [INDEX] [OPTION ...]\n");
						ok=false;
						break;
					}
				}

				//명령어가 무사히 받아졌다면 diff함수 실행, 아니라면 다시 받아옴
				if(ok)
				{
					diff(idx, isDIR, option);
					check = 1;
				}
				else
				{
					continue;
				}
			}
		}
		else if(!strcmp(tmp, "exit"))
		{
			if((tmp = strtok(NULL, " "))!=NULL)
			{
				printf(EXIT_ERR_MSG);
				continue;
			}

			gettimeofday(&endT, NULL);

			t.tv_sec = endT.tv_sec - startT.tv_sec;
			t.tv_usec = endT.tv_usec - startT.tv_usec;

			if(t.tv_usec < 0){
				t.tv_sec -= 1;
				t.tv_usec+= 1000000;
			}

			printf("Prompt End\n");
			printf("Runtime: %ld:%ld(sec:usec)\n", t.tv_sec, t.tv_usec);

			exit(0);
		}
		else
		{
			FILE *fp;
			char *fname = "command.txt";
			char line[100];

			//help 호출을 위한 파일 오픈
			if((fp = fopen(fname, "r")) == NULL){
				fprintf(stderr, "fopen error for %s\n", fname);
				exit(1);
			}

			//help 명령어 실행
			while(fgets(line, sizeof(line), fp) != NULL)
				printf("%s", line);

			fclose(fp);
			continue;
		}
	}

	return 0;
}
//특정 경로를 제외한 파일들을 추출할 수 있도록 조건을 설정한 함수
int filter(const struct dirent *file){
	return (strcmp(file->d_name, ".") && strcmp(file->d_name, "..") && strcmp(file->d_name, "proc") && strcmp(file->d_name, "run") && strcmp(file->d_name, "sys"));
}
//원본 파일과 이름, 크기를 비교하고 결과값을 리턴하는 함수
int isSearchFile(struct dirent **namelist, int i){
	return (!strcmp(namelist[i]->d_name, fileName) && (searchFile.st_size == tmpFile.st_size));
}
//탐색에 성공한 파일/디렉토리의 경로를 저장하기 위한 배열의 사이즈를 동적으로 할당
void resize(int size)
{
	searchFilePath = (char **)realloc(searchFilePath, sizeof(char*)*size);

	if(searchFilePath == NULL){
		printf("searchFilePath is NULL\n");
		exit(1);
	}
	else
	{
		for(int i=size-10; i<size; i++)
			searchFilePath[i] = (char *)malloc(sizeof(char)*(N+1));
	}
}
//디렉토리를 탐색하고, 결과로 나온 디렉토리의 크기를 리턴
int getDirSize(char *fileName)
{
	Queue queue;
	queue.front = NULL;
	queue.rear = NULL;

	struct dirent **tmpFiles;
	int count;
    long int dirSize=0;
	char tmp[N+1], aPath[N+1];

	enqueue(&queue, fileName);

	while(!isEmpty(&queue))
	{
		dequeue(&queue, tmp);
		count = scandir(tmp, &tmpFiles, filter, alphasort);

		for(int i=0; i<count; i++)
		{
			 memset(aPath, '\0', sizeof(char)*(N+1));

             //하위 파일 경로 설정
             if(strcmp(tmp, "/"))
				 strcpy(aPath, tmp);
             strcat(aPath, "/");
             strcat(aPath, tmpFiles[i]->d_name);

             //파일 정보 가져오기
             stat(aPath, &tmpFile);

             //디렉터리인 경우 탐색을 위해 큐에 넣는다
			 if(S_ISDIR(tmpFile.st_mode))
		  		enqueue(&queue, aPath);
			 else
				dirSize += tmpFile.st_size;
		}
	}

	return dirSize;
}
int find(char *file, char *path)
{
	Queue queue;
	queue.front = NULL;
	queue.rear = NULL;

	int count, dirSize, size=20;
	char tmp[N+1], aPath[N+1];
	fileNumber = 0;

	//찾은 파일 경로를 저장하는 배열
	searchFilePath = (char **)malloc(sizeof(char*)*size);
	for(int i=0; i<size; i++)
		searchFilePath[i] = (char *)malloc(sizeof(char)*(N+1));

	//탐색을 시작할 path를 큐에 넣는다
	realpath(path, aPath);
	enqueue(&queue, aPath);

	printf("Index  Size  Mode        Blocks  Links  UID   GID   Access          Change          Modify          Path\n");

	while(!isEmpty(&queue))
	{
		//path의 하위 파일 탐색 시작
		dequeue(&queue, tmp);
		count = scandir(tmp, &findFile, filter, alphasort);

		for(int i=0; i<count; i++)
		{
			dirSize = 0;
			memset(aPath, '\0', sizeof(char)*(N+1));

			//하위 파일 경로 설정
			if(strcmp(tmp, "/"))
				strcpy(aPath, tmp);
			strcat(aPath, "/");
			strcat(aPath, findFile[i]->d_name);

			//파일 정보 가져오기
			stat(aPath, &tmpFile);

			//디렉토리인 경우 큐에 넣는다
			if(S_ISDIR(tmpFile.st_mode))
				enqueue(&queue, aPath);

			//찾는 파일이면 출력
			if(isSearchFile(findFile, i)){
				if(size<=fileNumber){
					resize(size+10);
					size+=10;
				}

				//디렉토리인 경우 크기를 측정하고, 원본 디렉토리와 비교 크기가 다를 시 넘어간다
				if(S_ISDIR(tmpFile.st_mode)){
					
					dirSize = getDirSize(aPath);

					if(dirSize != directorySize)
						continue;
				}
				
				//경로를 저장하고, 찾은 파일의 정보 출력
				strcpy(searchFilePath[fileNumber], aPath);
				printFileInfo(fileNumber, aPath);
				fileNumber++;
			}
		}
	}

	//조건에 부합하는 파일/디렉토리를 찾지 못한 경우
	if(fileNumber==1){
		printf("(None)\n");
		return 1;
	}

	return 0;
}
void diff(int idx, bool isDIR, bool *option)
{

	//디렉토리의 경우
	if(isDIR)
	{
		Queue queue1;
		queue1.front = NULL;
		queue1.rear = NULL;

		Queue queue2;
		queue2.front = NULL;
		queue2.rear = NULL;

		struct dirent **tmpF1;
		struct dirent **tmpF2;
		struct stat leftFile, rightFile;

		char tmp1[N+1], tmp2[N+1], aPath[N+1];
		int count1, count2, c, k=0;

		enqueue(&queue1, searchFilePath[0]);


//		if(option[3])
//		{
//			;
//		}
//		else
//		{
			count1 = scandir(searchFilePath[0], &tmpF1, filter, alphasort);
			count2 = scandir(searchFilePath[idx], &tmpF2, filter, alphasort);

			int i, j;

			for(i=0; i<count1; i++){

				memset(tmp1, '\0', sizeof(char)*(N+1));

				//하위 파일 경로 설정
				if(strcmp(searchFilePath[idx], "/"))
					strcpy(tmp1, searchFilePath[0]);
				strcat(tmp1, "/");
				strcat(tmp1, tmpF1[i]->d_name);

				stat(tmp1, &leftFile);

				for(j=k; j<count2; j++){

					memset(tmp2, '\0', sizeof(char)*(N+1));

					//하위 파일 경로 설정
					if(strcmp(searchFilePath[idx], "/"));
						strcpy(tmp2, searchFilePath[idx]);
					strcat(tmp2, "/");
					strcat(tmp2, tmpF2[j]->d_name);

					stat(tmp2, &rightFile);

					//원본 디렉토리 탐색이 끝났지만, 비교본 디렉토리 탐색이 끝나지 않은 경우
					if(count1==i)
					{
						printf("Only in ");
						printPath(searchFilePath[idx], searchFilePath[0]);
						printf(": %s\n", tmpF2[j]->d_name);
						continue;
					}

					c = strcmp(tmpF1[i]->d_name, tmpF2[j]->d_name);

					//원본 하위 파일의 이름이 사전순으로 앞선 경우
					if(c < 0){
						printf("Only in ");
						printPath(searchFilePath[0], searchFilePath[idx]);
						printf(": %s\n", tmpF1[i]->d_name);

						k = j;

						//원본 하위 파일이 더이상 없을 경우
						if((i+1)==count1){
							i++;
							continue;
						}
						else
							break;
					}
					else if (c > 0){
						//비교본 하위 파일의 이름이 사전순으로 앞선 경우
						printf("Only in ");
						printPath(searchFilePath[idx], searchFilePath[0]);
						printf(": %s\n", tmpF2[j]->d_name);
						continue;
					}
					else if(c == 0)
					{
						//이름이 같은 경우
						//만약에 디렉토리라면
						if(S_ISDIR(leftFile.st_mode)&&S_ISDIR(rightFile.st_mode))
						{
							printf("Common subdirectories : ");
							printPath(tmp1, tmp2);
							printf(" and ");
							printPath(tmp2, tmp1);
							printf("\n");

							k = j+1;

							//원본 하위 파일이 더이상 없을 경우
							if((i+1)==count1){
								i++;;
								continue;
							}
							else
								break;
						}

						//내용이 다른 경우
						compareTwoFile(idx, option, tmp1, tmp2);

						//종류가 다른 경우
						if(leftFile.st_mode!=rightFile.st_mode){
							printf("File ");
							printPath(tmp1, tmp2);
							printf(" is a ");
							printMode(leftFile.st_mode);
							printf(" while file ");
							printPath(tmp2, tmp1);
						    printf(" is a ");
							printMode(rightFile.st_mode);
							printf("\n");
						}

						k = j+1;

						//원본 하위 파일이 더이상 없을 경우
						if((i+1)==count1){
							i++;
							continue;
						}
						else
							break;
					}
				}

				//비교본 하위 파일이 더이상 없을 경우
				if(i<count1 && j==count2)
				{
					printf("Only in ");
					printPath(searchFilePath[0], searchFilePath[idx]);
					printf(": %s\n", tmpF1[i]->d_name);
				}
			}
//		}
	}
	else
	{
		//파일인 경우, 탐색 없이 두 개의 파일 바로 비교 진행
		compareTwoFile(idx, option, NULL, NULL);
	}
}
void compareTwoFile(int idx, bool *option, char *tmp1, char *tmp2)
{
		FILE *fp1, *fp2;

		//원본파일 열기
		if(tmp1==NULL)
		{
			if((fp1=fopen(searchFilePath[0], "r"))==NULL){
					fprintf(stderr, "fopen error for %s\n", searchFilePath[0]);
					exit(1);
			}
		}
		else
		{
			if((fp1=fopen(tmp1, "r"))==NULL){
				fprintf(stderr, "fopen error for %s\n", tmp1);
				exit(1);
			}
		}

		//비교파일 열기
		if(tmp2==NULL)
		{
			if((fp2=fopen(searchFilePath[idx], "r"))==NULL){
				fprintf(stderr, "fopen error for %s\n", searchFilePath[idx]);
				exit(1);
			}
		}
		else
		{
			if((fp2=fopen(tmp2, "r"))==NULL){
				fprintf(stderr, "fopen error for %s\n", tmp2);
				exit(1);
			}
		}

		long fpIdx=0;
		int i, j, k, tmp=0, isSame=1;
		int line1, line2;
		char mode;

		for(i=0; i<2; i++)
			memset(section[i], 0, sizeof(section[i]));

		c.s1=0; c.s2=0; c.e1=0; c.e2=0;
		c.diffNum=0; c.sameNum=0;

		line1 = getLineNumber(fp1);
		line2 = getLineNumber(fp2);

		for(i=1; fgets(file1, N+1, fp1)!= NULL; i++)
		{
			//파일의 마지막 라인인 경우 개행문자 제거
			if(i==line1)
				file1[strlen(file1)-1] = '\0';

			//틀린 부분의 처음으로 파일 커서 이동
			fseek(fp2, fpIdx, SEEK_SET);

			for(j = tmp ; fgets(file2, N+1, fp2)!= NULL; )
			{

				//파일의 마지막 라인인 경우 개행문자 제거
				if(j+1==line2)
					file2[strlen(file2)-1] = '\0';
		
				//만약 같으면
				if(!strcmp(file1, file2)||(option[2] && !strcasecmp(file1, file2))){

					//다르다가 처음으로 같은 경우
					if(!isSame||(isSame&&i==1&&j==0))
						c.sameNum++;

					j++; tmp=j;
					isSame = 1;
					fpIdx = ftell(fp2);

					section[0][i] = c.sameNum;
					section[1][j] = c.sameNum;

					break;
				}
				else{
					j++;

					//같다가 처음으로 다른 경우
					if(isSame)
						c.diffNum--;

					isSame = 0;
				
					section[0][i] = c.diffNum;
					section[1][j] = c.diffNum;

					continue;
				}
			}
		}

		bool left, right;

		//파일이 다른 경우
		if(c.diffNum<0){

			//디렉토리라면
			if(isDIR){
				printf("diff ");

				if(option[0])
					printf("-q ");
				if(option[1])
					printf("-s ");
				if(option[2])
					printf("-i ");
				if(option[3])
					printf("-r ");
				
				printPath(tmp1, tmp2); printf(" ");
				printPath(tmp2, tmp1); printf("\n");
			}

			//옵션 q
			if(option[0]){
				printf("Files ");
				printPath(searchFilePath[0], searchFilePath[idx]);
				printf(" and ");
			    printPath(searchFilePath[idx], searchFilePath[0]);
				printf(" differ\n");
			}
			else
			{
				//두 파일의 다른 부분 탐색
				for(i=-1; i>=c.diffNum; i--)
				{
					left=right=false;
					c.s1=c.s2=0;
					j=1;

					while((tmp = section[0][j])!=0)
					{
						//다른 부분이 원본에 존재
						if(tmp==i)
						{
							left = true;
		
							//다른 부분의 시작 c.s1 지정
							if(c.s1==0)
								c.s1=j;

							//다른 부분의 끝 c.e1 지정
							c.e1=j;

							if(section[0][j+1]!=i)
								break;
						}

						j++;
					}

					j = 1;

					while((tmp = section[1][j])!=0)
					{
						//다른 부분이 비교본에 존재
						if(tmp==i)
						{
							right = true;

							//다른 부분의 시작 c.s2 지정
							if(c.s2==0)
								c.s2=j;

							//다른 부분의 끝 c.e2 지정
							c.e2=j;

							if(section[1][j+1]!=i)
								break;
						}

						j++;
					}
	
					//다른 부분이 동시에 존재할 경우 change
					if(right && left)
						mode = printChange();
					else if(right)
					{
						//비교본에만 존재할 경우 add

						tmp = section[1][c.s2-1];

						for(k=1; section[0][k]!=0; k++)
							if(section[0][k]==tmp)
								c.e1=k;

						mode = printAdd();
					}
					else if(left)
					{
						//원본에만 존재할 경우 delete

						tmp = section[0][c.s1-1];

						for(k=1; section[1][k]!=0; k++)
							if(section[1][k]==tmp)
								c.e2=k;

						mode = printDelete();
					}
					else
						return;
	
					//다른 부분 출력
					printLine(fp1, fp2, mode);
				}
			}
		}
		else
		{
			//s옵션
			if(option[1]){
				printf("Files ");
				printPath(searchFilePath[0], searchFilePath[idx]);
				printf(" and ");
			    printPath(searchFilePath[idx], searchFilePath[0]);
				printf(" are identical\n");
			}
		}

		fclose(fp1);
		fclose(fp2);
}
//파일의 라인 수를 리턴한다
int getLineNumber(FILE *fp)
{
	int line=0;
	char tmp[N+1];

	while(fgets(tmp, N+1, fp)!=NULL)
		line++;

	rewind(fp);
	return line;
}
void printPath(char *tmp1, char *tmp2)
{
	int i, tmp;
	bool diff = false;

	for(i=0; i<strlen(tmp1);i++){

		if(tmp1[i]=='/'){
			if(!diff)
				tmp = i;
			diff = false;
		}

		if(tmp1[i]!=tmp2[i])
			diff = true;
	}

	for(i=tmp+1; i<strlen(tmp1); i++)
		printf("%c", tmp1[i]);
}
//파일의 종류를 출력한다
void printMode(int mode)
{
	int tmp = mode & S_IFMT;

	switch(tmp){
		case S_IFIFO:
			printf("FIFO");
			break;
		case S_IFCHR:
			printf("character device");
			break;
		case S_IFDIR:
			printf("directory");
			break;
		case S_IFBLK:
			printf("block device");
			break;
		case S_IFREG:
			printf("regular file");
			break;
		case S_IFLNK:
			printf("symbolic link");
			break;
		case S_IFSOCK:
			printf("socket");
			break;
	}
}
char printDelete()
{
	if(c.s1>=c.e1)
		printf("%dd%d\n", c.s1, c.e2);
	else
		printf("%d,%dd%d\n", c.s1, c.e1, c.e2);
	return 'd';
}
char printAdd()
{
	if(c.s2>=c.e2)
		printf("%da%d\n", c.e1, c.s2);
	else
		printf("%da%d,%d\n", c.e1, c.s2, c.e2);
	return 'a';
}
char printChange()
{
	if((c.s1>=c.e1)&&(c.s2<c.e2))
		printf("%dc%d,%d\n", c.s1, c.s2, c.e2);
	else if((c.s1>=c.e1)&&(c.s2>=c.e2))
		printf("%dc%d\n", c.s1, c.s2);
	else if((c.s1<c.e1)&&(c.s2>=c.e2))
		printf("%d,%dc%d\n", c.s1, c.e1, c.s2);
	else
		printf("%d,%dc%d,%d\n", c.s1, c.e1, c.s2, c.e2);
	
	return 'c';
}
void printLine(FILE *fp1, FILE *fp2, char mode)
{
	int i;
	bool isEOF1=false, isEOF2=false;

	if(mode=='d'||mode=='c')
	{
		fseek(fp1, 0, SEEK_SET);

		for(i=0; i<c.s1-1; i++)
			fgets(file1, N+1, fp1);

		for(i=c.s1; i<=c.e1; i++){
			if(fgets(file1, N+1, fp1)==NULL){
				if(feof(fp1)){
					isEOF1 = true;
					printf("\\ No newline at end of file\n");
				}
				else
					printf("error\n");
				break;
			}
			else
				printf("< %s", file1);
		}

		fgets(file1, (N+1), fp1);

		if(feof(fp1)&&(!isEOF1))
			printf("\\ No newline at end of file\n");
	}

	if(mode=='c')
		printf("---\n");

	if(mode=='a'||mode=='c')
	{
		fseek(fp2, 0, SEEK_SET);

		for(i=0; i<c.s2-1; i++)
			fgets(file2, N+1, fp2);

		for(i=c.s2; i<=c.e2; i++){
			if(fgets(file2, N+1, fp2)==NULL){
				if(feof(fp2)){
					isEOF2 = true;
					printf("\\ No newline at end of file\n");
				}
				else
					printf("error\n");
				break;
			}
			else
				printf("> %s", file2);
		}

		fgets(file2, N+1, fp2);

		if(feof(fp2)&&(!isEOF2))
			printf("\\ No newline at end of file\n");
	}
}
//파일의 권한을 체크한다
char* fileMode(struct stat file)
{
	int i;
	static char mode[11];
	
	memset(mode, '-', 10);

	i = file.st_mode & S_IFMT;

	switch(i){
		case S_IFDIR:
			mode[0] = 'd';
			break;
		case S_IFREG:
			mode[0] = '-';
			break;
	}

	if((file.st_mode & S_IRUSR))
		mode[1] = 'r';

  	if((file.st_mode & S_IWUSR))
		mode[2] = 'w';
 
    if((file.st_mode & S_IXUSR))
        mode[3] = 'x';
 
    if((file.st_mode & S_IRGRP))
        mode[4] = 'r';
 
    if((file.st_mode & S_IWGRP))
        mode[5] = 'w';
 
    if((file.st_mode & S_IXGRP))
        mode[6] = 'x';
 
    if((file.st_mode & S_IROTH))
        mode[7] = 'r';
 
    if((file.st_mode & S_IWOTH))
        mode[8] = 'w';

	if((file.st_mode & S_IXOTH))
        mode[9] = 'x';

    return mode;
}
//파일 정보 출력
void printFileInfo(int i, char *file)
{
	struct stat f;
	struct tm* t;
	long int size;

	stat(file, &f);

	if(S_ISDIR(f.st_mode))
		size = directorySize;
	else
		size = f.st_size;

	printf("%-6d %-5ld %-11s %-7ld %-6ld %-5d %-5d "
             , i, size, fileMode(f), f.st_blocks, f.st_nlink, f.st_uid, f.st_gid);
 
     t = localtime(&f.st_atime);
     printf("%02d-%02d-%02d %02d:%02d  "
			 , t->tm_year-100, t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min);
 
     t = localtime(&f.st_ctime);
     printf("%02d-%02d-%02d %02d:%02d  "
			 , t->tm_year-100, t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min);
 
     t = localtime(&f.st_mtime);
     printf("%02d-%02d-%02d %02d:%02d  "
			 , t->tm_year-100, t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min);
 
     printf("%s\n", file);
}
void enqueue(Queue *queue, char data[])
{
	Node * newNode = (Node *)malloc(sizeof(Node));

	strcpy(newNode->data, data);
	newNode -> next = NULL;

	if(queue -> front == NULL)
		queue -> front = newNode;
	else
		queue -> rear -> next = newNode;

	queue -> rear = newNode;
}
void dequeue(Queue *queue, char data[])
{
	if(!isEmpty(queue))
	{
		Node *tmpNode = queue -> front;
		strcpy(data, tmpNode -> data);

		if(tmpNode == queue -> rear)
		{
			queue -> front = NULL;
			queue -> rear = NULL;
		}
		else
			queue -> front = tmpNode -> next;

		free(tmpNode);
	}
}
bool isEmpty(Queue *queue)
{
	return (queue -> front == NULL);
}
