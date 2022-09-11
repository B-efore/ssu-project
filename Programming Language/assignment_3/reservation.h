#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PATH_MAX 4096
#define INPUT_MAX 256
#define ID_MAX 10
#define ID_MIN 5
#define DB "Database"
#define ROOMINFO "roominfo"

void print_room(char *fullpath);

typedef struct roominfo {
	char roomName[10];
	int floor;
	int desk;
	int computer;
	int people;
} Roominfo;

typedef struct data {
	int cafe;
	int room;
	int people;
	char userID[ID_MAX+1];
	char name[10];
	char phone[15];
	char date[10];
	char check[20];
} Data;

char DBPath[PATH_MAX];
char ManagerPath[PATH_MAX];
char UserPath[PATH_MAX];

// 데이터를 관리하는 디렉터리들 생성
void getDataDir()
{
	realpath(DB, DBPath);
	sprintf(ManagerPath, "%s/%s", DBPath, "Manager");
	sprintf(UserPath, "%s/%s", DBPath, "User");

	if(access(DBPath, F_OK)!=0)
		mkdir(DBPath, 0755);

	if(access(ManagerPath, F_OK)!=0)
		mkdir(ManagerPath, 0755);

	if(access(UserPath, F_OK)!=0)
		mkdir(UserPath, 0755);
}

void getPath(char path1[], char path2[], char result[])
{
	if(path1[strlen(path1)-1]=='/')
		sprintf(result, "%s%s", path1, path2);
	else
		sprintf(result, "%s/%s", path1, path2);
}

void remove_file(char *dir)
{
	struct dirent **namelist;
	int listcnt;

	if((listcnt=scandir(dir, &namelist, NULL, alphasort))==-1){
		printf("ERROR: scandir error for %s\n", dir);
		return;
	}

	for(int i=0; i<listcnt; i++){

		char fullpath[PATH_MAX] = {0, };

		if(!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, ".."))
			continue;

		getPath(dir, namelist[i]->d_name, fullpath);
		remove(fullpath);
	}
}

int getInput(char *str)
{
	char buf[PATH_MAX];

	while(1){

		printf("%s", str);
		fgets(buf, INPUT_MAX, stdin);
		buf[strlen(buf)-1] = '\0';

		if(buf[0]=='\0') continue;

		if(strcmp(buf, "10") && strlen(buf)>1) continue;
	
		if(!(buf[0]>='0' && buf[0]<='9')) continue;

		return atoi(buf);
	}
}
