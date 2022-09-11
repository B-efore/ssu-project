#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <dirent.h>
#include <ctype.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <openssl/sha.h>
#include <errno.h>

#define NAMEMAX 255
#define PATHMAX 4096

#define STRMAX 10000
#define ARGMAX 15
#define OPTMAX 10

#define DIRECTORY 1
#define REGFILE 2

#define KB 1000
#define MB 1000000
#define GB 1000000000
#define KiB 1024
#define MiB 1048576
#define GiB 1073741824
#define SIZE_ERROR -2

#define HASHMAX 41

// 파일 정보를 담는 구조체
typedef struct fileInfo	{
	char path[PATHMAX];
	struct stat statbuf;
	struct fileInfo *next;
} fileInfo;

// 중복 파일 세트 관리 리스트
typedef struct fileList {
	long long filesize;
	char hash[HASHMAX];
	fileInfo *fileInfoList;
	struct fileList *next;
} fileList;

typedef struct dirList {
	char dirpath[PATHMAX];
	struct dirList *next;
} dirList;

void get_same_size_files_dir(void);
void remove_files(char *dir);
char *log_msg(int mode, char *path);
void get_log_path();
void get_log_file();
void get_trash_path();
void get_trash_file(char *path);

int get_argv(char *input, char *argv[]);
char *get_extension(char *filename);
long long get_size(char *filesize);
void set_filesize(long long filesize, char *filesize_w_comma);
void get_path_from_home(char *path, char *path_from_home);

void fileinfo_append(fileInfo *head, char *path);
fileInfo *fileinfo_delete_node(fileInfo *head, char *path);
int fileinfolist_size(fileInfo *head);

void filelist_append(fileList *head, long long filesize, char *path, char *hash);
void filelist_delete_node(fileList *head, char *hash);
int filelist_size(fileList *head);
int filelist_search(fileList *head, char *hash);
void print_filelist(fileList *head);

void dirlist_append(dirList *head, char *path);
void dirlist_delete_all(dirList *head);

void get_time(struct tm *time, char *set_time);
time_t get_recent_mtime(fileInfo *head, char *last_filepath);

int get_file_mode(char *target_file, struct stat *statbuf);
int get_dirlist(char *target_dir, struct dirent ***namelist);
int is_dir(char *target_dir);
void get_filename(char *path, char *filename);
void get_new_file_name(char *path, char *org_filename, char *new_filename);
void get_fullpath(char *target_dir, char *target_file, char *fullpath);

int minbsize, maxbsize, t_num;
char extension[10];
char target_dir[PATHMAX];
char trash_path[PATHMAX];
char trash_info[PATHMAX];
char log_path[PATHMAX];
char same_size_files_dir[PATHMAX];
fileList *dups_list_h;

// 같은 크기의 파일을 관리하는 디렉터리 초기화
// 디렉터리가 이미 존재하는 경우에는 디렉터리를 비우고, 존재하지 않는 경우 생성한다
void get_same_size_files_dir(void)
{
	get_path_from_home("~/20202914", same_size_files_dir);

	if(access(same_size_files_dir, F_OK)==0)
		remove_files(same_size_files_dir);
	else
		mkdir(same_size_files_dir, 0755);
}

// dir 안의 파일을 전부 삭제한다
void remove_files(char *dir)
{
	struct dirent **namelist;
	int listcnt = get_dirlist(dir, &namelist);

	for(int i=0; i<listcnt; i++){

		char fullpath[PATHMAX] = {0, };
		
		if(!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, ".."))
			continue;

		get_fullpath(dir, namelist[i]->d_name, fullpath);
		remove(fullpath);
	}
}

// 로그를 생성
char *log_msg(int mode, char *path)
{
	char log_buf[STRMAX];
	char command[10];
	char *user_time;
	time_t now;
	struct tm *_time;

	if(mode==0)
		strcpy(command, "[REMOVE]");
	else if(mode==1)
		strcpy(command, "[DELETE]");
	else if(mode==2)
		strcpy(command, "[RESTORE]");

	user_time = (char *)malloc(sizeof(char*));

	time(&now);
	_time = localtime(&now);
	get_time(_time, user_time);

	sprintf(log_buf, "%s %s %s %s\n", command, path, user_time, getenv("LOGNAME"));

	FILE *fp;
	fp = fopen(log_path, "a");

	if(fp!=NULL){
		fprintf(fp, log_buf);
	}
	else{
		printf("ERROR: 로그 작성에 실패했습니다\n");
		return NULL;
	}
	
	fclose(fp);
	return user_time;	
}

// 로그 파일 경로를 설정하고, 로그 파일을 생성
void get_log_path()
{
	char *home_path;
	home_path = getenv("HOME");
	sprintf(log_path, "%s/%s", home_path, ".duplicate_20202914.log");

	get_log_file();
}

void get_log_file()
{
	if(access(log_path, F_OK)==0){
		if(open(log_path, O_RDWR|O_APPEND)<0){
			fprintf(stderr, "open error for %s\n", log_path);
		}
	}
	else {
		if(open(log_path, O_RDWR|O_CREAT, 0640)<0){
			fprintf(stderr, "creat error for %s\n", log_path);
			return;
		}
	}
}

// 삭제 파일을 관리할 디렉터리의 경로를 설정하고 생성
void get_trash_path()
{
	char *home_path;
	home_path = getenv("HOME");
	sprintf(trash_path, "%s/%s", home_path, ".Trash/files");
	sprintf(trash_info, "%s/%s", home_path, ".Trash/info");

	get_trash_file(trash_path);
	get_trash_file(trash_info);
}

void get_trash_file(char *path)
{
	char tmp_path[PATH_MAX];
	char *tmp = tmp_path;

	strcpy(tmp_path, path);
	tmp_path[strlen(path)] = '\0';

	while(*tmp)
	{
		if('/' == *tmp && tmp!=tmp_path)
		{
			*tmp = '\0';

			if(access(tmp_path, F_OK)==0){
				;
			}
			else{
				if(mkdir(tmp_path, 0751)<0){
					fprintf(stderr, "mkdir error for %s\n", tmp_path);
					printf("error number = %d, error str = %s\n", errno, strerror(errno));
					return;
				}
			}

			*tmp = '/';
		}

		tmp++;
	}

	if(access(tmp_path, F_OK)==0){
		;
//		remove_files(tmp_path);
	}
	else{
		if(mkdir(tmp_path, 0751)<0){
			fprintf(stderr, "mkdir error for %s\n", tmp_path);
			printf("error number = %d, error str = %s\n", errno, strerror(errno));
			return;
		}
	}
}

// 인자 추출
int get_argv(char *input, char *argv[])
{
	int argc = 0;
	char *ptr = NULL;

	ptr = strtok(input, " ");
	while(ptr != NULL){
		argv[argc++] = ptr;
		ptr = strtok(NULL, " ");
	}

	return argc;
}

// filename에서 파일 확장자를 추출한다
char *get_extension(char *filename)
{
	int size = strlen(filename);
	char *tmp = filename;
	//	char *tmp = (char*)malloc(sizeof(char)*256);

	for(int i=size-1; i>=0; i--){
		if(filename[i]=='/') break;
		if(filename[i]=='.') {
			return tmp+i+1;
		}
	}
	return NULL;
}

// 문자열로 들어온 수를 long long 형으로 변환
long long get_size(char *filesize)
{
	double size_bytes = 0;
	char size[STRMAX] = {0, };
	char size_unit[4] = {0, };
	int size_idx = 0;
	int _filesize;

	if(!strcmp(filesize, "~"))
		size_bytes = -1;
	else{
		_filesize = strlen(filesize);

		for(int i=0; i<_filesize; i++){
			// 사이즈 입력 받기
			if(isdigit(filesize[i]) || filesize[i] == '.'){
				size[size_idx++] = filesize[i];
				if(filesize[i]=='.' && !isdigit(filesize[i+1]))
					return SIZE_ERROR;
			}
			// 단위 입력 받기
			else{
				strcpy(size_unit, filesize+i);
				break;
			}
		}

		size_bytes = atof(size);

		// 단위가 있는 경우 값 변환
		if(strlen(size_unit) != 0){
			if(!strcmp(size_unit, "kb") || !strcmp(size_unit, "KB"))
				size_bytes *= KB;
			else if(!strcmp(size_unit, "mb") || !strcmp(size_unit, "MB"))
				size_bytes *= MB;
			else if(!strcmp(size_unit, "gb") || !strcmp(size_unit, "GB"))
				size_bytes *= GB;
			else if(!strcmp(size_unit, "kib") || !strcmp(size_unit, "KiB"))
				size_bytes *= KiB;
			else if(!strcmp(size_unit, "mib") || !strcmp(size_unit, "MiB"))
				size_bytes *= MiB;
			else if(!strcmp(size_unit, "gib") || !strcmp(size_unit, "GiB"))
				size_bytes *= GiB;
			else
				return SIZE_ERROR;
		}
	}

	return (long long) size_bytes;
}

// 파일 사이즈에 콤마(,)를 추가하여 만든 문자열을 filesize_w_comma 배열에 넣음
void set_filesize(long long filesize, char *filesize_w_comma)
{
	char tmp[STRMAX] = {0, };
	int comma;
	int len, idx = 0;

	sprintf(tmp, "%lld", filesize);
	comma = strlen(tmp)%3;
	len = strlen(tmp);

	for(int i=0; i<len; i++){
		if(i>0 && (i%3)==comma)
			filesize_w_comma[idx++] = ',';

		filesize_w_comma[idx++] = tmp[i];
	}

	filesize_w_comma[idx] = '\0';
}

// 홈 디렉터리가 포함된 경로의 절대경로
void get_path_from_home(char *path, char *path_from_home)
{
	char path_without_home[PATHMAX] = {0, };
	char *home_path;

	// 홈 디렉토리 주소 받아오기
	home_path = getenv("HOME");

	// ~ 인 경우
	if(strlen(path)==1){
		strncpy(path_from_home, home_path, strlen(home_path));
	}
	// ~ + @ 인 경우
	else{
		strncpy(path_without_home, path+1,  strlen(path)-1);
		sprintf(path_from_home, "%s%s", home_path, path_without_home);
	}
}

// head로부터 시작해서 fileInfo를 탐색해가며 가장 끝에 데이터를 삽입한다
void fileinfo_append(fileInfo *head, char *path)
{
	fileInfo *fileinfo_cur;

	fileInfo *newinfo = (fileInfo *)malloc(sizeof(fileInfo));
	memset(newinfo, 0, sizeof(fileInfo));
	strcpy(newinfo->path, path);
	lstat(newinfo->path, &newinfo->statbuf);
	newinfo->next = NULL;

	if (head->next == NULL)
		head->next = newinfo;
	else {
		fileinfo_cur = head->next;
		while (fileinfo_cur->next != NULL)
			fileinfo_cur = fileinfo_cur->next;

		fileinfo_cur->next = newinfo;
	}
}

// path와 일치하는 데이터를 fileinfo에서 삭제하고 그 다음 데이터를 가리키는 포인터 리턴
fileInfo *fileinfo_delete_node(fileInfo *head, char *path)
{
	fileInfo *deleted;

	if (!strcmp(head->next->path, path)){
		deleted = head->next;
		head->next = head->next->next;
		return head->next;
	}
	else {
		fileInfo *fileinfo_cur = head->next;

		while (fileinfo_cur->next != NULL){
			if (!strcmp(fileinfo_cur->next->path, path)){
				deleted = fileinfo_cur->next;
				
				fileinfo_cur->next = fileinfo_cur->next->next;
				return fileinfo_cur->next;
			}

			fileinfo_cur = fileinfo_cur->next;
		}
	}
}

// fileinfolist의 size를 리턴
int fileinfolist_size(fileInfo *head)
{
	fileInfo *cur = head->next;
	int size = 0;
	
	while (cur != NULL){
		size++;
		cur = cur->next;
	}
	
	return size;
}

// head로부터 시작해서 filelist를 탐색해가며 데이터를 오름차순으로 삽입한다
void filelist_append(fileList *head, long long filesize, char *path, char *hash)
{
	struct stat statbuf;
	
	if(lstat(path, &statbuf)<0){
		printf("ERROR: lstat error for %s\n", path);
		return;
	}

    fileList *newfile = (fileList *)malloc(sizeof(fileList));
    memset(newfile, 0, sizeof(fileList));

    newfile->filesize = filesize;
    strcpy(newfile->hash, hash);

    newfile->fileInfoList = (fileInfo *)malloc(sizeof(fileInfo));
    memset(newfile->fileInfoList, 0, sizeof(fileInfo));

    fileinfo_append(newfile->fileInfoList, path);
    newfile->next = NULL;

    if (head->next == NULL) {
        head->next = newfile;
    }    
    else {
        fileList *cur_node = head->next, *prev_node = head, *next_node;

		// newfile보다 큰 사이즈를 가지는 파일을 탐색하고 찾으면 그 앞에 삽입
        while (cur_node != NULL && cur_node->filesize < newfile->filesize) {
            prev_node = cur_node;
            cur_node = cur_node->next;
        }

        newfile->next = cur_node;
        prev_node->next = newfile;
    }    
}

// hash 값을 가지는 filelist를 찾아 filelist에서 삭제한다
void filelist_delete_node(fileList *head, char *hash)
{
	fileList *deleted;

	if (!strcmp(head->next->hash, hash)){
		deleted = head->next;
		head->next = head->next->next;
	}
	else {
		fileList *filelist_cur = head->next;

		while (filelist_cur->next != NULL){
			if (!strcmp(filelist_cur->next->hash, hash)){
				
				deleted = filelist_cur->next;
				filelist_cur->next = filelist_cur->next->next;

				break;
			}

			filelist_cur = filelist_cur->next;
		}
	}

	free(deleted);
}

// filelist의 size를 리턴
int filelist_size(fileList *head)
{
	fileList *cur = head->next;
	int size = 0;
	
	while (cur != NULL){
		size++;
		cur = cur->next;
	}
	
	return size;
}

// hash값을 가지는 filelist를 탐색하고 idx를 리턴
int filelist_search(fileList *head, char *hash)
{
	fileList *cur = head;
	int idx = 0;

	while (cur != NULL){
		if (!strcmp(cur->hash, hash))
			return idx;
		cur = cur->next;
		idx++;
	}

	return 0;
}

// 파일 리스트를 출력
void print_filelist(fileList *head)
{
	fileList *filelist_cur = head->next;
	int filelist_idx = 1;

	while(filelist_cur != NULL)
	{
		fileInfo *fileinfolist_cur = filelist_cur->fileInfoList->next;
		char mtime[STRMAX];
		char atime[STRMAX];
		char filesize_w_comma[STRMAX] = {0, };
		int i=1;

		set_filesize(filelist_cur->filesize, filesize_w_comma);
		printf("---- Identical files #%d (%s bytes - %s) ----\n",
				filelist_idx++, filesize_w_comma, filelist_cur->hash);

		while(fileinfolist_cur != NULL)
		{
			struct stat statbuf;
			lstat(fileinfolist_cur->path, &statbuf);

			get_time(localtime(&fileinfolist_cur->statbuf.st_mtime), mtime);
			get_time(localtime(&fileinfolist_cur->statbuf.st_atime), atime);

			printf("[%d] %s (mtime : %s) (atime : %s) (uid : %d) (gid : %d) (mode : %o)\n",
				   	i++, fileinfolist_cur->path, mtime, atime,
					statbuf.st_uid, statbuf.st_gid, statbuf.st_mode);

			fileinfolist_cur = fileinfolist_cur->next;
		}

		printf("\n");
		filelist_cur = filelist_cur->next;
	}
}
// head로부터 시작해서 dirlist를 탐색해가며 가장 끝에 데이터를 삽입한다
void dirlist_append(dirList *head, char *path)
{
	dirList *newFile = (dirList *)malloc(sizeof(dirList));

	strcpy(newFile->dirpath, path);
	newFile->next = NULL;
	
	if (head->next == NULL)
		head->next = newFile;
	else{
		dirList *cur = head->next;
		
		while(cur->next != NULL)
			cur = cur->next;
		
		cur->next = newFile;
	}
}

// dirlist를 전부 삭제한다
void dirlist_delete_all(dirList *head)
{
	dirList *dirlist_cur = head->next;
	dirList *tmp;

	while (dirlist_cur != NULL){
		tmp = dirlist_cur->next;
		free(dirlist_cur);
		dirlist_cur = tmp;
	}

	head->next = NULL;
}

// time으로부터 구한 시간을 형식에 맞추어 set_time에 저장
void get_time(struct tm *time, char *set_time)
{
	sprintf(set_time, "%04d-%02d-%02d %02d:%02d:%02d",
			time->tm_year+1900, time->tm_mon+1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);
}

// 가장 최근에 수정된 파일을 탐색하고, last_filepath 배열에 파일 경로를 저장한다
// 가장 최근에 수정된 파일의 수정시간을 리턴한다
time_t get_recent_mtime(fileInfo *head, char *last_filepath)
{
	fileInfo *cur = head->next;
	time_t mtime = 0;

	while(cur != NULL)
	{
		if(cur->statbuf.st_mtime > mtime){
			mtime = cur->statbuf.st_mtime;
			strcpy(last_filepath, cur->path);
		}
		cur = cur->next;
	}

	return mtime;
}

// file mode를 리턴한다
int get_file_mode(char *target_file, struct stat *statbuf)
{
	if(lstat(target_file, statbuf)<0){
		printf("ERROR: lstat error for %s\n", target_file);
		return 0;
	}

	if(S_ISREG(statbuf->st_mode))
		return REGFILE;
	else if(S_ISDIR(statbuf->st_mode))
		return DIRECTORY;
	else
		return 0;
}

// target_dir에서 디렉터리 안의 파일들을 namelist로 가져온다
// 디렉터리 안 파일의 개수를 리턴한다
int get_dirlist(char *target_dir, struct dirent ***namelist)
{
	int cnt = 0;

	if((cnt = scandir(target_dir, namelist, NULL, alphasort))==-1){
		printf("ERROR: scandir error for %s\n", target_dir);
		return -1;
	}

	return cnt;
}

// 디렉터리인지 확인
int is_dir(char *target_dir)
{
	struct stat statbuf;

	if(lstat(target_dir, &statbuf)<0){
		printf("ERROR: lstat error for %s\n", target_dir);
		return 1;
	}

	// DIR이면 DIRECTORY 리턴, 아니면 0을 리턴
	return S_ISDIR(statbuf.st_mode) ? DIRECTORY : 0;
}


// path에서 이름만 추출한다
void get_filename(char *path, char *filename)
{
	char tmp_name[NAMEMAX];
	char *pt = NULL;

	memset(tmp_name, 0, sizeof(tmp_name));

	// 파일 이름을 얻어옴
	if (strrchr(path, '/') != NULL)
		strcpy(tmp_name, strrchr(path, '/') + 1);
	else
		strcpy(tmp_name, path);

	if ((pt = get_extension(tmp_name)) != NULL){
		pt[-1] = '\0';
	}

	if (strchr(path, '/') == NULL && (pt = strrchr(tmp_name, '.')) != NULL)
		pt[0] = '\0';

	strcpy(filename, tmp_name);
}

// 새로운 filename을 생성한다
void get_new_file_name(char *path, char *org_filename, char *new_filename)
{
	char new_trash_path[PATHMAX];
	char tmp[NAMEMAX];
	struct dirent **namelist;
	int trashlist_cnt;
	int same_name_cnt = 1;

	get_filename(org_filename, new_filename);
	trashlist_cnt = get_dirlist(path, &namelist);

	// 휴지통의 파일 검사
	for (int i = 0; i < trashlist_cnt; i++){
		if (!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, ".."))
			continue;

		memset(tmp, 0, sizeof(tmp));
		get_filename(namelist[i]->d_name, tmp);

		// 이름이 같은 경우 카운트
		if (!strcmp(new_filename, tmp))
			same_name_cnt++;
	}

	// new_filename의 뒤에 .same_name_cnt를 붙임 
	sprintf(new_filename + strlen(new_filename), ".%d", same_name_cnt);

	// 확장자명을 붙임
	if (get_extension(org_filename) != NULL)
		sprintf(new_filename + strlen(new_filename), ".%s", get_extension(org_filename));
}

// target_dir와 target_file를 target_dir/target_file 형식의 경로를 만들어 fullpath에 저장
void get_fullpath(char *target_dir, char *target_file, char *fullpath)
{
	strcat(fullpath, target_dir);

	if(fullpath[strlen(target_dir)-1] != '/')
		strcat(fullpath, "/");

	strcat(fullpath, target_file);
	fullpath[strlen(fullpath)] = '\0';
}
