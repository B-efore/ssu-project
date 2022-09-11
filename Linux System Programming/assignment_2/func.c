#include "DataStructure.h"
#include "func.h"
#include "ssu_find-md5.h"
#include "ssu_find-sha1.h"

//탐색 파일 필터
int filter(const struct dirent *file)
{
	return (strcmp(file->d_name, ".") && strcmp(file->d_name, ".."));
}

//명령어 추출 함수
int getCommand(char *input, char **argv)
{
	int argc=0;
	char *tmp = strtok(input, " ");

	while(tmp!=NULL){
		argv[argc++] = tmp;
		tmp = strtok(NULL, " ");
	}

	return argc;
}

//파일의 확장자
char *getExtension(char *file)
{
	char *extension = (char *)malloc(sizeof(char)*FILE_NAME_SIZE);
	int len = strlen(file);
	int i;

	for(i=len; i>0; i--){
		if(file[i] == '.')
			break;
	}

	strcpy(extension, (file+i));
	return extension;
}

//탐색 파일 사이즈 범위 인자 구하기
double getSize(char *str)
{
	char *size = (char*)malloc(sizeof(char)*500);
	int i =0;
	int dot =0;

	//숫자와 '.'만 받기
	while(*str && i<500){
		if(isdigit(*str)||str[0]=='.'){
			size[i++] = *str;

			if(str[0]=='.')
				dot++;
		}

		str++;
	}

	if(dot>1) return -1;

	double s = atof(size);
	free(size);

	return s;
}

//탐색 파일 단위 인자 구하기
int getUnit(char *str)
{
	char *unit = (char*)malloc(sizeof(char)*4);
	int i = 0;

	//숫자와 '.'를 제외한 문자 받기
	while(*str && i<3){
		if(!isdigit(*str)&&str[0]!='.')
			unit[i++] = *str;
		str++;
	}

	unit[i] = '\0';

	if(!strcmp(unit, "~"))
		return 0;
	else if(!strcmp(unit, "KB") || !strcmp(unit, "kb"))
		return 1000;
	else if(!strcmp(unit, "MB") || !strcmp(unit, "mb"))
		return 1000000;
	else if(!strcmp(unit, "GB") || !strcmp(unit, "gb"))
		return 1000000000;
	else if(strlen(unit)==0)	//단위가 들어오지 않은 경우
		return 1;
	else						//잘못된 단위가 들어온 경우
		return -1;
}

//경로에서 파일 이름 추출
char *getFileName(char *file)
{
	int i;
	int len = strlen(file);

	for(i = len-1; len>=0; i--)
		if(file[i]=='/')
			break;

	return (file+i+1);
}

// 파일 목록 출력하기
void printFileList(Table *table, char m)
{
	int i, j;

	i = 1;
	table->cur = table->head->next;
	table->cur->cur = table->cur->head->next;

	while(table->cur!=NULL){
		printf("---- Identical files #%d (%s bytes - ", i, printFileSize(table->cur->size));
		if(m=='m')
			pt_md5(table->cur->key);
		else
			pt_sha1(table->cur->key);
		printf(") ----\n");
		
		j = 1;
		while(table->cur->cur != NULL){
			printf("[%d] %s ", j, table->cur->cur->data);
			printTime(table->cur);
			table->cur->cur = table->cur->cur->next;
			j++;
		}
		printf("\n");

		if(table->cur->next==NULL)
			break;

		table->cur = table->cur->next;
		table->cur->cur = table->cur->head->next;
		i++;
	}
}

char *printFileSize(int number)
{
    char p[500];
    char *result = (char*)malloc(sizeof(char)*500);
	int i=0, j=0;

	if(number==0){
		result[0] = '0';
		result[1] = '\0';
		return result;
	}

	for(i=0; i<20 && number>0; i++){
    	if(i && (i%3) == 0)
        	p[j++] =  ',';

		p[j++] = (number%10) + '0';
        	number /= 10;
	}

	i=0, j--;
    while(j>=0)
    	result[i++] = p[j--];

	result[i] = '\0';
	return result;
}

// 파일 시간 추출하기
void printTime(List *list)
{
	struct stat f;
	struct tm *t;

	stat(list->cur->data, &f);

	t = localtime(&f.st_mtime);
	printf("(mtime : %04d-%02d-%02d %02d:%02d:%02d) ",
			1900 + t->tm_year, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

	t = localtime(&f.st_atime);
	printf("(atime : %04d-%02d-%02d %02d:%02d:%02d)\n",
			1900 + t->tm_year, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}
//file의 mTime을 출력
void print_mTime(List *list)
{
	struct stat f;
	struct tm *t;

	stat(list->cur->data, &f);

	t = localtime(&f.st_mtime);
	printf("(%04d-%02d-%02d %02d:%02d:%02d)\n\n",
			1900 + t->tm_year, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}

void prompt(char m)
{
	int argc;
	char *argv[BUFFER_SIZE];
	char input[BUFFER_SIZE];

	while(1){
		printf(">> ");
		fgets(input, sizeof(input), stdin);
		input[strlen(input)-1] = '\0';

		argc = getCommand(input, argv);

		if(!strcmp(argv[0] ,"exit")){
			printf(">> Back to Prompt\n\n");
			break;
		}

		if(argc<2){
			print_command2_usage();
			continue;
		}

		int set_index, list_index;

		if(!strcmp(argv[1], "d")){

			if(argc!=3){
				print_command2_usage();
				continue;
			}

			set_index = atoi(argv[0]);
			if(set_index<0 || set_index>table.dataNum){
				printf("Error: 올바른 범위를 입력해주세요.\n");
				continue;
			}

			TableSearch(&table, set_index);

			list_index = atoi(argv[2]);
			if(list_index<0 || list_index>table.cur->dataNum){
				printf("Error: 올바른 범위를 입력해주세요.\n");
				continue;
			}

			ListSearch(&table, table.cur, list_index);

			if(remove(table.cur->cur->data)<0)
				fprintf(stderr, "remove error for %s\n", table.cur->cur->data);

			printf("\"%s\" has been deleted in #%d\n\n", Delete(table.cur), set_index);

			// 세트 데이터의 개수가 1개 이하인 경우 테이블에서 제외
			if(table.cur->dataNum<=1)
				TableDelete(&table);

		}
		else if(!strcmp(argv[1], "i")){

			char c[5];

			if(argc!=2){
				print_command2_usage();
				continue;
			}

			set_index = atoi(argv[0]);
			if(set_index<0 || set_index>table.dataNum){
				printf("Error: 올바른 범위를 입력해주세요.\n");
				continue;
			}


			TableSearch(&table, set_index);
			table.cur->cur = table.cur->head->next;
			table.cur->before = table.cur->head;

			while(table.cur->cur != NULL){
				printf("Delete \"%s\"? [y/n] ", table.cur->cur->data);
				fgets(c, sizeof(c), stdin);
				c[strlen(c)-1] = '\0';

				if(!strcmp(c, "y") || !strcmp(c, "Y")){
					if(remove(table.cur->cur->data)<0)
						fprintf(stderr, "remove error for %s\n", table.cur->cur->data);
					Delete(table.cur);
				}
				else if(!strcmp(c, "n") || !strcmp(c, "N"))
					;
				else{
					printf("Error: y(Y) or n(N)\n");
					break;
				}

				table.cur->cur = table.cur->cur->next;
			}

			if(table.cur->dataNum<=1)
				TableDelete(&table);

			printf("\n");
		}
		else if(!strcmp(argv[1], "f")){

			if(argc!=2){
				print_command2_usage();
				continue;
			}

			set_index = atoi(argv[0]);
			if(set_index<0 || set_index>table.dataNum){
				printf("Error: 올바른 범위를 입력해주세요.\n");
				continue;
			}

			TableSearch(&table, set_index);
			table.cur->cur = table.cur->head->next;
			table.cur->before = table.cur->head;

			List tmp;

			time_t last=0;
			struct stat curFile;

			//가장 최근에 수정한 파일 찾기
			while(table.cur->cur != NULL){

				lstat(table.cur->cur->data, &curFile);

				if(curFile.st_mtime > last){
					last = curFile.st_mtime;
					tmp = *(table.cur);
					tmp.cur = table.cur->cur;
				}

				table.cur->before = table.cur->cur;
				table.cur->cur = table.cur->cur->next;
			}

			table.cur->cur = table.cur->head->next;
			table.cur->before = table.cur->head;

			// 나머지 중복 파일 삭제
			while(table.cur->cur != NULL){

				lstat(table.cur->cur->data, &curFile);

				if(table.cur->cur != tmp.cur){
					if(remove(table.cur->cur->data)<0)
						fprintf(stderr, "remove error for %s\n", table.cur->cur->data);
					Delete(table.cur);
				}

				table.cur->before = table.cur->cur;
				table.cur->cur = table.cur->cur->next;
			}

			printf("Left file in #%d : %s ", set_index, tmp.cur->data);
			print_mTime(&tmp);
			printf("\n");
			
			if(table.cur->dataNum<=1)
				TableDelete(&table);

		}
		else if(!strcmp(argv[1], "t")){

			if(argc!=2){
				print_command2_usage();
				continue;
			}

			set_index = atoi(argv[0]);
			if(set_index<0 || set_index>table.dataNum){
				printf("Error: 올바른 범위를 입력해주세요.\n");
				continue;
			}

			TableSearch(&table, set_index);
			table.cur->cur = table.cur->head->next;
			table.cur->before = table.cur->head;

			List tmp;

			time_t last=0;
			struct stat curFile;

			// 가장 최근에 수정한 파일 찾기 -> 이거 함수로 만들자 ㅇㅇ
			while(table.cur->cur != NULL){

				lstat(table.cur->cur->data, &curFile);

				if(curFile.st_mtime > last){
					last = curFile.st_mtime;
					tmp = *(table.cur);
					tmp.cur = table.cur->cur;
				}

				table.cur->cur = table.cur->cur->next;
			}

			table.cur->cur = table.cur->head->next;
			table.cur->before = table.cur->head;

			//휴지통으로 보냄
			while(table.cur->cur != NULL){

				lstat(table.cur->cur->data, &curFile);

				char fileName[FILE_NAME_SIZE*2];

				sprintf(fileName, "%s/%s", trash, getFileName(table.cur->cur->data));

				if(table.cur->cur != tmp.cur){
					link(table.cur->cur->data, fileName);
					unlink(table.cur->cur->data);
					Delete(table.cur);
				}

				table.cur->before = table.cur->cur;
				table.cur->cur = table.cur->cur->next;
			}

			printf("All files in #%d have moved to Trash except \"%s\" ", set_index, tmp.cur->data);
			print_mTime(&tmp);			

			if(table.cur->dataNum<=1)
				TableDelete(&table);
		}
		else{
			print_command2_usage();
			continue;
		}

		//테이블에 데이터가 없는 경우
		if(table.dataNum==0)
			break;

		//중복 파일 세트 재출력
		printFileList(&table, m);
	}
}

void do_fp_md5(FILE *f, unsigned char *tmp)
{
	MD5_CTX c;
	unsigned char md[MD5_DIGEST_LENGTH];
	int fd;
	int i;
	static unsigned char buf[BUFFER_SIZE];

	fd=fileno(f);
	MD5_Init(&c);
	for (;;)
		{
		i=read(fd,buf,BUFFER_SIZE);
		if (i <= 0) break;
		MD5_Update(&c,buf,(unsigned long)i);
		}
	MD5_Final(&(md[0]),&c);
	strcpy(tmp, md);
//	pt(md);
}

void pt_md5(unsigned char *md)
{
	int i;

	for (i=0; i<MD5_DIGEST_LENGTH; i++)
		printf("%02x",md[i]);
}

void do_fp_sha1(FILE *f, unsigned char *tmp)
{
	SHA_CTX c;
	unsigned char md[SHA_DIGEST_LENGTH];
	int fd;
	int i;
	unsigned char buf[BUFFER_SIZE];

	fd=fileno(f);
	SHA1_Init(&c);
	for (;;)
		{
		i=read(fd,buf,BUFFER_SIZE);
		if (i <= 0) break;
		SHA1_Update(&c,buf,(unsigned long)i);
		}
	SHA1_Final(&(md[0]),&c);
	strcpy(tmp, md);
//	pt_sha1(md);
}

void pt_sha1(unsigned char *md)
{
	int i;

	for (i=0; i<SHA_DIGEST_LENGTH; i++)
		printf("%02x",md[i]);
//	printf("\n");
}

void print_command2_usage()
{
	printf(">> [SET_INDEX] [OPTION ... ]\n");
	printf("   [OPTION ... ]\n");
	printf("   d [LIST_IDX] : delete [LIST_IDX] file\n");
	printf("   i : ask for confirmation before delete\n");
	printf("   f : force delete except the recently modified file\n");
	printf("   t : force move to Trash except the recently modified file\n");
	printf("\n");
}
